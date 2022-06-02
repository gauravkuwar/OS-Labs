#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

// prints usage of program (proper arguments)
void usage() {
  printf("\n Usage : Needs arguments:\n");
  printf("\tfn: path to input file, with a table rows separated by \\n and columns separated by any whitespace (expect \\n)\n");
}

int main(int argc, char *argv[]) {
  // This is part of Director code
  // checks valid number of arguments
  if(2 != argc)
  {
    usage();
    return 1;
  }

  char* fn = argv[1];
  FILE *fd;
  fd = fopen(fn, "r");

  char c;
  int tot_j = 1, tot_i = 1;
  int count_j = 1;

  // count the number of tabs and newlines in the file
  // which allows us to get the number of rows and columns

  while(fd) {
    c = fgetc(fd);
    if (c == EOF)
       break;
    if (count_j == 1 && c == '\t')
       tot_j++;
    if (c == '\n') {
       tot_i++;
       count_j = 0;
     }
  }

  int num;
  int i = 0;
  int j = 0;
  int allGrades[tot_i][tot_j];

  // printf("Number of tabs = %d\n", tot_j);
  // printf("Number of newline = %d\n", tot_i);

  // get every number in the file and organize it
  // into the allGrades matrix using tot_i and tot_j

  fseek(fd, 0, SEEK_SET);
  while(fscanf(fd, "%d", &num) != EOF) {
    allGrades[i][j] = num;
    j++;
    if (j == tot_j) {
      j = 0;
      i++;
    }
  }

  // Director part of the code ends here
  // Code below does the management of process into
  // manager and worker process as shown in the tree (From lab4 outline)
  
  int y = 2; // I assume that y = 2, because we only have 1 input
             // But it should work for every value of y where
             // tot_j % y == 0
  int x = tot_j / y;

  for (int m=0; m <= x - 1; m++) {
    int manager = fork(); // forks manager process
    int status;

    if (manager == 0) {
      // Can use this print statement shows manager num and its pid
      // printf("I am manager %d, my pid is: %d\n", m, getpid());

      for (int w=0; w < y; w++) {
        int worker = fork(); // forks worker process
        int status;

        if (worker == 0) {
          // Can use this print statement shows worker num and its pid
          // printf("\tI am worker %d, my pid is: %d\n", w, getpid());

          // Calculates per chapter student grade average
          int chapterGradeSum = 0;
          int j = (m * y) + w;

          for (i=0; i<tot_i; i++) {
              chapterGradeSum += allGrades[i][j];
          }

          float chapterAvg = chapterGradeSum / (tot_i * 1.0);
          printf("X%dY%d Avg: %.2f\n", m+1, w+1, chapterAvg);
          exit(0);
        }
        else
          waitpid(worker, &status, 0);
      }
      exit(0);
    }
    else
      waitpid(manager, &status, 0);
  }

  return 0;
}
