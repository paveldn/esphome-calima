#pragma once

#include "esphome/core/component.h"
#include "esphome/components/fan/fan.h"
#include "../calima.h"

namespace esphome {
namespace pax_calima {

class CalimaFan : public Component, 
                  public Parented<PaxCalima>, 
                  public fan::Fan {
public:
  CalimaFan();
  void setup() override;
  void dump_config() override;
  fan::FanTraits get_traits() override;
  void set_boost_duration(uint16_t duration) { boost_mode_duration_ = duration; };
protected:
  void control(const fan::FanCall &call) override;
  void boost_mode_update_(bool, uint16_t, uint16_t);
  uint16_t boost_mode_duration_;
};

}  // namespace pax_calima
}  // namespace esphome
