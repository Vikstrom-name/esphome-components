from esphome.components.lora import (
    LoRa,
    new_lora,
    lora_schema,
)
from esphome.cpp_types import Component

from .. import SX127X_SCHEMA, register_sx127x_client, sx127x_ns

SX127xLoRa = sx127x_ns.class_("SX127xLoRa", LoRa, Component)

CONFIG_SCHEMA = lora_schema(SX127xLoRa).extend(SX127X_SCHEMA)


async def to_code(config):
    var, _ = await new_lora(config)
    await register_sx127x_client(var, config)