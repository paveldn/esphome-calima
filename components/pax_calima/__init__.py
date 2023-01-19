import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client

from esphome.const import (
    CONF_ID,
)

# Pax Calima
# Vent-Axia Svara

CODEOWNERS = ["@paveldn"]
DEPENDENCIES = ["ble_client"]

pax_calima_ns = cg.esphome_ns.namespace("pax_calima")
PaxCalima = pax_calima_ns.class_(
    "PaxCalima", cg.PollingComponent, ble_client.BLEClientNode
)

CONF_PAX_CALIMA_ID = "pax_calima_id"

PAX_CALIMA_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_PAX_CALIMA_ID): cv.use_id(PaxCalima),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaxCalima),
        }
    )
    .extend(cv.polling_component_schema("3min"))
    .extend(ble_client.BLE_CLIENT_SCHEMA),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
