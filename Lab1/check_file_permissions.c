// Question 1a
// Updated file
// checks if file exists
// then checks if it is readable and writeable
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main (int argc, char* argv[])
{
  char* filepath = argv[1];
  int returnval;
  int isReadable, isWriteable;

  // Check file existence
  returnval = access (filepath, F_OK);
  if (returnval == 0)
    printf ("\n %s exists\n", filepath);
  else
  {
    if (errno == ENOENT)
      printf ("%s does not exist\n", filepath);
    else if (errno == EACCES)
      printf ("%s is not accessible\n", filepath);
    return 0;
  }

  // Check read access ...
  isReadable = access (filepath, R_OK);
  if (isReadable == 0)
    printf ("\n %s is readable\n", filepath);
  else
    printf ("\n %s is not readable\n", filepath);

  // Check write access ...
  isWriteable = access (filepath, W_OK);
  if (isWriteable == 0)
    printf ("\n %s is writeable\n", filepath);
  else
    printf ("\n %s is not writeable\n", filepath);

  return 0;
}
