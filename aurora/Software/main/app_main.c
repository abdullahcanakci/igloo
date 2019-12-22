#include "app_main.h"

static void gpio_isr_handler(void *args){
    uint32_t gpio_num = (uint32_t) args;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task(void *arg){
    uint32_t io_num;
    for (;;){
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            if(io_num == GPIO_BUT_FUNC){
                debounce(gpio_get_level(io_num));
            }
        }
    }
}

static void debounce(int in_state){
    static bool state = false; // UP
    if (state != in_state){
        state = in_state;
        ESP_LOGI("Debounce", "Button %d", in_state);
        return;
    }
}

void app_main()
{
     gpio_config_t but_conf;
    // Disable interrupts
    but_conf.intr_type = GPIO_INTR_ANYEDGE;

    but_conf.mode = GPIO_MODE_INPUT;
    but_conf.pin_bit_mask = 1ULL << GPIO_BUT_FUNC;
    but_conf.pull_up_en = 1;

    gpio_config(&but_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_BUT_FUNC, gpio_isr_handler, (void *)GPIO_BUT_FUNC);
}
