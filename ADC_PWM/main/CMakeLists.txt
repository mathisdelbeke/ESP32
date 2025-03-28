idf_component_register(SRCS "ADC_PWM.c"
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS ""
                    REQUIRES driver esp_adc)