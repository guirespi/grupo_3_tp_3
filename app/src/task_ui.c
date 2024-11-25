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

#include "task_ui.h"
#include "task_led.h"

/********************** macros and definitions *******************************/

#define AO_UI_QUEUE_LENGTH_    (3)
#define AO_UI_QUEUE_ITEM_SIZE_ (sizeof(ao_ui_message_t))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern ao_led_handle_t ao_led;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void task_ui(void *argument)
{
    ao_ui_handle_t *hao = (ao_ui_handle_t *)argument;
    LOGGER_INFO("User interface for led activate");
    while (true)
    {
        ao_ui_message_t ao_message;
        if (pdTRUE == xQueueReceive(hao->hqueue, (void *)&ao_message, portMAX_DELAY))
        {
            switch (ao_message)
            {
            // Highest priority.
            case AO_UI_PRESS_PULSE:
                ao_led_send(&ao_led, AO_LED_MESSAGE_PULSE);
                break;
            // Medium priority.
            case AO_UI_PRESS_SHORT:
                ao_led_send(&ao_led, AO_LED_MESSAGE_SHORT);
                break;
            // Lowest priority.
            case AO_UI_PRESS_LONG:
                ao_led_send(&ao_led, AO_LED_MESSAGE_LONG);
                break;
            default:
                LOGGER_INFO("Unknown event for UI object");
                break;
            }
        }
    }
}

bool ao_ui_send(ao_ui_handle_t *hao, ao_ui_message_t msg)
{
    return (pdPASS == xQueueSend(hao->hqueue, (void *)&msg, 0));
}

void ao_ui_init(ao_ui_handle_t *hao)
{
    hao->hqueue = xQueueCreate(AO_UI_QUEUE_LENGTH_, AO_UI_QUEUE_ITEM_SIZE_);
    while (NULL == hao->hqueue)
    {
        // error
    }

    BaseType_t status;
    status = xTaskCreate(task_ui, "task_ao_ui", 128, (void * const)hao, tskIDLE_PRIORITY + 2, NULL);
    while (pdPASS != status)
    {
        // error
    }
}

/********************** end of file ******************************************/
