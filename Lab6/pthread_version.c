#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

// #include <sys/types.h>
// #include <sys/wait.h>

#define TRUE       			 1
#define FALSE      			 0
#define SMOKER_THREAD_NUM   3
#define AGENT_THREAD_NUM   1

#define NumOfAgentPlaceIngre  10           /* Number of times agent places ingredients */
#define sleepTime 1 // A variable I made to adjust how much time the processes waits
										// So I can test the code faster (I set it to 1)

int isAgentDone = FALSE;    // a variable represents if agent has anymore attempts left
sem_t agent, smoker_match, smoker_paper, smoker_tobacco;
pthread_mutex_t buffer_mutex;

void *agent_func() {
  srand(time(NULL));
  int i;							// Loop index
  int N;							// Number of times agent places ingredients
  int randNum;        // random number

  printf("Initialized Agent thread\n");
  N=NumOfAgentPlaceIngre;

  for(i=1;i<=N; i++) {
    pthread_mutex_lock(&buffer_mutex);
    printf("Agent has %d attempts left\n", (N - i + 1));
    randNum = rand()%3+1; // Pick random num from range 1 to 3

    if ( randNum == 1) {
      // Put tobacco on table
      // Put paper on table
      printf("Agent puts tobacco on table\n");
      printf("Agent puts paper on table\n");
      sem_post( &smoker_match );  // Wake up smoker with match
    } else if ( randNum == 2) {
      // Put tobacco on table
      // Put match on table
      printf("Agent puts tobacco on table\n");
      printf("Agent puts match on table\n");
      sem_post( &smoker_paper );  // Wake up smoker with paper
    } else {
      // Put match on table
      // Put paper on table
      printf("Agent puts match on table\n");
      printf("Agent puts paper on table\n");
      sem_post( &smoker_tobacco ); // Wake up smoker with tobacco
    }

    if (i == N) {
      isAgentDone = TRUE;
    }

    pthread_mutex_unlock(&buffer_mutex);
    sem_wait(&agent); // Agent sleeps
    sleep(rand()%(sleepTime)+1);
  }
  // wake up the smokers to make sure they quit
  sem_post( &smoker_match );  // Wake up smoker with match
  sem_post( &smoker_paper );  // Wake up smoker with paper
  sem_post( &smoker_tobacco ); // Wake up smoker with tobacco
  printf("Agent finished\n");
}

void* smoker_match_func() {
  char* smoker_type = "match";
  printf("Smoker with %s initialized thread\n", smoker_type);

  while(TRUE) {
    sem_wait(&smoker_match);
    pthread_mutex_lock(&buffer_mutex);

    if (isAgentDone == TRUE) {
    	pthread_mutex_unlock(&buffer_mutex);
    	break;
    }

    // Pick up smoker_tobacco
    printf("Smoker with %s picks up tobacco.\n", smoker_type);
    // Pick up smoker_paper
    printf("Smoker with %s picks up paper.\n", smoker_type);

    sem_post(&agent);
    pthread_mutex_unlock(&buffer_mutex);

    // Smoke (but don't inhale).
    sleep(rand()%(sleepTime)+1);
  }
  printf("Smoker with %s finished\n", smoker_type);
}

void* smoker_paper_func() {
  char* smoker_type = "paper";
  printf("Smoker with %s initialized thread\n", smoker_type);

  while(TRUE) {
    sem_wait(&smoker_paper);
    pthread_mutex_lock(&buffer_mutex);

    if (isAgentDone == TRUE) {
    	pthread_mutex_unlock(&buffer_mutex);
    	break;
    }

    // Pick up smoker_tobacco
    printf("Smoker with %s picks up tobacco.\n", smoker_type);
    // Pick up smoker_match
    printf("Smoker with %s picks up match.\n", smoker_type);

    sem_post(&agent);
    pthread_mutex_unlock(&buffer_mutex);

    // Smoke (but don't inhale).
    sleep(rand()%(sleepTime)+1);
  }
  printf("Smoker with %s finished\n", smoker_type);
}

void* smoker_tobacco_func() {
  char* smoker_type = "tobacco";
  printf("Smoker with %s initialized thread\n", smoker_type);

  while(TRUE) {
    sem_wait(&smoker_tobacco);
    pthread_mutex_lock(&buffer_mutex);

    if (isAgentDone == TRUE) {
    	pthread_mutex_unlock(&buffer_mutex);
    	break;
    }

    // Pick up smoker_paper
    printf("Smoker with %s picks up paper.\n", smoker_type);
    // Pick up smoker_match
    printf("Smoker with %s picks up match.\n", smoker_type);

    sem_post(&agent);
    pthread_mutex_unlock(&buffer_mutex);

    // Smoke (but don't inhale).
    sleep(rand()%(sleepTime)+1);
  }
  printf("Smoker with %s finished\n", smoker_type);
}

int main() {
  sem_init(&agent, 0, 1);
  sem_init(&smoker_match, 0, 0);
  sem_init(&smoker_paper, 0, 0);
  sem_init(&smoker_tobacco, 0, 0);

  pthread_mutex_init(&buffer_mutex, NULL);
  pthread_t agent_t, smoker_match_t, smoker_paper_t, smoker_tobacco_t;

  // create threads for smokers and agent

  if (pthread_create(&agent_t, NULL,&agent_func,NULL) != 0)
       perror("Failed to create agent thread");

  if (pthread_create(&smoker_match_t, NULL,&smoker_match_func, NULL) != 0)
      perror("Failed to create smoker match thread");

  if (pthread_create(&smoker_paper_t, NULL,&smoker_paper_func, NULL) != 0)
       perror("Failed to create smoker paper thread");

  if (pthread_create(&smoker_tobacco_t, NULL,&smoker_tobacco_func, NULL) != 0)
      perror("Failed to create smoker tobacco thread");


  // join threads for smokers and agent

  if (pthread_join(agent_t, NULL) != 0)
      perror("Failed to join agent thread");
  if (pthread_join(smoker_match_t, NULL) != 0)
      perror("Failed to join smoker match thread");
  if (pthread_join(smoker_paper_t, NULL) != 0)
      perror("Failed to join smoker paper thread");
  if (pthread_join(smoker_tobacco_t, NULL) != 0)
      perror("Failed to join smoker tobacco thread");


  sem_destroy(&agent);
  sem_destroy(&smoker_match);
  sem_destroy(&smoker_paper);
  sem_destroy(&smoker_tobacco);

  return 0;
}
