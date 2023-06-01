#include <assert.h>
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
  IteratorState *iteratorState;
} HashMap;

/////////

HashMap *newHasmap(int capacity) {
  HashMap *map = malloc(sizeof(HashMap));

  *map = (HashMap){.capacity = capacity,
                   .size = 0,
                   .pairs = malloc(sizeof(KeyValuePair **) * capacity),
                   .iteratorState = NULL};

  map->iteratorState = malloc(sizeof(IteratorState));
  map->iteratorState->index = 0;
  map->iteratorState->pair = map->pairs[0];

  for (int i = 0; i < capacity; i++) {
    map->pairs[i] = NULL;
  }

  return map;
}

/////////////

int _hash(int key, HashMap *map) { return key % map->capacity; }

////////////

void _insertToPair(KeyValuePair **pair, KeyValuePair *newPair) {
  if (*pair == NULL) {
    *pair = newPair;
    return;
  }

  if ((*pair)->key == newPair->key) {
    (*pair)->value = newPair->value;
    free(newPair);
    return;
  }

  _insertToPair(&((*pair)->next), newPair);
}

void insert(HashMap *map, int key, int value) {
  int index = _hash(key, map);

  KeyValuePair *newPair = malloc(sizeof(KeyValuePair));
  *newPair = (KeyValuePair){.key = key, .value = value, .next = NULL};

  _insertToPair(&map->pairs[index], newPair);
};

///////////

int _getFromPair(KeyValuePair *pair, int key) {
  if (pair == NULL) {
    return -1;
  }

  if (pair->key == key) {
    return pair->value;
  }

  if (pair->next != NULL) {
    return _getFromPair(pair->next, key);
  }

  return -1;
}

int get(HashMap *map, int key) {
  int index = _hash(key, map);

  return _getFromPair(map->pairs[index], key);
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

  _removeFromPair(&map->pairs[index], key);
};

////////////

void poll(HashMap *map, int key);

// What when item added in iteration?


/* KeyValuePair *next(HashMap *map) { */
/*   if (map->iteratorState->index == map->capacity) { */
/*     map->iteratorState->index = 0; */
/*     map->iteratorState->pair = map->pairs[0]; */
/*     return NULL; */
/*   } */

/*   while (map->pairs[map->iteratorState->index]) */
  


/* }; */

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
  }
  free(map->iteratorState);
  free(map->pairs);
  free(map);
}

void testHashMap() {
  HashMap *map = newHasmap(10); // Create a new hashmap with capacity 10

  // Test inserting values
  insert(map, 1, 100);
  insert(map, 2, 200);
  insert(map, 3, 300);
  insert(map, 4, 400);
  insert(map, 5, 500);

  // Test getting values
  assert(get(map, 3) == 300);
  assert(get(map, 5) == 500);
  assert(get(map, 10) == -1); // Key not found

  // Test removing values
  removeKey(map, 2);
  removeKey(map, 4);

  // Test getting values after removal
  assert(get(map, 2) == -1); // Key not found
  assert(get(map, 4) == -1); // Key not found

  // Clean up
  clean(map);
}

int main() {
  HashMap *map = newHasmap(10);
  clean(map);

  testHashMap();
  return 0;
}
