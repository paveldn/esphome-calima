#pragma once

#ifdef USE_ESP32

#include <esp_gattc_api.h>
#include "esphome/components/ble_client/ble_client.h"

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace pax_calima {

class PaxCalima : public PollingComponent, public ble_client::BLEClientNode {
 public:
  PaxCalima();

  void dump_config() override;
  void update() override;

  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;

  void set_temperature_sensor(sensor::Sensor* temperature) { temperature_sensor_ = temperature; }
  void set_humidity_sensor(sensor::Sensor* humidity) { humidity_sensor_ = humidity; }
  void set_illuminance_sensor(sensor::Sensor* illuminance) { illuminance_sensor_ = illuminance; }
  void set_rotation_sensor(sensor::Sensor* rotation) { rotation_sensor_ = rotation; }

 protected:
  void read_sensors_(uint8_t *value, uint16_t value_len);
  void request_read_values_();

  sensor::Sensor* temperature_sensor_{nullptr};
  sensor::Sensor* humidity_sensor_{nullptr};
  sensor::Sensor* illuminance_sensor_{nullptr};
  sensor::Sensor* rotation_sensor_{nullptr};
  
  uint16_t read_sensor_handle_;

};

}  // namespace pax_calima
}  // namespace esphome

#endif  // USE_ESP32
