#pragma once
#include "host/ble_gatt.h"

namespace cs::midi_ble_nimble {
    void setDISManufacturer(const char *name);
    void setDISModel(const char *model);
    const struct ble_gatt_svc_def *dis_get_service();
} // namespace cs::midi_ble_nimble
