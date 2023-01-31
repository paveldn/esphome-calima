#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "calima.h"
#include <math.h>   

namespace esphome {
namespace pax_calima {

// Services
esp32_ble_tracker::ESPBTUUID SERVICE_PAX_STATUS(esp32_ble_tracker::ESPBTUUID::from_raw("1a46a853-e5ed-4696-bac0-70e346884a26"));
esp32_ble_tracker::ESPBTUUID SERVICE_PAX_CONFIG(esp32_ble_tracker::ESPBTUUID::from_raw("c119e858-0531-4681-9674-5a11f0e53bb4"));
esp32_ble_tracker::ESPBTUUID SERVICE_PAX_CONNECTION(esp32_ble_tracker::ESPBTUUID::from_raw("e6834e4b-7b3a-48e6-91e4-f1d005f564d3"));
// Characteristics
esp32_ble_tracker::ESPBTUUID CHARACTERISTIC_SENSOR_DATA(esp32_ble_tracker::ESPBTUUID::from_raw("528b80e8-c47a-4c0a-bdf1-916a7748f412"));
esp32_ble_tracker::ESPBTUUID CHARACTERISTIC_CLOCK(esp32_ble_tracker::ESPBTUUID::from_raw("6dec478e-ae0b-4186-9d82-13dda03c0682"));
esp32_ble_tracker::ESPBTUUID CHARACTERISTIC_BOOST(esp32_ble_tracker::ESPBTUUID::from_raw("118c949c-28c8-4139-b0b3-36657fd055a9"));
esp32_ble_tracker::ESPBTUUID CHARACTERISTIC_PIN_CODE(esp32_ble_tracker::ESPBTUUID::from_raw("4cad343a-209a-40b7-b911-4d9b3df569b2"));
esp32_ble_tracker::ESPBTUUID CHARACTERISTIC_PIN_CONFIRMATION(esp32_ble_tracker::ESPBTUUID::from_raw("d1ae6b70-ee12-4f6d-b166-d2063dcaffe1"));
static const char *const TAG = "pax_calima";

const char hexmap[] =
    "00" "01" "02" "03" "04" "05" "06" "07" "08" "09" "0A" "0B" "0C" "0D" "0E" "0F"
    "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "1A" "1B" "1C" "1D" "1E" "1F"
    "20" "21" "22" "23" "24" "25" "26" "27" "28" "29" "2A" "2B" "2C" "2D" "2E" "2F"
    "30" "31" "32" "33" "34" "35" "36" "37" "38" "39" "3A" "3B" "3C" "3D" "3E" "3F"
    "40" "41" "42" "43" "44" "45" "46" "47" "48" "49" "4A" "4B" "4C" "4D" "4E" "4F"
    "50" "51" "52" "53" "54" "55" "56" "57" "58" "59" "5A" "5B" "5C" "5D" "5E" "5F"
    "60" "61" "62" "63" "64" "65" "66" "67" "68" "69" "6A" "6B" "6C" "6D" "6E" "6F"
    "70" "71" "72" "73" "74" "75" "76" "77" "78" "79" "7A" "7B" "7C" "7D" "7E" "7F"
    "80" "81" "82" "83" "84" "85" "86" "87" "88" "89" "8A" "8B" "8C" "8D" "8E" "8F"
    "90" "91" "92" "93" "94" "95" "96" "97" "98" "99" "9A" "9B" "9C" "9D" "9E" "9F"
    "A0" "A1" "A2" "A3" "A4" "A5" "A6" "A7" "A8" "A9" "AA" "AB" "AC" "AD" "AE" "AF"
    "B0" "B1" "B2" "B3" "B4" "B5" "B6" "B7" "B8" "B9" "BA" "BB" "BC" "BD" "BE" "BF"
    "C0" "C1" "C2" "C3" "C4" "C5" "C6" "C7" "C8" "C9" "CA" "CB" "CC" "CD" "CE" "CF"
    "D0" "D1" "D2" "D3" "D4" "D5" "D6" "D7" "D8" "D9" "DA" "DB" "DC" "DD" "DE" "DF"
    "E0" "E1" "E2" "E3" "E4" "E5" "E6" "E7" "E8" "E9" "EA" "EB" "EC" "ED" "EE" "EF"
    "F0" "F1" "F2" "F3" "F4" "F5" "F6" "F7" "F8" "F9" "FA" "FB" "FC" "FD" "FE" "FF";

std::string buf_to_hex(const uint8_t *buffer, size_t size)
{
  if (size == 0)
    return "";
  std::string raw(size * 3 - 1, ' ');
  for (size_t i = 0; i < size; ++i)
  {
    const char *p = hexmap + (buffer[i] * 2);
    raw[3 * i] = p[0];
    raw[3 * i + 1] = p[1];
  }
  return raw;
}


void PaxCalima::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
  ESP_LOGV(TAG, "Pax Calima BT event: %d", event);
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      if (param->open.status == ESP_GATT_OK) {
        ESP_LOGI(TAG, "Connected!");
      }
      break;
    }

