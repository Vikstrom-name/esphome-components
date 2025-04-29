#pragma once

#include "esphome/core/component.h"
#include "esphome/components/lora/lora.h"
#include <vector>
#include "../sx127x.h"

namespace esphome {
namespace sx127x {

class SX127xLoRa;

class SX127xLoRaListener : public SX127xListener {
 public:
  SX127xLoRaListener(SX127xLoRa *parent);
  void on_packet(const std::vector<uint8_t> &packet, float rssi, float snr) override;

 protected:
  SX127xLoRa *parent_;
};

class SX127xLoRa : public lora::LoRa, public Parented<SX127x> {
 public:
  void setup() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
  void packet_received(const std::vector<uint8_t> &packet, float rssi, float snr);

 protected:
  void send_packet(std::vector<uint8_t> &buf) const override { this->parent_->transmit_packet(buf); }
  bool should_send() override { return true; };
  size_t get_max_packet_size() override { return 255u; }
};

}  // namespace sx127x
}  // namespace esphome