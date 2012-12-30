#include<stdio.h>
#include "gtthread.h"
 
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define N 5  //number of philosophers
 
gtthread_mutex_t mutex;
int DetectDeadlock[N];
void * start_function(void *num);
void acquire(int);
void release(int);
void test(int);
int action[N];
int philosophers[N]={0,1,2,3,4};
int chopsticks[N];
int i,k;
 

void wait(int *mutex)
{
int mutex2 = *mutex;
if(mutex2 > 0){
*mutex--;
}
else{
while(1){if(mutex2 > 0){break;}}
}
}

void signalup(int *mutex)
{
int mutex2 = *mutex;
if(mutex2 <= 0){
*mutex++;
}
else{
while(1){if(mutex2 <= 0){break;}}
}
}

void test(int philosophers)
{
if (action[philosophers] == HUNGRY && action[(philosophers+4)%N] != EATING && action[(philosophers+1)%N] != EATING)
   		 {
    //chopsticks[philosophers+1%N] = 1;
    //chopsticks[philosophers+4%N] = 1;
    printf("Philosopher %d is dining and takes chopsticks %d and %d\n",philosophers+1,(philosophers+4)%N,philosophers+1);
      action[philosophers] = EATING;
	signalup(&DetectDeadlock[philosophers]);
   }
}

void *start_function(void *diningnow)
{
    do
    {
    int *dining = diningnow;
    int philosophers = *dining;
    // acquire
    gtthread_mutex_lock(&mutex);
    action[philosophers] = HUNGRY;
    printf("Philosopher %d is Hungry\n",philosophers+1);
    test(philosophers);
    gtthread_mutex_unlock(&mutex);
    wait(&DetectDeadlock[philosophers]);
    printf("waiting");

    for(k=0;k<1000000000;k++);
    //release
    gtthread_mutex_lock(&mutex);
    action[philosophers] = THINKING;
    printf("Philosopher %d releases chopsticks %d and %d\n",philosophers+1,(philosophers+4)%N,philosophers+1);
    printf("Philosopher %d is now thinking\n",philosophers+1);
    test((philosophers+4)%N);
    test((philosophers+1)%N);
    gtthread_mutex_unlock(&mutex);
    }while(1); // infinite food
}

void set_values()
{
   gtthread_init(1000L);
    gtthread_mutex_init(&mutex);
    for(i=0;i<N;i++)
	chopsticks[i]=0;
        DetectDeadlock[i] = 0;
}
int main()
{
    int i;
	set_values();
	gtthread_t threads_philosophers[N];
	for(i=0;i<N;i++)
    {
	printf("Philosopher number %d is thinking\n",i+1);
        gtthread_create(&threads_philosophers[i],start_function,&philosophers[i]);
    }
    //gtthread_exit(NULL);
    for(;;);
}
