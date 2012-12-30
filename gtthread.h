//#define gtthread_t unsigned long int
typedef unsigned long int gtthread_t;
#define STACKSIZE 4096
#define MAXTHREADS 20
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <malloc.h>

typedef struct
{
  gtthread_t owner;
  int lock;
  unsigned int count;
}gtthread_mutex_t;

extern void gtthread_init(long period);

extern int gtthread_mutex_init(gtthread_mutex_t *mutex);
extern int gtthread_mutex_lock(gtthread_mutex_t *mutex);
extern int gtthread_mutex_unlock(gtthread_mutex_t *mutex);

extern int gtthread_create(gtthread_t *thread, void*(*start_routine)(void *), void*arg);
extern int gtthread_join(gtthread_t thread,void **status);
extern void gtthread_exit(void *retval);
extern void gtthread_yield(void);
extern int gtthread_equal(gtthread_t t1, gtthread_t t2);
extern void gtthread_cancel(gtthread_t thread);
extern gtthread_t gtthread_self(void);
