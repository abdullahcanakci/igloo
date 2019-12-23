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

typedef struct Button {
  enum {DOWN, UP} state;
  uint32_t time_down;
  uint32_t time_up;
}Button;

Button func_button = {.state = UP, .time_down = 0, .time_up = 0};

static void debounce(int);
static void gpio_isr_handler(void *args);
static void gpio_task(void *arg);
static void system_timer(void *arg);

static uint32_t timer = 0;



#endif