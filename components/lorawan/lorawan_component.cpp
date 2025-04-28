#include "lorawan_component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lorawan {

static const char *const TAG = "lorawan";

void LoRaWANComponent::setup() {
  this->builder_ = new PacketBuilder(this->session_);
  ESP_LOGI(TAG, "LoRaWAN Component initialized");
}

void LoRaWANComponent::send_unconfirmed_data(const uint8_t *payload, size_t length) {
  uint8_t buffer[256];
  size_t packet_len = this->builder_->build_uplink(MessageType::UnconfirmedDataUp, payload, length, buffer, sizeof(buffer));

  if (packet_len > 0) {
    ESP_LOGI(TAG, "Built uplink packet of size %u", packet_len);
    // Här ska du skicka bufferten via din radio
    // Exempel: radio_send(buffer, packet_len);
  } else {
    ESP_LOGW(TAG, "Failed to build LoRaWAN uplink packet");
  }
}

}  // namespace lorawan
}  // namespace esphome
