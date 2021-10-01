#include "ultrasonic.h"


#define TRIGGER_GPIO 16
#define ECHO_GPIO 17
#define MAX_DISTANCE_CM 500 
#define ROUNDTRIP_CM 58

#define DELAY_10US 10
#define DELAY_4US 4
#define PING_TIMEOUT 6000
uint32_t distance1;

esp_err_t ultrasonic_init( ultrasonic_sensor_t *dev)
{
    gpio_set_direction(dev->echo_pin,GPIO_MODE_INPUT);
    gpio_set_direction(dev->trigger_pin,GPIO_MODE_OUTPUT);
    return gpio_set_level(dev->trigger_pin,0);
}
esp_err_t ultrasonic_measure_raw( ultrasonic_sensor_t *dev, uint32_t max_time, uint32_t *time_us)
{
    ESP_ERROR_CHECK(gpio_set_level(dev->trigger_pin,0));
    ets_delay_us(DELAY_4US);
    ESP_ERROR_CHECK(gpio_set_level(dev->trigger_pin,1));
    ets_delay_us(DELAY_10US);
    ESP_ERROR_CHECK(gpio_set_level(dev->trigger_pin,0));
    if (gpio_get_level(dev->echo_pin) == 1)
        return ESP_FAIL;
    int64_t start = esp_timer_get_time();
    while(!gpio_get_level(dev->echo_pin)) //wait for trigger
    {
        if((esp_timer_get_time() - start) > PING_TIMEOUT)
            return ESP_FAIL;
    }
    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;
    while(gpio_get_level(dev->echo_pin))
    {
        time = esp_timer_get_time();
        if( (time - echo_start)> max_time)
            return ESP_FAIL;
    }
    *time_us = time - echo_start;
    
    return ESP_OK;
}
esp_err_t ultrasonice_distance_cm(ultrasonic_sensor_t *dev,uint32_t max_distance, uint32_t *distance)
{
    uint32_t time_us ;
   if ( ultrasonic_measure_raw(dev,max_distance*ROUNDTRIP_CM,&time_us) == ESP_OK )
    {
        *distance = time_us/ROUNDTRIP_CM;
        printf("%d\n", time_us);
        return ESP_OK;
    }
    else    
        return ESP_FAIL;
}
void hc_sr04_task(void *arg)
{   

    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };
    ultrasonic_init(&sensor);
    while(1)
    {
        
        esp_err_t res = ultrasonice_distance_cm(&sensor, MAX_DISTANCE_CM, &distance1);
        if( res == ESP_OK)
            ESP_LOGI("HC_SR04","Distance: %dcm\n",distance1);
        else 
            { ESP_LOGW("HC_SR04","Unable to read Ultrasonic Sensor\n");}
        vTaskDelay(500/portTICK_RATE_MS);
    }
}