#include "calima_fan.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pax_calima {

static const char *const TAG = "pax_calima";

void CalimaFan::setup() {
}
void CalimaFan::dump_config() {
  LOG_FAN("", "Pax Calima Fan", this);
}

fan::FanTraits CalimaFan::get_traits() {
  // No 
  return fan::FanTraits(false, true, false, 100);
}

void CalimaFan::control(const fan::FanCall &call) {
  if (call.get_state().has_value())
    this->state = *call.get_state();
  if (call.get_speed().has_value())
    this->speed = *call.get_speed();
  this->publish_state();
}

}  // namespace pax_calima
}  // namespace esphome
