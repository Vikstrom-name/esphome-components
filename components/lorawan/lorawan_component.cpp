#include "esphome/core/hal.h"
#include "esphome/core/log.h"
// #include "mbedtls/aes.h"
// #include "mbedtls/cmac.h"

#include "lorawan_component.h"

namespace esphome {
namespace lorawan {

static const char *const TAG = "LoRaWAN";


void LoRaWANComponent::setup() {
  ESP_LOGI(TAG, "Setting up LoRaWAN component");

  if (radio_ == nullptr) {
    ESP_LOGE(TAG, "No radio interface provided");
    return;
  }

  if (!radio_->begin()) {
    ESP_LOGE(TAG, "Failed to initialize radio");
    return;
  }

  radio_->set_rx_callback([this](const std::vector<uint8_t> &packet) {
    this->on_radio_receive_(packet);
  });

  this->session_ = std::make_unique<LoRaWANSession>();

  this->send_join_request_();
  this->join_request_time_ = millis();
}


void LoRaWANComponent::loop() {
  if (!joined_ && millis() - join_request_time_ > 5000) {
    this->send_join_request_();
    this->join_request_time_ = millis();
  }
}


void LoRaWANComponent::send_join_request_() {
  ESP_LOGI(TAG, "Sending join request");

  this->session_ = std::make_unique<LoRaWANSession>();

  LoRaWANPacket packet;
  packet.construct_join_request(this->app_eui_, this->dev_eui_, this->dev_nonce_++);

  // Calculate MIC using Crypto (AES-CMAC)
  // LoRaWANCrypto::calculate_mic(packet.get_payload(), this->app_key_, packet.get_mic());
  uint8_t *mic_out;
  LoRaWANCrypto::calculate_mic(packet.get_payload(), this->app_key_, mic_out);
  std::array<uint8_t, 4> mic_array;
  std::copy(mic_out, mic_out + 4, mic_array.begin()); 
  packet.set_mic(mic_array);

  // Send the packet using the radio interface
  radio_->send(packet.get_payload());
  radio_->receive();
}


void LoRaWANComponent::on_radio_receive_(const std::vector<uint8_t> &packet) {
  if (packet.empty()) return;

  LoRaWANPacket lorawan_packet(packet);

  if (lorawan_packet.is_join_accept()) {
    ESP_LOGI(TAG, "Received Join Accept");
    if (this->process_join_response(lorawan_packet)){
      joined_ = true;
      ESP_LOGI(TAG, "Join successful, session keys derived");
    }
    else {
      joined_ = false;
      ESP_LOGE(TAG, "Failed to process Join Accept response");
    }
  } else {
    ESP_LOGI(TAG, "Received normal data packet");
    // Handle normal data packets later
  }
}


bool LoRaWANComponent::process_join_response(const LoRaWANPacket lorawan_packet){

  std::vector<uint8_t> decrypted;
  LoRaWANCrypto::decrypt_join_accept(lorawan_packet.get_payload(), decrypted, this->app_key_);
  
  std::array<uint8_t, 16> nwk_skey_;
  std::array<uint8_t, 16> app_skey_;
  LoRaWANCrypto::derive_session_keys(decrypted, app_key_, nwk_skey_, app_skey_);
  this->session_->set_keys(nwk_skey_, app_skey_);

  std::array<uint8_t, 4> dev_addr = {
    decrypted[7], decrypted[8], decrypted[9], decrypted[10]  // LSB order
  };
  this->session_->set_dev_addr(dev_addr);    

  return true;
}

}  // namespace lorawan
}  // namespace esphome
