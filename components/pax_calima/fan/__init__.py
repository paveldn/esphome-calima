import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan
from .. import CONF_PAX_CALIMA_ID, pax_calima_ns, PaxCalima

from esphome.const import (
    CONF_ID,
    CONF_RUN_DURATION,
)

CODEOWNERS = ["@paveldn"]

CalimaFan = pax_calima_ns.class_("CalimaFan", cg.Component, fan.Fan)

CONFIG_SCHEMA = fan.FAN_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(CalimaFan),
        cv.Required(CONF_PAX_CALIMA_ID): cv.use_id(PaxCalima),
        cv.Optional(CONF_RUN_DURATION, default="15m"): cv.All(
            cv.positive_time_period_seconds,
            cv.Range(
                min=cv.TimePeriod(seconds=30), 
                max=cv.TimePeriod(seconds=64800)
            ),
        )
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_parented(var, config[CONF_PAX_CALIMA_ID])
    await cg.register_component(var, config)
    await fan.register_fan(var, config)
    if CONF_RUN_DURATION in config:
        cg.add(var.set_boost_duration(config[CONF_RUN_DURATION]))

