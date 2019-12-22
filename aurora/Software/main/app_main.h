#ifndef APP_MAIN
#define APP_MAIN



#include "driver/gpio.h"
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"


#define GPIO_BUT_FUNC 5

static xQueueHandle gpio_evt_queue = NULL;

static void debounce(int);
static void gpio_isr_handler(void *args);
static void gpio_task(void *arg);

#endif