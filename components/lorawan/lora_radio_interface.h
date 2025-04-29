#pragma once

#include <vector>
#include <functional>

namespace esphome {
namespace lorawan {

class LoRaRadioInterface {
 public:
  using RxCallback = std::function<void(const std::vector<uint8_t> &packet)>;

  virtual ~LoRaRadioInterface() = default;

  virtual bool begin() = 0;
  virtual void send(const std::vector<uint8_t> &packet) = 0;
  virtual void set_rx_callback(RxCallback cb) = 0;
  virtual void set_frequency(uint32_t frequency_hz) = 0;
  virtual void set_tx_power(uint8_t dbm) = 0;
  virtual void sleep() = 0;
  virtual void receive() = 0;
};

}  // namespace lorawan
}  // namespace esphome
