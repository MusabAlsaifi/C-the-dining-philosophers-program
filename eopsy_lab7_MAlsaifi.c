#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <math.h>

#define N	5
#define LEFT	( i + N - 1 ) % N
#define RIGHT	( i + 1 ) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

//mutex	m 		initialized to 1
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int	state[N];	//initiated to THINKING's

//mutex	s[N]		initialized to 0's in main function
pthread_mutex_t s[N] = PTHREAD_MUTEX_INITIALIZER;


int mealsCount[N];
int totalMeals = 0;

int externalInterrupt = false;
void interruptHandler() {
	externalInterrupt = true;
	printf("Interrupt Received\n");
}

void test( int i )
{
	if( state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING )
	{
		state[i] = EATING;
		
		// If the neighbours are not eating then the current thread can unlock it's s mutex 
		// and does not get blocked when it tries to lock it again in grab_forks()
		//up( &s[i] );
		pthread_mutex_unlock(&s[i]);
	}
}
void grab_forks( int i )
{
	//down( &m ); Locking the mutex
	pthread_mutex_lock(&m);
	
	state[i] = HUNGRY;
	test(i);
	
	//up( &m );
	pthread_mutex_unlock(&m);
	
	
	//down( &s[i] );
	pthread_mutex_lock(&s[i]);
	
}

void put_away_forks( int i )
{
	//down( &m ); Locking the m mutex so only one thread may put away the forks at once
	pthread_mutex_lock(&m);

	state[i] = THINKING;
	test( LEFT );
	test( RIGHT );

	//up( &m );
	pthread_mutex_unlock(&m);
}



void* philosopher(void* n){

	if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) !=0){
		exit(1);
	}
	int*i = (int*) n;
	printf("[ Philosopher: %d] Created \n",*i);
	//int count = 0;
	while(true){
	
		printf("[ Philosopher: %d] Thinking\n", *i);
		sleep(1);
		
//---------------------------------------------ADDED----------------------------------------------------------------
		while(mealsCount[*i] != floor(totalMeals/N) ){
		//This loop makes sure that the philosophers eat fairly by comparing the no of meals had by one philosopher with the others.
		// Using the total counter of meals for all philosophers
			sleep(1);
		}
//------------------------------------------------------------------------------------------------------------------		
		
		grab_forks(*i);


		printf("[ Philosopher: %d] Eating \n", *i);
		sleep(1);

		mealsCount[*i]++;
		
//-------------------------------------------------ADDED-------------------------------------------
		// Making sure that only one thread at a time increments the global total counter
		pthread_mutex_lock(&m);
		totalMeals++;
		pthread_mutex_unlock(&m);
//---------------------------------------------------------------------------------------------


		put_away_forks(*i);
		printf("[ Philosopher: %d] Meals Eaten: %d \n",*i, mealsCount[*i]);
	}
	
	
}

int main(){

	signal(SIGINT, interruptHandler);

	// Initializing mutexes from aray s to locked - 0's
	for(int i =0; i<N; i++){
		pthread_mutex_lock(s+i);
	}
	
	// To pass the respective philospher id(number) to each philosopher thread
	int* philosopherNumbers = (int*)malloc(N*sizeof(int));
	
	pthread_t philosophers[N];
	
	for(int i=0; i< N; i++){
		philosopherNumbers[i] = i;
		if(pthread_create(&philosophers[i], NULL, philosopher, (void*) (philosopherNumbers+i)) != 0 ){
			printf("[Philosopher %d ] Error: In Philosopher thread creation.", i);
			exit(1);
		}
	}
	
	//sleep(15);
	while(!externalInterrupt){
		sleep(1);
	}
	int z;
	for(int i=0; i<N; i++){
		// cancel all philosopher threads, make them leave
		z = pthread_cancel(philosophers[i]);
		if(z != 0 ){
			printf("[Philosopher %d] : Error in cancel \n",i);
		}
		
		z = pthread_join(philosophers[i], NULL);
		if(z != 0 ){
			printf("[Philosopher %d] : Error in join \n",i);
		}
		
		printf("[Philosopher: %d ] : Terminanted\n", i);
		printf("[Philosopher: %d ] : Total Meals Eaten: %d\n", i, mealsCount[i]);
		
	}


    	// free the memory
	free(philosopherNumbers);


	return 0;
}



