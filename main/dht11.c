#include "dht11.h"
static const gpio_num_t dht_gpio = 4;

#define DHT_DATA_BITS (40)
#define DHT_DATA_BYTES (5)
#define DHT_TIMER_INTERVAL (2)
#define DELAY_20MS 20000
#define DELAY_40US 40

static const char *TAG1 = "DHT";
static esp_err_t dht_await_pin;
static esp_err_t dht_await_pin_state (gpio_num_t pin, uint32_t timeout, int expected_pin_state, uint32_t *duration)
{
    gpio_set_direction(pin,GPIO_MODE_INPUT);
    for( int i = 0; i <  timeout ; i += DHT_TIMER_INTERVAL)
    {
        ets_delay_us(DHT_TIMER_INTERVAL);
        if( gpio_get_level(pin) == expected_pin_state)
        {
            if (duration)
                *duration = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_TIMEOUT;
}
static esp_err_t dht_fetch_data(gpio_num_t pin, uint8_t data[DHT_DATA_BYTES])
{
    uint32_t low_duration;
    uint32_t high_duration; 
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin,GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin,0);
    ets_delay_us(DELAY_20MS);
    gpio_set_level(pin,1);
    ets_delay_us(DELAY_40US);
    dht_await_pin_state(pin, 40, 0, NULL);
    dht_await_pin_state(pin, 88, 1, NULL);
    dht_await_pin_state(pin, 88, 0, NULL);
    for ( int i = 0; i < DHT_DATA_BITS; i++)
    {
        dht_await_pin_state(pin, 60, 1, &low_duration);
        dht_await_pin_state(pin, 80, 0, &high_duration);
        if(!(i%8))
            data[i/8] = 0;
        data[i/8] |= (high_duration > low_duration)  << (7 - (i%8));  
    }
    return ESP_OK;
}
static int16_t convert_data(uint8_t msb, uint8_t lsb)
{
    uint16_t data;
    data = msb*10;
    return data;
}
esp_err_t dht_read_data(gpio_num_t pin, int16_t *humidity, int16_t *temperature)
{
    uint8_t data[5]= {0,0,0,0,0};
    gpio_set_direction(pin,GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin,1);
    esp_err_t check = dht_fetch_data(pin,data);
    gpio_set_direction(pin,GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin,1);
    if (data[4] != ((data[0]+data[1]+data[2]+data[3])&0xFF))
        {
            ESP_LOGE(TAG1,"Checksum failed\n");
            return ESP_ERR_INVALID_CRC;
        }
    *humidity = convert_data(data[0],data[1]);
    *temperature = convert_data(data[2],data[3]);
    return ESP_OK;
}

void dht_test(void *arg)
{
    int16_t humidity = 0;
    int16_t temperature = 0;
    while(1)
    {
        if(dht_read_data(dht_gpio,&humidity,&temperature) == ESP_OK)
            ESP_LOGI(TAG1,"Humidity:%d and Temperature:%d\n",humidity/10,temperature/10);
        else 
            printf("Cannot read sensor data\n");
        vTaskDelay(2000/portTICK_RATE_MS);
    }
}