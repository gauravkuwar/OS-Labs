// task 1
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  int child1 = fork(); // forks first child process
  int status;

  if (child1 == 0) {
    printf("I am child one, my pid is: %d\n", getpid());
  }
  else {
    int child2 = fork(); // forks second child process
    int status;

    if (child2 == 0) {
      printf("I am child two, my pid is: %d\n", getpid());
    }
    else {
      waitpid(child1, &status, 0); // parent waits for child1 to finish
      waitpid(child2, &status, 0); // parent waits for child2 to finish
    };
  };
}
