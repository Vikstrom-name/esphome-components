#pragma once

extern "C" {
  #define MBEDTLS_AES_ALT
  #include "mbedtls/aes.h"
}

#include <cstdint>

namespace esphome {
namespace lorawan {

class AESHelper {
 public:
  AESHelper() {
    mbedtls_aes_init(&ctx_);
  }

  ~AESHelper() {
    mbedtls_aes_free(&ctx_);
  }

  void set_key(const uint8_t *key, size_t key_bits = 128) {
    mbedtls_aes_setkey_enc(&ctx_, key, key_bits);
  }

  void encrypt_block(const uint8_t *input, uint8_t *output) {
    mbedtls_aes_crypt_ecb(&ctx_, MBEDTLS_AES_ENCRYPT, input, output);
  }

 protected:
  mbedtls_aes_context ctx_;
};

}  // namespace lorawan
}  // namespace esphome
