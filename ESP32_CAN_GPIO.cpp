#include <ESP32_CAN_GPIO.h>

int ESP32_CAN_GPIO_Base::ESP32_CAN_GPIO_OBJECTS = 0;
static ESP32_CAN_GPIO_Base* _ESP32_CAN_GPIO_OBJ[16] = { nullptr };

ESP32_CAN_GPIO::ESP32_CAN_GPIO(FlexCAN_T4_Base* _busWritePtr, uint32_t id) {
  _esp32_can_gpio_busToWrite = _busWritePtr;
  _id = id;
  _ESP32_CAN_GPIO_OBJ[ESP32_CAN_GPIO_Base::ESP32_CAN_GPIO_OBJECTS] = this;
  ESP32_CAN_GPIO_Base::ESP32_CAN_GPIO_OBJECTS++;
}

void ESP32_CAN_GPIO::digitalWrite(uint8_t pin, bool state) {
  CAN_message_t f;
  f.id = _id;
  f.flags.extended = (_id > 0x7FF) ? 1 : 0;
  f.buf[0] = 1;
  f.buf[1] = 0;
  f.buf[2] = constrain(pin, 0, 13);
  f.buf[3] = state;
  _esp32_can_gpio_busToWrite->write(f);
}

bool ESP32_CAN_GPIO::digitalRead(uint8_t pin) {
  pin = constrain(pin, 0, 13);
  CAN_message_t f;
  f.id = _id;
  f.flags.extended = (_id > 0x7FF) ? 1 : 0;
  f.buf[0] = 255;
  _esp32_can_gpio_busToWrite->write(f);
  return pin_states & (1UL << pin);
}

void ESP32_CAN_GPIO::setPins(uint32_t mask, bool state) {
  CAN_message_t f;
  f.id = _id;
  f.flags.extended = (_id > 0x7FF) ? 1 : 0;
  f.buf[0] = 2;
  f.buf[1] = mask >> 8;
  f.buf[2] = mask;
  f.buf[3] = state;
  _esp32_can_gpio_busToWrite->write(f);
}

void ESP32_CAN_GPIO::toggle(uint8_t pin, bool state, uint16_t time) {
  CAN_message_t f;
  f.id = _id;
  f.flags.extended = (_id > 0x7FF) ? 1 : 0;
  f.buf[0] = 1;
  f.buf[1] = 2;
  f.buf[2] = constrain(pin, 0, 13);
  f.buf[3] = state;
  f.buf[4] = time >> 8;
  f.buf[5] = time;
  _esp32_can_gpio_busToWrite->write(f);
}

void ESP32_CAN_GPIO::timed(uint8_t pin, bool state, uint16_t time) {
  CAN_message_t f;
  f.id = _id;
  f.flags.extended = (_id > 0x7FF) ? 1 : 0;
  f.buf[0] = 1;
  f.buf[1] = 1;
  f.buf[2] = constrain(pin, 0, 13);
  f.buf[3] = state;
  f.buf[4] = time >> 8;
  f.buf[5] = time;
  _esp32_can_gpio_busToWrite->write(f);
}

void ESP32_CAN_GPIO::_process_frame_data(const CAN_message_t &msg) {
  if ( msg.id != _id ) return;
  if ( msg.buf[0] == 254 ) pin_states = ((uint16_t)(msg.buf[1] << 8) | msg.buf[2]);
}


void ext_output3(const CAN_message_t &msg) {
  for ( int i = 0; i < ESP32_CAN_GPIO_Base::ESP32_CAN_GPIO_OBJECTS; i++ ) if ( _ESP32_CAN_GPIO_OBJ[i] ) _ESP32_CAN_GPIO_OBJ[i]->_process_frame_data(msg);
}
