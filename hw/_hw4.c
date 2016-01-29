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

//#define DEBUG
#define MARKERVALUE -9999 

char * opline = NULL;

//tag:arg_struct
struct arg_struct {
    int *x; // result buffer created by parent thread
    int l; // string left bound by parent
    int r; // string right bound by parent
    pthread_mutex_t *pmutex;//the mutex given to a process
    pthread_cond_t *pcond;//the mutex given to a process
    int child_num;//the number of child process from left to right
    char op;// the operation of parent thread
}; 

struct segments {
    int lb[OPLENGTH];
    int rb[OPLENGTH];
    int cap;
}; 

void addSeg(int l, int r, struct segments * pseg)
{
    pseg->lb[pseg->cap] = l;
    pseg->rb[pseg->cap] = r;
    pseg->cap++;
#ifdef DEBUG
    printf("addSeg: %d %d cap=%d\n", l, r, pseg->cap);
#endif
}

void splitInput(int l, int r, struct segments * pseg)
{
  //(* a b (..))
  int flag = l+1;
  int i = l+1;
  int bracket = 0;
  char * str = opline;

  for(; i<r-1; i++)
  { 
    if(str[i]=='(')
    {
      if(bracket==0)  flag = i;
      bracket++;
    }
    else if(str[i]==')')
    {
      bracket--;
      if(bracket==0)  
        addSeg( flag, i+1, pseg);
    }
    if(str[i]==' ' && bracket==0)
    { 
      if (i > l+1 && str[i-1] != ')')
          addSeg( flag, i, pseg);
      flag = i+1;
    }
    if(i==r-2 && str[i]!=' ' && str[i]!=')')
    {
      addSeg( flag, i+1, pseg);
    }
  }
}

//tag:operate
//operation should be done in critical section (mutual exclusive)
int operate(int *x, const int val, char op, int flag)
{
    
    //printing
    switch (op)
    {
        case '+' : 
            printf("THREAD %u: Adding '%d'\n", (unsigned int) pthread_self(), val); 
            break;
        case '-' : 
            if (flag == 0)
                printf("THREAD %u: Starting with first operand '%d'\n", (unsigned int) pthread_self(), val);
            else
                printf("THREAD %u: Subtracting '%d'\n", (unsigned int) pthread_self(), val);
            break;
        case '*' : 
            printf("THREAD %u: Multiplying by '%d'\n", (unsigned int) pthread_self(), val);
            break;
        case '/' : 
            if (flag == 0)
                printf("THREAD %u: Starting with first operand '%d'\n", (unsigned int) pthread_self(), val);
            else
                printf("THREAD %u: Dividing '%d'\n", (unsigned int) pthread_self(), val);
            break;
    } 

    if (flag == 0) //flag == 0 means this func is called by the left-most sibling 
    {
        *x = val;
        return 0;
    }

    if (op == '+')
    {
        *x += val;
        return 0;
    }
    else if (op == '-')
    {
        *x -= val;
        return 0;
    }
    else if(op == '*')
    {
        *x *= val;
        return 0;
    }
    else if(op == '/')
    {
        if (val == 0)
        {
            printf("Error divisor 0\n");
            return -1;
        }
        *x /= val;
        return 0;
    }
    //printf("Unknown operator.\n");
    return -1;
};

//tag:critical
void criticalSession(int *my_x, int my_num, char my_op, int value, pthread_mutex_t * p_my_mutex, pthread_cond_t * p_my_cond)
{
#ifdef DEBUG
        printf("MUTEX LOCK: THREAD%u : %d\n", (unsigned int) pthread_self(), *my_x); 
#endif
        if ( my_op == '*' || my_op == '+' )
        {
            operate( my_x, value, my_op, 1);
        }
        else
        {
            
            if ( my_num == 1 )
            {
                operate( my_x, value, my_op, 0);
#ifdef DEBUG
                printf("broadcast p_my_cond=%u\n", (unsigned int)p_my_cond);
#endif
                pthread_cond_broadcast(p_my_cond);
            }
            else
            {
                if ( *my_x == MARKERVALUE )
                {
#ifdef DEBUG
                    printf("*******waiting for cond=%u\n", (unsigned int)p_my_cond);
#endif
                    pthread_cond_wait(p_my_cond, p_my_mutex );
#ifdef DEBUG
                    printf("*******received cond=%u\n", (unsigned int)p_my_cond);
#endif
                    operate( my_x, value, my_op, 1);
                }
                else 
                {
#ifdef DEBUG
                    printf("*******skip wait waiting for cond=%u\n", (unsigned int)p_my_cond); 
#endif
                    operate( my_x, value, my_op, 1);
                }
            }
        }
#ifdef DEBUG
        printf("MUTEX UNLOCK: THREAD%u : %d\n", (unsigned int) pthread_self(), *my_x); 
#endif
}

