#include "include/aw9523b.h"
#include "esp_log.h"

static const char *TAG = "AW9523B";

static esp_err_t aw9523b_write_register(aw9523b_t *dev, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    return i2c_master_write_to_device(dev->i2c_port, dev->address, data, 2, 1000 / portTICK_PERIOD_MS);
}

static esp_err_t aw9523b_read_register(aw9523b_t *dev, uint8_t reg, uint8_t *value) {
    return i2c_master_write_read_device(dev->i2c_port, dev->address, &reg, 1, value, 1, 1000 / portTICK_PERIOD_MS);
}

esp_err_t aw9523b_init(aw9523b_t *dev, i2c_port_t i2c_port, uint8_t address) {
    dev->i2c_port = i2c_port;
    dev->address = address;

    uint8_t id;
    esp_err_t ret = aw9523b_read_register(dev, AW9523B_REG_ID, &id);
    if (ret != ESP_OK || id != 0x23) {
        ESP_LOGE(TAG, "Failed to detect AW9523B");
        return ESP_FAIL;
    }

    aw9523b_set_all_interrupt(dev, false);
    aw9523b_set_all_mode(dev, GPIO_MODE_INPUT);

    return ESP_OK;
}

esp_err_t aw9523b_reset(aw9523b_t *dev) {
    return aw9523b_write_register(dev, AW9523B_REG_SOFTRESET, 0x00);
}

esp_err_t aw9523b_set_pin_mode(aw9523b_t *dev, uint8_t pin, gpio_mode_t mode) {
    uint8_t reg = (pin < 8) ? AW9523B_REG_CONFIG0 : AW9523B_REG_CONFIG1;
    uint8_t shift = (pin % 8);
    
    uint8_t config;
    aw9523b_read_register(dev, reg, &config);
    
    if (mode == GPIO_MODE_OUTPUT) {
        config &= ~(1 << shift);
    } else  {
        config |= (1 << shift);
    }

    return aw9523b_write_register(dev, reg, config);
}

esp_err_t aw9523b_set_all_mode(aw9523b_t *dev, gpio_mode_t mode) {
    if ((aw9523b_write_register(dev, AW9523B_REG_CONFIG0, mode == GPIO_MODE_INPUT ? 255 : 0) == ESP_OK) 
    && aw9523b_write_register(dev, AW9523B_REG_CONFIG1, mode == GPIO_MODE_INPUT ? 255 : 0) == ESP_OK ) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t aw9523b_set_pin_interrupt(aw9523b_t *dev, uint8_t pin, bool enable) {
    uint8_t reg = (pin < 8) ? AW9523B_REG_INTR0 : AW9523B_REG_INTR1;
    uint8_t shift = (pin % 8);

    uint8_t intr;
    aw9523b_read_register(dev, reg, &intr);

    if (enable) {
        intr &= ~(1 << shift);
    } else {
        intr |= (1 << shift);
    }

    return aw9523b_write_register(dev, reg, intr);

}

esp_err_t aw9523b_set_all_interrupt(aw9523b_t *dev, bool enable) {
    if ((aw9523b_write_register(dev, AW9523B_REG_INTR0, enable ? 0 : 255) == ESP_OK) 
    && aw9523b_write_register(dev, AW9523B_REG_INTR1, enable ? 0 : 255) == ESP_OK ) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t aw9523b_write_pin(aw9523b_t *dev, uint8_t pin, bool value) {
    uint8_t reg = (pin < 8) ? AW9523B_REG_OUTPUT0 : AW9523B_REG_OUTPUT1;
    uint8_t shift = (pin % 8);
    
    uint8_t output;
    aw9523b_read_register(dev, reg, &output);

    if (value) {
        output |= (1 << shift);
    } else {
        output &= ~(1 << shift);
    }

    return aw9523b_write_register(dev, reg, output);
}

bool aw9523b_read_pin(aw9523b_t *dev, uint8_t pin) {
    uint8_t reg = (pin < 8) ? AW9523B_REG_INPUT0 : AW9523B_REG_INPUT1;
    uint8_t shift = (pin % 8);
    
    uint8_t input;
    esp_err_t ret = aw9523b_read_register(dev, reg, &input);
    if (ret == ESP_OK) {
        return (input >> shift) & 1;
    } 
    return 0;
}

esp_err_t aw9523b_set_pin_led_mode(aw9523b_t *dev, uint8_t pin, bool enable) {
    uint8_t reg = (pin < 8) ? AW9523B_REG_LEDMODE0 : AW9523B_REG_LEDMODE1;
    uint8_t shift = (pin % 8);
    
    uint8_t led_mode;
    aw9523b_read_register(dev, reg, &led_mode);

    if (enable) {
        led_mode &= ~(1 << shift);
    } else {
        led_mode |= (1 << shift);
    }

    return aw9523b_write_register(dev, reg, led_mode);
}

esp_err_t aw9523b_openDrainPort0(aw9523b_t *dev, bool drain) {
    uint8_t gcr;
    aw9523b_read_register(dev, AW9523B_REG_GCR, &gcr);
    if (drain) {
        gcr |= (1 << 3);
    } else {
        gcr &= ~(1 << 3);
    }

    return aw9523b_write_register(dev, AW9523B_REG_GCR, gcr);
}