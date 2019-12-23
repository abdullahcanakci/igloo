#include "app_main.h"

void app_main()
{
    // SET FUNC BUTTON
    gpio_config_t but_conf;
    but_conf.intr_type = GPIO_INTR_ANYEDGE;
    but_conf.mode = GPIO_MODE_INPUT;
    but_conf.pin_bit_mask = 1ULL << GPIO_BUT_FUNC;
    but_conf.pull_up_en = 1;

    gpio_config(&but_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_BUT_FUNC, gpio_isr_handler, (void *)GPIO_BUT_FUNC);

    // SYSTEM TIMER
    xTaskCreate(system_timer, "system_timer", 256, NULL, 11, NULL);
}

static void debounce(int in_state)
{
    uint32_t temp = timer;
    if (func_button.state != in_state)
    {
        if (func_button.time_down - temp < 10 * 1000 / configTICK_RATE_HZ)
        {
            return;
        }
        func_button.state = in_state;
        if (func_button.state == DOWN)
        {
            func_button.time_down = temp;
        }
        else
        {
            func_button.time_up = temp;
        }
        if (func_button.state == UP && func_button.time_down != 0)
        {
            uint32_t interval = (func_button.time_up - func_button.time_down) * 1000 / configTICK_RATE_HZ;
            if (interval < 2000)
            {
                ESP_LOGI("BUTTON", "CLICK");
            }
            else
            {
                ESP_LOGI("BUTTON", "HOLD");
            }
            func_button.time_up = 0;
            func_button.time_down = 0;
        }
    }
}

/******************** TASKS ************************/

/**
 * freeRTOS does not have a neat system time function.
 * I need a basic one to debounce buttons.
 * timer is uint32_t which can hold about 29 days in 1ms resolution.
 */
static void system_timer(void *arg)
{
    for (;;)
    {
        timer = timer + 1;
        vTaskDelay(1);
    }
}

static void gpio_task(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            if (io_num == GPIO_BUT_FUNC)
            {
                debounce(gpio_get_level(io_num));
            }
        }
    }
}

/**************ISR HANDLERS***************/

static void gpio_isr_handler(void *args)
{
    uint32_t gpio_num = (uint32_t)args;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
