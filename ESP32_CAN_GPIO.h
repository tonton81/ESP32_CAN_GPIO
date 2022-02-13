#if !defined(_ESP32_CAN_GPIO_H_)
#define _ESP32_CAN_GPIO_H_
#include "FlexCAN_T4.h"

class ESP32_CAN_GPIO_Base {
  public:
    virtual void _process_frame_data(const CAN_message_t &msg) = 0;
    static int ESP32_CAN_GPIO_OBJECTS;
};

class ESP32_CAN_GPIO : public ESP32_CAN_GPIO_Base {
  public:
    ESP32_CAN_GPIO(FlexCAN_T4_Base* _busWritePtr, uint32_t id);
    bool digitalRead(uint8_t pin);
    void digitalWrite(uint8_t pin, bool state);
    void setPins(uint32_t mask, bool state);
    void toggle(uint8_t pin, bool state, uint16_t time);
    void timed(uint8_t pin, bool state, uint16_t time);

  private:
    FlexCAN_T4_Base* _esp32_can_gpio_busToWrite = nullptr;
    void _process_frame_data(const CAN_message_t &msg);
    uint32_t _id = 0;
    volatile uint16_t pin_states = 0;
};

#endif