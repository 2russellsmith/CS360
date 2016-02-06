#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <iostream>
#include <semephore.h>

using namespace std;

sem_t empty, full, mutex;
class myqueue {
  std::queue <int> stlqueue;
  
  public:
  void push(int socket){
    sem_wait(&empty);
    sem_wait(&mutex);
    stlqueue.push(sock);
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
  sock = dequeue();
  // Read request
  // write response
  // std::queue
  // close
  long tid;
  tid = (long)arg;
  printf("%s%lu\n", "HELLO!: ", tid);
}


int main(){
  int NS = 20;
  int QUEUESIZE = 10;
  long threadid;
  pthread_t threads[NS];

  sem_init(&mutex, PTHREAD_PROCESS_PRIVATE, 1);
  sem_init(&full, PTHREAD_PROCESS_PRIVATE, 0);
  sem_init(&empty, PTHREAD_PROCESS_PRIVATE, QUEUESIZE);

  for(int i = 0; i < NS; i++){
    sockqueue.push(i);
  }
  for(int i = 0; i < NS; i++){
    cout << sockqueue.pop() << endl;
  }
  exit(0);
  for(threadid = 0; threadid < NS; threadid++){
    pthread_create(&threads[threadid], NULL, howdy, (void*) threadid);
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
