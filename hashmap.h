#ifndef HASHMAP_H
#define HASHMAP_H
#endif

#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>

typedef struct KeyValuePair KeyValuePair;

struct KeyValuePair {
  int key;
  int value;
  KeyValuePair *next;
};

typedef struct {
  KeyValuePair **pairs;
  atomic_uint size;
  int capacity;
  pthread_mutex_t globalMutex;
  pthread_mutex_t *pairMutexes;
  pthread_cond_t polling_condition;
} HashMap;

/////////

HashMap *newHashMap(int capacity);

/////////////

// Returns true if key is new succeded, false if key already existed and updates
// value
bool insert(HashMap *map, int key, int value);

///////////

bool get(HashMap *map, int key, int *outValue);

////////////

int count(HashMap *map);

////////////

void removeKey(HashMap *map, int key);

////////////

void poll(HashMap *map, int key, int *out_value);

void iterate(HashMap *map);

void clean(HashMap *map);

