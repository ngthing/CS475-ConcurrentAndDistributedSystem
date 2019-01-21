// Thi Nguyen
// CS475 A2 Part B
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BR_CAP 5
#define CROSS_TIME 4
#define DIREC_PROB 0.7

#define handle_err(s) do{perror(s); exit(EXIT_FAILURE);}while(0)

typedef struct _thread_argv
{
	int vid;
	int direc;
	int time_to_cross;
} thread_argv;

/**
 * Student may add necessary variables to the struct
 **/
typedef struct _bridge {
	int dept_idx;
	int num_car;
	int curr_dir;
} bridge_t;

void bridge_init();
void bridge_destroy();
void dispatch(int n);
void *OneVehicle(void *argv);
void ArriveBridge(int vid, int direc);
void CrossBridge(int vid, int direc, int time_to_cross);
void ExitBridge(int vid, int direc);

pthread_t *threads = NULL;	/* Array to hold thread structs */
thread_argv *args = NULL;	/* Array to hold thread arguments */
int num_v = 30;			/* Total number of vehicles to be created */

bridge_t br;			/* Bridge struct shared by the vehicle threads*/

/* Declare custom share variables, condition variables and mutex*/
int waiters[2]; 	/* Number of cars waiting to go in each East and West direction */
int eastQueue[30]; /* FIFO queue that keeps the waiting cars want to go east */
int headEQ = 0;  /* the index of head of east queue */
int tailEQ = 0; 	/* the index of tail of east queue */
int westQueue[30]; /* FIFO queue that keeps the waiting cars want to go west */
int headWQ = 0;  /* the index of head of west queue */
int tailWQ = 0; 	/* the index of tail of west queue */
int numHaveCrossed = 0;  /* number Of Vehicles Have Crossed In Cur Direc */
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitingToGoEast = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitingToGoWest = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
	int sched_opt;
	int i;

	if(argc < 2)
	{
		printf("Usage: %s SCHED_OPT [SEED]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	/* Process Arguments */
	sched_opt = atoi(argv[1]);
	if(argc == 3)
		srand((unsigned int)atoi(argv[2]));
	else
		srand((unsigned int)time(NULL));

	/* Allocate memory for thread structs and arguments */
	if((threads = (pthread_t *)malloc(sizeof(pthread_t) * num_v)) == NULL)
		handle_err("malloc() Failed for threads");
	if((args = (thread_argv *)malloc(sizeof(thread_argv) * num_v)) == NULL)
		handle_err("malloc() Failed for args");

	/* Init bridge struct */
	bridge_init();

	/* Create vehicle threads */
	switch(sched_opt)
	{
		case 1 : dispatch(5); break;
		case 2 : dispatch(10); break;
		case 3 : dispatch(30); break;
		default:
			fprintf(stderr, "Bad Schedule Option %d\n", sched_opt);
			exit(EXIT_FAILURE);
	}
	
	/* Join all the threads */
	for(i = 0; i < num_v; i++)
		pthread_join(threads[i], NULL);

	/* Clean up and exit */
	bridge_destroy();

	exit(EXIT_SUCCESS);
}

/**
 *	Create n vehicle threads for every 10 seconds until the total
 * 	number of vehicles reaches num_v
 *	Each thread handle is stored in the shared array - threads
 */
void dispatch(int n)
{
  int k, i;
  
	for(k = 0; k < num_v; k += n)
	{
		printf("Dispatching %d vehicles\n", n);

		for( i = k; i < k + n && i < num_v; i++)
		{
			/* The probability of direction 0 is DIREC_PROB */
			int direc = rand() % 1000 > DIREC_PROB * 1000 ? 0 : 1;

			args[i] = (thread_argv){i, direc, CROSS_TIME};
			if(pthread_create(threads + i, NULL, &OneVehicle, args + i) != 0)
				handle_err("pthread_create Failed");
		}
		
		printf("Sleep 10 seconds\n"); sleep(10);
	}
}

void *OneVehicle(void *argv)
{
	thread_argv *args = (thread_argv *)argv;

	ArriveBridge(args->vid, args->direc);
	CrossBridge(args->vid, args->direc, args->time_to_cross);
	ExitBridge(args->vid, args->direc);

	pthread_exit(0);
}

/**
 *	Students to complete the following functions
 */

void bridge_init()
{
	br.dept_idx = 0;
	br.curr_dir = 0;
	br.num_car = 0;

	return;
}

void bridge_destroy()
{
	return;
}

void ArriveBridge(int vid, int direc)
{
	pthread_mutex_lock(&lock);
	fprintf(stderr, "vid=%d dir=%d arrives at the bridge. Bridge num_car=%d curr_dir=%d\n", 
		vid, direc, br.num_car, br.curr_dir);
	/* While cannot get on the  bridge, wait */
	while ((br.num_car == 6) || (br.curr_dir != direc && numHaveCrossed < 6) || 
			(br.curr_dir == direc && numHaveCrossed == 6 && waiters[1-direc])) {
		waiters[direc]++;
		if (direc == 0) { 
			eastQueue[tailEQ] = vid;
			tailEQ++;
			pthread_cond_wait(&waitingToGoEast,&lock); 
			
			if (br.curr_dir != direc) {
				while (numHaveCrossed != 6) {
					pthread_cond_wait(&waitingToGoEast,&lock); 
				}
				numHaveCrossed = 0; // Once the bridge switches direction, set the numHaveCrossed to 0
			}
			/* Once the thread receives signal to wake up to cross the bridge, 
				It still has to wait till its turn. So if there is any vehicle 
				before it its queue, it has to wait
			 */
			while (eastQueue[headEQ] != vid) 
				pthread_cond_wait(&waitingToGoEast,&lock); 
			headEQ++;

		}
		else { 
			westQueue[tailWQ] = vid;
			tailWQ++;
			pthread_cond_wait(&waitingToGoWest,&lock); 
	
			if (br.curr_dir != direc) {
				while (numHaveCrossed != 6) {
					pthread_cond_wait(&waitingToGoWest,&lock); 
				}
				numHaveCrossed = 0; // Once the bridge switches direction, set the numHaveCrossed to 0
			}
			/* Once the thread receives signal to wake up to cross the bridge, 
				It still has to wait till its turn. So if there is any vehicle 
				before it its queue, it has to wait
			 */
			while (westQueue[headWQ] != vid )
				pthread_cond_wait(&waitingToGoWest,&lock); 
			headWQ++;
		}
		/* Once the car is woke up */ 
		waiters[direc]--;
	}
	/* Car get on the bridge */
	br.num_car++;
	br.curr_dir = direc;
	pthread_mutex_unlock(&lock);
	return;
}

void CrossBridge(int vid, int direc, int time_to_cross)
{
	pthread_mutex_lock(&lock);
	fprintf(stderr, "vid=%d dir=%d starts crossing. Bridge num_car=%d curr_dir=%d\n", 
		vid, direc, br.num_car, br.curr_dir);
	fprintf(stderr, "Waiters to cross east: %d, Waiters to cross west: %d\n", 
		waiters[0], waiters[1]);
	pthread_mutex_unlock(&lock);
	sleep(time_to_cross);
	return;
}

void ExitBridge(int vid, int direc)
{
	pthread_mutex_lock(&lock);
	/* If the cur num vehicles == 6 and there is waiter in the other side
		wake up the waiters in the other side and don't allow car in the current 
		direction to continue cross the bridge
	*/
	if (br.num_car == 6 && waiters[1-direc] > 0) {
		if (br.curr_dir == 0) { pthread_cond_broadcast(&waitingToGoWest); }
		else { pthread_cond_broadcast(&waitingToGoEast); }
	}
	/* else if the cur num vehicle < 6 and If anybody wants to go on the same direction of the bridge curr_dir, wake them up */
	else if (waiters[br.curr_dir] > 0) {
		if (br.curr_dir == 0) { pthread_cond_signal(&waitingToGoEast); }
		else { pthread_cond_signal(&waitingToGoWest); }
	}

	/* car gets off the bridge */
	br.num_car--;
	br.dept_idx++;
	numHaveCrossed++;
	fprintf(stderr, "vid=%d dir=%d exit with departure idx=%d, numHaveCrossed=%d\n", 
		vid, direc, br.dept_idx,numHaveCrossed);
	pthread_mutex_unlock(&lock);
	return;
}
