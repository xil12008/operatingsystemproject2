/* fd-open.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/** TO DO: read and display the entire file contents....

    ...add a loop to keep calling read() until the entire
    file contents have been read and displayed.

    TO DO: write the file contents to another file using
            open() and write()

    TO DO: use command-line arguments to specify file names

 **/

int main(int argc, char *argv[])
{
  char name[80];
  strcpy( name, argv[1] );

   /* name: "testfile.txt\0"                         */
   /*                    ^^                          */
   /*                 the '\0' is a single character */

  int fd = open( name, O_RDONLY );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }

  /*  fd table:

      fd
      0 stdin      fd table exists within process memory space
      1 stdout
      2 stderr
      3 testfile.txt (O_RDONLY)  */

  printf( "fd for file %s is %d\n", name, fd );

  char buffer[81];
  int rc = read( fd, buffer, 80 );
  printf( "read() returned %d\n", rc );

  if ( rc == -1 )
  {
    perror( "read() failed" );
    return EXIT_FAILURE;
  }

  /* assuming that we're reading text data ... */
  buffer[rc] = '\0';
  printf( "read: %s\n", buffer );

  close( fd );  /* remove entry 3 from the fd table */

  return EXIT_SUCCESS;
}

