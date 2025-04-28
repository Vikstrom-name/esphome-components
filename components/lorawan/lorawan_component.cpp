#include "lorawan_component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lorawan {

static const char *const TAG = "lorawan";

LoRaWANComponent::LoRaWANComponent() : joined_(false) {}

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
  if ((data[0] & 0xE0) == 0x20) {
    if (length < 17) {
      ESP_LOGW(TAG, "Join Accept packet too short");
      return;
    }
    if (builder.parse_join_accept(data, length, this->session_)) {
      ESP_LOGI(TAG, "Join Accept received, session keys updated");
      this->joined_ = true;
    } else {
      ESP_LOGW(TAG, "Received non-join packet or invalid");
    }
  }
  else if (this->joined_) {
    handle_normal_packet(data, length);
  } else {
    ESP_LOGW(TAG, "Received packet before joining");
  }
}

void LoRaWANComponent::handle_normal_packet(const uint8_t *data, size_t length) {
  // Step 1: Extract MIC (last 4 bytes of packet)
  uint8_t mic[4];
  memcpy(mic, data + length - 4, sizeof(mic));
  
  // Step 2: Verify MIC with AppSKey or NwkSKey
  bool is_valid = false;
  if (this->joined_) {
      // Verify MIC with AppSKey for application data
      is_valid = verify_mic(data, length - 4, mic, this->session_.app_skey);
  } else {
      // Verify MIC with NwkSKey for network data
      is_valid = verify_mic(data, length - 4, mic, this->session_.nwk_skey);
  }

  if (!is_valid) {
      ESP_LOGW(TAG, "Invalid MIC, dropping packet");
      return;  // Drop invalid packet
  }

  // Step 3: Decrypt payload based on the key used
  uint8_t decrypted_payload[MAX_PAYLOAD_SIZE];
  if (this->joined_) {
      aes_decrypt(this->session_.app_skey, data, decrypted_payload, length - 4);
  } else {
      aes_decrypt(this->session_.nwk_skey, data, decrypted_payload, length - 4);
  }

  // Step 4: Process decrypted payload
  process_payload(decrypted_payload, length - 4);
}

bool LoRaWANComponent::verify_mic(const uint8_t *data, size_t data_len, const uint8_t *mic, const uint8_t *key) {
  // Calculate MIC using AES-CMAC (you can implement this or use an existing AES-CMAC library)
  uint8_t calculated_mic[4];
  cmac_calculate(key, data, data_len, calculated_mic);

  return (memcmp(calculated_mic, mic, 4) == 0);
}

void LoRaWANComponent::process_payload(const uint8_t *payload, size_t length) {
  // Handle the decrypted payload (process application data)
  // Example: Log payload
  ESP_LOGI(TAG, "Received decrypted payload of length: %zu", length);
  ESP_LOGI(TAG, "Received decrypted payload: %s", payload);
}

}  // namespace lorawan
}  // namespace esphome
