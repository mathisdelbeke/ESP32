idf_component_register(SRCS "GPIO.c" "hello_world_main.c"
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS 
                    REQUIRES driver driver freertos"")
