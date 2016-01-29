/* fork-with-loop.c */

/* create a child process */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{ 
  if(argc != 3){
    printf("ERROR: Invalid arguments\nUSAGE: ./a.out <input-file> <chunk-size>\n");
    return EXIT_FAILURE;
  }
  int chunkSize = atoi(argv[2]);
  if(chunkSize <=0){
    printf("ERROR: Invalid arguments\nUSAGE: ./a.out <input-file> <chunk-size>\n");
    return EXIT_FAILURE;
  }
   
  struct stat buf;
  char name[80];
  strcpy( name, argv[1]);
  int rc = lstat( name , &buf );
  if ( rc == -1 )
  {
    perror( "lstat() failed" );
    return EXIT_FAILURE;
  }
  
  int numChildren = (int)( ((int)buf.st_size + chunkSize - 1) / chunkSize);

  printf("PARENT: File 'hw2-sample-input.txt' contains %d bytes\nPARENT: ... and will be processed via %d child processes\n", (int)buf.st_size, numChildren);
  fflush(NULL);

  int fd = open( name, O_RDONLY );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }

  pid_t pid;     /* process id (pid)  unsigned int/short */

  int i = numChildren;

while ( i > 0 )
{
  i--;

  pid = fork();  /* create a child process */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }
  if ( pid == 0 )  /* child process fork() returns 0 */
  {
    char *buffer = calloc(chunkSize + 1, sizeof(char)); // one char is one byte
    int rc = read( fd, buffer, chunkSize);
    if ( rc == -1 )
    {
      perror( "read() failed" );
      return EXIT_FAILURE;
    }
    buffer[rc] = '\0';
    printf( "CHILD %d CHUNK: %s\n", getpid(), buffer);
    free(buffer);
    return EXIT_SUCCESS;
  }
  else 
  {
    int status;
    pid_t child_pid = wait( &status );

    if ( WIFSIGNALED( status ) )   /* core dump or kill or kill -9 */
    {
      printf( "PARENT: child %d terminated ", (int)child_pid );
      printf( "abnormally\n" );
    }
    else if ( WIFEXITED( status ) ) /* child called return or exit() */
    {
      int rc = WEXITSTATUS( status );
      if(rc != 0){
        printf("PARENT: child %d terminated with nonzero exit status %d\n", (int)child_pid, rc );
      }
    }
  }
}
  close( fd );  /* remove entry 3 from the fd table */
  return EXIT_SUCCESS;
}
