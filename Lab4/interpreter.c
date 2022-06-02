#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CMD_SIZE 256 // total size of the command
#define MAX_ARR_SIZE 10 // Num of parts to the command

// Fixed Problem: when inputted an invalid command into the interpreter
// exit would not work, because the child process would go into the while
// loop, and you would have to input exit twice to exit the interpreter
// I fixed this by added exit(1); when the execvp failed.

int main() {
  char cmd[MAX_CMD_SIZE];

  while (1)  {
    printf("command: "); // prints command:
    scanf(" %[^\n]", cmd); // gets user input into cmd

    // if user inputs "exit" the the while loop breaks
    if (strcmp(cmd, "exit") == 0) {
      exit(0);
    }

    char* args[MAX_ARR_SIZE]; // array of arguments for execvp
    int i = 0;

    char* tok = strtok(cmd, " "); // splits the cmd by space
    while (tok != NULL) { // gets each part of the cmd and adds it to the array args
      args[i] = tok;
      i++;
      tok = strtok(NULL, " "); // gets next part of the command
    }
    args[i] = NULL; // Since last argument should be NULL


    // child is forked
    int child = fork();
    int status;

    if (child == -1) {
      printf ("Failed to fork childt\n");
      exit(1);
    }

    if (child == 0) {
      // executes user command
      execvp(args[0], args);
      // prints this if the execl command failes
      printf("EXECL Failed\n");
      exit(1); // Added this line

    } else {
      wait(&status);
    }
  }
  return 0;
}
