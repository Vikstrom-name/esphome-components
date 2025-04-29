import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
)

CODEOWNERS = ["@dala318"]
# DEPENDENCIES = [ "sx126x", "sx127x" ]

lorawan_ns = cg.esphome_ns.namespace("lorawan")
LoRaWANComponent = lorawan_ns.class_("LoRaWANComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LoRaWANComponent),
    cv.Required("app_key"): cv.All(cv.ensure_list(cv.hex_uint8_t), cv.Length(min=16, max=16)),
    cv.Required("dev_eui"): cv.All(cv.ensure_list(cv.hex_uint8_t), cv.Length(min=8, max=8)),
    cv.Required("app_eui"): cv.All(cv.ensure_list(cv.hex_uint8_t), cv.Length(min=8, max=8)),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_app_key(config["app_key"]))
    cg.add(var.set_dev_eui(config["dev_eui"]))
    cg.add(var.set_app_eui(config["app_eui"]))