    case ESP_GATTC_DISCONNECT_EVT: {
      ESP_LOGI(TAG, "Disconnected!");
      break;
    }

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      static const std::pair<esp32_ble_tracker::ESPBTUUID, esp32_ble_tracker::ESPBTUUID> characteristics_uuids[CALIMA_MAX_HANDLERS] = {
        {CHARACTERISTIC_SENSOR_DATA, SERVICE_PAX_STATUS},           // CALIMA_READ_SENSOR_HANDLER
        {CHARACTERISTIC_BOOST, SERVICE_PAX_CONFIG},                 // CALIMA_BOOST_MODE_HANDLER
        {CHARACTERISTIC_PIN_CODE, SERVICE_PAX_CONNECTION},          // CALIMA_SEND_PIN_CODE_HANDLER
        {CHARACTERISTIC_PIN_CONFIRMATION, SERVICE_PAX_CONNECTION},  // CALIMA_CALIMA_CHECK_PIN_CODE_HANDLER
      };
      for (size_t ch_index = 0; ch_index < CALIMA_MAX_HANDLERS; ch_index++) {
        this->handlers_[ch_index] = 0;
        esp32_ble_client::BLECharacteristic *chr = this->parent()->get_characteristic(characteristics_uuids[ch_index].second, characteristics_uuids[ch_index].first);
        if (chr == nullptr) {
          ESP_LOGW(TAG, "No characteristic found at service %s char %s", characteristics_uuids[ch_index].first.to_string().c_str(),
                 characteristics_uuids[ch_index].second.to_string().c_str());
        } else {
          this->handlers_[ch_index] = chr->handle;
        }
      }
      this->node_state = esp32_ble_tracker::ClientState::ESTABLISHED;
	    //this->send_pin_code_();
      this->request_read_values_(this->handlers_[CALIMA_READ_SENSOR_HANDLER]);
      break;
    }

    case ESP_GATTC_READ_CHAR_EVT: {
      if (param->read.conn_id != this->parent()->get_conn_id())
        break;
      if (param->read.status != ESP_GATT_OK) {
        ESP_LOGW(TAG, "Error reading char at handle 0x%04X, status=%d", param->read.handle, param->read.status);
        break;
      }
      ESP_LOGD(TAG, "Received data, handle 0x%04X, data: %s", param->read.handle, buf_to_hex(param->read.value, param->read.value_len).c_str());
      if (param->read.handle == this->handlers_[CALIMA_READ_SENSOR_HANDLER]) {
        read_sensors_(param->read.value, param->read.value_len);
        this->request_read_values_(this->handlers_[CALIMA_BOOST_MODE_HANDLER]);
      } else if (param->read.handle == this->handlers_[CALIMA_BOOST_MODE_HANDLER]) {
        read_boost_(param->read.value, param->read.value_len);
        this->parent()->set_enabled(false);
      } else {
        ESP_LOGW(TAG, "Unknown characteristic read %d, datya size %d", param->read.handle, param->read.value_len);
      }
      break;
    }

    default:
      break;
  }
}

constexpr size_t SENSOR_STRUCTURE_SIZE = 12;

