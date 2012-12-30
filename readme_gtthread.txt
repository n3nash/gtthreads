Name - Nishith Agarwal

GT ID - 902840078

﻿README :


Platform used : LINUX. Developed on Fedora 15. Tested on Fedora 13 (32 bit).


The preemptive scheduler :


1) The main threads starts execution.It is treated as just another thread which can be preempted and re- scheduled.So it is added to the round robin queue.
2) As soon as a thread is created using ‘gtthread_create’ , the thread is added to the thread Queue , switches to the scheduler which in turn schedules the ‘New’ thread to start  execution.
3) On complemetion of the thread or on a signal raise (SIGPROF) , I swap context to the scheduler saving the current threads execution status and schedule the next thread in the queue.
4) Thus preemption is achieved by the signals that are raised.
5) Also , if gtthread_yield() is called , then I will again save the current threads context and yield the context to the scheduler which will in turn schedule some other thread in the thread queue.


Compiling my library :
1) The created makefile is submitted.
2) Make command will make the object file for the C code.
3) One can compile the library in the following way :
        # gcc main.c gtthread.a
4) You can run the file by typing the following executable
        # ./a.out


Preventing Deadlocks in Dining Philosophers problem:


The dining philosophers problem is a case of concurrency and deadlock. Concurrency(Race Condition)  is prevented by locking and unlocking the code which enables one to acquire the chopsticks / forks.
To avoid deadlock :
For example two philosphers contend for the acquiring the critical section.Now the locks prevent the concurrency.But after acquiring the critical condition what if the other philosopher locks the critical section and begins.In this case both will wait for each other.


Solution :  Set a wait condition in the beginning where the philosopher acquiring the  critical section which is the eat section.The wait decrements the value of mutex to <0.Now when the new philosopher comes in it waits in the wait() function untill the value of the mutex is greater than or equal to 0.To make this happen I set up a signal function which increments the value.This function will be done only when the philosopher exits the critical section.Thus deadlock is prevented.


Thoughts on the Project :


1) The approach of using <ucontext.h> poses many problems in setcontext() and signal handler.There is a lot of time wasted in analysing the program context rather than the problems of <ucontext.h>.So , we should list down some problems which one can face while using that kind of approach.


2) The issue with swapcontext() still persists as mentioned in the glib library docs.The swapcontext() function leads to number of errors even with the dining philosopher implementation.When coming back to the threads , the context is somehow not found / corrupted and thus prevented from further future context executions when the context changes.
