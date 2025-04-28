#pragma once

#include "esphome/core/component.h"
#include "lorawan.h"

namespace esphome {
namespace lorawan {

class LoRaWANComponent : public Component {
 public:
  LoRaWANComponent() = default;

  void setup() override;
  void loop() override {}

  void send_unconfirmed_data(const uint8_t *payload, size_t length);

  Session session_;

 protected:
  PacketBuilder *builder_;
};

}  // namespace lorawan
}  // namespace esphome
