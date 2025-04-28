from esphome import automation
import esphome.codegen as cg

lorawan_ns = cg.esphome_ns.namespace('lorawan')
LoRaWANComponent = lorawan_ns.class_('LoRaWANComponent', cg.Component)

CONFIG_SCHEMA = cg.Schema({
    cg.GenerateID(): cg.declare_id(LoRaWANComponent),
})

async def to_code(config):
    var = cg.new_Pvariable(config[cg.CONF_ID])
    await cg.register_component(var, config)
