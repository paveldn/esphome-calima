#pragma once

#ifdef USE_ESP32

#include "esphome/core/component.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

namespace esphome {
namespace pax_calima_ble {

class PaxCalimaListener : public esp32_ble_tracker::ESPBTDeviceListener {
 public:
  bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;
};

} // namespace pax_calima_ble
} // namespace esphome

#endif
