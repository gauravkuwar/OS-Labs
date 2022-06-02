#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sem.h"

#define CHILD      			 0  		/* Return value of child process from fork call */
#define TRUE       			 0
#define FALSE      			 1

#define NumOfDepositAttempt  5           /* How many times Dad will deposit money */
#define NumOfWithdrawAttempt 20          /* Total number of attempts SON_1 and SON_2 has */

#define DepositAmount        80         /* The amound of money Dad deposits at a time */
#define WithdrawAmount       20         /* The amount of money Son withdraws at a time */
#define initBalance          40
#define sleepTime 5 // A variable I made to adjust how much time the processes waits
										// So I can test the code faster (I set it to 1)

FILE *fp1, *fp2, *fp3, *fp4;			/* File Pointers */

int main()
{
	int pid;						// Process ID after fork call
	int i;							// Loop index
	int N;							// Number of times dad does update
	int N_Att;						// Number of time sons allowed to do update
	int status;						// Exit status of child process
	int bal1, bal2, bal3;					// Balance read by processes
	int flag, flag1;				// End of loop variables
	srand(time(NULL));

	int semAtms = semget(IPC_PRIVATE,1,0666 | IPC_CREAT);
	int semMutex = semget(IPC_PRIVATE,1,0666 | IPC_CREAT);
	int semNoBalance = semget(IPC_PRIVATE,1,0666 | IPC_CREAT);

	sem_create(semMutex, 1); // used to lock a transction, to prevent race conditions
	sem_create(semAtms, 2); // This makes sure only 2 process are running at once
													// Since there are only 2 ATM machines

	// this semaphore is used to check if there is enough balance to be withdrawn
	if (initBalance >= WithdrawAmount)
		sem_create(semNoBalance, 1);
	else
		sem_create(semNoBalance, 0);

	//Initialize the file balance to be $40
	//You should try with different initial balance, after you formulate the solution. Like initial Balance to be 0.
	fp1 = fopen("balance.txt","w+");
	bal1 = initBalance;
	fprintf(fp1, "%d\n", bal1);
	fclose(fp1);

	//Initialize the number of withdraw attempts
	fp4 = fopen("attempt.txt", "w+");
	N_Att = NumOfWithdrawAttempt;
	fprintf(fp4, "%d\n", N_Att);
	fclose(fp4);

	//Create child processes that will do the updates
	if ((pid = fork()) == -1)
	{
		//fork failed!
		perror("fork");
		exit(1);
	}

	if (pid == CHILD){
	    //First Child Process.
        //Dad tries to do some updates.
        //**you need to identify the logical issues and synchronization issues and solve them using logic and semaphores.**
	    printf("Dad's Pid: %d\n",getpid());
		N=NumOfDepositAttempt;
		for(i=1;i<=N; i++)
		{
            //Dad process need some time to go to the bank.
            int r = rand()%sleepTime+1;
            sleep(r);
            //After r second Dad process reached the Bank.

			printf("Dad is requesting to view the balance.\n"); //Dad is requesting to get hold of an ATM.

			P(semAtms); // Dad waits for access to ATM
			fp1 = fopen("balance.txt", "r+"); //Dad successfully got hold of the ATM.
			fscanf(fp1, "%d", &bal2);
			printf("Dad reads balance = %d \n", bal2);
		    r = rand()%sleepTime+1;
            printf("Dad wants to deposit money\n");
			printf("Dad needs %d sec to prepare money.\n", r);
			V(semAtms); // Dad loses access to ATM
			sleep(r); //Dad Process is sleeping for r sec. You need to make sure that other processes can work in the mean time.
			//After some time Dad process wakes up.
			//It is possible that the balance has changed during the time dad process is sleeping
			//Dad process starts to deposit the money. So Dad process needs the access to ATM.
			//Only after getting access to the ATM dad process can deposit money.

			P(semAtms); // Dad waits for access to ATM again

			// read balance again
			P(semMutex); // lock this transction / atomic
			fp1 = fopen("balance.txt", "r+"); //Dad successfully got hold of the ATM.
			fscanf(fp1, "%d", &bal2);

			fseek(fp1,0L,0); //Dad will now deposit the money. And update the current balance.
			bal3 = bal2 + DepositAmount; // bal2 is saved as previous balance
			fprintf(fp1, "%d \n", bal3);
			fclose(fp1);
			printf("Dad writes new balance = %d \n", bal3);

			// for 1 son process waiting for enough balance
			if (bal2 < WithdrawAmount && bal3 >= WithdrawAmount)
				V(semNoBalance);
			// for both son process waiting for enough balance
			if (bal2 < (2*WithdrawAmount) && bal3 >= (2*WithdrawAmount))
				V(semNoBalance);

			V(semMutex); // unlock

			printf("Dad will deposit %d more time\n",N-i); //Dad depostited the money.
			V(semAtms); // Dad loses access to ATM

			sleep(rand()%(sleepTime*2)+1);	/* Dad will wait some time for requesting to see balance again.*/
		}
	}

	else
	{

		//Parent Process. Fork off another child process.
		if ((pid = fork()) == -1)
		{
			//Fork failed!
			perror("fork");
			exit(1);
		}
		if (pid == CHILD)
		{
			printf("First Son's Pid: %d\n",getpid());
			//Second child process.
            //**you need to identify the logical issues and synchronization issues and solve them using logic and semaphores.**
            //Son1 tries to view the balance
			flag = FALSE;
			while(flag == FALSE)
			{
				//Son1 process needs some time to go to the bank.
                int r = rand()%sleepTime+1;
                sleep(r);
                //After r second Son1 process reached the Bank.

                printf("SON_1 is requesting to view the balance.\n"); //Son_1 is requesting to get hold of the ATM.

				P(semAtms);
				fp3 = fopen("attempt.txt" , "r+"); //Son_1 successfully got hold of the ATM.
				fscanf(fp3, "%d", &N_Att); // Son_1 Checks if he has more than 0 attempt remaining.
				printf("Attempt remaining: %d.\n", N_Att);
				if(N_Att == 0)
				{
					fclose(fp3);
					flag = TRUE;
				}
				else
				{

					fp2 = fopen("balance.txt", "r+");//Son_1 reads the balance.
					fscanf(fp2,"%d", &bal2);
					printf("SON_1 reads balance. Available Balance: %d \n", bal2);
					printf("SON_1 wants to withdraw money. \n"); //And if balance is greater than Withdraw amount, then son can withdraw money.

					// when there is not enough balance / to prevent negative balance
					if (bal2 < WithdrawAmount) {
						V(semAtms); // son loses access to ATM
						P(semNoBalance); // waits for dad to put balance
						P(semAtms); // son gains access to ATM again
					}

					P(semMutex); // lock this transction / atomic
					// read balance again since it could have changed
					fp2 = fopen("balance.txt", "r+");
					fscanf(fp2,"%d", &bal2);

					fseek(fp2,0L, 0);
					bal2 -=WithdrawAmount;
					fprintf(fp2,"%d\n", bal2);
					fclose(fp2);
					printf("SON_1 withdrawed %d. New Balance: %d \n",WithdrawAmount, bal2);

					// read attempts again since it could have changed
					fp3 = fopen("attempt.txt" , "r+");
					fscanf(fp3, "%d", &N_Att);

					fseek(fp3,0L, 0); //SON_1 will write the number of  attempt remaining in the attampt.txt file.
					N_Att -=1;
					fprintf(fp3, "%d\n", N_Att);
					fclose(fp3);
					printf("Number of attempts remaining:%d \n", N_Att);
					V(semMutex); // unlock
				}
				V(semAtms); // son loses access to ATM
				sleep(rand()%(sleepTime*2)+1); //SON_1 will wait some time before the next request.
			}
		}
		else
		{

		//Parent Process. Fork off one more child process.
			if ((pid = fork()) == -1)
			{
				//fork failed!
				perror("fork");
				exit(1);
			}
			if (pid == CHILD)
			{
				printf("Second Son's Pid: %d\n",getpid());
				//Third child process.
                //**you need to identify the logical issues and synchronization issues and solve them using logic and semaphores.**
                //Son2 tries to view the balance.
				flag1 = FALSE;
				while(flag1 == FALSE)
				{
					//Son2 process needs some time to go to the bank.
                    int r = rand()%sleepTime+1;
                    sleep(r);
                    //After r second Son2 process reached the Bank.
                    printf("SON_2 is requesting to view the balance.\n"); //Son_2 is requesting to get hold of the ATM.

					P(semAtms);
					fp3 = fopen("attempt.txt" , "r+"); //Son_2 successfully got hold of the ATM.
					fscanf(fp3, "%d", &N_Att); // Son_2 Checks if he has more than 0 attempt remaining.
					printf("Attempt remaining: %d.\n", N_Att);
					if(N_Att == 0)
					{
						fclose(fp3);
						flag1 = TRUE;
					}
					else
					{

						fp2 = fopen("balance.txt", "r+"); //Son_2 reads the balance.
						fscanf(fp2,"%d", &bal2);
						printf("SON_2 reads balance. Available Balance: %d \n", bal2);
						printf("SON_2 wants to withdraw money. \n"); //And if balance is greater than Withdraw amount, then son can withdraw money.

						// when there is not enough balance / to prevent negative balance
						if (bal2 < WithdrawAmount) {
							V(semAtms); // son loses access to ATM
							P(semNoBalance); // waits for dad to put balance
							P(semAtms); // son gains access to ATM again
						}

						P(semMutex); // lock this transction / atomic
						// read balance again since it could have changed
						fp2 = fopen("balance.txt", "r+");
						fscanf(fp2,"%d", &bal2);

						fseek(fp2,0L, 0);
						bal2 -=WithdrawAmount;
						fprintf(fp2,"%d\n", bal2);
						fclose(fp2);
						printf("SON_2 withdrawed %d. New Balance: %d \n",WithdrawAmount, bal2);

						// read attempts again since it could have changed
						fp3 = fopen("attempt.txt" , "r+");
						fscanf(fp3, "%d", &N_Att);

						fseek(fp3,0L, 0); //SON_2 will write the number of  attempt remaining in the attempt.txt file.
						N_Att -=1;
						fprintf(fp3, "%d\n", N_Att);
						fclose(fp3);
						printf("Number of attempts remaining: %d \n", N_Att);
						V(semMutex); // unlock
					}
					V(semAtms); // son loses access to ATM
					sleep(rand()%(sleepTime*2)+1);//SON_2 will wait some time before the next request.
				}
			}
			else
			{

				//Now parent process waits for the child processes to finish

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
