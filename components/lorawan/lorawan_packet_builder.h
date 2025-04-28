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

  bool PacketBuilder::build_join_request(std::vector<uint8_t> &out) {
    uint8_t buffer[23];  // MHDR(1) + JoinReq(18) + MIC(4)
    buffer[0] = 0x00;    // MHDR: Join Request
  
    memcpy(buffer + 1, app_eui, 8);  // AppEUI (LSB first)
    memcpy(buffer + 9, dev_eui, 8);  // DevEUI (LSB first)
  
    buffer[17] = dev_nonce & 0xFF;
    buffer[18] = (dev_nonce >> 8) & 0xFF;
  
    // Compute MIC
    cmac_calculate(app_key, buffer, 19, buffer + 19);  // MIC at buffer[19]
  
    out.assign(buffer, buffer + 23);
    return true;
  }  

  bool PacketBuilder::parse_join_accept(const uint8_t *data, size_t length, LoRaWANSessionKeys &session_out) {
    if (length < 17) return false;
    if ((data[0] & 0xE0) != 0x20) return false;  // Check MHDR
  
    uint8_t decrypted[17];
    aes_decrypt(app_key, data + 1, decrypted, 16);
  
    // TODO: verify MIC (decrypted data + MHDR)
  
    // Parse JoinAccept fields
    uint32_t app_nonce = decrypted[0] | (decrypted[1] << 8) | (decrypted[2] << 16);
    uint32_t net_id    = decrypted[3] | (decrypted[4] << 8) | (decrypted[5] << 16);
    uint32_t dev_addr  = decrypted[6] | (decrypted[7] << 8) | (decrypted[8] << 16) | (decrypted[9] << 24);
  
    // Derive session keys
    derive_session_keys(app_key, app_nonce, net_id, dev_nonce, session_out);
  
    session_out.dev_addr = dev_addr;
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

  void derive_session_keys(const uint8_t *app_key, uint32_t app_nonce, uint32_t net_id, uint32_t dev_nonce, LoRaWANSessionKeys &keys) {
    uint8_t buf[16];
    uint8_t out[16];

    // Derivation of NwkSKey (K1) and AppSKey (K2)
    // K1 and K2 are derived from AppKey, AppNonce, NetID, DevNonce, and other parameters.
    
    // Step 1: Derive NwkSKey (K1)
    // Generate input buffer (NwkSKey derivation)
    memset(buf, 0, sizeof(buf));
    buf[0] = 0x01;  // LoRaWAN specification: 0x01 for K1 derivation
    memcpy(buf + 1, &app_nonce, sizeof(app_nonce));
    memcpy(buf + 5, &net_id, sizeof(net_id));
    memcpy(buf + 9, &dev_nonce, sizeof(dev_nonce));

    aes_ctr(app_key, buf, sizeof(buf), out);  // AES in CTR mode (from AppKey and parameters)

    // Store NwkSKey
    memcpy(keys.nwk_skey, out, sizeof(keys.nwk_skey));

    // Step 2: Derive AppSKey (K2)
    memset(buf, 0, sizeof(buf));
    buf[0] = 0x02;  // LoRaWAN specification: 0x02 for K2 derivation
    memcpy(buf + 1, &app_nonce, sizeof(app_nonce));
    memcpy(buf + 5, &net_id, sizeof(net_id));
    memcpy(buf + 9, &dev_nonce, sizeof(dev_nonce));

    aes_ctr(app_key, buf, sizeof(buf), out);  // AES in CTR mode (from AppKey and parameters)

    // Store AppSKey
    memcpy(keys.app_skey, out, sizeof(keys.app_skey));
}

};

}  // namespace lorawan
}  // namespace esphome
