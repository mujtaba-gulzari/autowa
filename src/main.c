#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#define SONAR_TRIG_PIN GPIO_NUM_26
#define SONAR_ECHO_PIN GPIO_NUM_25
static const char *TAG = "wifi";

static esp_err_t event_handler(void *ctx, system_event_t *event){
    return ESP_OK;
}

void wifi_init_sta() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t sta_config = {
        .sta = {
            .ssid = "Upper Portion",
            .password = "home1955",
        },
    };
}

void triggerSonar(){
    gpio_set_level(SONAR_TRIG_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    gpio_set_level(SONAR_TRIG_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(2));
    gpio_set_level(SONAR_TRIG_PIN, 0);
}
    

void app_main() {

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);


    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SONAR_TRIG_PIN) | (1ULL << SONAR_ECHO_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    gpio_config_t io_conf_input = {
        .pin_bit_mask = (1ULL << SONAR_ECHO_PIN),
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&io_conf_input);



while(1){
        triggerSonar();

        while(gpio_get_level(SONAR_ECHO_PIN) == 0) {}
        int64_t start_time = esp_timer_get_time();
        while(gpio_get_level(SONAR_ECHO_PIN) == 1) {}
        int64_t end_time = esp_timer_get_time();
        long duration = end_time - start_time; 


        float distance_cm = duration * 0.034 / 2;


        printf("Sonar Reading: %.2f cm\n", distance_cm);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}