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

// guard_state with a value of k:
//          k < 0 : means guard is waiting in the room
//          k = 0 : means guard is in the hall of department
//          k > 0 : means guard is IN the room
int guard_state;         // waiting, in the hall, or in the room
int num_students;        // number of students in the room


// will malloc space for seeds[] in the main
unsigned int *seeds;     // rand seeds for guard and students generating delays

// NOTE:  globals below are initialized by command line args and never changed !
int capacity;       // maximum number of students in a room
int num_checks;     // number of times agent places supplies on table

// *******************************************************************************
// Code to be completed by you. See TODO comments.
// *******************************************************************************

// TODO:  list here the "handful" of semaphores you will need to synchronize
//        I've listed one you will need for sure, to "get you going"
binary_semaphore table;  // to protect the table
binary_semaphore papers; //semaphore for person who only has papers
binary_semaphore tobacco; //semaphore for person who only has tobacco
binary_semaphore lighter; //semaphore for person who only has lighter

//this function contains the logic for the agent
inline void agent()
{
  int selected_package;
  //aquire semaphore for table so no one else can
  //semWaitB(&table);
  //get a random number 1-3, this relates to what items agent places on table
  selected_package = (rand() % 3) + 1;
  if(selected_package == TOBACCO_PAPERS){
    printf("The agent is placing tobacco and rolling papers on the table \n");
    semSignalB(&lighter);
  }else if(selected_package == LIGHTER_TOBACCO){
    printf("The agent is placing tobacco and a lighter on the table \n");
    semSignalB(&papers);
  }else{
    printf("The agent is placing a lighter and rolling papers on the table \n");
    semSignalB(&tobacco);
  }
  //wait on table to be released by the smoker once they finish smoking
  semWaitB(&table);
  printf("The agent has finished waiting for the smoker to smoke \n");
  printf("The agent will go outside for a while and then return \n");
  
}


// this function contains the main synchronization logic for the guard
/*
inline void guard_check_room()
{
  semWaitB(&mutex);
  //printf("Num Studends: %d\n", num_students);
  if(num_students == 0){
    assess_security();

  //capacity of room meet/exceeded. kick students out
  }else if(num_students > 0 && num_students < capacity){
    guard_state = -1;
    printf("\tguard waiting to enter room with %d students\n", num_students);
    //don't proceed until room at capacity or empty
    semSignalB(&mutex);
    semWaitB(&guard_wait_mutex);
    semWaitB(&guard_wait_mutex);
    semSignalB(&guard_wait_mutex);

    //semWaitB(&num_students_mutex);
    printf("\tguard done waiting to enter room with %d students\n", num_students);
    if(num_students >= capacity){
      guard_state = 1;
      //own this mutex so no more students can enter room
      semWaitB(&student_enter_room);
      printf("\tguard clearing out room with %d students...\n", num_students);
      printf("\tguard waiting for students to clear out with %d students...\n", num_students);
      semSignalB(&mutex);
      //pauses on second wait until a signal is sent from studens in room
      semWaitB(&student_leave_room);
      semWaitB(&student_leave_room);
      semSignalB(&student_leave_room);
      printf("\tguard done clearing out room\n");

      semSignalB(&student_enter_room);

    }else if(num_students == 0){
      assess_security();
    }
  }else if(num_students >= capacity){
    guard_state = 1;
    semWaitB(&student_enter_room);
    printf("\tguard clearing out room with %d students...\n", num_students);
    printf("\tguard waiting for students to clear out with %d students...\n", num_students);
    semSignalB(&mutex);
    //need to stop from going, so wait on guard_wait_mutex twice, since
    //first time the flag is set to 1, need it to be set to 0.
    //first signal comes from students when 0 in room.
    semWaitB(&student_leave_room);
    semWaitB(&student_leave_room);
    semSignalB(&student_leave_room);
    printf("\tguard done clearing out room\n");
    semSignalB(&student_enter_room);
}
  guard_state = 0;
  printf("\tguard left room\n");
  semSignalB(&mutex);

}
*/