void * func( void * args){
    //tag:args
    struct arg_struct *local_args = (struct arg_struct *)args;
    int *my_x = local_args->x;
    int l = local_args->l;
    int r = local_args->r;
    pthread_mutex_t * p_my_mutex = local_args->pmutex;
    pthread_cond_t * p_my_cond = local_args->pcond;
    char my_op = local_args->op;
    int my_num = local_args->child_num;
    free(local_args);


    /* dynamically allocate space to hold a return value */
    unsigned int * func_ret = (unsigned int *)malloc( sizeof( unsigned int ) );
    *func_ret = 0;//default value is 0, normal exit

    char buffer[OPLENGTH];
    strcpy(buffer, opline + l);
    buffer[r-l] = '\0';
#ifdef DEBUG
    printf("func l=%d r=%d child#%d op=%c \n", l, r, my_num, my_op);
    printf("func: %s \n", buffer);
#endif

    //tag:leaf
    int i209; 
    int flag209 = 1;
    for ( i209 = l; i209 < r; i209++) {
        if ( ! ( opline[i209] >= '0' && opline[i209] <= '9') )
        {
            flag209 = 0;
            break;
        }
    }

    if (flag209)
    {
        int leafval = atoi(buffer); 
#ifdef DEBUG
        printf("leafval=%d\n", leafval);
#endif
        pthread_mutex_lock( p_my_mutex );
        criticalSession(my_x, my_num, my_op, leafval, p_my_mutex, p_my_cond);
        pthread_mutex_unlock( p_my_mutex ); 

        pthread_exit( func_ret ); 
        return NULL;
    }
   
    //tag:split input array
    struct segments seg;
    seg.cap = 0;
    splitInput(l, r, &seg);
#ifdef DEBUG
    printf("seg.cap=%d\n", seg.cap);
#endif
    if (seg.cap < 3)
    {
        printf("THREAD %u: ERROR: not enough operands\n",
                      (unsigned int)pthread_self());
        *func_ret = 1;
        pthread_exit( func_ret ); 
        return NULL;
    }
#ifdef DEBUG
    printf("seg.lb[0]=%d, rb[0]=%d \n", seg.lb[0], seg.rb[0]);
#endif
    if (seg.lb[0] != seg.rb[0]-1 || !(opline[seg.lb[0]] == '+' || opline[seg.lb[0]] == '-' || opline[seg.lb[0]] == '*' || opline[seg.lb[0]] == '/'))
    { 
        char buffer240[OPLENGTH];
        strcpy(buffer240, opline + seg.lb[0]);
        buffer240[seg.rb[0]-seg.lb[0]] = '\0';
        printf("THREAD %u: ERROR: unknown '%s' operator\n",
                      (unsigned int)pthread_self(), buffer240);
        *func_ret = 1;
        pthread_exit( func_ret ); 
        return NULL;
    }

    //tag:create childrens
    pthread_t *child_threads = (pthread_t *)calloc(seg.cap, sizeof(pthread_t));
    pthread_mutex_t child_mutex;
    if (pthread_mutex_init(&child_mutex, NULL) != 0)
    {
        perror("\n mutex init failed\n");
        *func_ret = 1;
        pthread_exit( func_ret );
        return NULL;
    }
    pthread_cond_t child_cond;
    if (pthread_cond_init(&child_cond, NULL) != 0)
    {
        perror("\n cond init failed\n");
        *func_ret = 1;
        pthread_exit( func_ret );
        return NULL;
    }
    int * child_x = (int *)malloc( sizeof( int ) );
   
    if ( opline[seg.lb[0]] == '*' ) 
    {
        *child_x = 1;
#ifdef DEBUG
        printf("initial buffer value = %d\n", *child_x);
#endif
    }
    else if ( opline[seg.lb[0]] == '+')
    {
        *child_x = 0;
#ifdef DEBUG
        printf("initial buffer value = %d\n", *child_x);
#endif
    }
    else
    {
        *child_x = MARKERVALUE ; //marker of uninitialized
#ifdef DEBUG
        printf("initial buffer value = %d\n", *child_x); 
#endif
    }

    printf("THREAD %u: Starting '%c' operation\n",
           (unsigned int)pthread_self(), opline[seg.lb[0]]);
 
    int i105; 
    for( i105 = 1; i105 < seg.cap; i105++)
    { 
        struct arg_struct *child_args = (struct arg_struct *)malloc( sizeof(struct arg_struct)) ; 
        child_args->x = child_x; 
        child_args->l = seg.lb[i105]; 
        child_args->r = seg.rb[i105]; 
        child_args->pmutex = &child_mutex;
        child_args->child_num = i105;
        child_args->op = opline[seg.lb[0]];
        child_args->pcond = &child_cond;
 
        int rc = pthread_create( &child_threads[i105], NULL, &func, (void *)child_args );
        if ( rc != 0 )
        {
          perror("SUB: Could not create child thread\n");
          *func_ret = 1;
          pthread_exit( func_ret );
          return NULL;
        }
    }
   
    //tag:join all children
    int i122; 
    for( i122 = 1; i122 < seg.cap; i122++)
    { 
        unsigned int * child_ret;
        int rc283 = pthread_join( child_threads[i122], (void **)&child_ret );  /* BLOCK */
        if (rc283 != 0)
        {
#ifdef DEBUG
            printf("Child Thread failed\n");
#endif
            *func_ret = 1;
            pthread_exit( func_ret );
            return NULL;
        }
        if ( *child_ret != 0 )
        {
            printf( "THREAD %u: child %u terminated with nonzero exit status %d\n", (unsigned int)pthread_self(), (unsigned int)child_threads[i122], *child_ret); 
            *func_ret = 1;
            pthread_exit( func_ret );
            return NULL;
        }
#ifdef DEBUG
        printf( "SUB: Joined a child thread that returned %u.\n", *child_ret );
#endif
        free( child_ret );
    }

    free(child_threads);
    pthread_mutex_destroy(&child_mutex);
    pthread_cond_destroy(&child_cond);

    printf("THREAD %u: Ended '%c' operation with result '%d'\n",
           (unsigned int)pthread_self(), opline[seg.lb[0]], *child_x);
    if (my_op=='R')
    {
        //if it's the root node of tree
        printf("THREAD %u: Final answer is '%d'\n" ,
                (unsigned int)pthread_self(), *child_x);
    }
    else 
    {
        //tag:report to parent
        pthread_mutex_lock( p_my_mutex );
        criticalSession(my_x, my_num, my_op, *child_x, p_my_mutex, p_my_cond);
        pthread_mutex_unlock( p_my_mutex ); 
    }
    free(child_x);
  
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

#ifdef DEBUG
  printf("0123456789012345\n");
  printf("%s\n", opline);
#endif
  
  // pthread
  pthread_t main_thread;   /* keep track of the thread IDs */

  int rc;

  struct arg_struct *args = (struct arg_struct *)malloc( sizeof(struct arg_struct)) ; 
  args->x = (int *)malloc( sizeof( int ) );
  *(args->x) = -1; //marker of uninitialized
  args->l = 0; 
  args->r = strlen(opline) - 1;//the last symbol is \n 
  args->op = 'R'; // this is the root thread. It has no parent operation. 
  args->pmutex = (pthread_mutex_t *)malloc( sizeof(pthread_mutex_t) );
  args->pcond = (pthread_cond_t *)malloc( sizeof(pthread_cond_t) );
  if (pthread_mutex_init(args->pmutex, NULL) != 0)
  {
    perror("\n mutex init failed\n");
  }
  if (pthread_cond_init(args->pcond, NULL) != 0)
  {
    perror("\n cond init failed\n");
  }

  rc = pthread_create( &main_thread, NULL, &func, (void *)args );
  if ( rc != 0 )
  {
    fprintf( stderr, "MAIN: Could not create child thread (%d)\n", rc );
  }

  unsigned int * ret;
  pthread_join( main_thread, (void **)&ret );  /* BLOCK */
  pthread_mutex_destroy(args->pmutex);
  pthread_cond_destroy(args->pcond);
#ifdef DEBUG
  printf( "MAIN: Joined a child thread that returned %u.\n", *ret );
#endif
  if ( *ret != 0 )
  {
#ifdef DEBUG
      printf("Program Exit with EXIT_FAILURE.\n");
#endif
      free( ret );
      return EXIT_FAILURE;
  }
  free( ret );
  //NOTE: args already freed by the root thread, so we do not need to free it here
  return EXIT_SUCCESS;
}

/*=================Conclusion===================
1. Dynamically create buffer to pass parameters from parent thread to child threads. Child threads will free the memory for parameter structure and create it's own copies of the paremeter varables. With this pattern in mind, it's less possible to make mistakes.  

2. Synchronazation could be implements by three paradiagm: mutex, condition and join.

3. To avoid variable name conflict, you should not always use similar names in one thread. Because it's very easy that you make the same variable name in different thread and one is likely to pass one variable to the other.  
=================================================*/
