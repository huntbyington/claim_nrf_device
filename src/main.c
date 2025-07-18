#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <nrf_modem_at.h>
#include <modem/nrf_modem_lib.h>
#include <modem/at_monitor.h>
#include <modem/modem_info.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define DEBOUNCE_TIME 20

#define SW1_NODE DT_ALIAS(sw1)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW1_NODE, gpios);

static struct gpio_callback button_cb_data;
static struct k_timer debounce_timer;

void send_at_command(void)
{
    char response[256];
    int ret;

    // Send an AT command and get the response
    ret = nrf_modem_at_cmd(response, sizeof(response), "AT%%ATTESTTOKEN");
    if (ret == 0)
    {
        // Success, response now contains the modem's reply
        LOG_INF("Modem response: %s\n", response);
    }
    else
    {
        // Error
        LOG_ERR("Failed to send AT command, error: %d\n", ret);
    }
}

static void button_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    // Wait to prevent incorrectly doubling inputs
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_DISABLE);
    k_timer_start(&debounce_timer, K_MSEC(DEBOUNCE_TIME), K_NO_WAIT);

    send_at_command();
}

static void debounce_timer_cb(struct k_timer *timer)
{
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);
}

int main(void)
{
    if (nrf_modem_lib_init())
    {
        LOG_ERR("Failed to initialize modem library");
        return -1;
    }

    // Initialize button port
    if (!device_is_ready(button.port))
    {
        LOG_ERR("Failed to initialize button");
        return -1;
    }
    gpio_flags_t flags = GPIO_INPUT | GPIO_PULL_UP;
    gpio_pin_configure_dt(&button, flags);

    // Configure button interrupt and debounce
    k_timer_init(&debounce_timer, debounce_timer_cb, NULL);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);
    gpio_init_callback(&button_cb_data, button_handler, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
}