// Question 3
// Copies content from one file to another
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  // checks valid number of arguments
  if(3 != argc)
  {
    printf("\n Usage : Needs argument specifying a source filename and destination filename\n");
    return 1;
  }

  char* source = argv[1]; // source file filepath
  char* dest = argv[2]; // destination file filepath
  int source_fd, dest_fd;

  // opens the source file as read only
  errno = 0;
  source_fd = open(source ,O_RDONLY);

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

  // Reads source file
  int size = lseek(source_fd, 0, SEEK_END) - 1 ; // counts bytes in file
  lseek(source_fd, 0, SEEK_SET); // sets location back to start of the file
  char c[size];

  read(source_fd, c, size); // reads file to c with size.

  // creates new destination file with O_CREAT flag with 0644 permissions
  errno = 0;
  dest_fd = open(dest, O_CREAT, 0644);

  if(-1 == dest_fd)
  {
    printf("\n Failed to create new file with error [%s]\n",strerror(errno));
    return 1;
  }

  close(dest_fd); // close destination file because its in read only mode

  dest_fd = open(dest, O_WRONLY); // open destination file in write only mode
  write(dest_fd, c, size); // writes to destination file

  close(source_fd); // closes source file
  close(dest_fd); // closes destination file
  return 0;
}
