
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp32/rom/ets_sys.h"
#include "esp_timer.h"
#include "esp_log.h"
enum dht11_status {
    DHT_CRC_ERROR = -2,
    DHT_TIMEOUT_ERROR,
    DHT11_OK,
};
struct dht11_reading {
    int status;
    int temperature;
    int humidity;
};
extern char sensor_data_str[100];
void DHT11_init(gpio_num_t);
extern esp_err_t dht_read_data(gpio_num_t pin, int16_t *humidity, int16_t *temperature);
struct dht11_reading DHT11_read(void);