#ifndef APP_MAIN
#define APP_MAIN


#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/pwm.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wps.h"
#include "nvs_flash.h"

#define GPIO_BUT_FUNC 5
#define GPIO_LED_CH1 4  //RED
#define GPIO_LED_CH2 12 //BLUE
#define GPIO_LED_CH3 14 //GREEN
#define PWM_PERIOD 1000 //PWM period in ms

static esp_wps_config_t wps_config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);

static xQueueHandle gpio_evt_queue = NULL;

const uint32_t pwm_channels[3] = {
  GPIO_LED_CH1,
  GPIO_LED_CH2,
  GPIO_LED_CH3
};

uint32_t pwm_duties[3] = {
  500, 500, 500
};

int16_t pwm_phases[3] = {
  -120, 0, 120
};

typedef struct Button
{
  enum
  {
    DOWN,
    UP
  } state;
  uint32_t time_down;
  uint32_t time_up;
} Button;

Button func_button = {.state = UP, .time_down = 0, .time_up = 0};

static void debounce(int);
static void gpio_isr_handler(void *args);
static void gpio_task(void *arg);
static void system_timer(void *arg);
static void led_control_task(void *arg);
static esp_err_t event_handler(void *ctx, system_event_t *event);
static void start_wps();


static uint32_t timer = 0;

#endif