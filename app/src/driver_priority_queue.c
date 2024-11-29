/*
 * driver_priority_queue.c
 *
 *  Created on: Nov 27, 2024
 *      Author: rpalma
 */

#include "driver_priority_queue.h"

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <cmsis_os.h>

#include <stdio.h>
#include <stdlib.h>


// Función para crear una cola de prioridad
struct priority_queue_t* priority_queue_create(void)
{
    struct priority_queue_t* p_queue = (struct priority_queue_t*)malloc(sizeof(struct priority_queue_t));
    p_queue->head = NULL;
    return p_queue;
}

// Función para insertar un valor en la cola de prioridad
bool priority_queue_send(struct priority_queue_t* heap, int value)
{
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->value = value;
    new_node->next = NULL;

    // Si la lista está vacía, el nuevo nodo será el primero
    if (heap->head == NULL || heap->head->value < value) {
        new_node->next = heap->head;
        heap->head = new_node;
        return true;
    }

    // Buscar la posición correcta para insertar el nuevo nodo
    struct node* current = heap->head;
    while (current->next != NULL && current->next->value >= value) {
        current = current->next;
    }

    // Insertar el nuevo nodo en la posición correcta
    new_node->next = current->next;
    current->next = new_node;

    return true;
}

// Función para extraer el valor de la cabeza (máximo valor)
bool priority_queue_receive(struct priority_queue_t* heap, int* value)
{
    if (heap->head == NULL) {
        return false;  // La lista está vacía
    }

    // Obtener el valor de la cabeza
    *value = heap->head->value;

    // Mover la cabeza al siguiente nodo
    struct node* temp = heap->head;
    heap->head = heap->head->next;

    // Liberar la memoria del nodo extraído
    free(temp);

    return true;
}

// Función para imprimir la cola de prioridad
void priority_queue_print(struct priority_queue_t* heap)
{
    struct node* current = heap->head;
    while (current != NULL) {
        printf("%d ", current->value);
        current = current->next;
    }
    printf("\n");
}
