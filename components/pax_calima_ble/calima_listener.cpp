#include "calima_listener.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace pax_calima_ble {

static const char *const TAG = "pax_calima_ble";

bool PaxCalimaListener::parse_device(const esp32_ble_tracker::ESPBTDevice &device) {
  if (device.address_str().rfind("58:2B:DB", 0) == 0) {
    ESP_LOGD(TAG, "Found Pax Calima compatible device, name %s, MAC: %s", device.get_name().empty() ? "<empty>" : device.get_name().c_str(),
            device.address_str().c_str());
  }
  return false;
}

} // namespace pax_calima_ble
} // namespace esphome

#endif
