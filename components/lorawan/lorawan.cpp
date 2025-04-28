#include "lorawan.h"
#include <cstring>

namespace esphome {
namespace lorawan {

PacketBuilder::PacketBuilder(Session &session) : session_(session) {}

size_t PacketBuilder::build_uplink(MessageType type, const uint8_t *payload, size_t payload_len, uint8_t *out_buffer, size_t out_buffer_size) {
  if (out_buffer_size < (13 + payload_len + 4)) {
    return 0;
  }

  uint8_t *ptr = out_buffer;
  *ptr++ = static_cast<uint8_t>(type);

  *ptr++ = session_.dev_addr & 0xFF;
  *ptr++ = (session_.dev_addr >> 8) & 0xFF;
  *ptr++ = (session_.dev_addr >> 16) & 0xFF;
  *ptr++ = (session_.dev_addr >> 24) & 0xFF;

  *ptr++ = 0x00;  // FCtrl
  *ptr++ = session_.uplink_counter & 0xFF;
  *ptr++ = (session_.uplink_counter >> 8) & 0xFF;

  *ptr++ = 0x01;  // FPort

  encrypt_payload(payload, payload_len, ptr, session_.uplink_counter);
  ptr += payload_len;

  size_t len_without_mic = ptr - out_buffer;

  uint32_t mic = calculate_mic(out_buffer, len_without_mic);
  *ptr++ = mic & 0xFF;
  *ptr++ = (mic >> 8) & 0xFF;
  *ptr++ = (mic >> 16) & 0xFF;
  *ptr++ = (mic >> 24) & 0xFF;

  session_.uplink_counter++;
  return ptr - out_buffer;
}

void PacketBuilder::encrypt_payload(const uint8_t *input, size_t len, uint8_t *output, uint32_t counter) {
  memcpy(output, input, len);
}

uint32_t PacketBuilder::calculate_mic(const uint8_t *data, size_t len) {
  return 0x12345678;
}

void PacketBuilder::aes_encrypt(const uint8_t *key, const uint8_t *input, uint8_t *output) {
  // Placeholder
}

void PacketBuilder::aes_cmac(const uint8_t *key, const uint8_t *input, size_t length, uint8_t *output) {
  // Placeholder
}

}  // namespace lorawan
}  // namespace esphome
