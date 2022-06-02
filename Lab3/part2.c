#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  // child is forked
  int child = fork();
  int status;

  if (child == -1) {
    printf ("Failed to fork childt\n");
    return 1;
  }

  if (child == 0) {
    printf("Inside child process with PID: %d\n", getpid());

    // uses l option for permissions, owners and date modified options
    // uses a option to show hidden files
    char* args[] = {"ls", "-la", NULL}; // list of arguments for execvp

    // executes date command
    execvp("ls", args);

    // prints this if the execl command failes
    printf("EXECL Failed\n");
  } else {
    // waits for child to finish
    // So exec output is printed properly by ending with a \n
    waitpid(child, &status, 0);
  }

  return 0;
}
