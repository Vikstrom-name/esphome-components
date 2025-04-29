#define MBEDTLS_AES_ALT
#include "lorawan_packet.h"
#include "mbedtls/aes.h"
#include "mbedtls/cmac.h"

namespace esphome {
namespace lorawan {


LoRaWANPacket::LoRaWANPacket(const std::vector<uint8_t> &payload) {
  if (payload.size() < 12) {
    valid_ = false;
    return;
  }
    
  this->mhdr_ = payload[0];
  this->dev_addr_ = { payload[1], payload[2], payload[3], payload[4] };
  this->fcnt_ = static_cast<uint16_t>(payload[6]) | (static_cast<uint16_t>(payload[7]) << 8);
  this->fport_ = payload[8];
    
  size_t payload_start = 9;
  size_t mic_start = payload.size() - 4;
    
  if (mic_start > payload_start)
    this->payload_ = std::vector<uint8_t>(payload.begin() + payload_start, payload.begin() + mic_start);
    
  std::copy(payload.end() - 4, payload.end(), mic_.begin());
    
  valid_ = true;
}
      

void LoRaWANPacket::construct_join_request(const std::array<uint8_t, 8>& app_eui, const std::array<uint8_t, 8>& dev_eui,
                                           uint16_t dev_nonce) {
  // Construct the Join Request packet
  payload_.clear();

  payload_.push_back(0x00);  // MHDR for Join Request

  // Add AppEUI and DevEUI
  payload_.insert(payload_.end(), app_eui.rbegin(), app_eui.rend());
  payload_.insert(payload_.end(), dev_eui.rbegin(), dev_eui.rend());

  // Add DevNonce
  payload_.push_back(dev_nonce & 0xFF);
  payload_.push_back((dev_nonce >> 8) & 0xFF);
}


void LoRaWANPacket::construct_join_accept(const std::vector<uint8_t>& decrypted_payload) {
  // Construct the Join Accept packet from the decrypted payload
  payload_ = decrypted_payload;
}


bool LoRaWANPacket::calculate_mic(const std::array<uint8_t, 16>& app_key, uint8_t* out_mic) {
  mbedtls_cipher_context_t ctx;
  const mbedtls_cipher_info_t *cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
  
  mbedtls_cipher_init(&ctx);
  mbedtls_cipher_setup(&ctx, cipher_info);
  mbedtls_cipher_cmac_starts(&ctx, app_key.data(), 128);
  mbedtls_cipher_cmac_update(&ctx, payload_.data(), payload_.size());
  mbedtls_cipher_cmac_finish(&ctx, out_mic);
  mbedtls_cipher_free(&ctx);

  return true;
}


bool LoRaWANPacket::verify_mic(const std::array<uint8_t, 16>& app_key) {
  uint8_t mic_calculated[4];
  if (!calculate_mic(app_key, mic_calculated)) {
    return false;
  }

  return std::memcmp(mic_calculated, mic_.data(), 4) == 0;
}


void LoRaWANPacket::encode(std::vector<uint8_t>& encoded_packet) {
  encoded_packet.clear();
  encoded_packet.push_back(mhdr_);
  encoded_packet.insert(encoded_packet.end(), payload_.begin(), payload_.end());
  encoded_packet.insert(encoded_packet.end(), mic_.begin(), mic_.end());
}


bool LoRaWANPacket::decode(const std::vector<uint8_t>& encoded_packet) {
  if (encoded_packet.size() < 5) {
    return false; // Invalid packet size
  }

  mhdr_ = encoded_packet[0];
  payload_.assign(encoded_packet.begin() + 1, encoded_packet.end() - 4);
  std::copy(encoded_packet.end() - 4, encoded_packet.end(), mic_.begin());

  return true;
}

}  // namespace lorawan
}  // namespace esphome
