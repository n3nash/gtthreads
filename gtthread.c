#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <malloc.h>
#include <sys/time.h>
#include "gtthread_test.h"

#define STACKSIZE 4096
#define MAX_THREADS 10

//typedef unsigned long int gtthread_t;

static ucontext_t mainContext;
static ucontext_t scheduler_context;
static int thread_finished;
int i;
long quantum;
int visited=0;
struct itimerval timeslice;
struct sigaction scheduling_interuption_handler;
static int numberofthreads_index=-1;
static int entered=0;
static int exiting=0;
static int threadone = 0;
static int switchedtomain = 0;
static int first_created = 1;
static int numberoftotalthreads=0;
static int numberoftotalthreadsnow=0;
static int current_thread=0;
static int alarmraise = 0;
static int finishnow = 0;
void gtthread_exit(void *);
int mainflag;

typedef struct
{
	ucontext_t thread_context;
	int active;
	gtthread_t threadid;
	int executed;
	int exited; 
	void *retvalue;
} threads;

typedef struct
{
int waiting;
int running;
}joining;

static threads thread_array[MAX_THREADS];
static void scheduler() {
entered = 0;
	while(!finishnow) {
		if(thread_array[current_thread].executed != 1)
		{
		swapcontext(&scheduler_context, &thread_array[current_thread].thread_context);
		timeslice.it_value.tv_sec = 0; 
		timeslice.it_value.tv_usec =0;
		timeslice.it_interval = timeslice.it_value;
		setitimer(ITIMER_PROF , &timeslice, NULL);
		}
		if(numberoftotalthreadsnow!=0){
			if(current_thread < numberofthreads_index)
			current_thread++;
			else
			current_thread = 0;}
			if(numberoftotalthreadsnow == 0)
			finishnow = 1;
			timeslice.it_value.tv_sec = 0; 
			timeslice.it_value.tv_usec = quantum;
			timeslice.it_interval = timeslice.it_value;
			setitimer(ITIMER_PROF , &timeslice, NULL);
		}
		if(thread_array[0].exited==1){
		timeslice.it_value.tv_sec = 0; 
		timeslice.it_value.tv_usec =0;
		timeslice.it_interval = timeslice.it_value;
		setitimer(ITIMER_PROF , &timeslice, NULL);
		exit(0);}
}

static void signal_handler_function(int sig_nr, siginfo_t* info, void *old_context) {
		timeslice.it_value.tv_sec = 0; 
		timeslice.it_value.tv_usec =0;
		timeslice.it_interval = timeslice.it_value;
		setitimer(ITIMER_PROF , &timeslice, NULL);
		if (sig_nr == SIGPROF) {
			alarmraise = 1;
			//thread_array[current_thread].thread_context = *((ucontext_t*) old_context);
			swapcontext(&thread_array[current_thread].thread_context,&scheduler_context);
			timeslice.it_value.tv_sec = 0; 
			timeslice.it_value.tv_usec =quantum;
			timeslice.it_interval = timeslice.it_value;
			setitimer(ITIMER_PROF , &timeslice, NULL);
		//setcontext(&scheduler_context);
				}
		else {
			return;
		}

}

static void allotmaincontext()
{
	setcontext(&thread_array[0].thread_context);
}

void gtthread_init(long period)
{
	static char scheduler_stack[STACKSIZE];
	visited = 1;
	quantum = period;
	for ( i = 0; i < MAX_THREADS; ++ i )
	{
		thread_array[i].active = 0;
		thread_array[i].executed = 0;
	}

	if (getcontext(&scheduler_context) == 0) {
		scheduler_context.uc_stack.ss_sp = scheduler_stack;
		scheduler_context.uc_stack.ss_size = sizeof(scheduler_stack);
		scheduler_context.uc_stack.ss_flags = 0;
		scheduler_context.uc_link = NULL;
	} else {
		printf("\n\tError while initializing scheduler_context...");
		exit(-1);
	}
	timeslice.it_value.tv_sec = 0; 
	timeslice.it_value.tv_usec =(long)period;
	timeslice.it_interval = timeslice.it_value;
  	
	
	scheduling_interuption_handler.sa_sigaction = signal_handler_function;
	scheduling_interuption_handler.sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&scheduling_interuption_handler.sa_mask);
	if (sigaction(SIGPROF , &scheduling_interuption_handler, NULL) == -1) {
		exit(-1);
	}

	first_created = 0;
	int repeat=0;
	if(numberoftotalthreads==0)
	{
		threadone  = 1;
	if (setitimer(ITIMER_PROF , &timeslice, NULL) == 0) {
	} else {
		printf("\n\tAn error occurred while executing setitimer...");
		exit(-1);
	}

	thread_array[numberoftotalthreads].active = 1;
	long int temp = rand();
	thread_array[numberoftotalthreads].threadid  = temp;
	if (getcontext(&thread_array[numberoftotalthreads].thread_context) == 0) {
		thread_array[numberoftotalthreads].thread_context.uc_stack.ss_sp = malloc(STACKSIZE);
		thread_array[numberoftotalthreads].thread_context.uc_stack.ss_size = STACKSIZE;
		thread_array[numberoftotalthreads].thread_context.uc_stack.ss_flags = 0;
		thread_array[numberoftotalthreads].thread_context.uc_link = &scheduler_context;
		thread_array[numberoftotalthreads].executed= 0;
		thread_array[numberoftotalthreads].active= 1;
		makecontext(&thread_array[0].thread_context, &allotmaincontext, 0);
		}
	numberoftotalthreads++;
	numberoftotalthreadsnow++;
	first_created = 1;
	numberofthreads_index++;
}

	exiting = 1;
	getcontext(&thread_array[0].thread_context);
	if(first_created == 1 && visited==1)
	{
		first_created = 0;
		scheduler();
	}

}

