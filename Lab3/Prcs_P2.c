#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

int main() {
  char* source = "source.txt";
  char* dest1 = "destination1.txt";
  char* dest2 = "destination2.txt";

  // opens the source as read only
  int source_fd = open(source, O_RDONLY);
  errno = 0;

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

  char c[1];
  // finds the size of bytes in file
  int size = lseek(source_fd, 0, SEEK_END);
  lseek(source_fd, 0, SEEK_SET);


  int dest_fd1 = open(dest1, O_WRONLY); // open destination 1 file in write only mode
  int dest_fd2 = open(dest2, O_WRONLY); // open destination 2 file in write only mode

  bool cpToDest1 = true; // true if copying to dest1 and false if copying to dest2
  int counter = 0; // counts to 100 or 50 depending on cpToDest1 value

  // loops until reached last char in file
  while (lseek(source_fd, 0, SEEK_CUR) < size) {
    read(source_fd, c, 1); // reads 1 char at a time from source file
    counter++;

    // replace '1' char to 'L', if copying to dest1
    if (cpToDest1) {
      if (c[0] == '1')
        c[0] = 'L';
      write(dest_fd1, c, 1); // writes char to destination 1 file

      if (counter == 100) {
        counter = 0;
        cpToDest1 = false;
      }
    }
    else { // replace '3' char to 'E', if copying to dest2
      if (c[0] == '3')
        c[0] = 'E';
      write(dest_fd2, c, 1); // writes char to destination 2 file

      if (counter == 50) {
        counter = 0;
        cpToDest1 = true;
      }
    }
  }

  close(source_fd); // closes source file
  close(dest_fd1); // closes destination 1 file
  close(dest_fd2); // closes destination 2 file
  return 0;
}
