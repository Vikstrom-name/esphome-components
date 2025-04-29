/*
Explanation:
1. AES Encryption/Decryption: The LoRaWANCrypto class provides two sets of methods: one for encrypting
   and one for decrypting. Each set uses either the NwkSKey or the AppSKey from the Session class.

2. aes_encrypt and aes_decrypt: These functions use the mbedtls_aes_* functions to perform AES
   encryption and decryption in ECB mode.

3. Session Reference: The class holds a reference to the Session object to access the AppSKey and NwkSKey.

Next Steps:
* With this LoRaWANCrypto class, you can now encrypt or decrypt data packets before sending or after receiving them.

* You would likely call the encrypt_app() or decrypt_app() methods for application layer data
  encryption/decryption and the encrypt_nwk() or decrypt_nwk() methods for network layer data.
*/
#pragma once

#include <vector>
#include <array>

#define MBEDTLS_AES_ALT
#include <aes_alt.h>
// #include <mbedtls/aes.h>

namespace esphome {
namespace lorawan {

class LoRaWANCrypto {
 public:
  void decrypt_join_accept(const std::vector<uint8_t>& encrypted,
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
