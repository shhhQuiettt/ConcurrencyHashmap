#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct KeyValuePair KeyValuePair;

typedef struct {
  int index;
  KeyValuePair *pair;
} IteratorState;

struct KeyValuePair {
  int key;
  int value;
  KeyValuePair *next;
};

typedef struct {
  KeyValuePair **pairs;
  int size;
  int capacity;
  /* IteratorState *iteratorState; */
  pthread_mutex_t *pairMutexes;
  pthread_cond_t polling_condition;
} HashMap;

/////////

HashMap *newHashMap(int capacity) {
  HashMap *map = malloc(sizeof(HashMap));

  *map = (HashMap){
      .capacity = capacity,
      .size = 0,
      .pairs = malloc(sizeof(KeyValuePair **) * capacity),
      .pairMutexes = malloc(capacity * sizeof(pthread_mutex_t)),
  };
  /* .iteratorState = NULL}; */
  pthread_cond_init(&map->polling_condition, NULL);

  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
  for (int i = 0; i < capacity; i++) {
    pthread_mutex_init(&map->pairMutexes[i], &mutex_attr);
    map->pairs[i] = NULL;
  }

  return map;
}

/////////////

int _hash(int key, HashMap *map) { return key % map->capacity; }

////////////

// Returns true if inserting succeded, false if key already existed and updates
// value
bool _insertToPair(KeyValuePair **pair, KeyValuePair *newPair) {
  if (*pair == NULL) {
    *pair = newPair;
    return true;
  }

  if ((*pair)->key == newPair->key) {
    (*pair)->value = newPair->value;
    free(newPair);
    return false;
  }

  return _insertToPair(&((*pair)->next), newPair);
}

// Returns true if key is new succeded, false if key already existed and updates
// value
bool insert(HashMap *map, int key, int value) {
  int index = _hash(key, map);

  KeyValuePair *newPair = malloc(sizeof(KeyValuePair));
  *newPair = (KeyValuePair){.key = key, .value = value, .next = NULL};

  pthread_mutex_lock(&map->pairMutexes[index]);
  bool res = _insertToPair(&map->pairs[index], newPair);
  if (res) {
    map->size++;
    pthread_cond_broadcast(&map->polling_condition);
  }
  pthread_mutex_unlock(&map->pairMutexes[index]);
  return res;
};

///////////

bool _getFromPair(KeyValuePair *pair, int key, int *out_value) {
  if (pair == NULL) {
    return false;
  }

  if (pair->key == key) {
    *out_value = pair->value;
    return true;
  }

  if (pair->next != NULL) {
    return _getFromPair(pair->next, key, out_value);
  }

  return false;
}

bool get(HashMap *map, int key, int *outValue) {
  int index = _hash(key, map);

  pthread_mutex_lock(&map->pairMutexes[index]);
  bool ok = _getFromPair(map->pairs[index], key, outValue);
  pthread_mutex_unlock(&map->pairMutexes[index]);
  return ok;
}
////////////

int count(HashMap *map) { return map->size; };

////////////

// concurrency problem
void _removeFromPair(KeyValuePair **pair, int key) {
  if (pair == NULL) {
    return;
  }

  if (*pair == NULL) {
    return;
  }

  if ((*pair)->key == key) {
    KeyValuePair *toRemove = *pair;
    *pair = (*pair)->next;
    free(toRemove);
    return;
  }

  _removeFromPair(&(*pair)->next, key);
}

void removeKey(HashMap *map, int key) {
  int index = _hash(key, map);

  pthread_mutex_lock(&map->pairMutexes[index]);
  _removeFromPair(&map->pairs[index], key);
  pthread_mutex_unlock(&map->pairMutexes[index]);
};

////////////

void poll(HashMap *map, int key, int *out_value) {
  int index = _hash(key, map);

  // execution order with removing
  pthread_mutex_lock(&map->pairMutexes[index]);

  while (!get(map, key, out_value)) {
    pthread_cond_wait(&map->polling_condition, &map->pairMutexes[index]);
  }

  pthread_mutex_unlock(&map->pairMutexes[index]);
}

////
void _print_pair(KeyValuePair *pair) {
  if (pair == NULL) {
    return;
  }

  printf("%d -> %d\n", pair->key, pair->value);
  _print_pair(pair->next);
}
void iterate(HashMap *map) {

  printf("Start of iteration\n");
  for (int i = 0; i < map->capacity; ++i) {
    int index = _hash(i, map);
    pthread_mutex_lock(&map->pairMutexes[index]);
    _print_pair(map->pairs[i]);
    pthread_mutex_unlock(&map->pairMutexes[index]);
  }
  printf("End of iteration\n\n");
}
///

void _cleanPair(KeyValuePair *pair) {
  if (pair == NULL) {
    return;
  }

  if (pair->next != NULL)
    _cleanPair(pair->next);

  free(pair);
}

void clean(HashMap *map) {
  for (int i = 0; i < map->capacity; i++) {
    _cleanPair(map->pairs[i]);
    pthread_mutex_destroy(&map->pairMutexes[i]);
  }
  free(map->pairs);
  free(map->pairMutexes);

  pthread_cond_destroy(&map->polling_condition);
  free(map);
}
