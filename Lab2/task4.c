#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd[2], nbytes;
    char readbuffer[100];
    char* source = "readme.txt";
    pipe(fd);

    int child = fork();

    if (child == -1) {
      printf ("Failed to fork childt\n");
      return 1;
    }

    if (child == 0) {
      // child reads file content and writes to pipe
      int source_fd = open(source, O_RDONLY);\

      if(-1 == source_fd)
      {
        if (errno == ENOENT)
          printf ("%s does not exist\n", source);
        else if (errno == EACCES)
          printf ("%s is not readable\n", source);
        else
          printf("\n open() failed with error [%s]\n",strerror(errno));
        return 1;
      }

      // Reads file
      int size = lseek(source_fd, 0, SEEK_END) - 1 ; // counts bytes in file
      lseek(source_fd, 0, SEEK_SET); // sets location back to start of the file
      char c[size];

      read(source_fd, c, size); // reads file to c with size.

      close(fd[0]); // close read pipe
      write(fd[1], c, (strlen(c))); // write contents of file to pipe

      close(source_fd);
      return 1;
    }
    else {
      close(fd[1]); // close read pipe

      nbytes = read(fd[0], readbuffer, sizeof(readbuffer)); // read string from pipe

      int dest_fd = open(source, O_WRONLY); // open destination file in write only mode
      char parentText[] = "Parent is writing:\n";

      write(dest_fd, parentText, strlen(parentText)); // writes "Parent is writing:\n" to readme.txt
      write(dest_fd, readbuffer, nbytes); // writes contents read in child to readme.txt

      close(dest_fd);
    }

    return 0;
}
