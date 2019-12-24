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

    // PWM SYSTEM
    pwm_init(PWM_PERIOD, pwm_duties, 3, pwm_channels);
    pwm_set_channel_invert(0);
    pwm_set_phases(pwm_phases);
    pwm_start();

    xTaskCreate(led_control_task, "led_control", 256, NULL, 10, NULL);

    nvs_flash_init(); // Needed for WIFI credentials
    esp_event_loop_init(event_handler, NULL);
    start_wifi();
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
                ESP_LOGI("BUTTON", "CLICK, WPS starting.");
                start_wps();
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

static void led_control_task(void *arg){
    static bool toggle = false;
    for (;;)
    {
        if(toggle){
            pwm_set_duty(0, 300);
            pwm_set_duty(1, 300);
            pwm_set_duty(2, 300);
        }
        else
        {
            pwm_set_duty(0, 900);
            pwm_set_duty(1, 0);
            pwm_set_duty(2, 400);
        }
        toggle = !toggle;
        pwm_start();
        vTaskDelay(1000 * 1000 / configTICK_RATE_HZ);
    }
}
/**************ISR HANDLERS***************/

static void gpio_isr_handler(void *args)
{
    uint32_t gpio_num = (uint32_t)args;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

/*************WIFI*******************/

static esp_err_t event_handler(void* ctx, system_event_t* event)
{
    const char *TAG = "WIFI_EVENT_HANDLER";
    /* For accessing reason codes in case of disconnection */
    system_event_info_t *info = &event->event_info;
    
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
            ESP_LOGI(TAG, "got ip:%s\n",
                     ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);
            if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
                /*Switch to 802.11 bgn mode */
                esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
            }
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            /*point: the function esp_wifi_wps_start() only get ssid & password
             * so call the function esp_wifi_connect() here
             * */
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
            esp_wifi_wps_disable();
            esp_wifi_set_storage(WIFI_STORAGE_FLASH);
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
            esp_wifi_wps_disable();
            esp_wifi_wps_enable(&wps_config);
            esp_wifi_wps_start(0);
            break;

        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
            esp_wifi_wps_disable();
            esp_wifi_wps_enable(&wps_config);
            esp_wifi_wps_start(0);
            break;

        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN");
            break;

        default:
            break;
    }

    return ESP_OK;
}

static void start_wps(void){
    esp_wifi_disconnect();

    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    ESP_LOGI("WPS", "start wps...");

    esp_wifi_wps_enable(&wps_config);
    esp_wifi_wps_start(0);
}

static void start_wifi(void){
    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&cfg);
    esp_wifi_start();
    esp_wifi_connect();
}