import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from .. import CONF_PAX_CALIMA_ID, PAX_CALIMA_COMPONENT_SCHEMA 

from esphome.const import (
    CONF_FAN_MODE,
)

CODEOWNERS = ["@paveldn"]

ICON_FAN = "mdi:fan"

CONFIG_SCHEMA = PAX_CALIMA_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_FAN_MODE): text_sensor.text_sensor_schema(
            icon=ICON_FAN
        )
    }
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_PAX_CALIMA_ID])
    if CONF_FAN_MODE in config:
        sens = await text_sensor.new_text_sensor(config[CONF_FAN_MODE])
        cg.add(paren.set_fan_mode_sensor(sens))
