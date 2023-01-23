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


void PaxCalima::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                        esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      if (param->open.status == ESP_GATT_OK) {
        ESP_LOGI(TAG, "Connected!");
      }
      break;
    }

    case ESP_GATTC_DISCONNECT_EVT: {
      ESP_LOGW(TAG, "Disconnected!");
      break;
    }

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      this->read_sensor_handle_ = 0;
      esp32_ble_client::BLECharacteristic *chr = this->parent()->get_characteristic(SERVICE_PAX_STATUS, CHARACTERISTIC_SENSOR_DATA);
      if (chr == nullptr) {
        ESP_LOGW(TAG, "No sensor read characteristic found at service %s char %s", SERVICE_PAX_STATUS.to_string().c_str(),
                 CHARACTERISTIC_SENSOR_DATA.to_string().c_str());
      } else {
        this->read_sensor_handle_ = chr->handle;
      }
      this->read_clock_handle_ = 0;
      chr = this->parent()->get_characteristic(SERVICE_PAX_CONFIG, CHARACTERISTIC_CLOCK);
      if (chr == nullptr) {
        ESP_LOGW(TAG, "No sensor read characteristic found at service %s char %s", SERVICE_PAX_CONFIG.to_string().c_str(),
                 CHARACTERISTIC_CLOCK.to_string().c_str());
      } else {
        this->read_clock_handle_ = chr->handle;
      }
      this->node_state = esp32_ble_tracker::ClientState::ESTABLISHED;
      request_read_values_(this->read_sensor_handle_);
      request_read_values_(this->read_clock_handle_);
      break;
    }

    case ESP_GATTC_READ_CHAR_EVT: {
      if (param->read.conn_id != this->parent()->get_conn_id())
        break;
      if (param->read.status != ESP_GATT_OK) {
        ESP_LOGW(TAG, "Error reading char at handle 0x%04X, status=%d", param->read.handle, param->read.status);
        break;
      }
      ESP_LOGV(TAG, "Received data, handle 0x%04X, data: %s", param->read.handle, buf_to_hex(param->read.value, param->read.value_len).c_str());
      if (param->read.handle == this->read_sensor_handle_) {
        read_sensors_(param->read.value, param->read.value_len);
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
  if (this->temperature_sensor_ != nullptr)
  {
	uint16_t sensor = value[2] + (value[3] << 8);
	this->temperature_sensor_->publish_state(sensor / 4.0f);
  }
  if (this->humidity_sensor_ != nullptr)
  {
	uint16_t sensor = value[0] + (value[1] << 8);
	float sensor_val = (sensor == 0) ? 0.0f : log2(sensor) * 10.0f;
	this->humidity_sensor_->publish_state(sensor_val);
  }
  if (this->illuminance_sensor_ != nullptr) {
	uint16_t sensor = value[4] + (value[5] << 8);
	this->illuminance_sensor_->publish_state(sensor);
  }
  if (this->rotation_sensor_ != nullptr) {
	uint16_t sensor = value[6] + (value[7] << 8);
	this->rotation_sensor_->publish_state(sensor);
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
  parent()->set_enabled(false);
}

void PaxCalima::request_read_values_(uint16_t handle) {
  auto status = esp_ble_gattc_read_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                        handle, ESP_GATT_AUTH_REQ_NONE);
  if (status) {
    ESP_LOGW(TAG, "Error sending read request for handle 0x%04X, status=%d", handle, status);
  }
}

PaxCalima::PaxCalima() : PollingComponent()
{
}

void PaxCalima::dump_config() {
}

void PaxCalima::update() {
  if (this->node_state != esp32_ble_tracker::ClientState::ESTABLISHED) {
    if (!parent()->enabled) {
      ESP_LOGW(TAG, "Reconnecting to device");
      parent()->set_enabled(true);
      parent()->connect();
    } else {
      ESP_LOGW(TAG, "Connection in progress");
    }
  }
}

} // namespace pax_calima
} // namespace esphome 
