#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
  int child1 = fork(); // forks first child process
  int status;

  if (child1 == 0) {
    char* fn = "./Prcs_P1";

    printf("Child1 executes %s, pid is: %d\n", fn, getpid());
    execl(fn, fn, NULL);
  }
  else {
    waitpid(child1, &status, 0); // parent waits for child1 to finish so files can be created
    int child2 = fork(); // forks second child process
    int status;

    if (child2 == 0) {
      char* fn = "./Prcs_P2";
      printf("Child2 executes %s, pid is: %d\n", fn, getpid());

      execl(fn, fn, NULL);
    }
    else {
      waitpid(child2, &status, 0); // parent waits for child2 to finish
    };
  };
  return 0;
}
