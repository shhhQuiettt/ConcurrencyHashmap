#include "hashmap.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAP_CAPACITY 787
#define RAND_SEED 12345

int randomKey() { return rand() % 200; }

void waitUpToSecond() {
  useconds_t sleepTime = rand() % 1000000;
  usleep(sleepTime);
}

/////

void *iterator_worker(void *rawMap) {
  HashMap *map = (HashMap *)rawMap;

  while (0 == 0) {
    iterate(map);
  }

  waitUpToSecond();

  return NULL;
}

void *addRemoveWorker(void *rawMap) {

  HashMap *map = (HashMap *)rawMap;

  while (0 == 0) {
    int key = randomKey();
    insert(map, key, rand());
    removeKey(map, key);
  }

  return NULL;
}

void *batchAddRemoveWorker(void *rawMap) {
  HashMap *map = (HashMap *)rawMap;
  const int noOfElements = 16;
  int keys[noOfElements];

  while (0 == 0) {
    for (int i = 0; i < noOfElements; ++i) {
      keys[i] = randomKey();
    }

    for (int i = 0; i < noOfElements; ++i) {
      insert(map, keys[i], rand());
    }

    /* waitUpToSecond(); */

    for (int i = 0; i < 10; ++i) {
      removeKey(map, keys[i]);
    }
  }
  return NULL;
}

void *randomKeyGetWorker(void *rawMap) {
  HashMap *map = (HashMap *)rawMap;
  int key = randomKey();
  int val;
  while (0 == 0) {
    get(map, key, &val);
  }

  return NULL;
}

void *pollingWorker(void *rawMap) {

  HashMap *map = (HashMap *)rawMap;
  while (0 == 0) {

    int key = randomKey();
    int val;
    poll(map, key, &val);
    printf("Finally! Key: %d, Value: %d\n", key, val);
  }

  return NULL;
}

/* void* pollAwait(void* rawMap) { */
/*     HashMap* map = (HashMap*) rawMap; */
/*     int val; */
/*     poll(map, 3, &val); */
/*     printf("Val %d\n", val); */
/*     fflush(stdout); */
/*     return NULL; */
/* }; */

/* void* pollSet(void *rawMap) { */
/*     HashMap* map = (HashMap*) rawMap; */
/*     usleep(1e6*1); */
/*     insert(map, 3, 10); */
/*     printf("Set\n!"); */
/*     return NULL; */
/* } */

int main() {
  srand(RAND_SEED);

  HashMap *map = newHashMap(MAP_CAPACITY);

  pthread_t threads[5];
  pthread_create(&threads[0], NULL, iterator_worker, map);
  pthread_create(&threads[1], NULL, addRemoveWorker, map);
  pthread_create(&threads[2], NULL, batchAddRemoveWorker, map);
  pthread_create(&threads[3], NULL, randomKeyGetWorker, map);
  pthread_create(&threads[4], NULL, pollingWorker, map);

  /* pthread_create(&threads[0], NULL, pollAwait, map); */
  /* pthread_create(&threads[1], NULL, pollSet, map); */
  for (int i = 0; i < 5; ++i) {
    pthread_join(threads[i], NULL);
  }
}
