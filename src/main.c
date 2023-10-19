//  SPDX-FileCopyrightText: 2023 Matthew Kosmoski <mkosmo@gmail.com>
//  SPDX-License-Identifier: MIT

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"               // NOLINT
#include "semphr.h"             // NOLINT
#include "pico/stdlib.h"

SemaphoreHandle_t serialMutex = NULL;

void vGuardedPrint(char *out) {
    xSemaphoreTake(serialMutex, portMAX_DELAY);
    puts(out);
    xSemaphoreGive(serialMutex);
}

void vTaskSMP_print_core(void *p) {
    char *task_name = pcTaskGetName(NULL);
    char out[12];

    for (;;) {
        snprintf(out, "%s %d", task_name, get_core_num());
        vGuardedPrint(out);
        vTaskDelay(100);
    }

    vTaskDelete(NULL);
}

void led_task(void *p) {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(
            LED_PIN,
            gpio_get(LED_PIN) ^ 1);
        vTaskDelay(100);
    }

    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();

    serialMutex = xSemaphoreCreateMutex();

    TaskHandle_t handleA;
    TaskHandle_t handleB;

    // xTaskCreate(idle_task, "Idle", 0, NULL, 0, NULL);
    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(vTaskSMP_print_core, "Task 1", 256, NULL, 2, NULL);
    xTaskCreate(vTaskSMP_print_core, "Task 2", 256, NULL, 2, &handleA);
    xTaskCreate(vTaskSMP_print_core, "Task 3", 256, NULL, 2, &handleB);
    xTaskCreate(vTaskSMP_print_core, "Task 4", 256, NULL, 2, NULL);

    vTaskStartScheduler();

    while (1) {}
}
