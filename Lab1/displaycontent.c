// Question 2
// Creates displays content of a textfile
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

  char* filepath = argv[1];
  int fd;

  // opens the file as read only
  errno = 0;
  fd = open(filepath ,O_RDONLY);

  if(-1 == fd)
  {
    if (errno == ENOENT)
      printf ("%s does not exist\n", filepath);
    else if (errno == EACCES)
      printf ("%s is not readable\n", filepath);
    else
      printf("\n open() failed with error [%s]\n",strerror(errno));
    return 1;
  }
  else
  {
    // Reads file
    int size = lseek(fd, 0, SEEK_END) - 1 ; // counts bytes in file
    lseek(fd, 0, SEEK_SET); // sets location back to start of the file
    char c[size];

    read(fd, c, size); // reads file to c with size.
    printf("%s", c);
  }

  close(fd); // closes file
  return 0;
}