void PaxCalima::read_sensors_(uint8_t *value, uint16_t value_len) {
  if (value_len < SENSOR_STRUCTURE_SIZE)
  {
	ESP_LOGW(TAG, "Wrong structure size %d expected %d", value_len, SENSOR_STRUCTURE_SIZE);
	return;
  }
  if (this->sensors_[CALIMA_SENSOR_TYPE_HUMIDITY] != nullptr)
  {
	uint16_t sensor = value[0] + (value[1] << 8);
	float sensor_val = (sensor == 0) ? 0.0f : log2(sensor) * 10.0f;
	this->sensors_[CALIMA_SENSOR_TYPE_HUMIDITY]->publish_state(sensor_val);
  }
  if (this->sensors_[CALIMA_SENSOR_TYPE_TEMPERATURE] != nullptr)
  {
	uint16_t sensor = value[2] + (value[3] << 8);
	this->sensors_[CALIMA_SENSOR_TYPE_TEMPERATURE]->publish_state(sensor / 4.0f);
  }
  if (this->sensors_[CALIMA_SENSOR_TYPE_ILLUMINANCE] != nullptr) {
	uint16_t sensor = value[4] + (value[5] << 8);
	this->sensors_[CALIMA_SENSOR_TYPE_ILLUMINANCE]->publish_state(sensor);
  }
  if (this->sensors_[CALIMA_SENSOR_TYPE_ROTATION] != nullptr) {
	uint16_t sensor = value[6] + (value[7] << 8);
	this->sensors_[CALIMA_SENSOR_TYPE_ROTATION]->publish_state(sensor);
  }
  if (this->fan_mode_sensor_ != nullptr)
  {
	uint8_t mode = value[8];
	if ((mode >> 4) & 1 == 1)
	  this->fan_mode_sensor_->publish_state("Boost");
    else if ((mode & 3) == 1)
	  this->fan_mode_sensor_->publish_state("Trickle ventilation");
    else if ((mode & 3) == 2)
	  this->fan_mode_sensor_->publish_state("Light ventilation");
    else if ((mode & 3) == 3)
	  this->fan_mode_sensor_->publish_state("Humidity ventilation");
	else
	  this->fan_mode_sensor_->publish_state("Off");
  }
}

constexpr size_t BOOST_STRUCTURE_SIZE = 5;

void PaxCalima::read_boost_(uint8_t *value, uint16_t value_len) {
  if (value_len < BOOST_STRUCTURE_SIZE)
  {
	ESP_LOGW(TAG, "Wrong structure size %d expected %d", value_len, BOOST_STRUCTURE_SIZE);
	return;
  }
  bool state = value[0] > 0;
  uint16_t speed = value[1] + (value[2] << 8);
  uint16_t time = value[3] + (value[4] << 8);
  ESP_LOGD(TAG, "Boost mode update: status %s, speed: %d rpm, time: %d seconds", state ? "On" : "Off", speed * 25, time);
  this->boost_mode_callback_.call(state, speed, time);
}

void PaxCalima::request_read_values_(uint16_t handle) {
  auto status = esp_ble_gattc_read_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                        handle, ESP_GATT_AUTH_REQ_NONE);
  if (status) {
    ESP_LOGW(TAG, "Error sending read request for handle 0x%04X, status=%d", handle, status);
  }
}

void PaxCalima::send_pin_code_() {
  ESP_LOGV(TAG, "PaxCalima::send_pin_code_ step1");
  if (this->has_pin_code_ && (this->handlers_[CALIMA_SEND_PIN_CODE_HANDLER] != 0))
  {
    ESP_LOGV(TAG, "PaxCalima::send_pin_code_ pin: %u", this->pin_code_);
    uint8_t buffer[4];	
	  buffer[0] = this->pin_code_ & 0xFF;
	  buffer[1] = (this->pin_code_ >> 8) & 0xFF;
	  buffer[2] = (this->pin_code_ >> 16)& 0xFF;
	  buffer[3] = (this->pin_code_ >> 24)& 0xFF;
    this->write_characteristic_(CALIMA_SEND_PIN_CODE_HANDLER, buffer, sizeof(buffer));
	  ESP_LOGV(TAG, "PaxCalima::send_pin_code_ step3");
  }
  this->check_pin_code_();
}

void PaxCalima::check_pin_code_() {
  auto status = esp_ble_gattc_read_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                        this->handlers_[CALIMA_CALIMA_CHECK_PIN_CODE_HANDLER], ESP_GATT_AUTH_REQ_NONE);
  if (status) {
    ESP_LOGW(TAG, "Error sending read request for handle 0x%04X, status=%d", this->handlers_[CALIMA_CALIMA_CHECK_PIN_CODE_HANDLER], status);
  }
  this->request_read_values_(this->handlers_[CALIMA_READ_SENSOR_HANDLER]);
}

PaxCalima::PaxCalima() : PollingComponent()
{
}

