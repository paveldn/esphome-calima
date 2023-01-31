import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client

from esphome.const import (
    CONF_ID,
    CONF_PIN,
)

CODEOWNERS = ["@paveldn"]
DEPENDENCIES = ["ble_client"]

pax_calima_ns = cg.esphome_ns.namespace("pax_calima")
PaxCalima = pax_calima_ns.class_(
    "PaxCalima", cg.PollingComponent, ble_client.BLEClientNode
)

CONF_PAX_CALIMA_ID = "pax_calima_id"
ICON_FAN = "mdi:fan"

PAX_CALIMA_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_PAX_CALIMA_ID): cv.use_id(PaxCalima),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaxCalima),
            cv.Optional(CONF_PIN): cv.int_range(min=0, max=0xFFFFFFFF),
        }
    )
    .extend(cv.polling_component_schema("3min"))
    .extend(ble_client.BLE_CLIENT_SCHEMA),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
    if (CONF_PIN in config) and (config[CONF_PIN] is not None):
        cg.add(var.add_pin_code(config[CONF_PIN]));
