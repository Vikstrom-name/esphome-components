#pragma once

#define MBEDTLS_AES_ALT
#include <aes_alt.h>

#include <vector>
#include <array>


namespace esphome {
namespace lorawan {

class LoRaWANCrypto {
 public:
  static void calculate_mic(const std::vector<uint8_t>& packet,
                            const std::array<uint8_t, 16>& key,
                            uint8_t* mic_out);
  static void decrypt_join_accept(const std::vector<uint8_t>& encrypted,
                                  std::vector<uint8_t>& decrypted,
                                  const std::array<uint8_t, 16>& key);
  static void derive_session_keys(const std::vector<uint8_t>& decrypted,
                                  const std::array<uint8_t, 16>& key,
                                  std::array<uint8_t, 16>& nwk_skey,
                                  std::array<uint8_t, 16>& app_skey);
  static void encrypt_app(const std::vector<uint8_t> &payload, 
                          const std::array<uint8_t, 16> &app_skey,
                          std::vector<uint8_t> &encrypted_data);
                          static void decrypt_app(const std::vector<uint8_t> &encrypted_data, 
                          const std::array<uint8_t, 16> &app_skey,
                          std::vector<uint8_t> &decrypted_data);
  static void encrypt_ntw(const std::vector<uint8_t> &payload, 
                          const std::array<uint8_t, 16> &nwk_skey,
                          std::vector<uint8_t> &encrypted_data);
  static void decrypt_ntw(const std::vector<uint8_t> &encrypted_data, 
                          const std::array<uint8_t, 16> &nwk_skey,
                          std::vector<uint8_t> &decrypted_data);
  
 private:
  static void aes_crypt(const uint8_t* input, uint8_t* output,
                        const std::array<uint8_t, 16>& key,
                        bool encrypt);
};

}  // namespace lorawan
}  // namespace esphome
