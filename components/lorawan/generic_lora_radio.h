#pragma once

#include "lora_radio_interface.h"

// Forward declarations for SX drivers
class SX127x;
class SX126x;

namespace esphome {
namespace lorawan {

class GenericLoRaRadio : public LoRaRadioInterface {
 public:
  explicit GenericLoRaRadio(SX127x *radio);
  explicit GenericLoRaRadio(SX126x *radio);

  bool begin() override;
  void send(const std::vector<uint8_t> &packet) override;
  void set_rx_callback(RxCallback cb) override;
  void set_frequency(uint32_t frequency_hz) override;
  void set_tx_power(uint8_t dbm) override;
  void sleep() override;
  void receive() override;

 private:
  enum RadioType {
    RADIO_SX127X,
    RADIO_SX126X,
  };

  RadioType radio_type_;
  void *radio_;  // raw pointer to keep it generic
  RxCallback rx_callback_;
};

}  // namespace lorawan
}  // namespace esphome
