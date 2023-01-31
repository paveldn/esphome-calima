#pragma once

#if defined(USE_ESP32) || defined(__INTELLISENSE__)

#include <esp_gattc_api.h>
#include "esphome/components/ble_client/ble_client.h"

#include <functional>
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
  void set_humidity_sensor(sensor::Sensor* humidity);
  void set_temperature_sensor(sensor::Sensor* temperature);
  void set_illuminance_sensor(sensor::Sensor* illuminance);
  void set_rotation_sensor(sensor::Sensor* rotation);
  void set_fan_mode_sensor(text_sensor::TextSensor *fan_mode);
  void set_boost_mode(bool mode, uint8_t speed, uint16_t duration);
  void add_boost_mode_callback(std::function<void(bool, uint16_t, uint16_t)>&& callback);
  void add_pin_code(unsigned int pin) { this->pin_code_ = pin; this->has_pin_code_ = true; };
protected:
  enum CalimaSensors : size_t {
    CALIMA_SENSOR_TYPE_HUMIDITY = 0,
	  CALIMA_SENSOR_TYPE_TEMPERATURE,
	  CALIMA_SENSOR_TYPE_ILLUMINANCE,
	  CALIMA_SENSOR_TYPE_ROTATION,
	  CALIMA_SENSOR_TYPE_MAX
  };
  enum CharacteristicsHandlers {
    CALIMA_READ_SENSOR_HANDLER = 0,
    CALIMA_BOOST_MODE_HANDLER,
    CALIMA_SEND_PIN_CODE_HANDLER,
    CALIMA_CALIMA_CHECK_PIN_CODE_HANDLER,
    CALIMA_MAX_HANDLERS,
  };
  void read_sensors_(uint8_t *value, uint16_t value_len);
  void read_boost_(uint8_t *value, uint16_t value_len);
  void request_read_values_(uint16_t handle);
  void send_pin_code_();
  void check_pin_code_();
  bool write_characteristic_(CharacteristicsHandlers handler_type, uint8_t* buffer, size_t buffer_size);
  sensor::Sensor* sensors_[CALIMA_SENSOR_TYPE_MAX]{nullptr};
  sensor::Sensor* humidity_sensor_{nullptr};
  sensor::Sensor* illuminance_sensor_{nullptr};
  sensor::Sensor* rotation_sensor_{nullptr};
  text_sensor::TextSensor* fan_mode_sensor_{nullptr};
  bool has_sensors_{false};
  bool has_fan_{false};
  CallbackManager<void(bool, uint16_t, uint16_t)> boost_mode_callback_{};
  uint16_t handlers_[CALIMA_MAX_HANDLERS]{0};
  unsigned int pin_code_{0};
  bool has_pin_code_{false};

};

}  // namespace pax_calima
}  // namespace esphome

#endif  // USE_ESP32
