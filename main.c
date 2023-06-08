#include "hashmap.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAP_CAPACITY 29
/* #define RAND_SEED 12345 */

int randomKey() { return rand() % 200; }

void waitUpToSecond() {
  useconds_t sleepTime = rand() % 1000000;
  usleep(sleepTime);
}

/////

void *iteratorWorker(void *rawMap) {
  HashMap *map = (HashMap *)rawMap;

  while (0 == 0) {
    iterate(map);
  }

  /* waitUpToSecond(); */

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
  const int noOfElements = 24;
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

void *countWorker(void *rawMap) {

  HashMap *map = (HashMap *)rawMap;
  while (0 == 0) {
    waitUpToSecond();
    printf("Size of map: %d\n", count(map));
  }

  return NULL;
}

int main() {
  srand(time(NULL));

  HashMap *map = newHashMap(MAP_CAPACITY);

  pthread_t threads[6];
  pthread_create(&threads[0], NULL, iteratorWorker, map);
  pthread_create(&threads[1], NULL, addRemoveWorker, map);
  pthread_create(&threads[2], NULL, batchAddRemoveWorker, map);
  pthread_create(&threads[3], NULL, randomKeyGetWorker, map);
  pthread_create(&threads[4], NULL, pollingWorker, map);
  pthread_create(&threads[5], NULL, countWorker, map);

  for (int i = 0; i < 6; ++i) {
    pthread_join(threads[i], NULL);
  }
}
