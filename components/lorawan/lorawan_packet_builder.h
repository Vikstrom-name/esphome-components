#pragma once

#include "aes_helper.h"
#include "cmac_helper.h"
#include <cstdint>
#include <vector>

namespace esphome {
namespace lorawan {

struct LoRaWANSessionKeys {
  uint8_t nwk_skey[16];
  uint8_t app_skey[16];
  uint32_t dev_addr;
  uint16_t frame_counter;
};

class PacketBuilder {
 public:
  PacketBuilder(const LoRaWANSessionKeys &session)
      : session_(session) {}

  bool build_uplink(const uint8_t *app_payload, size_t app_payload_len, std::vector<uint8_t> &out_packet) {
    out_packet.clear();

    // MHDR (Unconfirmed Data Up)
    out_packet.push_back(0x40);

    // FHDR
    write_devaddr(out_packet, session_.dev_addr);
    out_packet.push_back(0x00); // FCtrl (no options)
    write_le16(out_packet, session_.frame_counter);
    // No FOpts, so nothing to add here.

    // FPort
    out_packet.push_back(1); // Application port 1

    // Encrypt FRMPayload
    std::vector<uint8_t> enc_payload;
    encrypt_frmpayload(app_payload, app_payload_len, enc_payload);
    out_packet.insert(out_packet.end(), enc_payload.begin(), enc_payload.end());

    // Calculate MIC
    uint8_t mic[16];
    calculate_mic(out_packet.data(), out_packet.size(), mic);

    // Append first 4 bytes of MIC
    out_packet.insert(out_packet.end(), mic, mic + 4);

    return true;
  }

 protected:
  const LoRaWANSessionKeys &session_;

  void write_devaddr(std::vector<uint8_t> &buf, uint32_t devaddr) {
    buf.push_back(devaddr & 0xFF);
    buf.push_back((devaddr >> 8) & 0xFF);
    buf.push_back((devaddr >> 16) & 0xFF);
    buf.push_back((devaddr >> 24) & 0xFF);
  }

  void write_le16(std::vector<uint8_t> &buf, uint16_t val) {
    buf.push_back(val & 0xFF);
    buf.push_back((val >> 8) & 0xFF);
  }

  void encrypt_frmpayload(const uint8_t *input, size_t length, std::vector<uint8_t> &output) {
    output.resize(length);

    AESHelper aes;
    aes.set_key(session_.app_skey);

    uint8_t block_a[16];
    uint8_t s[16];
    uint32_t block_counter = 1;
    size_t offset = 0;

    while (offset < length) {
      build_block_a(block_a, block_counter++);
      aes.encrypt_block(block_a, s);

      for (size_t i = 0; i < 16 && offset + i < length; i++) {
        output[offset + i] = input[offset + i] ^ s[i];
      }
      offset += 16;
    }
  }

  void build_block_a(uint8_t *block, uint32_t counter) {
    memset(block, 0, 16);
    block[0] = 0x01; // Flags
    // block[1..4] already 0
    block[5] = 0x00; // Direction (0 = uplink)
    block[6] = (session_.dev_addr) & 0xFF;
    block[7] = (session_.dev_addr >> 8) & 0xFF;
    block[8] = (session_.dev_addr >> 16) & 0xFF;
    block[9] = (session_.dev_addr >> 24) & 0xFF;
    block[10] = (session_.frame_counter) & 0xFF;
    block[11] = (session_.frame_counter >> 8) & 0xFF;
    block[12] = 0x00; // Upper FrameCounter bytes
    block[13] = 0x00;
    block[14] = 0x00;
    block[15] = counter;
  }

  void calculate_mic(const uint8_t *data, size_t len, uint8_t *mic_out) {
    uint8_t b0[16] = { 0 };
    b0[0] = 0x49; // b0 flags
    b0[5] = 0x00; // Direction uplink
    b0[6] = (session_.dev_addr) & 0xFF;
    b0[7] = (session_.dev_addr >> 8) & 0xFF;
    b0[8] = (session_.dev_addr >> 16) & 0xFF;
    b0[9] = (session_.dev_addr >> 24) & 0xFF;
    b0[10] = (session_.frame_counter) & 0xFF;
    b0[11] = (session_.frame_counter >> 8) & 0xFF;
    b0[12] = 0x00;
    b0[13] = 0x00;
    b0[14] = (len) & 0xFF;
    b0[15] = (len >> 8) & 0xFF;

    std::vector<uint8_t> mic_input;
    mic_input.insert(mic_input.end(), b0, b0 + 16);
    mic_input.insert(mic_input.end(), data, data + len);

    CMACHelper::calculate_cmac(session_.nwk_skey, mic_input.data(), mic_input.size(), mic_out);
  }
};

}  // namespace lorawan
}  // namespace esphome
