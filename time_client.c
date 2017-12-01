#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

int   shared_mem_id;
int   *shared_mem_ptr;

pthread_mutexattr_t mutex_shared_attr;
pthread_mutex_t	*mptr;

time_t rawtime;
int main(void)
{
	pid_t  child_pid;
	int  status, rtn;
	int shmid;
	key_t key;
	time_t *shm, *s;

	key = 9999;

	/*
	 * Locate the segment.
	 */
	if ((shmid = shmget(key, sizeof(time_t), 0666)) < 0) {
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


	/* initialize shared memory segment */
	if ((shared_mem_id = shmget(5678, 1*sizeof(pthread_mutex_t), 0660)) < 0)
		perror("shmget"), exit(1) ;

	if ((shared_mem_ptr = (int *)shmat(shared_mem_id, (void *)0, 0)) == NULL)
		perror("shmat"), exit(1);

	mptr = (pthread_mutex_t *)shared_mem_ptr;



	/*
	 * Now read what the server put in the memory.
	 */
    int i =0;
    struct tm * timeinfo;
	for (i = 0; i <= 10; i++)
	{
        timeinfo = localtime (shm);
		printf("====CLIENT=====waiting for lock\n");
		if ((rtn = pthread_mutex_lock(mptr)) != 0)
			fprintf(stderr,"child:pthread_mutex_lock %s",strerror(rtn)),exit(1);
        printf ( "Current local time and date: %s", asctime (timeinfo) );
		printf("\n====CLIENT=====got lock sleeping 2\n");
		sleep(2);
		if ((rtn = pthread_mutex_unlock(mptr)) != 0)
			fprintf(stderr,"child:pthread_unmutex_lock %s",strerror(rtn)),exit(1);
	}
	*shm = '*';
	printf("====CLIENT=====exit\n");

	return 0;
}
