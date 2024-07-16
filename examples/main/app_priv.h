#include <esp_log.h>
#include <esp_matter.h>
#include <app_priv.h>

using namespace esp_matter;

static const char *TAG = "app_main";
uint16_t light_endpoint_id;

extern "C" void app_main()
{
    ESP_LOGI(TAG, "Initializing Matter");

    /* Initialize the ESP Matter stack */
    node_t *node = node::get();
    if (!node) {
        ESP_LOGE(TAG, "Failed to get node");
        return;
    }

    /* Initialize the light driver */
    app_driver_light_init();

    /* Initialize the light endpoint */
    endpoint_config_t ep_config;
    ep_config.device_type_id = 0x0100; // On/Off Light
    ep_config.device_version = 1;
    ep_config.endpoint_id = 1;
    ep_config.device_name = "Light";
    light_endpoint_id = endpoint::add(node, &ep_config);

    if (!light_endpoint_id) {
        ESP_LOGE(TAG, "Failed to create light endpoint");
        return;
    }

    /* Add On/Off cluster to the light endpoint */
    cluster_t *cluster = cluster::add(light_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, NULL);
    if (!cluster) {
        ESP_LOGE(TAG, "Failed to add OnOff cluster");
        return;
    }

    /* Set default attribute values */
    app_driver_light_set_defaults(light_endpoint_id);

    ESP_LOGI(TAG, "Matter initialization completed");
}
