# ESP32_CAN_GPIO
For controlling GPIOs over CAN from Teensy to ESP32, using skpang's ESP32's CAN board


ESP32 board used for this project & code: https://www.skpang.co.uk/products/esp32-can-bus-board

Use this code to control remote relays/mosfets while minimizing wiring in your setup, by controlling your devices remotely.
```
To use:
  Upload the ESP32 sketch to the ESP32 board.
  On Teensy:
    #include <ESP32_CAN_GPIO.h>
    ESP32_CAN_GPIO power = ESP32_CAN_GPIO(&Can0, 0x9000); // power being name of device, relay? Can0 as bus, 0x9000 as the CAN ID of the ESP32 CAN board.
    
    Then in your sketch you can just control the GPIOs (0->13):
      power.toggle(1, HIGH, 2000); // toggle pin 1 every 2 seconds, starting in HIGH state
      power.timed(5, HIGH, 8000); // set pin 5 HIGH for 8 seconds
      power.digitalWrite(3, LOW); // set pin 3 LOW
      power.digitalRead(3); // get pin 3 state
      power.setPins(0b11110, HIGH); // set pins 1, 2, 3 & 4 HIGH
```


Debug output:
```
Pin 0(IO14):	0	Toggle: Off	Timer: Off
Pin 1(RX):	1	Toggle: Off	Timer: Off
Pin 2(TX):	0	Toggle: Off	Timer: Off
Pin 3(SCK):	0	Toggle: Off	Timer: Off
Pin 4(MOSI):	0	Toggle: Off	Timer: Off
Pin 5(MISO):	0	Toggle: On	Timer: 614 / 2000
Pin 6(IO21):	0	Toggle: Off	Timer: Off
Pin 7(SCL):	0	Toggle: Off	Timer: Off
Pin 8(SDA):	0	Toggle: Off	Timer: Off
Pin 9(IO13):	0	Toggle: Off	Timer: Off
Pin 10(IO12):	0	Toggle: Off	Timer: Off
Pin 11(IO27):	0	Toggle: Off	Timer: Off
Pin 12(IO33):	0	Toggle: Off	Timer: Off
Pin 13(IO32):	0	Toggle: Off	Timer: Off
```
