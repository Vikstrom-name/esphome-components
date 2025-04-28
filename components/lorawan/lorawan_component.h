#pragma once

#include "esphome/core/component.h"
#include "lorawan_packet_builder.h"

namespace esphome {
namespace lorawan {

class LoRaWANComponent : public Component {
 public:
  void setup() override {}
  void loop() override {}

  void set_session(const LoRaWANSessionKeys &session) {
    this->session_ = session;
  }

  void start_join();
  void publish_payload(const uint8_t *payload, size_t length);

 protected:
  LoRaWANSessionKeys session_;
  bool joined_;

  void send_packet(const uint8_t *data, size_t length) {
    // Add radio send logic here.
    // For example:
    // radio.send(data, length);
  }
  void on_radio_receive(const uint8_t *data, size_t length);
};

}  // namespace lorawan
}  // namespace esphome
