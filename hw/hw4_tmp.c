/* octuplets-threads.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>

/* use -lpthread or -pthread on the gcc line */
/* bash$ gcc -Wall octuplets-threads.c -pthread */
#include <pthread.h>

#define OPLENGTH 100

#define DEBUG

char * opline = NULL;

struct arg_struct {
    int x; // result buffer created by parent thread
    int l; // string left bound by parent
    int r; // string right bound by parent
}; 

struct segments {
    int lb[OPLENGTH];
    int rb[OPLENGTH];
    int cap;
}; 

void splitInput(int l, int r, struct segments * pseg)
{
  int flag = l+1;
  int i = l+1;
  int bracket = 0;
  char * str = opline;

  for(; i<r; i++)
  { 
    if((str[i]==' '||str[i]==')')&& bracket==0)
    { 
      //buffer = calloc(i-flag+1, sizeof(char));
      //strncpy(buffer, str+flag, i-flag); 
      pseg->lb[pseg->cap] = l+flag;
      pseg->rb[pseg->cap] = l+i;
      pseg->cap++;
#ifdef DEBUG
      printf("split: %d %d\n", l+flag, l+i);
#endif
    }
    else if(str[i]=='('){
      bracket++;
    }
    else if(str[i]==')'){
      bracket--;
    }
    if(str[i]==' ' && bracket==0){
      flag = i+1;
    }
  }
}

void * func( void * args){
    struct arg_struct *local_args = (struct arg_struct *)args;
    int x = local_args->x;
    int l = local_args->l;
    int r = local_args->r;
    free(local_args);
    unsigned int * ret;
    struct arg_struct * child_args;
    pthread_t child_thread;
    int rc;
    unsigned int * child_ret;

    //distinguish if I am the leaf on the tree 
    if(l==-1)
    {
        ret = (unsigned int *)malloc( sizeof( unsigned int ) );
        *ret = 0;
        pthread_exit( ret ); 
        return NULL;
    }
   
    //create childrens
    child_args = (struct arg_struct *)malloc( sizeof(struct arg_struct)) ; 
    child_args->x = 99; 
    child_args->l = -1; 
    child_args->r = - 1; 
 
    rc = pthread_create( &child_thread, NULL, &func, (void *)child_args );
    if ( rc != 0 )
    {
      fprintf( stderr, "SUB: Could not create child thread (%d)\n", rc );
    }
   
    //join all children
    pthread_join( child_thread, (void **)&child_ret );  /* BLOCK */
    printf( "SUB: Joined a child thread that returned %u.\n", *child_ret );
    free( child_ret );
  
    /* dynamically allocate space to hold a return value */
    unsigned int * func_ret = (unsigned int *)malloc( sizeof( unsigned int ) );
    *func_ret = 0;
    /* return value is the thread ID */
    pthread_exit( func_ret ); /* terminate the thread, returning
                            x to a pthread_join() call */
    return NULL;
}

int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("ERROR: Invalid arguments\nUSAGE: ./a.out <input-file>\n");
    return EXIT_FAILURE;
  }
   
  char name[80];
  strcpy( name, argv[1]);
  int fd = open( name, O_RDONLY );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(name, "r");
  if (fp == NULL)
  {
    perror( "read file failed" );
    return EXIT_FAILURE;
  }
  while ((read = getline(&line, &len, fp)) != -1)
  {
    if(line != NULL  && read >= 1)
    {
      if(line[0]!='#')
      {
        opline = (char *)calloc(len + 1, sizeof(char)); 
	strcpy(opline, line);
      }
    }
  }
  fclose(fp);

  printf("0123456789\n");
  printf("%s\n", opline);
  
  // pthread
  pthread_t main_thread;   /* keep track of the thread IDs */

  int rc;

  struct arg_struct *args = (struct arg_struct *)malloc( sizeof(struct arg_struct)) ; 
  args->x = 99; 
  args->l = 0; 
  args->r = strlen(opline) - 1; 

  rc = pthread_create( &main_thread, NULL, &func, (void *)args );
  if ( rc != 0 )
  {
    fprintf( stderr, "MAIN: Could not create child thread (%d)\n", rc );
  }

  unsigned int * ret;
  pthread_join( main_thread, (void **)&ret );  /* BLOCK */
  printf( "MAIN: Joined a child thread that returned %u.\n", *ret );
  free( ret );
  return EXIT_SUCCESS;
}