void PaxCalima::dump_config() {
  ESP_LOGCONFIG(TAG, "Pax Calima clinet: %s", this->parent_->address_str().c_str()); 
  ESP_LOGCONFIG(TAG, "  Temperature sensor: %s", (this->sensors_[CALIMA_SENSOR_TYPE_TEMPERATURE] != nullptr) ? "Yes" : "No");
  ESP_LOGCONFIG(TAG, "  Humidity sensor: %s", (this->sensors_[CALIMA_SENSOR_TYPE_HUMIDITY] != nullptr) ? "Yes" : "No");
  ESP_LOGCONFIG(TAG, "  Illuminance sensor: %s", (this->sensors_[CALIMA_SENSOR_TYPE_ILLUMINANCE] != nullptr) ? "Yes" : "No");
  ESP_LOGCONFIG(TAG, "  Rotation speed sensor: %s", (this->sensors_[CALIMA_SENSOR_TYPE_ROTATION] != nullptr) ? "Yes" : "No");
  ESP_LOGCONFIG(TAG, "  Fan mode sensor: %s", (this->fan_mode_sensor_ != nullptr) ? "Yes" : "No");
  ESP_LOGCONFIG(TAG, "  Boost fan: %s", this->has_fan_ ? "Yes" : "No");
  ESP_LOGCONFIG(TAG, "  Handlers: %d %d %d %d", this->handlers_[0], this->handlers_[1], this->handlers_[2], this->handlers_[3]);
}

void PaxCalima::update() {
  ESP_LOGV(TAG, "PaxCalima::update");
  if (this->node_state != esp32_ble_tracker::ClientState::ESTABLISHED) {
    if (!parent()->enabled) {
      ESP_LOGW(TAG, "Reconnecting to device");
      parent()->set_enabled(true);
      parent()->connect();
    } else {
      ESP_LOGW(TAG, "Connection in progress");
    }
  }
  else
    this->request_read_values_(this->handlers_[CALIMA_READ_SENSOR_HANDLER]);
}

void PaxCalima::add_boost_mode_callback(std::function<void(bool, uint16_t, uint16_t)>&& callback) {
  this->boost_mode_callback_.add(std::move(callback)); 
  has_fan_=true;
}

void PaxCalima::set_humidity_sensor(sensor::Sensor* humidity) {
  this->sensors_[CALIMA_SENSOR_TYPE_HUMIDITY] = humidity;
  this->has_sensors_=true;
}

void PaxCalima::set_temperature_sensor(sensor::Sensor* temperature) {
  this->sensors_[CALIMA_SENSOR_TYPE_TEMPERATURE] = temperature; 
  this->has_sensors_=true;
}

void PaxCalima::set_illuminance_sensor(sensor::Sensor* illuminance) {
  this->sensors_[CALIMA_SENSOR_TYPE_ILLUMINANCE] = illuminance; 
  this->has_sensors_=true;
}

void PaxCalima::set_rotation_sensor(sensor::Sensor* rotation) {
  this->sensors_[CALIMA_SENSOR_TYPE_ROTATION] = rotation; 
  this->has_sensors_=true;
}

void PaxCalima::set_fan_mode_sensor(text_sensor::TextSensor *fan_mode) {
  this->fan_mode_sensor_ = fan_mode;
  this->has_sensors_=true;
}
 
void PaxCalima::set_boost_mode(bool mode, uint8_t speed, uint16_t duration) {
  ESP_LOGD(TAG, "Boost mode: %s, speed: %d, duration: %d", mode ? "On" : "Off", speed, duration);
  uint8_t buffer[BOOST_STRUCTURE_SIZE];
  buffer[0] = mode ? 0x01 : 0x00;
  uint16_t converted_speed = speed * 25;
  buffer[1] = converted_speed & 0xFF;
  buffer[2] = converted_speed >> 8;
  buffer[3] = duration & 0xFF;
  buffer[4] = duration >> 8;
  this->write_characteristic_(CALIMA_BOOST_MODE_HANDLER, buffer, BOOST_STRUCTURE_SIZE);
}
 
bool PaxCalima::write_characteristic_(CharacteristicsHandlers handler_type, uint8_t* buffer, size_t buffer_size) {
  if (handler_type >= CALIMA_MAX_HANDLERS) {
    ESP_LOGW(TAG, "Wrong characteristic type %d", handler_type);
    return false;
  }
  if (this->handlers_[handler_type] == 0) {
    ESP_LOGW(TAG, "Charactersistic unavailable: %d", handler_type);
    return false;
  }
  ESP_LOGV(TAG, "Writing characteristic: %d, data: %s", handler_type, buf_to_hex(buffer, buffer_size).c_str());
  esp_err_t status = esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(),
  									 this->handlers_[handler_type], buffer_size, buffer,
  									 ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
  if (status != ESP_OK) {
    ESP_LOGW(TAG, "Failed to write characteristic %d, error #%d: %s", handler_type, status, esp_err_to_name(status));
    return false;
  } 
  return true;
}

} // namespace pax_calima
} // namespace esphome 
