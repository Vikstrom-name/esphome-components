import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID

# Definiera ditt C++ namespace och klass
lorawan_ns = cg.esphome_ns.namespace('lorawan')
LoRaWANComponent = lorawan_ns.class_('LoRaWANComponent', cg.Component)

CONF_DEV_ADDR = "dev_addr"
CONF_NWK_SKEY = "nwk_skey"
CONF_APP_SKEY = "app_skey"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LoRaWANComponent),
    cv.Required(CONF_DEV_ADDR): cv.uint32_t,
    cv.Required(CONF_NWK_SKEY): cv.hex_bytes(16),
    cv.Required(CONF_APP_SKEY): cv.hex_bytes(16),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.session_.dev_addr) = config[CONF_DEV_ADDR]
    cg.add(cg.raw_expression(f'memcpy({var.get_object_name()}.session_.nwk_skey, {config[CONF_NWK_SKEY]}, 16);'))
    cg.add(cg.raw_expression(f'memcpy({var.get_object_name()}.session_.app_skey, {config[CONF_APP_SKEY]}, 16);'))
