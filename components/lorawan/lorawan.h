#pragma once

#include <cstdint>

namespace esphome {
namespace lorawan {

struct Session {
  uint32_t dev_addr;
  uint8_t nwk_skey[16];
  uint8_t app_skey[16];
  uint32_t uplink_counter;
  uint32_t downlink_counter;
};

enum class MessageType : uint8_t {
  UnconfirmedDataUp = 0x40,
  ConfirmedDataUp = 0x80
};

class PacketBuilder {
 public:
  PacketBuilder(Session &session);

  size_t build_uplink(MessageType type, const uint8_t *payload, size_t payload_len, uint8_t *out_buffer, size_t out_buffer_size);

 protected:
  Session &session_;

  void encrypt_payload(const uint8_t *input, size_t len, uint8_t *output, uint32_t counter);
  uint32_t calculate_mic(const uint8_t *data, size_t len);

  void aes_encrypt(const uint8_t *key, const uint8_t *input, uint8_t *output);
  void aes_cmac(const uint8_t *key, const uint8_t *input, size_t length, uint8_t *output);
};

}  // namespace lorawan
}  // namespace esphome
