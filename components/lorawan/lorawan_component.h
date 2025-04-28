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

  void publish_payload(const uint8_t *payload, size_t length) {
    PacketBuilder builder(this->session_);
    std::vector<uint8_t> packet;

    if (builder.build_uplink(payload, length, packet)) {
      send_packet(packet.data(), packet.size());
      this->session_.frame_counter++;
    } else {
      ESP_LOGE(TAG, "Failed to build LoRaWAN packet");
    }
  }

 protected:
  LoRaWANSessionKeys session_;

  void send_packet(const uint8_t *data, size_t length) {
    // Add radio send logic here.
    // For example:
    // radio.send(data, length);
  }
};

}  // namespace lorawan
}  // namespace esphome
