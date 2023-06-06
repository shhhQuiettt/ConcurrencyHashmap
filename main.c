#include "hashmap.h"
#include <pthread.h>
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
  int keys[10];
  while (0 == 0) {
    for (int i = 0; i < 10; ++i) {
      keys[i] = randomKey();
    }

    for (int i = 0; i < 10; ++i) {
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
    printf("Finally! Value: %d\n", val);
  }

  return NULL;
}

int main() {
  srand(RAND_SEED);

  HashMap *map = newHashMap(MAP_CAPACITY);

  pthread_t threads[5];
  pthread_create(&threads[0], NULL, iterator_worker, map);
  pthread_create(&threads[1], NULL, addRemoveWorker, map);
  pthread_create(&threads[2], NULL, batchAddRemoveWorker, map);
  pthread_create(&threads[3], NULL, randomKeyGetWorker, map);
  pthread_create(&threads[4], NULL, pollingWorker, map);

  for (int i = 0; i < 5; ++i) {
    pthread_join(threads[i], NULL);
  }
}
