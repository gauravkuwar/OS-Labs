#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sem.h"

#define CHILD      			 0  		/* Return value of child process from fork call */
#define TRUE       			 1
#define FALSE      			 0

#define NumOfAgentPlaceIngre  10           /* Number of times agent places ingredients */
#define sleepTime 1 // A variable I made to adjust how much time the processes waits
										// So I can test the code faster (I set it to 1)

FILE *fp1, *fp2, *fp3, *fp4;			/* File Pointers */

int main()
{
	int pid;						// Process ID after fork call
	int i;							// Loop index
	int N;							// Number of times agent places ingredients
  int randNum;        // random number
	int isAgentDone;    // a variable represents if agent has anymore attempts left

	int status;					// Exit status of child process
	srand(time(NULL));

	int lock = semget(IPC_PRIVATE,1,0666 | IPC_CREAT); // mutex lock
  int agent = semget(IPC_PRIVATE,1,0666 | IPC_CREAT);
  int smoker_match = semget(IPC_PRIVATE,1,0666 | IPC_CREAT);
  int smoker_paper = semget(IPC_PRIVATE,1,0666 | IPC_CREAT);
  int smoker_tobacco = semget(IPC_PRIVATE,1,0666 | IPC_CREAT);

	sem_create(lock, 1);
  sem_create(agent, 1);
  sem_create(smoker_match, 0);
  sem_create(smoker_paper, 0);
  sem_create(smoker_tobacco, 0);

	// the value in this file is set to 1 if agent is agentDone
	// and 0 if agent is still running

	fp1 = fopen("agentDone.txt","w+");
	isAgentDone = FALSE;
	fprintf(fp1, "%d\n", isAgentDone);
	fclose(fp1);

	//Create child processes that will do the updates
	if ((pid = fork()) == -1)
	{
		//fork failed!
		perror("fork");
		exit(1);
	}

	if (pid == CHILD){
	  printf("Agent's Pid: %d\n",getpid());
		N=NumOfAgentPlaceIngre;
		for(i=1;i<=N; i++) {
      P(lock);
			printf("Agent has %d attempts left\n", (N - i + 1));
      randNum = rand()%3+1; // Pick random num from range 1 to 3

      if ( randNum == 1) {
        // Put tobacco on table
        // Put paper on table
        printf("Agent puts tobacco on table\n");
        printf("Agent puts paper on table\n");
        V( smoker_match );  // Wake up smoker with match
      } else if ( randNum == 2) {
        // Put tobacco on table
        // Put match on table
        printf("Agent puts tobacco on table\n");
        printf("Agent puts match on table\n");
        V( smoker_paper );  // Wake up smoker with paper
      } else {
        // Put match on table
        // Put paper on table
        printf("Agent puts match on table\n");
        printf("Agent puts paper on table\n");
        V( smoker_tobacco ); // Wake up smoker with tobacco
      }

      V(lock);
      P(agent); // Agent sleeps
			sleep(rand()%(sleepTime)+1);
		}

		// this section of code allows all the smoker process
		// to exit, once agent is done.

		P(lock);

		fp2 = fopen("agentDone.txt","w+");
		isAgentDone = TRUE;
		fprintf(fp2, "%d\n", isAgentDone);
		fclose(fp2);

		// this allows smoker process waiting to exit

		V( smoker_match );
		V( smoker_tobacco );
		V( smoker_paper );

		V(lock);
	}

	else {
		//Parent Process. Fork off another child process.
		if ((pid = fork()) == -1) {
			//Fork failed!
			perror("fork");
			exit(1);
		}
		if (pid == CHILD) {
      printf("Smoker with tobacco's Pid: %d\n",getpid());

			while(TRUE) {
        P(smoker_tobacco);
        P(lock);

				fp3 = fopen("agentDone.txt", "r+"); //Dad successfully got hold of the ATM.
				fscanf(fp3, "%d", &isAgentDone);
				fclose(fp3);

				if (isAgentDone == TRUE) {
					V(lock);
					exit(0);
				}

        // Pick up smoker_match
        printf("Smoker with tobacco picks up match.\n");
        // Pick up smoker_paper
        printf("Smoker with tobacco picks up paper.\n");

        V(agent);
        V(lock);

        // Smoke (but don't inhale).
        sleep(rand()%(sleepTime)+1);
      }
		}
		else {

		//Parent Process. Fork off one more child process.
			if ((pid = fork()) == -1) {
				//fork failed!
				perror("fork");
				exit(1);
			}
			if (pid == CHILD) {
        printf("Smoker with match's Pid: %d\n",getpid());

  			while(TRUE) {
          P(smoker_match);
          P(lock);

					fp3 = fopen("agentDone.txt", "r+"); //Dad successfully got hold of the ATM.
					fscanf(fp3, "%d", &isAgentDone);
					fclose(fp3);

					if (isAgentDone == TRUE) {
						V(lock);
						exit(0);
					}

          // Pick up smoker_tobacco
          printf("Smoker with match picks up tobacco.\n");
          // Pick up smoker_paper
          printf("Smoker with match picks up paper.\n");

          V(agent);
          V(lock);

          // Smoke (but don't inhale).
          sleep(rand()%(sleepTime)+1);
        }
			}
			else {

        //Parent Process. Fork off one more child process.
    			if ((pid = fork()) == -1)
    			{
    				//fork failed!
    				perror("fork");
    				exit(1);
    			}
          if (pid == CHILD) {
            printf("Smoker with paper's Pid: %d\n",getpid());

      			while(TRUE) {
              P(smoker_paper);
              P(lock);

							fp3 = fopen("agentDone.txt", "r+"); //Dad successfully got hold of the ATM.
							fscanf(fp3, "%d", &isAgentDone);
							fclose(fp3);

							if (isAgentDone == TRUE) {
								V(lock);
								exit(0);
							}

              // Pick up smoker_match
              printf("Smoker with paper picks up match.\n");
              // Pick up smoker_tobacco
              printf("Smoker with paper picks up tobacco.\n");

              V(agent);
              V(lock);

              // Smoke (but don't inhale).
              sleep(rand()%(sleepTime)+1);
            }
    			}
    			else {

    				//Now parent process waits for the child processes to finish

    				pid = wait(&status);
          	printf("child(pid = %d) exited with the status %d. \n", pid, status);

          	pid = wait(&status);
          	printf("child(pid = %d) exited with the status %d. \n", pid, status);

          	pid = wait(&status);
          	printf("child(pid = %d) exited with the status %d. \n", pid, status);

            pid = wait(&status);
          	printf("child(pid = %d) exited with the status %d. \n", pid, status);
    			}
          exit(0);
			}
			exit(0);
		}
		exit(0);
	}
	exit(0);
}
