"""ESPHome LoRa component."""

import hashlib
import logging

import esphome.codegen as cg
# from esphome.components.api import CONF_ENCRYPTION
# from esphome.components.binary_sensor import BinarySensor
# from esphome.components.sensor import Sensor
import esphome.config_validation as cv
from esphome.const import (
    # CONF_BINARY_SENSORS,
    CONF_ID,
    CONF_INTERNAL,
    CONF_KEY,
    CONF_NAME,
    CONF_PLATFORM,
    # CONF_SENSORS,
)
from esphome.core import CORE
from esphome.cpp_generator import MockObjClass

CODEOWNERS = ["@dala318"]
# AUTO_LOAD = ["xxtea"]

lora_ns = cg.esphome_ns.namespace("lora")
LoRa = lora_ns.class_("LoRa", cg.Component)

IS_PLATFORM_COMPONENT = True

DOMAIN = "lora"
# CONF_BROADCAST = "broadcast"
# CONF_BROADCAST_ID = "broadcast_id"
CONF_PROVIDER = "provider"
CONF_PROVIDERS = "providers"
# CONF_REMOTE_ID = "remote_id"
# CONF_PING_PONG_ENABLE = "ping_pong_enable"
# CONF_PING_PONG_RECYCLE_TIME = "ping_pong_recycle_time"
# CONF_ROLLING_CODE_ENABLE = "rolling_code_enable"
CONF_LORA_ID = "lora_id"


_LOGGER = logging.getLogger(__name__)


def provider_name_validate(value):
    value = cv.valid_name(value)
    if "_" in value:
        _LOGGER.warning(
            "Device names typically do not contain underscores - did you mean to use a hyphen in '%s'?",
            value,
        )
    return value

# ENCRYPTION_SCHEMA = {
#     cv.Optional(CONF_ENCRYPTION): cv.maybe_simple_value(
#         cv.Schema(
#             {
#                 cv.Required(CONF_KEY): cv.string,
#             }
#         ),
#         key=CONF_KEY,
#     )
# }

PROVIDER_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_NAME): provider_name_validate,
    }
)
# ).extend(ENCRYPTION_SCHEMA)


def validate_(config):
    # if CONF_ENCRYPTION in config:
    #     if CONF_SENSORS not in config and CONF_BINARY_SENSORS not in config:
    #         raise cv.Invalid("No sensors or binary sensors to encrypt")
    # elif config[CONF_ROLLING_CODE_ENABLE]:
    #     raise cv.Invalid("Rolling code requires an encryption key")
    # if config[CONF_PING_PONG_ENABLE]:
    #     if not any(CONF_ENCRYPTION in p for p in config.get(CONF_PROVIDERS) or ()):
    #         raise cv.Invalid("Ping-pong requires at least one encrypted provider")
    return config

LORA_SCHEMA = cv.Schema({
    # cv.Optional(CONF_ROLLING_CODE_ENABLE, default=False): cv.boolean,
    # cv.Optional(CONF_PING_PONG_ENABLE, default=False): cv.boolean,
    # cv.Optional(
    #     CONF_PING_PONG_RECYCLE_TIME, default="600s"
    # ): cv.positive_time_period_seconds,
    # cv.Optional(CONF_SENSORS): cv.ensure_list(sensor_validation(Sensor)),
    # cv.Optional(CONF_BINARY_SENSORS): cv.ensure_list(
    #     sensor_validation(BinarySensor)
    # ),
    cv.Optional(CONF_PROVIDERS, default=[]): cv.ensure_list(PROVIDER_SCHEMA),
}).extend(cv.COMPONENT_SCHEMA).add_extra(validate_)


def lora_schema(cls):
    return LORA_SCHEMA.extend({cv.GenerateID(): cv.declare_id(cls)})


# Build a list of sensors for this platform
# CORE.data[DOMAIN] = {CONF_SENSORS: []}


# def get_sensors(lora_id):
#     """Return the list of sensors for this platform."""
#     return (
#         sensor
#         for sensor in CORE.data[DOMAIN][CONF_SENSORS]
#         if sensor[CONF_LORA_ID] == lora_id
#     )


# def validate_lora_sensor(config):
#     if CONF_NAME in config and CONF_INTERNAL not in config:
#         raise cv.Invalid("Must provide internal: config when using name:")
#     CORE.data[DOMAIN][CONF_SENSORS].append(config)
#     return config


# def lora_sensor_schema(base_schema):
#     return cv.All(
#         base_schema.extend(
#             {
#                 cv.GenerateID(CONF_LORA_ID): cv.use_id(LoRa),
#                 cv.Optional(CONF_REMOTE_ID): cv.string_strict,
#                 cv.Required(CONF_PROVIDER): provider_name_validate,
#             }
#         ),
#         cv.has_at_least_one_key(CONF_ID, CONF_REMOTE_ID),
#         validate_lora_sensor,
#     )


def hash_encryption_key(config: dict):
    return list(hashlib.sha256(config[CONF_KEY].encode()).digest())


async def register_lora(var, config):
    var = await cg.register_component(var, config)
    # cg.add(var.set_rolling_code_enable(config[CONF_ROLLING_CODE_ENABLE]))
    # cg.add(var.set_ping_pong_enable(config[CONF_PING_PONG_ENABLE]))
    # cg.add(
    #     var.set_ping_pong_recycle_time(
    #         config[CONF_PING_PONG_RECYCLE_TIME].total_seconds
    #     )
    # )
    # Get directly configured providers, plus those from sensors and binary sensors
    # providers = {
    #     sensor[CONF_PROVIDER] for sensor in get_sensors(config[CONF_ID])
    # }.union(x[CONF_NAME] for x in config[CONF_PROVIDERS])
    # for provider in providers:
    #     cg.add(var.add_provider(provider))
    # for provider in config[CONF_PROVIDERS]:
    #     name = provider[CONF_NAME]
    #     if encryption := provider.get(CONF_ENCRYPTION):
    #         cg.add(var.set_provider_encryption(name, hash_encryption_key(encryption)))

    # for sens_conf in config.get(CONF_SENSORS, ()):
    #     sens_id = sens_conf[CONF_ID]
    #     sensor = await cg.get_variable(sens_id)
    #     bcst_id = sens_conf.get(CONF_BROADCAST_ID, sens_id.id)
    #     cg.add(var.add_sensor(bcst_id, sensor))
    # for sens_conf in config.get(CONF_BINARY_SENSORS, ()):
    #     sens_id = sens_conf[CONF_ID]
    #     sensor = await cg.get_variable(sens_id)
    #     bcst_id = sens_conf.get(CONF_BROADCAST_ID, sens_id.id)
    #     cg.add(var.add_binary_sensor(bcst_id, sensor))

    # if encryption := config.get(CONF_ENCRYPTION):
    #     cg.add(var.set_encryption_key(hash_encryption_key(encryption)))
    # return providers


async def new_lora(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_platform_name(config[CONF_PLATFORM]))
    providers = await register_lora(var, config)
    return var, providers   