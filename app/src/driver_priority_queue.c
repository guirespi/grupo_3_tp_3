/*
 * driver_priority_queue.c
 *
 *  Created on: Nov 24, 2024
 *      Author: guirespi
 */
#include "driver_priority_queue.h"

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <cmsis_os.h>

// Structure to represent a priority queue
struct priority_queue_t {
    int * array; /**< Array of elements with of integer size */
    unsigned int size; /**< Actual size of the priority queue */
    unsigned int capacity; /**< Capacity of priority queue */
};

static void swap(int* a, int* b);
static void heapify(priority_queue_t heap, unsigned int i);

// Function to swap two integers
static void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to heapify the node at index i
static void heapify(priority_queue_t heap, unsigned int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap->size
        && heap->array[left] > heap->array[largest])
        largest = left;

    if (right < heap->size
        && heap->array[right] > heap->array[largest])
        largest = right;

    if (largest != i) {
        swap(&heap->array[i], &heap->array[largest]);
        heapify(heap, largest);
    }
}

// Function to create a heap
priority_queue_t priority_queue_create(unsigned int capacity)
{
    priority_queue_t p_queue = (priority_queue_t)malloc(sizeof(*p_queue));
    p_queue->size = 0;
    p_queue->capacity = capacity;

    p_queue->array = (int*)malloc(capacity * sizeof(*p_queue->array));
    return p_queue;
}

// Function to insert a new value into the heap
bool priority_queue_send(priority_queue_t heap, int value)
{
    if (heap->size == heap->capacity) {
        printf("priority_queue_t overflow. Value is lost\n");
        return false;
    }

    taskENTER_CRITICAL();

    heap->size++;
    int i = heap->size - 1;
    heap->array[i] = value;

    // Fix the heap property if it is violated
    while (i != 0
           && heap->array[(i - 1) / 2] < heap->array[i]) {
        swap(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }

    taskEXIT_CRITICAL();

    return true;
}

// Function to extract the root (maximum element)
bool priority_queue_receive(priority_queue_t heap, int * value)
{
	bool rt = false;

    taskENTER_CRITICAL();

    if (heap->size <= 0)
    {
        rt = false;
    }
    else
    {
		// Store the maximum value, and remove it
		*value = heap->array[0];
		// If the heap is just one element. We do not need to heapify, so no need to take the last element of the heap.
		if(heap->size > 1)
			heap->array[0] = heap->array[heap->size - 1];

		// Reduce number of elements in heap.
		heap->size--;

		// If the heap is empty after the removal no need to re-arrange.
		if(heap->size > 0)
			heapify(heap, 0);

		rt = true;
    }

    taskEXIT_CRITICAL();
    return rt;
}

void priority_queue_print(priority_queue_t heap)
{
    taskENTER_CRITICAL();
    for (int i = 0; i < heap->size; ++i)
        printf("%d ", heap->array[i]);
    printf("\n");
    taskEXIT_CRITICAL();
}
