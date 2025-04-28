#pragma once

extern "C" {
  #define MBEDTLS_AES_ALT
  #include "mbedtls/cmac.h"
}

#include <cstdint>

namespace esphome {
namespace lorawan {

class CMACHelper {
 public:
  static bool calculate_cmac(const uint8_t *key, const uint8_t *input, size_t input_len, uint8_t *output) {
    // MBedTLS expects key size in bits
    int ret = mbedtls_cipher_cmac(
        mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB),
        key, 128,
        input, input_len,
        output);

    return ret == 0;
  }
};

}  // namespace lorawan
}  // namespace esphome
