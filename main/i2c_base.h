#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"


extern esp_err_t i2c_master_init(void);
extern void i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size,uint8_t address);
extern void i2c_master_read_slave(i2c_port_t i2c_num, uint8_t *data_rd, size_t size,uint8_t address);