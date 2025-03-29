#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "string.h"

#define UART_NUM UART_NUM_0  
#define TXD_PIN GPIO_NUM_1
#define RXD_PIN GPIO_NUM_3
#define BUF_SIZE (1024)

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Install UART driver
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void send_data(const char* data) {
    uart_write_bytes(UART_NUM, data, strlen(data));
}

void receive_data() {
    char data[BUF_SIZE];
    int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 100 / portTICK_PERIOD_MS);
    if (len > 0) {
        data[len] = '\0';  // Null terminate the received string
        printf("%hhn\n", data);
    }
}

void app_main(void) {
    uart_init();
    while(1) {
        send_data("HELLO\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        receive_data();
    }
}
