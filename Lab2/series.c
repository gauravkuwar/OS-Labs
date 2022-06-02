// task 3
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  if(2 != argc)
  {
    printf("\n Usage : Needs argument specifying integer n\n");
    return 1;
  }

  int n = atoi(argv[1]);

  int fd1[2]; // Used to store two ends of first pipe
  int fd2[2]; // Used to store two ends of second pipe

  if (pipe(fd1) == -1) {
      fprintf(stderr, "Pipe 1 Failed");
      return 1;
  }
  if (pipe(fd2) == -1) {
      fprintf(stderr, "Pipe 2 Failed");
      return 1;
  }

  int child1 = fork(); // forks first child process
  int status;

  if (child1 == 0) { // in child 1
    // child 1 computes S1
    int S1 = 0;
    for (int i=1; i <= n ;i=i+2) {
      S1 = S1 + i;
    }

    write(fd1[1], &S1, sizeof(S1)); // write S1 to pipe fd1
    printf("S1 = %d\n", S1);
  }
  else {
    int child2 = fork(); // forks second child process
    int status;

    if (child2 == 0) { // in child 2
      // child 2 computes S2
      int S2 = 0;
      for (int j=2; j <= n ;j=j+2) {
        S2 = S2 + j;
      }
      
      write(fd2[1], &S2, sizeof(S2)); // write S2 to pipe fd2
      printf("S2 = %d\n", S2);
    }
    else { // in parent
      waitpid(child1, &status, 0); // parent waits for child1 to finish
      waitpid(child2, &status, 0); // parent waits for child2 to finish

      int newS1, newS2;

      read(fd1[0], &newS1, sizeof(newS1)); // read S1 to pipe fd1
      read(fd2[0], &newS2, sizeof(newS2)); // read S2 to pipe fd2

      int S3 = newS1 + newS2;
      printf("S3 = %d\n", S3);
    };
  };
}
