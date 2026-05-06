#include "dis.hpp"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"

namespace cs::midi_ble_nimble {

static const char *dis_manufacturer = "Unknown Manufacturer";
static const char *dis_model        = "Unknown Model";

void setDISManufacturer(const char *name) { dis_manufacturer = name; }
void setDISModel(const char *model)       { dis_model = model; }

static int dis_access_cb(uint16_t, uint16_t,
                         struct ble_gatt_access_ctxt *ctxt, void *arg) {
    const char *str = *(const char **)arg;
    return os_mbuf_append(ctxt->om, str, strlen(str)) == 0
               ? 0
               : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static const ble_uuid16_t dis_svc_uuid = { .u = { .type = BLE_UUID_TYPE_16 }, .value = 0x180A };
static const ble_uuid16_t dis_mfr_uuid = { .u = { .type = BLE_UUID_TYPE_16 }, .value = 0x2A29 };
static const ble_uuid16_t dis_mdl_uuid = { .u = { .type = BLE_UUID_TYPE_16 }, .value = 0x2A24 };

static const struct ble_gatt_chr_def dis_chars[] = {
    {
        .uuid      = &dis_mfr_uuid.u,
        .access_cb = dis_access_cb,
        .arg       = (void *)&dis_manufacturer,
        .flags     = BLE_GATT_CHR_F_READ,
    },
    {
        .uuid      = &dis_mdl_uuid.u,
        .access_cb = dis_access_cb,
        .arg       = (void *)&dis_model,
        .flags     = BLE_GATT_CHR_F_READ,
    },
    {0},
};

static const struct ble_gatt_svc_def dis_svcs[] = {
    {
        .type            = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid            = &dis_svc_uuid.u,
        .characteristics = dis_chars,
    },
    {0},
};

const struct ble_gatt_svc_def *dis_get_service() { return dis_svcs; }

} // namespace cs::midi_ble_nimble
