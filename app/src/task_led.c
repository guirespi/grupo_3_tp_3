/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "task_led.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_     	(10)
#define AO_LED_ON_TIME 		(5000) /*< milliseconds */
#define TASK_PERIOD_MS_     (100)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void ao_led_turn_on(GPIO_TypeDef * led_port, uint16_t led_num, uint32_t duration)
{
    TickType_t       xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    // Encender el LED
    HAL_GPIO_WritePin(led_port, led_num, GPIO_PIN_SET);
    // Parpadeo hasta que pase el tiempo de duración
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(AO_LED_ON_TIME));
    // Apagar el LED
    HAL_GPIO_WritePin(led_port, led_num, GPIO_PIN_RESET);
}

static void task_led(void *argument)
{
    ao_led_handle_t *hao = (ao_led_handle_t *)argument;
    LOGGER_INFO("AO led task initialized");

    while (true)
    {
        int temp;
        // Recibir el mensaje de la cola de prioridad
        if (true == priority_queue_receive(hao->hqueue, (int *)&temp))
        {
            ao_led_message_t msg = (ao_led_message_t) temp;
            uint16_t led_num = 0;
            GPIO_TypeDef * led_port = NULL;

            // Decidir qué LED encender según el mensaje recibido
            if (AO_LED_MESSAGE_PULSE == msg)
            {
                LOGGER_INFO("Triggered red led");
                led_port = (GPIO_TypeDef *)LED_RED_PORT;
                led_num = LED_RED_PIN;
            }
            if (AO_LED_MESSAGE_SHORT == msg)
            {
                LOGGER_INFO("Triggered green led");
                led_port = (GPIO_TypeDef *)LED_GREEN_PORT;
                led_num = LED_GREEN_PIN;
            }
            if (AO_LED_MESSAGE_LONG == msg)
            {
                LOGGER_INFO("Triggered blue led");
                led_port = (GPIO_TypeDef *)LED_BLUE_PORT;
                led_num = LED_BLUE_PIN;
            }

            ao_led_turn_on(led_port, led_num, AO_LED_ON_TIME);
        }
        // Como la implementación de la cola de prioridad no bloquea, se hace una espera bloqueante en esta tarea.
        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS_));
    }
}

/********************** external functions definition ************************/

bool ao_led_send(ao_led_handle_t *hao, ao_led_message_t msg)
{
    // Enviar un mensaje a la cola de prioridad (convertir el mensaje a int)
    return (true == priority_queue_send(hao->hqueue, (int)msg));
}

void ao_led_init(ao_led_handle_t *hao)
{
    // Crear la cola de prioridad
    hao->hqueue = priority_queue_create();
    while (NULL == hao->hqueue)
    {
        // Si la cola no se crea correctamente, el bucle sigue hasta que lo haga (en un caso real, agregar manejo de errores).
    }

    // Crear la tarea para controlar el LED
    BaseType_t status;
    status = xTaskCreate(task_led, "task_ao_led", 256, (void * const)hao, tskIDLE_PRIORITY + 1, NULL);
    while (pdPASS != status)
    {
        // Si la tarea no se crea correctamente, el bucle sigue hasta que lo haga (en un caso real, agregar manejo de errores).
    }
}

/********************** end of file ******************************************/
