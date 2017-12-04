/*
CSCI 340 - Operating Systems
Fall 2017
340 Final Project
Matthew Schwarz & Ricky Ramos


NOTE: you will get a warning about unused variable, sthreads, but
this will go away, as you complete the code.
*/

#include <stdio.h>
#include <stdlib.h>  // for exit(), rand(), strtol()
#include <pthread.h>
#include <time.h>    // for nanosleep()
#include <errno.h>   // for EINTR error check in millisleep()
#include <signal.h>  // for pthread_kill()

#include "binary_semaphore.h"

// you can adjust next two values to speedup/slowdown the simulation
#define MIN_SLEEP      20   // minimum sleep time in milliseconds
#define MAX_SLEEP     100   // maximum sleep time in milliseconds

#define START_SEED     11   // arbitrary value to seed random number generator
//values for the items agent places on table
#define TOBACCO_PAPERS 1
#define LIGHTER_TOBACCO 2
#define LIGHTER_PAPERS 3


int agent_ready;         // 0 if not in room, 1 if in room

// will malloc space for seeds[] in the main
unsigned int *seeds;     // rand seeds for guard and students generating delays

// NOTE:  globals below are initialized by command line args and never changed !
int amount_of_stuff;     // number of times agent places supplies on table

binary_semaphore table;  // to protect the table
binary_semaphore papers; //semaphore for person who only has papers
binary_semaphore tobacco; //semaphore for person who only has tobacco
binary_semaphore lighter; //semaphore for person who only has lighter
binary_semaphore smoking; //semaphore for when a person is smoking currently

//this function contains the logic for the agent
inline void agent_place_table()
{
  int selected_package;
  //get a random number 1-3, this relates to what items agent places on table
  selected_package = rand_range(&seeds[0], TOBACCO_PAPERS, LIGHTER_PAPERS);
  if(selected_package == TOBACCO_PAPERS){
    agent_ready = 1;
    printf("The agent is placing tobacco and rolling papers on the table \n");
    semSignalB(&lighter);
  }else if(selected_package == LIGHTER_TOBACCO){
    agent_ready = 1;
    printf("The agent is placing tobacco and a lighter on the table \n");
    semSignalB(&papers);
  }else{
    agent_ready = 1;
    printf("The agent is placing a lighter and rolling papers on the table \n");
    semSignalB(&tobacco);
  }
  //wait on table to be released by the smoker once they finish smoking
  semWaitB(&table);
  agent_ready = 0;
  printf("The agent will change the cigarette materials he has \n");
}

inline void smoker_smoke(long id)
{
  //aquire semaphore for the item that the smoker needs
  //then once signaled by the agent, the smoker will have all items needed
  while(agent_ready != 1){
    //do nothing, keep looping
  }
  if(id == TOBACCO_PAPERS){
    semWaitB(&lighter);
    printf("\tThe smoker with a lighter has acquired tobacco and rolling papers \n");
  }else if(id == LIGHTER_TOBACCO){
    semWaitB(&papers);
    printf("\tThe smoker with rolling papers has acquired tobacco and a lighter \n");
  }else{
    semWaitB(&tobacco);
    printf("\tThe smoker with tobacco has acquired a lighter and rolling papers \n");
  }
  printf("\tThe smoker has all the needed materials\n");
  printf("\tThe smoker is now smoking for a bit \n" );
  //smoke and then release the table semaphore which frees up the agent
  smoke(id);
  semSignalB(&table);
}


inline void millisleep(long millisecs)   // delay for "millisecs" milliseconds
{ // details of this function are unimportant for the assignment
  struct timespec req;
  req.tv_sec  = millisecs / 1000;
  millisecs -= req.tv_sec * 1000;
  req.tv_nsec = millisecs * 1000000;
  while(nanosleep(&req, &req) == -1 && errno == EINTR)
    ;
}

// generate random int in range [min, max]
inline int rand_range(unsigned int *seedptr, long min, long max)
{
  // using reentrante version of rand() function (because multithreaded)
  // NOTE: however, overall behavior of code will still be non-deterministic
  return min + rand_r(seedptr) % (max - min + 1);
}

inline void smoke(long id)  // smoker smokes for a period of time
{
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  printf("\t\tsmoker %2ld smoking at table for %3d millisecs\n",
	 id, ms);
  millisleep(ms);
}

inline void do_something_else(long id)    // smoker does something else
{
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  millisleep(ms);
}
/*
inline void agent_go_outside()  // agent goes outside
{
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tagent goes outside for %3d millisecs...\n", ms);
  millisleep(ms);
}
*/

void* agent(void* arg)
{
  int i;            // loop control variable
  srand(seeds[0]);  // seed the guard thread random number generator

  for (i = 0; i < amount_of_stuff; i++) {
    agent_place_table();
    //agent_go_outside();
  }

  pthread_exit((void*)0);   // thread needs to return a void*
}

void* smoker(void* arg)
{
  long id = (long) arg;  // determine thread id from arg
  srand(seeds[id]);      // seed this threads random number generator

  // repeatedly wait to smoke and do something else
  while (1) {
    smoker_smoke(id);
    do_something_else(id);
  }

  pthread_exit((void*)0);   // thread needs to return a void*
}


// *******************************************************************************
// Code to be completed by you. See TODO comments.
// *******************************************************************************

int main(int argc, char** argv)  // the main function
{
  int n;               // number of smoker threads
  pthread_t  athread;      // agent thread
  pthread_t* sthreads;     // smoker threads
  long i;                  // loop control variable

  printf("Enter the amount of times the agent will supply the table with cigarette matterials: \n");
  scanf("%d", &amount_of_stuff);
  //only 3 smokers
  n = 3;

  // allocate space for the seeds[] array
  seeds = (unsigned int*) malloc((n + 1)*sizeof(unsigned int));
  sthreads = (pthread_t*) malloc(n*sizeof(pthread_t));

  // Initialize global variables and semaphores
  agent_ready = 0;   // not in room (outside)

  //initialize semaphores to 0, so that they all function as blocks when waiting
  semInitB(&table, 0);
  semInitB(&papers, 0);
  semInitB(&tobacco, 0);
  semInitB(&lighter, 0);
  semInitB(&smoking, 1);

  //create the smoker threads
  for (i = 1; i <= n; i++) {
    seeds[i] = START_SEED + i;
    pthread_create(&sthreads[i-1], NULL, smoker, (void*) i);
  }
  // initialize agent seed and create the agent thread
  seeds[0] = START_SEED;
  pthread_create(&athread, NULL, agent, (void*) NULL);

  pthread_join(athread, NULL);   // wait for guard thread to complete

  for (i = 0; i < n; i++) {
    pthread_kill(sthreads[i],0); // stop all threads (guard and students)
  }

  free(seeds);
  free(sthreads);

  return 0;
}
