import base64

from esphome import automation
from esphome.automation import Condition
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_ACTION,
    CONF_ACTIONS,
    CONF_ID,
    CONF_KEY,
    CONF_ON_CLIENT_CONNECTED,
    CONF_ON_CLIENT_DISCONNECTED,
    CONF_PASSWORD,
    CONF_PORT,
    CONF_REBOOT_TIMEOUT,
    CONF_SERVICE,
    CONF_SERVICES,
    CONF_TAG,
    CONF_TRIGGER_ID,
    CONF_VARIABLES,
)
from esphome.core import coroutine_with_priority

DEPENDENCIES = ["lora"]
CODEOWNERS = ["@dala318"]

meshtastic_ns = cg.esphome_ns.namespace("meshtastic")
MeshtasticServer = meshtastic_ns.class_("MeshtasticServer", cg.Component, cg.Controller)
MeshtasticConnectedCondition = meshtastic_ns.class_("MeshtasticConnectedCondition", Condition)

SERVICE_ARG_NATIVE_TYPES = {
    "bool": bool,
    "int": cg.int32,
    "float": float,
    "string": cg.std_string,
    "bool[]": cg.std_vector.template(bool),
    "int[]": cg.std_vector.template(cg.int32),
    "float[]": cg.std_vector.template(float),
    "string[]": cg.std_vector.template(cg.std_string),
}
# CONF_ENCRYPTION = "encryption"


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MeshtasticServer),
            cv.Optional(CONF_PASSWORD, default=""): cv.string_strict,
            cv.Optional(
                CONF_REBOOT_TIMEOUT, default="15min"
            ): cv.positive_time_period_milliseconds,
            # cv.Optional(CONF_ENCRYPTION): _encryption_schema,
            cv.Optional(CONF_ON_CLIENT_CONNECTED): automation.validate_automation(
                single=True
            ),
            cv.Optional(CONF_ON_CLIENT_DISCONNECTED): automation.validate_automation(
                single=True
            ),
        }
    ).extend(cv.COMPONENT_SCHEMA),
    cv.rename_key(CONF_SERVICES, CONF_ACTIONS),
)


@coroutine_with_priority(40.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_port(config[CONF_PORT]))
    cg.add(var.set_password(config[CONF_PASSWORD]))
    cg.add(var.set_reboot_timeout(config[CONF_REBOOT_TIMEOUT]))

    for conf in config.get(CONF_ACTIONS, []):
        template_args = []
        func_args = []
        service_arg_names = []
        for name, var_ in conf[CONF_VARIABLES].items():
            native = SERVICE_ARG_NATIVE_TYPES[var_]
            template_args.append(native)
            func_args.append((native, name))
            service_arg_names.append(name)
        templ = cg.TemplateArguments(*template_args)
        trigger = cg.new_Pvariable(
            conf[CONF_TRIGGER_ID], templ, conf[CONF_ACTION], service_arg_names
        )
        cg.add(var.register_user_service(trigger))
        await automation.build_automation(trigger, func_args, conf)

    if CONF_ON_CLIENT_CONNECTED in config:
        await automation.build_automation(
            var.get_client_connected_trigger(),
            [(cg.std_string, "client_info"), (cg.std_string, "client_address")],
            config[CONF_ON_CLIENT_CONNECTED],
        )

    if CONF_ON_CLIENT_DISCONNECTED in config:
        await automation.build_automation(
            var.get_client_disconnected_trigger(),
            [(cg.std_string, "client_info"), (cg.std_string, "client_address")],
            config[CONF_ON_CLIENT_DISCONNECTED],
        )

    # if (encryption_config := config.get(CONF_ENCRYPTION, None)) is not None:
    #     if key := encryption_config.get(CONF_KEY):
    #         decoded = base64.b64decode(key)
    #         cg.add(var.set_noise_psk(list(decoded)))
    #     else:
    #         # No key provided, but encryption desired
    #         # This will allow a plaintext client to provide a noise key,
    #         # send it to the device, and then switch to noise.
    #         # The key will be saved in flash and used for future connections
    #         # and plaintext disabled. Only a factory reset can remove it.
    #         cg.add_define("USE_Meshtastic_PLAINTEXT")
    #     cg.add_define("USE_Meshtastic_NOISE")
    #     cg.add_library("esphome/noise-c", "0.1.6")
    # else:
    #     cg.add_define("USE_Meshtastic_PLAINTEXT")

    # cg.add_define("USE_MESHTASTIC")
    # cg.add_global(meshtastic_ns.using)
