#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *) malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long valorHash = hash(key, map->capacity);
    long k = valorHash + 1;
    //Pair* insertPair = createPair(key, value);

    //if(valorHash > map->size)
    //{
        //valorHash = valorHash % map->size;
    //}

    if(map->buckets[valorHash] != NULL)
    {
        // Valor repetido
        if(is_equal(map->buckets[valorHash]->key, key)) return;
        
        if(map->buckets[valorHash]->key == NULL)
        {
            // Casilla libre (key invalida)
            map->current = valorHash;
            map->buckets[valorHash]->key = key;
            map->buckets[valorHash]->value = value;
            map->size++;
        } else {
            // Algoritmo de resolución de colisiones
            while(1)
            {
                if(k >= map->capacity) k = k % map->capacity;

                if(map->buckets[k] == NULL)
                {
                    map->buckets[k] = createPair(key, value);
                    map->size++;
                    map->current = k;
                    break;
                }

                if((map->buckets[k] != NULL) && (map->buckets[k]->key == NULL))
                {
                    map->current = k;
                    map->buckets[k]->key = key;
                    map->buckets[k]->value = value;
                    map->size++;
                    break;
                }
                k++;
            }
        }
    } else {
        // Casilla NULL (crear Pair)
        map->buckets[valorHash] = createPair(key, value);
        map->current = valorHash;
        map->size++;
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)

    long capacityDouble = map->capacity * 2;
    long newHash;

    Pair ** bucketsDouble;
    bucketsDouble = (Pair **) calloc(capacityDouble, sizeof (Pair *));

    for (long i = 0; i < map->capacity; i++)
    {
        if(map->buckets[i] != NULL)
        {
            if(map->buckets[i]->key != NULL)
            {
                newHash = hash(map->buckets[i]->key, capacityDouble);
                bucketsDouble[newHash] = map->buckets[i];
            }
        }
    }
    
    free(map->buckets);
    map->buckets = bucketsDouble;
    map->capacity = capacityDouble;
}


HashMap * createMap(long capacity) {

    HashMap* nuevoMapa;

    nuevoMapa = (HashMap *) calloc(1, sizeof(HashMap));

    nuevoMapa->capacity = capacity;
    nuevoMapa->size = 0;
    nuevoMapa->current = -1;

    if(capacity == 0)
    {
        nuevoMapa->buckets = NULL;
        return nuevoMapa;
    }

    nuevoMapa->buckets = (Pair **) calloc(capacity, sizeof (Pair *));

    return nuevoMapa;
}

void eraseMap(HashMap * map,  char * key) {    

    //long valorHash = hash(key, map->capacity);

    Pair* elemento;

    elemento = searchMap(map, key);

    if(elemento != NULL)
    {
        elemento->key = NULL;
        map->size--;
    }
}

Pair * searchMap(HashMap * map,  char * key) {   

    long hashBusqueda = hash(key, map->capacity);

    long k = hashBusqueda + 1;

    Pair * resultadoBusqueda = NULL;

    if (is_equal(map->buckets[hashBusqueda]->key, key))
    {
        resultadoBusqueda = map->buckets[hashBusqueda];
        map->current = hashBusqueda;
    } else {
        while(1)
        {
            if(map->buckets[k] == NULL) break;
            if(is_equal(map->buckets[k]->key, key))
            {
                resultadoBusqueda = map->buckets[k];
                map->current = k;
                break;
            }
            k++;
        }
    }
    
    return resultadoBusqueda;
}

Pair * firstMap(HashMap * map) {

    long pos = 0;

    Pair * pairBusqueda = NULL;

    while(1)
    {
        if(pos >= map->capacity) pos = pos % map->capacity;

        if(map->buckets[pos] != NULL)
        {
            if(map->buckets[pos]->key != NULL)
            {
                pairBusqueda = map->buckets[pos];
                map->current = pos;
                break;
            }
        }

        pos++;   
    }

    return pairBusqueda;
}

Pair * nextMap(HashMap * map) {

    long pos = map->current + 1;

    Pair * pairBusqueda = NULL;

    while(1)
    {
        if(pos >= map->capacity) return NULL;

        if(map->buckets[pos] != NULL)
        {
            if(map->buckets[pos]->key != NULL)
            {
                pairBusqueda = map->buckets[pos];
                map->current = pos;
                break;
            }
        }

        pos++;   
    }

    return pairBusqueda;
}
