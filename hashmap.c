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

    long valorHash = hash(key, map->capacity); // calculamos el hash
    long k = valorHash + 1; // inicializamos k en la posicion siguiente a hash (para resolver colisiones)

    // si la posicion no es nula
    if(map->buckets[valorHash] != NULL)
    {
        // Valor repetido
        if(is_equal(map->buckets[valorHash]->key, key)) return;
        
        // si la llave es nula/invalida
        if(map->buckets[valorHash]->key == NULL)
        {
            map->current = valorHash;
            map->buckets[valorHash]->key = key;
            map->buckets[valorHash]->value = value;
            map->size++;
        } else {
            // algoritmo de resolución de colisiones
            while(1)
            {
                // al intentar resolver una colision, si k es mayor a la capacidad del arreglo
                // calculamos su modulo respecto a la capacidad
                if(k >= map->capacity) k = k % map->capacity;

                // encontramos una casilla nula
                if(map->buckets[k] == NULL)
                {
                    // creamos el par
                    map->buckets[k] = createPair(key, value);
                    map->size++;
                    map->current = k;
                    break;
                }

                // la casilla no esta nula pero la key es invalida/nula
                if((map->buckets[k] != NULL) && (map->buckets[k]->key == NULL))
                {
                    // actualizamos key y value
                    map->current = k;
                    map->buckets[k]->key = key;
                    map->buckets[k]->value = value;
                    map->size++;
                    break;
                }
                k++; // siguiente iteracion
            }
        }
    } else {
        // la casilla entregada por el hash es nula, se crea el par
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

    // recorremos todo el arreglo map->buckets
    for (long i = 0; i < map->capacity; i++)
    {
        if(map->buckets[i] != NULL) // si encontramos una posicion no nula
        {
            if(map->buckets[i]->key != NULL) // si su llave no es invalida/nula
            {
                newHash = hash(map->buckets[i]->key, capacityDouble); // calculamos su nuevo hash
                bucketsDouble[newHash] = map->buckets[i]; // insertamos el par
            }
        }
    }

    map->buckets = bucketsDouble; // actualizamos el puntero
    map->capacity = capacityDouble; // actualizamos la capacidad
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

    Pair* elemento;

    elemento = searchMap(map, key);

    if(elemento != NULL)
    {
        elemento->key = NULL; // hacemos la llave null
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
            // en caso de que se de una vuelta al arreglo
            if(k >= map->capacity) k = k % map->capacity;

            if(map->buckets[k] == NULL) return NULL;

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
