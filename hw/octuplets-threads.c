/* octuplets-threads.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* use -lpthread or -pthread on the gcc line */
/* bash$ gcc -Wall octuplets-threads.c -pthread */
#include <pthread.h>

#define CHILDREN 8

/* this function is called by each thread */
void * whattodo( void * arg );

int main()
{
  pthread_t tid[ CHILDREN ];   /* keep track of the thread IDs */
  int i, rc;

  int * t;

  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    t = (int *)malloc( sizeof( int ) );
    *t = 2 + i * 2;  /* 2, 4, 6, 8, 10 */

    printf( "MAIN: Next thread will nap for %d seconds.\n", *t );

    rc = pthread_create( &tid[i], NULL, whattodo, t );

    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: Could not create child thread (%d)\n", rc );
    }
  }

  /* wait for threads to terminate */
  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    unsigned int * x;
    pthread_join( tid[i], (void **)&x );  /* BLOCK */
    printf( "MAIN: Joined a child thread that returned %u.\n", *x );
    free( x );
  }

  printf( "MAIN: All threads successfully joined.\n" );

  return 0;
}


void * whattodo( void * arg )
{
/*  sleep( 1 );  */
  int t = *(int *)arg;
  free( arg );

  printf( "THREAD %u: I'm gonna nap for %d seconds.\n",
          (unsigned int)pthread_self(), t );
  sleep( t );
  printf( "THREAD %u: I'm awake now.\n",
          (unsigned int)pthread_self() );

  /* dynamically allocate space to hold a return value */
  unsigned int * x = (unsigned int *)malloc( sizeof( unsigned int ) );

  /* return value is the thread ID */
  *x = pthread_self();
  pthread_exit( x ); /* terminate the thread, returning
                          x to a pthread_join() call */
  return NULL;
}





