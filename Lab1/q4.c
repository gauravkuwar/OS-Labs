// Question 4
// Reads source.txt
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {
  char* source = "source.txt";
  char* dest = "destination.txt";
  int source_fd, dest_fd;

  // opens the source as read only
  errno = 0;
  source_fd = open(source, O_RDONLY);

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

  // creates new destination file
  errno = 0;
  dest_fd = open(dest, O_CREAT, 0644);

  if(-1 == dest_fd)
  {
    printf("\n Failed to create new file with error [%s]\n",strerror(errno));
    return 1;
  }

  close(dest_fd); // close destination file because its in read only mode


  char c[1];
  // finds the size of bytes in file
  int size = lseek(source_fd, 0, SEEK_END);
  lseek(source_fd, 0, SEEK_SET);

  dest_fd = open(dest, O_WRONLY); // open destination file in write only mode

  // loops until reached last char in file
  while (lseek(source_fd, 0, SEEK_CUR) < size) {
    read(source_fd, c, 1); // reads 1 char at a time from source file

    // replace '1' char to 'L'
    if (c[0] == '1')
      c[0] = 'L';

    write(dest_fd, c, 1); // writes char to destination file
    
    // current location of current char in file
    int loc = lseek(source_fd, 0, SEEK_CUR);
    // after 100 chars or at the end of file adds XYZ
    if (loc % 100 == 0 || loc == size) {
      write(dest_fd, "XYZ", 3);
    }
  }

  close(source_fd); // closes source file
  close(dest_fd); // closes destination file
  return 0;
}
