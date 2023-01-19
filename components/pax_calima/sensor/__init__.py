import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from .. import CONF_PAX_CALIMA_ID, PAX_CALIMA_COMPONENT_SCHEMA 

from esphome.const import (
    CONF_HUMIDITY,
    CONF_ILLUMINANCE,
    CONF_ROTATION,
    CONF_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_ILLUMINANCE,
    DEVICE_CLASS_SPEED,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_LUX,
    UNIT_PERCENT,   
)

UNIT_RPM = "rpm"

CODEOWNERS = ["@paveldn"]

CONFIG_SCHEMA = PAX_CALIMA_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_HUMIDITY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ILLUMINANCE): sensor.sensor_schema(
            unit_of_measurement=UNIT_LUX,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_ILLUMINANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ROTATION): sensor.sensor_schema(
            unit_of_measurement=UNIT_RPM,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_SPEED,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_PAX_CALIMA_ID])
    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(paren.set_temperature_sensor(sens))
    if CONF_HUMIDITY in config:
        sens = await sensor.new_sensor(config[CONF_HUMIDITY])
        cg.add(paren.set_humidity_sensor(sens))
    if CONF_ILLUMINANCE in config:
        sens = await sensor.new_sensor(config[CONF_ILLUMINANCE])
        cg.add(paren.set_illuminance_sensor(sens))
    if CONF_ROTATION in config:
        sens = await sensor.new_sensor(config[CONF_ROTATION])
        cg.add(paren.set_rotation_sensor(sens))    
