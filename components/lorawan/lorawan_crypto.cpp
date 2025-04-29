#include "esphome/core/log.h"
#include "lorawan_crypto.h"
// #include "mbedtls/aes.h"
#include "mbedtls/cmac.h"
#include <cstring>

namespace esphome {
namespace lorawan {

static const char *const TAG = "LoRaWAN_crypo";


// Calculate MIC using AES-CMAC
void LoRaWANCrypto::calculate_mic(const std::vector<uint8_t>& packet,
                                  const std::array<uint8_t, 16>& key,
                                  uint8_t* mic_out) {
  const mbedtls_cipher_info_t *cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
  if (cipher_info == nullptr) {
  ESP_LOGE(TAG, "Failed to get AES cipher info");
    return;
  }

  unsigned char *mic_out_local;
//   int err = mbedtls_cipher_cmac(cipher_info, key.data(), 128,
//                                 packet.data(), packet.size(), mic_out);
  int err = mbedtls_cipher_cmac(cipher_info, key.data(), 128,
                                packet.data(), packet.size(), mic_out_local);
  mic_out = mic_out_local;
  if (err != 0) {
    ESP_LOGE(TAG, "CMAC calculation failed: -0x%04X", -err);
  }
}


// Decrypt Join Accept message using AES
void LoRaWANCrypto::decrypt_join_accept(const std::vector<uint8_t>& encrypted,
                                        std::vector<uint8_t>& decrypted,
                                        const std::array<uint8_t, 16>& key) {
  decrypted.resize(encrypted.size());
  aes_crypt(encrypted.data(), decrypted.data(), key, false);
}


// Derive session keys from Join Accept response
void LoRaWANCrypto::derive_session_keys(const std::vector<uint8_t>& decrypted, const std::array<uint8_t, 16>& key, std::array<uint8_t, 16>& nwk_skey, std::array<uint8_t, 16>& app_skey) {
  // 1. Derive NwkSKey (AES key with key type 0x01)
  std::array<uint8_t, 16> nonce_data{};
  nonce_data[0] = 0x01;  // key type for NwkSKey
  std::copy(decrypted.begin() + 1, decrypted.begin() + 4, nonce_data.begin() + 1);  // AppNonce
  std::copy(decrypted.begin() + 4, decrypted.begin() + 7, nonce_data.begin() + 4);  // NetID
  nonce_data[7] = decrypted[9];  // DevNonce LSB
  nonce_data[8] = decrypted[10];  // DevNonce MSB

  aes_crypt(nonce_data.data(), nwk_skey.data(), key, true);

  // 2. Derive AppSKey (AES key with key type 0x02)
  nonce_data[0] = 0x02;  // key type for AppSKey
  aes_crypt(nonce_data.data(), app_skey.data(), key, true);
}


// Encrypt application payload
void LoRaWANCrypto::encrypt_app(const std::vector<uint8_t>& data, const std::array<uint8_t, 16>& key, std::vector<uint8_t>& encrypted) {
  encrypted.resize(data.size());
  aes_crypt(data.data(), encrypted.data(), key, true);
}


// Decrypt application payload
void LoRaWANCrypto::decrypt_app(const std::vector<uint8_t>& encrypted, const std::array<uint8_t, 16>& key, std::vector<uint8_t>& decrypted) {
  decrypted.resize(encrypted.size());
  aes_crypt(encrypted.data(), decrypted.data(), key, false);
}


// Encrypt network payload
void LoRaWANCrypto::encrypt_ntw(const std::vector<uint8_t>& data, const std::array<uint8_t, 16>& key, std::vector<uint8_t>& encrypted) {
  encrypted.resize(data.size());
  aes_crypt(data.data(), encrypted.data(), key, true);
  
}


// Decrypt network payload
void LoRaWANCrypto::decrypt_ntw(const std::vector<uint8_t>& encrypted, const std::array<uint8_t, 16>& key, std::vector<uint8_t>& decrypted) {
  decrypted.resize(encrypted.size());
  aes_crypt(encrypted.data(), decrypted.data(), key, false);
}


// Helper function for AES encryption/decryption (ECB mode)
void LoRaWANCrypto::aes_crypt(const uint8_t* input, uint8_t* output,
                              const std::array<uint8_t, 16>& key,
                              bool encrypt) {
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, key.data(), 128);  // Use encryption key for both encryption and decryption (ECB mode)
  if (encrypt) {
    mbedtls_aes_crypt_ecb(&aes, ESP_AES_ENCRYPT, input, output);
  } else {
    mbedtls_aes_crypt_ecb(&aes, ESP_AES_DECRYPT, input, output);
  }
  mbedtls_aes_free(&aes);
}

}  // namespace lorawan
}  // namespace esphome
