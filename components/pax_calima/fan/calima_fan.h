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
  void setup() override;
  void dump_config() override;
  fan::FanTraits get_traits() override;
protected:
  void control(const fan::FanCall &call) override;
};

}  // namespace pax_calima
}  // namespace esphome
