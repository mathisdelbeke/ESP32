#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          5  // GPIO where LED is connected
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // 13-bit resolution
#define LEDC_FREQUENCY          5000              // Frequency in Hz

#define ADC_WIDTH               ADC_BITWIDTH_12   // 12-bit resolution
#define ADC_CHANNEL             ADC_CHANNEL_4     // GPIO32 (ADC4)
 
void app_main(void) {
    // Configure the ADC oneshot mode for ADC4
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&adc_config, &adc_handle);

    adc_oneshot_chan_cfg_t adc_channel_cfg = {
        .bitwidth = ADC_WIDTH,
        .atten = ADC_ATTEN_DB_12
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &adc_channel_cfg);

    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configure the LEDC channel
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Start with LED off
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);

    int adc_value = 0;
    int duty_cycle = 0;
    while (1) {
        adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_value);
        duty_cycle = ((adc_value * ((1 << LEDC_DUTY_RES) - 1)) / 4095);

        printf("ADC: %d\tDuty Cycle: %d\n",adc_value, duty_cycle);
        
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty_cycle);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
