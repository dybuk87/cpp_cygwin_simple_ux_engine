#ifndef __UX_THREADS__
#define __UX_THREADS__

#include <iostream>
#include <pthread.h>

void* thread_invoker(void *arg);

class MutexHelper {
public:
	MutexHelper(pthread_mutex_t& m) : mutex(m){		
		pthread_mutex_lock(&mutex);	
	}
	
	~MutexHelper() {
		pthread_mutex_unlock(&mutex);	
	}
	

private:
	pthread_mutex_t& mutex;
};


class Runnable {

public:
	Runnable() {
		done = 0;
	}

	virtual void run() {
		std::cout<<"."<<std::endl;
	}
	
	void internalExecute() {
		pthread_mutex_lock(&mutex);
		done = 0;
		pthread_mutex_unlock(&mutex);
		run();
		
		pthread_mutex_lock(&mutex);
		done = 1;
		pthread_cond_signal(&finish_condition);
		pthread_mutex_unlock(&mutex);
		
	}
	
	void wait() {
		MutexHelper mutexLock(mutex); 
		if (done == 0) {
			pthread_cond_wait(&finish_condition, &mutex);
		}
	}
	
private:	
	int done;
	pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;		
	pthread_cond_t finish_condition = PTHREAD_COND_INITIALIZER;
};

extern Runnable null_runnable;


class Thread {
public:
	Thread();

	void mainLoop();
	
	void setRunnable(Runnable* r);
	
	int isWorking(); 
			
	int start();	
	int stop();		
	
	void join();
	
	~Thread();
	
private:
	Runnable* runnable;
	pthread_t thread;
	pthread_attr_t* attr  = NULL;		
	pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;		
	pthread_cond_t sleep_condition = PTHREAD_COND_INITIALIZER;
	
	int running;
};

class ThreadPool {
public:
	ThreadPool(int thNo);	
	
	void pushTask(Runnable* runnable);
	
	~ThreadPool();	
	
private:
	Thread* threads;
	int threadCount;
};


#endif