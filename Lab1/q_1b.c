// Question 1b
// Creates newfile user defined and opens it
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
  if(2 != argc)
  {
    printf("\n Usage : Needs argument specifying a filename\n");
    return 1;
  }

  int fd, newFile;
  char* filepath = argv[1];

  // creates new file with O_CREAT flag with 0644 permissions
  errno = 0;
  newFile = open(filepath, O_CREAT, 0644);

  if(-1 == newFile)
  {
    printf("\n Failed to create new file with error [%s]\n",strerror(errno));
    return 1;
  }
  else
    printf("\n Successful created new file: %s\n", filepath);

  // opens the newly created file as read only
  fd = open(filepath,O_RDONLY);

  if(-1 == fd)
  {
    printf("\n open() failed with error [%s]\n",strerror(errno));
    return 1;
  }
  else
    printf("\n Open() Successful\n");

  close(fd);
  return 0;
}
