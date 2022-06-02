#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

int createFile (char* filepath) {
  // creates new file with O_CREAT flag with 0644 permissions
  int newFile, errno;

  errno = 0;
  newFile = open(filepath, O_CREAT, 0777);

  if(-1 == newFile)
  {
    printf("\n Failed to create new file with error [%s]\n",strerror(errno));
    return 1;
  }
  else
    printf("\n Successful created new file: %s\n", filepath);
  return 0;
}

int main() {
  // if statement checks to make sure no errors occured
  if (createFile("destination1.txt") == 1)
    return 1;
  if (createFile("destination2.txt") == 1)
    return 1;
  return 0;
}
