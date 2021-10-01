#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp32/rom/ets_sys.h"
#include "esp_timer.h"
#include "esp_log.h"


typedef struct 
{
    gpio_num_t echo_pin;
    gpio_num_t trigger_pin;
}ultrasonic_sensor_t;

extern uint32_t distance1;
extern esp_err_t ultrasonic_init( ultrasonic_sensor_t *dev);
extern esp_err_t ultrasonice_distance_cm(ultrasonic_sensor_t *dev,uint32_t max_distance, uint32_t *distance);
