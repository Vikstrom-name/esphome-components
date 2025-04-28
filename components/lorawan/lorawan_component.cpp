#include "lorawan_component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lorawan {

static const char *const TAG = "lorawan";

void LoRaWANComponent::publish_payload(const uint8_t *payload, size_t length) {
  PacketBuilder builder(this->session_);
  std::vector<uint8_t> packet;

  if (builder.build_uplink(payload, length, packet)) {
    send_packet(packet.data(), packet.size());
    this->session_.frame_counter++;
  } else {
    ESP_LOGE(TAG, "Failed to build LoRaWAN packet");
  }
}

void LoRaWANComponent::start_join() {
  PacketBuilder builder(this->session_);
  std::vector<uint8_t> packet;
  if (builder.build_join_request(packet)) {
    send_packet(packet.data(), packet.size());
    ESP_LOGI(TAG, "Join request sent");
  } else {
    ESP_LOGE(TAG, "Failed to build Join Request");
  }
}

void LoRaWANComponent::on_radio_receive(const uint8_t *data, size_t length) {
  PacketBuilder builder(this->session_);
  if (builder.parse_join_accept(data, length, this->session_)) {
    ESP_LOGI(TAG, "Join Accept received, session keys updated");
    this->joined_ = true;
  } else {
    ESP_LOGW(TAG, "Received non-join packet or invalid");
  }
}
}  // namespace lorawan
}  // namespace esphome
