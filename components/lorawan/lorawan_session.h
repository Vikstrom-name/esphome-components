#pragma once

#include <array>
#include <cstdint>

namespace esphome {
namespace lorawan {

class LoRaWANSession {
 public:
  // Set the session keys and DevAddr
  void set_keys(const std::array<uint8_t, 16>& nwk_skey, const std::array<uint8_t, 16>& app_skey) {
    this->nwk_skey_ = nwk_skey;
    this->app_skey_ = app_skey;
  }

  // Set the device address
  void set_dev_addr(const std::array<uint8_t, 4> &addr) { dev_addr_ = addr; }
  // Get Network Session Key 
  std::array<uint8_t, 16> &get_nwk_skey() { return nwk_skey_; }
  // Get Application Session Key
  std::array<uint8_t, 16> &get_app_skey() { return app_skey_; }
  // Get Device Address
  const std::array<uint8_t, 4> &get_dev_addr() const { return dev_addr_; }
  uint32_t get_fcnt_up() const { return fcnt_up_; }
  uint32_t get_fcnt_down() const { return fcnt_down_; }
  void increment_fcnt_up() { fcnt_up_++; }
  void increment_fcnt_down() { fcnt_down_++; }

  void reset() {
    fcnt_up_ = 0;
    fcnt_down_ = 0;
  }

 private:
  std::array<uint8_t, 4> dev_addr_{};            // From Join Accept
  std::array<uint8_t, 16> nwk_skey_{};           // Derived key
  std::array<uint8_t, 16> app_skey_{};           // Derived key
  uint32_t fcnt_up_{0};                          // Frame counter
  uint32_t fcnt_down_{0};
};

}  // namespace lorawan
}  // namespace esphome