void gtthread_cancel(gtthread_t thread)
{
	int threadindex=0;
	if(thread != 0)
		{
		for(i=0;i<=numberofthreads_index;i++)
			{
				if(thread_array[i].threadid == thread)
				{
				threadindex = i;
				break;
				}
			}
	if(threadindex!=0){
		free(thread_array[threadindex].thread_context.uc_stack.ss_sp);
		thread_array[threadindex].executed = 1;
		thread_array[threadindex].exited = 1;}
		numberoftotalthreadsnow--;
	}
		}	


static void beginthread_init( void *(func)(),void *arg )
{
	thread_array[current_thread].active = 1;
	thread_array[current_thread].retvalue = func(arg);
	thread_array[current_thread].active = 0;
	thread_array[current_thread].executed = 1;
	gtthread_cancel(thread_array[current_thread].threadid);	
}


void gtthread_exit(void *retval)
{
	if(thread_array[current_thread].threadid != 0)
		{
			thread_array[current_thread].retvalue = retval;
			thread_array[current_thread].executed = 1;
			thread_array[current_thread].exited = 1;
			thread_array[current_thread].active = 0;
		}
		gtthread_cancel(thread_array[current_thread].threadid);
		setcontext(&scheduler_context);
}

void gtthread_yield(void)
{
	swapcontext(&thread_array[current_thread].thread_context,&scheduler_context);
	return;
}

int gtthread_equal(gtthread_t t1,gtthread_t t2)
{
	if(t1 == t2)
		return 1;
	else
		return 0;
}


gtthread_t gtthread_self(void)
{	
	gtthread_t t1;
	t1 = thread_array[current_thread].threadid;
	return t1;
}

int gtthread_join(gtthread_t thread,void **status)
{
	int index = 0;
	int i;
	for(i=0;i<=MAX_THREADS;i++)
		{
		if(thread_array[i].threadid == thread)
		{
		index = i;
		}}
	if(thread_array[index].executed!=1)while(1){if(thread_array[index].executed==1)break;}
//swapcontext(&thread_array[current_thread].thread_context,&scheduler_context);
	if(status != NULL && thread_array[index].retvalue!=NULL){
		*status = thread_array[index].retvalue;
		return 1;
		}
}

int gtthread_mutex_init(gtthread_mutex_t *mutex)
{
	mutex->lock = 0;
	mutex->count = 1;
	mutex->owner = 0;
}

int gtthread_mutex_lock(gtthread_mutex_t *mutex)
{
//if(mutex->lock==1);
	if(mutex->owner == 0 && mutex->owner!=thread_array[current_thread].threadid && mutex->lock==0)
		{
		while(mutex->count<=0);
			mutex->count--;
			mutex->lock = 1;
			mutex->owner = thread_array[current_thread].threadid;
			}
	else
		while(1){if(mutex->owner==0){break;}}
}


int gtthread_mutex_unlock(gtthread_mutex_t *mutex)
{
	if(mutex->owner == thread_array[current_thread].threadid)
		{
		mutex->count++;
		mutex->lock = 0;
		mutex->owner = 0;
		}
}

int gtthread_create(gtthread_t *thread,void*(*start_routine)(void *),void *arg)
{
long int temp;
	if(visited==1){
		int repeat=1;
		entered = 1;
		thread_array[numberoftotalthreads].active = 1;
		
		while(repeat==1)
			{temp = rand();
			for(i=0;i<numberoftotalthreads;i++)
				{
				if(thread_array[i].threadid == temp){
				repeat = 1;
				}}
			repeat = 0;
		}
	thread_array[numberoftotalthreads].threadid  = temp;
	*thread = temp;
	if (getcontext(&thread_array[numberoftotalthreads].thread_context) == 0) {
		thread_array[numberoftotalthreads].thread_context.uc_stack.ss_sp = malloc(STACKSIZE);
		thread_array[numberoftotalthreads].thread_context.uc_stack.ss_size = STACKSIZE;
		thread_array[numberoftotalthreads].thread_context.uc_stack.ss_flags = 0;
		thread_array[numberoftotalthreads].thread_context.uc_link = &scheduler_context;
		thread_array[numberoftotalthreads].executed= 0;
		makecontext(&thread_array[numberoftotalthreads].thread_context,(void (*)(void))&beginthread_init, 2, start_routine,arg );
	} else {
		printf("\n\tError while initializing thread_context...");
		exit(-1);
	}
	numberoftotalthreads++;
	numberoftotalthreadsnow++;
	//current_thread++;
	numberofthreads_index = numberoftotalthreads - 1;
	//current_thread = numberofthreads_index;
	if(threadone==1){
		threadone = 0;
		getcontext(&thread_array[0].thread_context);
		}
	if(entered == 1)
	{
	entered=0;
//setcontext(&scheduler_context);
	swapcontext(&thread_array[current_thread].thread_context,&scheduler_context);
	}
	return;
	}
	else
	exit(-1);
}
