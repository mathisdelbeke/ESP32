#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED1_PIN GPIO_NUM_2   
#define BUTTON_PIN GPIO_NUM_16

uint8_t led1_state = 0;
volatile uint32_t last_press_time = 0;  
static const char *TAG = "INTERRUPT";

// Interrupt handler function (ISR)
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    // Debounce the button
    uint32_t current_time = xTaskGetTickCountFromISR();
    if (current_time - last_press_time > pdMS_TO_TICKS(200)) {
        last_press_time = current_time;
        led1_state = !led1_state;
        gpio_set_level(LED1_PIN, led1_state);
    }
}

void gpio_task(void *arg)
{
    while (1)
    {
        ESP_LOGI(TAG, "GPIO %d state: %d", led1_state, led1_state);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay to reduce CPU load
    }
}

void app_main(void) 
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED1_PIN),    
        .mode = GPIO_MODE_OUTPUT,                // Set as output
        .pull_up_en = GPIO_PULLUP_DISABLE,       // No pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE,   // No pull-down resistor
        .intr_type = GPIO_INTR_DISABLE           // No interrupt
    };
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);  
    io_conf.mode = GPIO_MODE_INPUT;                // Set as input
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;       // Enable pull-up resistor
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  // No pull-down resistor
    io_conf.intr_type = GPIO_INTR_NEGEDGE;         // Falling edge interrupt
    gpio_config(&io_conf);

    // Install and attach ISR 
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)BUTTON_PIN);

    // Background task
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    while(1) {
        // Other work
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
