#pragma once

#include "esphome/core/component.h"
#include "lorawan_packet.h"
#include "lorawan_crypto.h"
#include "lorawan_session.h"
#include "lora_radio_interface.h"
#include "esphome/core/log.h"

#include <vector>
#include <memory>

namespace esphome {
namespace lorawan {

class LoRaWANComponent : public Component {
 public:
  LoRaWANComponent(std::shared_ptr<LoRaRadioInterface> radio)
      : radio_(radio), joined_(false), dev_nonce_(0),
        app_key_{}, dev_eui_{}, app_eui_{} {}

  void setup() override;
  void loop() override;
  void set_app_key(const std::vector<uint8_t> &app_key) {
    // ESPHOME_ASSERT(app_key.size() == app_key_.size());
    std::copy(app_key.begin(), app_key.end(), this->app_key_.begin());
  }
  void set_dev_eui(const std::vector<uint8_t> &dev_eui) {
    // ESPHOME_ASSERT(dev_eui.size() == dev_eui_.size());
    std::copy(dev_eui.begin(), dev_eui.end(), this->dev_eui_.begin());
  }
  void set_app_eui(const std::vector<uint8_t> &app_eui) {
    // ESPHOME_ASSERT(app_eui.size() == app_eui_.size());
    std::copy(app_eui.begin(), app_eui.end(), this->app_eui_.begin());
  }
 private:
  void send_join_request_();
  void on_radio_receive_(const std::vector<uint8_t> &packet);
  bool process_join_response(const LoRaWANPacket lorawan_packet);

  //   std::array<uint8_t, 16> nwk_skey_;
  //   std::array<uint8_t, 16> app_skey_;
  std::array<uint8_t, 16> app_key_;
  std::array<uint8_t, 8> dev_eui_;
  std::array<uint8_t, 8> app_eui_;
  uint16_t dev_nonce_;
  bool joined_;
  
  std::unique_ptr<LoRaWANSession> session_;
  std::shared_ptr<LoRaRadioInterface> radio_;

  unsigned long join_request_time_;
};

}  // namespace lorawan
}  // namespace esphome
