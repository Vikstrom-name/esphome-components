/*
Explanation of the Class and Methods:
1. Attributes:
  * mhdr_: Message Header (MHDR), used to indicate the type of the LoRaWAN
    packet (e.g., Join Request, Data Packet, etc.).
  * fcnt_: Frame counter, used to track packet sequencing in LoRaWAN.
  * fport_: Frame port, which identifies the application port of the message.
  * payload_: The actual data being transmitted in the packet.
  * mic_: Message Integrity Code (MIC), used to verify the integrity of the packet.

2. Methods:
  * construct_join_request: Creates a Join Request packet, adding the necessary fields such as AppEUI, DevEUI, and DevNonce.
  * construct_join_accept: Constructs a Join Accept packet from the decrypted payload.
  * calculate_mic: Computes the MIC for the packet using AES-CMAC with the provided app_key.
  * verify_mic: Verifies the MIC by comparing it to the provided MIC in the packet.
  * encode: Serializes the packet to a byte array for transmission.
  * decode: Deserializes the packet from a byte array.
*/

#ifndef LORAWAN_PACKET_H
#define LORAWAN_PACKET_H

#include <vector>
#include <array>
#include "esphome/core/log.h"

namespace esphome {
namespace lorawan {

class LoRaWANPacket {
 public:
  LoRaWANPacket() : mhdr_(0), fcnt_(0), fport_(0), mic_({0}) {}
  // Constructor to create a packet from a receive payload
  LoRaWANPacket(const std::vector<uint8_t>& payload);

  // Setters and Getters
  void set_mhdr(uint8_t mhdr) { mhdr_ = mhdr; }
  uint8_t get_mhdr() const { return mhdr_; }

  void set_fcnt(uint16_t fcnt) { fcnt_ = fcnt; }
  uint16_t get_fcnt() const { return fcnt_; }

  void set_fport(uint8_t fport) { fport_ = fport; }
  uint8_t get_fport() const { return fport_; }

  void set_mic(const std::array<uint8_t, 4>& mic) { mic_ = mic; }
  const std::array<uint8_t, 4>& get_mic() const { return mic_; }

  void set_payload(const std::vector<uint8_t>& payload) { payload_ = payload; }
  const std::vector<uint8_t>& get_payload() const { return payload_; }

  // Methods to construct the packet
  void construct_join_request(const std::array<uint8_t, 8>& app_eui, const std::array<uint8_t, 8>& dev_eui,
                               uint16_t dev_nonce);

  void construct_join_accept(const std::vector<uint8_t>& decrypted_payload);

  // Methods for MIC calculation and validation
  bool calculate_mic(const std::array<uint8_t, 16>& app_key, uint8_t* out_mic);
  bool verify_mic(const std::array<uint8_t, 16>& app_key);

  // Methods for encoding/decoding
  void encode(std::vector<uint8_t>& encoded_packet);
  bool decode(const std::vector<uint8_t>& encoded_packet);

 private:
  uint8_t mhdr_;  // Message header
  uint16_t fcnt_; // Frame counter
  uint8_t fport_; // Frame port
  std::array<uint8_t, 4> mic_;  // Message Integrity Code (MIC)
  std::vector<uint8_t> payload_; // Payload
};

}  // namespace lorawan
}  // namespace esphome

#endif  // LORAWAN_PACKET_H
