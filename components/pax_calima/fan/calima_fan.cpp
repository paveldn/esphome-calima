#include "calima_fan.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pax_calima {

static const char *const TAG = "pax_calima";

CalimaFan::CalimaFan() : boost_mode_duration_(900) {
}

void CalimaFan::setup() {
  this->parent_->add_boost_mode_callback([this](bool state, uint16_t speed, uint16_t time) {
    this->boost_mode_update_(state, speed, time);
  });
}
void CalimaFan::dump_config() {
  LOG_FAN("", "Pax Calima Fan", this);
}

fan::FanTraits CalimaFan::get_traits() {
  return fan::FanTraits(false, true, false, 100);
}

void CalimaFan::control(const fan::FanCall &call) {

  if (call.get_state().has_value())
    this->state = *call.get_state();
  if (call.get_speed().has_value())
    this->speed = *call.get_speed();
  ESP_LOGD(TAG, "Boost mode update: status %s, speed: %d rpm, time: %d seconds", this->state ? "On" : "Off", this->speed, 900);
  this->parent_->set_boost_mode(state, this->speed, this->boost_mode_duration_);
  this->publish_state();
}

void CalimaFan::boost_mode_update_(bool state, uint16_t speed, uint16_t time) {
  this->state = state;
  if (this->state)
    this->speed = speed;
  else
    this->speed = 0;
  this->publish_state();
}

}  // namespace pax_calima
}  // namespace esphome