// this function contains the main synchronization logic for a student
/*
inline void student_study_in_room(long id)
{
  semWaitB(&mutex);
  if( guard_state > 0){
    semSignalB(&mutex);
    //prevent students from entering (guard owns mutex)
    semWaitB(&student_enter_room);
    semSignalB(&student_enter_room);
    semWaitB(&mutex);
  }
  //guard not in room, student enters room
  if(guard_state != 1){
    if(guard_state < 0 && num_students == capacity -1){
        printf("Last student %2ld entering room with guard waiting\n", id);
    }
    num_students++;
    semSignalB(&mutex);
    study(id);
    semWaitB(&mutex);

  }
  if( guard_state < 0 && num_students>= capacity){
    semSignalB(&guard_wait_mutex);
  }else{
    semSignalB(&mutex);
  }

  //student leaves room now
  semWaitB(&mutex);
  num_students--;
  if(num_students > 0){
    printf("student %2ld left room\n", id);
  }else if(num_students == 0 && guard_state > 0){
    printf("LAST student %2ld left room with guard in it\n", id);
  }else{
    printf("student %2ld left room\n", id);
  }

  //guard waiting and room empty, signal guard to enter room
  if(num_students == 0 && guard_state < 0){
    semSignalB(&guard_wait_mutex);
  //guard in room and empty
  }else if(num_students == 0 && guard_state > 0){
    //signals to guard that room is now empty and guard can proceed
    semSignalB(&student_leave_room);
  }else{
    semSignalB(&mutex);
  }

}
*/


// *******************************************************************************
// Code that DOES NOT need to be modified
// *******************************************************************************

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
{ // details of this function are unimportant for the assignment
  // using reentrante version of rand() function (because multithreaded)
  // NOTE: however, overall behavior of code will still be non-deterministic
  return min + rand_r(seedptr) % (max - min + 1);
}

inline void study(long id)  // student studies for some random time
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  printf("student %2ld studying in room with %2d students for %3d millisecs\n",
	 id, num_students, ms);
  millisleep(ms);
}

inline void do_something_else(long id)    // student does something else
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  millisleep(ms);
}

inline void assess_security()  // guard assess room security
{ // details of this function are unimportant for the assignment
  // NOTE:  we have (own) the mutex when we first enter this routine
  guard_state = 1;     // positive means in the room
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard assessing room security for %3d millisecs...\n", ms);
  millisleep(ms);
  printf("\tguard done assessing room security\n");
}

inline void guard_walk_hallway()  // guard walks the hallway
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard walking the hallway for %3d millisecs...\n", ms);
  millisleep(ms);
}

// guard thread function  --- NO need to change this function !
void* guard(void* arg)
{
  int i;            // loop control variable
  srand(seeds[0]);  // seed the guard thread random number generator

  // the guard repeatedly checks the room (limited to num_checks) and
  // walks the hallway
  for (i = 0; i < num_checks; i++) {
    guard_check_room();
    guard_walk_hallway();
  }

  pthread_exit((void*)0);   // thread needs to return a void*
}

// student thread function --- NO need to change this function !
void* student(void* arg)
{
  long id = (long) arg;  // determine thread id from arg
  srand(seeds[id]);      // seed this threads random number generator

  // repeatedly study and do something else
  while (1) {
    student_study_in_room(id);
    do_something_else(id);
  }

  pthread_exit((void*)0);   // thread needs to return a void*
}


// *******************************************************************************
// Code to be completed by you. See TODO comments.
// *******************************************************************************

int main(int argc, char** argv)  // the main function
{
  int n = 0;               // number of student threads
  pthread_t  cthread;      // guard thread
  pthread_t* sthreads;     // student threads
  long i;                  // loop control variable

  if (argc < 4) {
    fprintf(stderr, "USAGE: %s num_threads capacity num_checks\n", argv[0]);
    return 0;
  }

  // TODO: get three input parameters, convert, and properly store
  // HINT: use atoi() function (see man page for more details).
  n = atoi(argv[1]);
  capacity = atoi(argv[2]);
  num_checks = atoi(argv[3]);

  // allocate space for the seeds[] array
  // NOTE: seeds[0] is guard seed, seeds[k] is the seed for student k
  // allocate space for the student threads array, sthreads

  seeds = (unsigned int*) malloc((n + 1)*sizeof(unsigned int));
  sthreads = (pthread_t*) malloc(n*sizeof(pthread_t));

  // Initialize global variables and semaphores
  guard_state = 0;   // not in room (walking the hall)
  num_students = 0;  // number of students in the room

  semInitB(&mutex, 1);  // initialize mutex
  // TODO: for all your binary semaphores, complete the semaphore initializations
  semInitB(&student_enter_room, 1);
  semInitB(&student_leave_room, 1);
  semInitB(&guard_wait_mutex, 1);

  // initialize guard seed and create the guard thread
  seeds[0] = START_SEED;
  pthread_create(&cthread, NULL, guard, (void*) NULL);

  for (i = 1; i <= n; i++) {
    seeds[i] = START_SEED + i;
    pthread_create(&sthreads[i-1], NULL, student, (void*) i);
  }

  pthread_join(cthread, NULL);   // wait for guard thread to complete

  for (i = 0; i < n; i++) {
    pthread_kill(sthreads[i],0); // stop all threads (guard and students)
  }

  free(seeds);
  free(sthreads);

  return 0;
}
