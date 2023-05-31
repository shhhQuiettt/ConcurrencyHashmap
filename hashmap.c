#include <stdio.h>
#include <stdlib.h>

typedef struct KeyValuePair KeyValuePair;

struct KeyValuePair {
  int key;
  int value;
  KeyValuePair *next;
};

typedef struct {
  KeyValuePair *pairs;
  int size;
  int capacity;
  KeyValuePair *iteratorState;
} HashMap;

void insert(HashMap *map, int key, int value);

int _getFromPair(KeyValuePair *pair, int key) {
  if (pair->key == key) {
    return pair->value;
  }

  // Concurency problem
  if (pair->next != NULL) {
    return _getFromPair(pair->next, key);
  }

  return -1;
}

int get(HashMap *map, int key) {}

void poll(HashMap *map, int key);

int count(HashMap *map);

void remove(HashMap *map);

void first(HashMap *map);

void next(HashMap *map);

void last(HashMap *map);

int main() { return 0; }
