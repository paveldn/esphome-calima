#pragma once

#ifdef USE_ESP32

#include <esp_gattc_api.h>
#include "esphome/components/ble_client/ble_client.h"

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
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

  void set_temperature_sensor(sensor::Sensor* temperature) { this->temperature_sensor_ = temperature; }
  void set_humidity_sensor(sensor::Sensor* humidity) { this->humidity_sensor_ = humidity; }
  void set_illuminance_sensor(sensor::Sensor* illuminance) { this->illuminance_sensor_ = illuminance; }
  void set_rotation_sensor(sensor::Sensor* rotation) { this->rotation_sensor_ = rotation; }
  void set_fan_mode_sensor(text_sensor::TextSensor *fan_mode) { this->fan_mode_sensor_ = fan_mode; }
  void set_time_sensor(text_sensor::TextSensor *time_sensor) { this->time_sensor_ = time_sensor; }
 protected:
  void read_sensors_(uint8_t *value, uint16_t value_len);
  void request_read_values_(uint16_t handle);

  sensor::Sensor* temperature_sensor_{nullptr};
  sensor::Sensor* humidity_sensor_{nullptr};
  sensor::Sensor* illuminance_sensor_{nullptr};
  sensor::Sensor* rotation_sensor_{nullptr};
  text_sensor::TextSensor* fan_mode_sensor_{nullptr};
  text_sensor::TextSensor* time_sensor_{nullptr};
  
  uint16_t read_clock_handle_;
  uint16_t read_sensor_handle_;

};

}  // namespace pax_calima
}  // namespace esphome

#endif  // USE_ESP32
