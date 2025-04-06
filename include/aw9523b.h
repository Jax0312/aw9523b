#include "driver/i2c_master.h"
#include <stdint.h>

#define AW9523B_I2C_ADDR  0x58  // Default I2C address
#define AW9523B_REG_INPUT0  0x00
#define AW9523B_REG_INPUT1  0x01
#define AW9523B_REG_OUTPUT0 0x02
#define AW9523B_REG_OUTPUT1 0x03
#define AW9523B_REG_CONFIG0 0x04
#define AW9523B_REG_CONFIG1 0x05
#define AW9523B_REG_LEDMODE0 0x12
#define AW9523B_REG_LEDMODE1 0x13
#define AW9523B_REG_INTR0 0x06
#define AW9523B_REG_INTR1 0x07
#define AW9523B_REG_ID  0x10
#define AW9523B_REG_SOFTRESET 0x7F
#define AW9523B_REG_GCR 0x11

typedef struct {
    i2c_master_dev_handle_t dev_handle;
} aw9523b_t;

esp_err_t aw9523b_init(aw9523b_t *dev, i2c_master_bus_handle_t *bus_handle, uint8_t address);
esp_err_t aw9523b_set_pin_mode(aw9523b_t *dev, uint8_t pin, gpio_mode_t mode);
esp_err_t aw9523b_set_all_mode(aw9523b_t *dev, gpio_mode_t mode);
esp_err_t aw9523b_write_pin(aw9523b_t *dev, uint8_t pin, bool value);
bool aw9523b_read_pin(aw9523b_t *dev, uint8_t pin);
esp_err_t aw9523b_set_pin_led_mode(aw9523b_t *dev, uint8_t pin, bool enable);
esp_err_t aw9523b_reset(aw9523b_t *dev);
esp_err_t aw9523b_openDrainPort0(aw9523b_t *dev, bool drain);
esp_err_t aw9523b_set_pin_interrupt(aw9523b_t *dev, uint8_t pin, bool enable);
esp_err_t aw9523b_set_all_interrupt(aw9523b_t *dev, bool enable);

