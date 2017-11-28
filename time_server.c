/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * process_shared_mutex.c -- 
 *
 * Demonstrating the shared process mutex
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#define SHMSZ     27



#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

int   shared_mem_id;
int   *shared_mem_ptr;

pthread_mutexattr_t mutex_shared_attr;
pthread_mutex_t	*mptr;

time_t rawtime;
	extern int
main(void)
{
	pid_t  child_pid;
	int  status, rtn;

	char c;
	int shmid;
	key_t key;
	time_t *shm, *s;
	key = 9999;

	/* initialize shared memory segment */
	if ((shared_mem_id = shmget(5678, 1*sizeof(pthread_mutex_t), IPC_CREAT |0666)) < 0)
		perror("shmget"), exit(1) ;

	if ((shared_mem_ptr = (int *)shmat(shared_mem_id, (void *)0, 0)) == NULL)
		perror("shmat"), exit(1);

	mptr = (pthread_mutex_t *)shared_mem_ptr;

	if ((shmid = shmget(key, sizeof(rawtime), IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	/*
	 * Now we attach the segment to our data space.
	 */
	if ((shm = shmat(shmid, NULL, 0)) == (time_t *) -1) {
		perror("shmat");
		exit(1);
	}


	s = shm;
	if (( rtn = pthread_mutexattr_init(&mutex_shared_attr)) != 0) 
		fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);

	if (( rtn = pthread_mutexattr_setpshared(&mutex_shared_attr,PTHREAD_PROCESS_SHARED)) != 0)
		fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);

	if (( rtn = pthread_mutex_init(mptr, &mutex_shared_attr)) != 0)
		fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);

    int i = 0;
    struct tm * timeinfo;
	for (i = 0; i <= 10; i++)
	{
        //time ( &rawtime );
        *s = time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        printf ( "Current local time and date: %s", asctime (timeinfo) );
		if ((rtn = pthread_mutex_lock(mptr)) != 0)
			fprintf(stderr,"parent:pthread_mutex_lock %d",strerror(rtn)),exit(1);
		printf("====PARENT=====Sleeping 5 sec with lock \n");
		/* parent */ 
		sleep(2); 
		if ((rtn = pthread_mutex_unlock(mptr)) != 0)
			fprintf(stderr,"child:pthread_unmutex_lock %d",strerror(rtn)),exit(1);
		sleep(1); 
		printf("====PARENT=====Sleeping 2 sec before taking lock\n");
	}

	/*
	 * Finally, we wait until the other process 
	 * changes the first character of our memory
	 * to '*', indicating that it has read what 
	 * we put there.
	 */
	return 0;
}
