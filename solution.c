#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#define BUFFSIZE 1024

struct args {
  char *buffer;
  sem_t *full;
  sem_t *empty;
  sem_t *mutex;
};

void *consumer(void *thread_args) {
  struct args *actual_args = thread_args;
  while (1) {
    sem_wait(actual_args->full);
    sem_wait(actual_args->mutex);
    printf("%s\n", actual_args->buffer);
    actual_args->buffer[0] = '\0';
    sem_post(actual_args->mutex);
    sem_post(actual_args->empty);
  }
  return NULL;
}

void *producer(void *thread_args) {
  struct args *actual_args = thread_args;
  int status;
  while (1) {
    sem_wait(actual_args->empty);
    status = sem_wait(actual_args->mutex);
    strcat(actual_args->buffer, "d");
    sem_post(actual_args->full);
    sem_post(actual_args->mutex);
  }
  return NULL;
}

int main() {
  pthread_t con, prod;
  int status_con, status_prod;
  char buffer[BUFFSIZE];

  sem_unlink("full");
  sem_unlink("empty");
  sem_unlink("mutex");
  sem_t *full = sem_open("full", O_CREAT, 7777, 0);
  sem_t *empty = sem_open("empty", O_CREAT, 7777, BUFFSIZE - 1);
  sem_t *mutex = sem_open("mutex", O_CREAT, 7777, 1);

  struct args arguements = {buffer, full, empty, mutex};

  status_prod = pthread_create(&prod, NULL, producer, &arguements);
  status_con = pthread_create(&con, NULL, consumer, &arguements);

  pthread_join(con, NULL);
  pthread_join(prod, NULL);

  return 0;
}
