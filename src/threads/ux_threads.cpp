#include "ux_threads.h"

Runnable null_runnable;

void ThreadPool::pushTask(Runnable* runnable) {
	for(int i=0; i<threadCount; i++) {
		if (threads[i].isWorking() == 0) {
			threads[i].setRunnable(runnable);	
			break;
		}
	}
}

ThreadPool::~ThreadPool() {
	delete[] threads;
}

ThreadPool::ThreadPool(int thNo) {
	threadCount = thNo;
	threads = new Thread[thNo];
	
	for(int i=0; i<thNo; i++) {
		threads[i].start();
	}
}

Thread::Thread() {	
	running = 0;
	runnable = &null_runnable;
}

Thread::~Thread() {		
	stop();		
	join();		
}

void Thread::setRunnable(Runnable* r) {
	MutexHelper mutexLock(mutex);	
	runnable = r;		
	pthread_cond_signal(&sleep_condition); 
}

int Thread::isWorking() {
	MutexHelper mutexLock(mutex);	
	return runnable != NULL && runnable != &null_runnable;
}

int Thread::start() {
	int retVal = 0;
	MutexHelper mutexLock(mutex);		
	if (running == 1) {
		retVal = 1;
	} else {
		running = 1;
		if (pthread_create(&thread, attr, thread_invoker, this) == 0) {
			retVal = 0;		
		} else {
			running = 0;
			retVal = 2;
		}	
	}	
	
	return retVal;
}

int Thread::stop() {
	int retVal = 0;		
	
	MutexHelper mutexLock(mutex);	
	
	if (running == 1) {
		running = 0;
		retVal = 0;
	} else {
		retVal = 1;
	}	
		
	pthread_cond_signal(&sleep_condition); 
	
	return retVal;
}

void Thread::mainLoop() {
	while(true) {		
		pthread_mutex_lock(&mutex);					
	
		if (running == 0) {		
			pthread_mutex_unlock(&mutex);
			break;
		}			
			
		if (runnable == NULL || runnable == &null_runnable) {		
			pthread_cond_wait(&sleep_condition, &mutex);										
		}		
		runnable->internalExecute();
		runnable = &null_runnable;
		
					
		pthread_mutex_unlock(&mutex);
																
		
	}
}

void Thread::join() {
	void* ret;
	if(pthread_join(thread, &ret)) {
	
	}
}

void* thread_invoker(void *arg) {
	Thread *thread = (Thread*)arg;
	
	thread->mainLoop();
	
	return NULL;
}

