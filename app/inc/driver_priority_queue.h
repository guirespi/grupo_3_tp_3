/*
 * driver_priority_queue.h
 *
 *  Created on: Nov 24, 2024
 *      Author: guirespi
 */

#ifndef INC_DRIVER_PRIORITY_QUEUE_H_
#define INC_DRIVER_PRIORITY_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct priority_queue_t * priority_queue_t;

priority_queue_t priority_queue_create(unsigned int capacity);

bool priority_queue_send(priority_queue_t p_queue, int value);

bool priority_queue_receive(priority_queue_t p_queue, int * value);

void priority_queue_print(priority_queue_t p_queue);

#endif /* INC_DRIVER_PRIORITY_QUEUE_H_ */
