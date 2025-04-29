#include "generic_lora_radio.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lorawan {


GenericLoRaRadio::GenericLoRaRadio(SX127x *radio) {
  this->radio_ = radio;
  this->radio_type_ = RADIO_SX127X;
}


GenericLoRaRadio::GenericLoRaRadio(SX126x *radio) {
  this->radio_ = radio;
  this->radio_type_ = RADIO_SX126X;
}


bool GenericLoRaRadio::begin() {
  switch (this->radio_type_) {
    case RADIO_SX127X:
      return static_cast<SX127x *>(radio_)->begin();
    case RADIO_SX126X:
      return static_cast<SX126x *>(radio_)->begin();
    default:
      return false;
  }
}


void GenericLoRaRadio::send(const std::vector<uint8_t> &packet) {
  switch (this->radio_type_) {
    case RADIO_SX127X:
      static_cast<SX127x *>(radio_)->send(packet.data(), packet.size());
      break;
    case RADIO_SX126X:
      static_cast<SX126x *>(radio_)->send(packet.data(), packet.size());
      break;
  }
}


void GenericLoRaRadio::set_rx_callback(RxCallback cb) {
  this->rx_callback_ = cb;

  // If the driver supports callbacks, connect it here
  // This would require you to patch your SX driver to support callbacks
  // this->rx_callback_(std::vector<uint8_t>(rx_buf, rx_buf + rx_len));}


void GenericLoRaRadio::set_frequency(uint32_t frequency_hz) {
  switch (this->radio_type_) {
    case RADIO_SX127X:
      static_cast<SX127x *>(radio_)->set_frequency(frequency_hz);
      break;
    case RADIO_SX126X:
      static_cast<SX126x *>(radio_)->set_frequency(frequency_hz);
      break;
  }
}


void GenericLoRaRadio::set_tx_power(uint8_t dbm) {
  switch (this->radio_type_) {
    case RADIO_SX127X:
      static_cast<SX127x *>(radio_)->set_tx_power(dbm);
      break;
    case RADIO_SX126X:
      static_cast<SX126x *>(radio_)->set_tx_power(dbm);
      break;
  }
}


void GenericLoRaRadio::sleep() {
  switch (this->radio_type_) {
    case RADIO_SX127X:
      static_cast<SX127x *>(radio_)->sleep();
      break;
    case RADIO_SX126X:
      static_cast<SX126x *>(radio_)->sleep();
      break;
  }
}


void GenericLoRaRadio::receive() {
  switch (this->radio_type_) {
    case RADIO_SX127X:
      static_cast<SX127x *>(radio_)->receive();
      break;
    case RADIO_SX126X:
      static_cast<SX126x *>(radio_)->receive();
      break;
  }
}

}  // namespace lorawan
}  // namespace esphome
