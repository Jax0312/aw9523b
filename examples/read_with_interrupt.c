#include "aw9523b.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define I2C_MASTER_SDA GPIO_NUM_40
#define I2C_MASTER_SCL GPIO_NUM_39
#define I2C_MASTER_PORT I2C_NUM_0
#define ISR_PIN GPIO_NUM_8

static QueueHandle_t gpio_evt_queue = NULL;

// Interrupt callback
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void app_main(void) {
    // Interrupt queue
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // Configure I2C Bus
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_MASTER_SCL,
        .sda_io_num = I2C_MASTER_SDA,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;
    
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};

    // config interrupt pin, interrupt pin should be pulled up, the pin is active low
    io_conf.intr_type = GPIO_INTR_NEGEDGE;    
    io_conf.pin_bit_mask = 1ULL << ISR_PIN;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(ISR_PIN, gpio_isr_handler, (void*) ISR_PIN);

    aw9523b_t aw9523b;
    // Initialise AW95236B passing the I2C bus and address
    if (aw9523b_init(&aw9523b, &bus_handle, 0x5B) == ESP_OK) {
        printf("AW9523B initialized\n");
        aw9523b_set_pin_mode(&aw9523b, 0, GPIO_MODE_INPUT);  // Set P0.0 as input
        // Enable interrupt on P0.0
        if (aw9523b_set_pin_interrupt(&aw9523b, 0, true) != ESP_OK) {
            return;
        }
        while(1) {
            uint32_t io_num;
            // Check queue for interrupt event
            if (xQueueReceive(gpio_evt_queue, &io_num, 0)) {
                // Interrupt is only cleared when GPIO is read
                printf("P0: %d\n", !aw9523b_read_pin(&aw9523b, 0));
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

