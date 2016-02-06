#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include <semaphore.h>

using namespace std;

sem_t empty, full, mutex;
class myqueue {
  queue <int> stlqueue;
  
  public:
  void push(int socket){
    sem_wait(&empty);
    sem_wait(&mutex);
    stlqueue.push(socket);
    sem_post(&mutex);
    sem_post(&full);
  }
  int pop(){
    sem_wait(&full);
    sem_wait(&mutex);
    int result = stlqueue.front();
    stlqueue.pop();
    sem_post(&mutex);
    sem_post(&empty);
    return result;
  }
} sockqueue;


void *howdy(void *arg){
    long tid;
    tid = (long)arg;
    while(true){ 
        //sock = dequeue();
        // Read request
        // write response
        // std::queue
        // close
        int value = sockqueue.pop();
        usleep(100);
        printf("Hello: %lu Handled By:%d\n", value, tid);
    }
}

int main(){
    int NS = 10;
    int QUEUESIZE = 200;
    long threadid;
    pthread_t threads[NS];
 
    sem_init(&mutex, PTHREAD_PROCESS_PRIVATE, 1);
    sem_init(&full, PTHREAD_PROCESS_PRIVATE, 0);
    sem_init(&empty, PTHREAD_PROCESS_PRIVATE, QUEUESIZE);
    for(int i = 0; i < QUEUESIZE; i++){
        sockqueue.push(i);
    } 
    for(threadid = 0; threadid < NS; threadid++){
      pthread_create(&threads[threadid], NULL, howdy, (void*) threadid);
    }
    while(true){
    }
    // //set up socket
    // //bind listen
    // for(;;){
    //   fd = accept;
    //   enqueue(fd);
    // }
    pthread_exit(NULL);
    return 0;
}
