/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <esp_matter.h>
#include <led_driver.h>

#include <app_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

#define LED_GPIO_PIN GPIO_NUM_2

static bool is_led_initialized = false;

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_on_off(esp_matter_attr_val_t *val)
{
    if (!is_led_initialized) {
        ESP_LOGI(TAG, "Initializing the GPIO LED!");
        esp_rom_gpio_pad_select_gpio(LED_GPIO_PIN);
        gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);
        is_led_initialized = true;
    }

    ESP_LOGI(TAG, "Changing the GPIO LED to %s", val->val.b ? "ON" : "OFF");
    esp_err_t err = gpio_set_level(LED_GPIO_PIN, val->val.b); 
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set GPIO level: %s", esp_err_to_name(err));
    }

    return err;
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;

    /* Do stuff here */
    if (endpoint_id == light_endpoint_id) {
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                err = app_driver_light_set_on_off(val);
            }
        }
    }

    return err;
}

app_driver_handle_t app_driver_light_init()
{
    /* Initialize LED */
    led_driver_config_t config = led_driver_get_config();
    led_driver_handle_t handle = led_driver_init(&config);
    return (app_driver_handle_t)handle;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = NULL;
    attribute_t *attribute = NULL;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    /* Setting power */
    cluster = cluster::get(endpoint, OnOff::Id);
    attribute = attribute::get(cluster, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_light_set_on_off(&val);

    return err;
}
