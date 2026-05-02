#ifdef ESP32
#include <sdkconfig.h>
#if CONFIG_BT_BLE_ENABLED || CONFIG_BT_NIMBLE_ENABLED

#include "ble.h"

#if CS_MIDI_BLE_ESP_IDF_NIMBLE
#include <nimble/nimble_port.h>
#else
#include <nimble/esp_port/esp-hci/include/esp_nimble_hci.h>
#include <nimble/porting/nimble/include/nimble/nimble_port.h>
#endif
#ifdef ESP_PLATFORM
#ifndef CONFIG_IDF_TARGET_ESP32P4
#include <esp_bt.h>
#endif
#include <nvs_flash.h>
#include <soc/soc_caps.h>
#endif
#if defined(CONFIG_ESP_HOSTED_ENABLE_BT_NIMBLE)
#include <esp32-hal-hosted.h>
#endif
#if __has_include(<esp32-hal-bt-mem.h>)
#include <esp32-hal-bt-mem.h>
#endif

#include "ble-macro-fix.h"
#include "util.hpp"

namespace cs::midi_ble_nimble {

inline bool init_hardware() {
#if defined(CONFIG_ESP_HOSTED_ENABLE_BT_NIMBLE)
    // Initialize esp-hosted transport for BLE HCI when explicitly enabled
    ESP_LOGI("CS-BLEMIDI", "Initializing hosted BLE");
    if (!hostedInitBLE()) {
        ESP_LOGE("CS-BLEMIDI", "Failed to initialize ESP-Hosted for BLE");
        return false;
    }
#endif
#if CS_MIDI_BLE_ESP_IDF_NIMBLE
    // Assumes that nvs_flash_init() has already been called.
    CS_CHECK_ESP(nimble_port_init());
#else // fall back to the h2zero/NimBLE-Arduino library
#ifdef ESP_PLATFORM
#if defined(CONFIG_ENABLE_ARDUINO_DEPENDS) && SOC_BT_SUPPORTED
    // make sure the linker includes esp32-hal-bt.c so Arduino init doesn't release BLE memory.
    btStarted();
#endif
    auto nvs_flash_init_rc = nvs_flash_init();
    if (nvs_flash_init_rc == ESP_ERR_NVS_NO_FREE_PAGES ||
        nvs_flash_init_rc == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_flash_init_rc = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_flash_init_rc);
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
#if defined(CONFIG_IDF_TARGET_ESP32)
    bt_cfg.mode = ESP_BT_MODE_BLE;
    bt_cfg.ble_max_conn = CONFIG_BT_NIMBLE_MAX_CONNECTIONS;
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
    bt_cfg.ble_max_act = CONFIG_BT_NIMBLE_MAX_CONNECTIONS;
#else
    bt_cfg.nimble_max_connections = CONFIG_BT_NIMBLE_MAX_CONNECTIONS;
#endif
#if CONFIG_IDF_TARGET_ESP32
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
#endif
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
#if CONFIG_BT_NIMBLE_LEGACY_VHCI_ENABLE
    ESP_ERROR_CHECK(esp_nimble_hci_init());
#endif
#endif
    nimble_port_init();
#endif // #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)

    return true;
}

} // namespace cs::midi_ble_nimble

#endif // CONFIG_BT_BLE_ENABLED
#endif // ESP32