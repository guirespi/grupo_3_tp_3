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

#define QUEUE_LENGTH_     (3)
#define QUEUE_ITEM_SIZE_  (sizeof(ao_led_message_t))
#define AO_LED_BLINK_TIME (1000) /*< milliseconds */

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void task_led(void *argument)
{
    TickType_t       xLastWakeTime;
    ao_led_handle_t *hao = (ao_led_handle_t *)argument;
    LOGGER_INFO("AO led task initialized");

    // Initialise the xLastWakeTime variable with the current time.

    while (true)
    {
        ao_led_message_t msg;
        if (pdPASS == xQueueReceive(hao->hqueue, &msg, portMAX_DELAY))
        {
            if (AO_LED_MESSAGE_BLINK == msg)
            {
                LOGGER_INFO("New led blinking");
                // Toogle 1 second on then off until new event for this AO led.
                xLastWakeTime = xTaskGetTickCount();
                HAL_GPIO_WritePin((GPIO_TypeDef *)hao->led_port, (uint16_t)hao->led_pin, GPIO_PIN_SET);
                // Blink until
                vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(AO_LED_BLINK_TIME));
                HAL_GPIO_WritePin((GPIO_TypeDef *)hao->led_port, (uint16_t)hao->led_pin, GPIO_PIN_RESET);
            }
        }
    }
}

/********************** external functions definition ************************/

bool ao_led_send(ao_led_handle_t *hao, ao_led_message_t msg)
{
    return (pdPASS == xQueueSend(hao->hqueue, (void *)&msg, 0));
}

void ao_led_init(ao_led_handle_t *hao, GPIO_TypeDef *led_port, uint16_t led_pin)
{
    hao->hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
    while (NULL == hao->hqueue)
    {
        // error
    }

    // Set port and pin for led
    hao->led_port = led_port;
    hao->led_pin  = led_pin;

    BaseType_t status;
    status = xTaskCreate(task_led, "task_ao_led", 128, (void * const)hao, tskIDLE_PRIORITY + 1, NULL);
    while (pdPASS != status)
    {
        // error
    }
}

/********************** end of file ******************************************/
