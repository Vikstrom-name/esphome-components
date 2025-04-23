#pragma once

#include "esphome/core/defines.h"
#ifdef USE_MESHTASTIC
#include "src/mesh/Router.h"
// #include "meshtastic_noise_context.h"
// #include "meshtastic_pb2.h"
// #include "meshtastic_pb2_service.h"
// #include "esphome/components/socket/socket.h"
// #include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/controller.h"
#include "esphome/core/log.h"
// #include "list_entities.h"
// #include "subscribe_state.h"
// #include "user_services.h"

#include <vector>

namespace esphome {
namespace meshtastic {

// #ifdef USE_MESHTASTIC_NOISE
// struct SavedNoisePsk {
//   psk_t psk;
// } PACKED;  // NOLINT
// #endif

class MeshtasticServer : public Component, public Controller {
 public:
  MeshtasticServer();
  void setup() override;
  uint16_t get_port() const;
  float get_setup_priority() const override;
  void loop() override;
  void dump_config() override;
  void on_shutdown() override;
  bool check_password(const std::string &password) const;
  bool uses_password() const;
  void set_port(uint16_t port);
  void set_password(const std::string &password);
  void set_reboot_timeout(uint32_t reboot_timeout);

// #ifdef USE_MESHTASTIC_NOISE
//   bool save_noise_psk(psk_t psk, bool make_active = true);
//   void set_noise_psk(psk_t psk) { noise_ctx_->set_psk(psk); }
//   std::shared_ptr<MeshtasticNoiseContext> get_noise_ctx() { return noise_ctx_; }
// #endif  // USE_MESHTASTIC_NOISE

  void handle_disconnect(MeshtasticConnection *conn);
#ifdef USE_BINARY_SENSOR
  void on_binary_sensor_update(binary_sensor::BinarySensor *obj, bool state) override;
#endif
#ifdef USE_COVER
  void on_cover_update(cover::Cover *obj) override;
#endif
#ifdef USE_FAN
  void on_fan_update(fan::Fan *obj) override;
#endif
#ifdef USE_LIGHT
  void on_light_update(light::LightState *obj) override;
#endif
#ifdef USE_SENSOR
  void on_sensor_update(sensor::Sensor *obj, float state) override;
#endif
#ifdef USE_SWITCH
  void on_switch_update(switch_::Switch *obj, bool state) override;
#endif
#ifdef USE_TEXT_SENSOR
  void on_text_sensor_update(text_sensor::TextSensor *obj, const std::string &state) override;
#endif
#ifdef USE_CLIMATE
  void on_climate_update(climate::Climate *obj) override;
#endif
#ifdef USE_NUMBER
  void on_number_update(number::Number *obj, float state) override;
#endif
#ifdef USE_DATETIME_DATE
  void on_date_update(datetime::DateEntity *obj) override;
#endif
#ifdef USE_DATETIME_TIME
  void on_time_update(datetime::TimeEntity *obj) override;
#endif
#ifdef USE_DATETIME_DATETIME
  void on_datetime_update(datetime::DateTimeEntity *obj) override;
#endif
#ifdef USE_TEXT
  void on_text_update(text::Text *obj, const std::string &state) override;
#endif
#ifdef USE_SELECT
  void on_select_update(select::Select *obj, const std::string &state, size_t index) override;
#endif
#ifdef USE_LOCK
  void on_lock_update(lock::Lock *obj) override;
#endif
#ifdef USE_VALVE
  void on_valve_update(valve::Valve *obj) override;
#endif
#ifdef USE_MEDIA_PLAYER
  void on_media_player_update(media_player::MediaPlayer *obj) override;
#endif
  void send_homeassistant_service_call(const HomeassistantServiceResponse &call);
  void register_user_service(UserServiceDescriptor *descriptor) { this->user_services_.push_back(descriptor); }
#ifdef USE_HOMEASSISTANT_TIME
  void request_time();
#endif

#ifdef USE_ALARM_CONTROL_PANEL
  void on_alarm_control_panel_update(alarm_control_panel::AlarmControlPanel *obj) override;
#endif
#ifdef USE_EVENT
  void on_event(event::Event *obj, const std::string &event_type) override;
#endif
#ifdef USE_UPDATE
  void on_update(update::UpdateEntity *obj) override;
#endif

  bool is_connected() const;

  struct HomeAssistantStateSubscription {
    std::string entity_id;
    optional<std::string> attribute;
    std::function<void(std::string)> callback;
    bool once;
  };

  void subscribe_home_assistant_state(std::string entity_id, optional<std::string> attribute,
                                      std::function<void(std::string)> f);
  void get_home_assistant_state(std::string entity_id, optional<std::string> attribute,
                                std::function<void(std::string)> f);
  const std::vector<HomeAssistantStateSubscription> &get_state_subs() const;
  const std::vector<UserServiceDescriptor *> &get_user_services() const { return this->user_services_; }

  Trigger<std::string, std::string> *get_client_connected_trigger() const { return this->client_connected_trigger_; }
  Trigger<std::string, std::string> *get_client_disconnected_trigger() const {
    return this->client_disconnected_trigger_;
  }

 protected:
  std::unique_ptr<socket::Socket> socket_ = nullptr;
  uint16_t port_{6053};
  uint32_t reboot_timeout_{300000};
  uint32_t last_connected_{0};
  std::vector<std::unique_ptr<MeshtasticConnection>> clients_;
  std::string password_;
  std::vector<HomeAssistantStateSubscription> state_subs_;
  std::vector<UserServiceDescriptor *> user_services_;
  Trigger<std::string, std::string> *client_connected_trigger_ = new Trigger<std::string, std::string>();
  Trigger<std::string, std::string> *client_disconnected_trigger_ = new Trigger<std::string, std::string>();

// #ifdef USE_MESHTASTIC_NOISE
//   std::shared_ptr<MeshtasticNoiseContext> noise_ctx_ = std::make_shared<MeshtasticNoiseContext>();
//   ESPPreferenceObject noise_pref_;
// #endif  // USE_MESHTASTIC_NOISE
};

extern MeshtasticServer *global_meshtastic_server;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

template<typename... Ts> class MeshtasticConnectedCondition : public Condition<Ts...> {
 public:
  bool check(Ts... x) override { return global_meshtastic_server->is_connected(); }
};

}  // namespace meshtastic
}  // namespace esphome
#endif
