#include "hashmap.h"
#include <pthread.h>

#define MAPCAPACITY 7537

void *iterator_worker(void *rawMap) { return NULL; }

void *add_remove_worker(void *rawMap) { return NULL; }

void *batch_add_remove(void *rawMap) { return NULL; }
void *random_key_worker(void *rawMap) { return NULL; }

void *polling_worker(void *rawMap) { return NULL; }

int main() { 
    HashMap *map = newHashMap(MAPCAPACITY); 
    pthread_t threads[5];
    pthread_create(&threads[0], NULL, iterator_worker, map);
    pthread_create(&threads[1], NULL, add_remove_worker, map);
    pthread_create(&threads[2], NULL, batch_add_remove, map);
    /* pthread_create(&threads[3], NULL, random_key_worker, map); */
    /* pthread_create(&threads[4], NULL, polling_worker, map); */

    for (int i = 0; i<2; ++i) {
        pthread_join(threads[i], NULL);
    }

}
