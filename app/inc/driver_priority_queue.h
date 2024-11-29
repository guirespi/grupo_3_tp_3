/*
 * driver_priority_queue.h
 *
 *  Created on: Nov 27, 2024
 *      Author: guirespi
 */

#ifndef INC_DRIVER_PRIORITY_QUEUE_H_
#define INC_DRIVER_PRIORITY_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

// Nodo de la lista enlazada
struct node {
    int value;              /**< Valor del nodo */
    struct node* next;      /**< Puntero al siguiente nodo */
};

// Estructura de la cola de prioridad usando lista enlazada
struct priority_queue_t {
    struct node* head;  /**< Cabeza de la lista enlazada */
};

// Declaraciones de las funciones

struct priority_queue_t* priority_queue_create(void);

bool priority_queue_send(struct priority_queue_t* heap, int value);

bool priority_queue_receive(struct priority_queue_t* heap, int* value);

void priority_queue_print(struct priority_queue_t* heap);



#endif /* INC_DRIVER_PRIORITY_QUEUE_H_ */
