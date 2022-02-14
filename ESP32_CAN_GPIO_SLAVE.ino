#include <ESP32_CAN.h>
ESP32_CAN<RX_SIZE_256, TX_SIZE_16> Can0;

const bool debug = 1;
const uint32_t CAN_ID = 0x9000;
const uint8_t pins_array[14] = { 14, 16, 17, 5, 18, 19, 21, 22, 23, 13, 12, 27, 33, 32 };
const char* pins_names[14] = { "IO14", "RX", "TX", "SCK", "MOSI", "MISO", "IO21", "SCL", "SDA", "IO13", "IO12", "IO27", "IO33", "IO32" };
volatile uint32_t pins_millis[14] = { 0 };
volatile uint16_t pins_time[14] = { 0 };
volatile bool pins_toggle[14] = { 0 };

void setup() {
  Serial.begin(115200);
  Can0.onReceive(onReceive);
  Can0.setRX(26);
  Can0.setTX(25);
  Can0.begin();
  Can0.setBaudRate(125000);

  /* Note: IO14, SCK, SCL, SDA pins pull high during reboot/programming. */
  /* Here we set all pins to output after boot */
  for ( int i = 0; i < 14; i++ ) {
    pinMode(pins_array[i], OUTPUT);
    digitalWrite(pins_array[i], LOW);
  }

  const uint8_t led_pins[3] = { 2, 15, 4 }; /* TURN OFF ONBOARD RGB LIGHT */
  for ( int i = 0; i < 3; i++ ) {
    pinMode(led_pins[i], OUTPUT);
    digitalWrite(led_pins[i], HIGH);
  }
}

void onReceive(const CAN_message_t &msg) {
  if ( msg.id != CAN_ID ) return;

  if ( msg.buf[0] == 0 ) { /* set all GPIOs off or on */
    for ( int i = 0; i < 14; i++ ) {
      digitalWrite(pins_array[i], msg.buf[1]);
      pins_millis[i] = 0;
    }
  }

  else if ( msg.buf[0] == 1 ) {
    if ( msg.buf[1] == 0 ) { /* control single GPIO */
      digitalWrite(pins_array[constrain(msg.buf[2], 0, 13)], msg.buf[3]);
      pins_millis[constrain(msg.buf[2], 0, 13)] = 0;
      pins_toggle[constrain(msg.buf[2], 0, 13)] = 0;
    }
    if ( msg.buf[1] == 1 ) { /* timed output */
      digitalWrite(pins_array[constrain(msg.buf[2], 0, 13)], msg.buf[3]);
      pins_millis[constrain(msg.buf[2], 0, 13)] = millis();
      pins_toggle[constrain(msg.buf[2], 0, 13)] = 0;
      pins_time[constrain(msg.buf[2], 0, 13)] = ((uint16_t)(msg.buf[4] << 8) | msg.buf[5]);
    }
    if ( msg.buf[1] == 2 ) { /* toggle output */
      digitalWrite(pins_array[constrain(msg.buf[2], 0, 13)], msg.buf[3]);
      pins_millis[constrain(msg.buf[2], 0, 13)] = millis();
      pins_time[constrain(msg.buf[2], 0, 13)] = ((uint16_t)(msg.buf[4] << 8) | msg.buf[5]);
      pins_toggle[constrain(msg.buf[2], 0, 13)] = 1;
    }
  }

  else if ( msg.buf[0] == 2 ) { /* set multiple pins */
    uint16_t pins = ((uint16_t)(msg.buf[1] << 8) | msg.buf[2]);
    for ( int i = 0; i < 14; i++ ) {
      if ( pins & (1UL << i) ) {
        digitalWrite(pins_array[i], msg.buf[3]);
        pins_millis[i] = pins_toggle[i] = 0;
      }
    }
  }

  CAN_message_t frame;
  frame.id = CAN_ID;
  frame.flags.extended = 1;
  frame.buf[0] = 254;
  uint16_t pins = 0;
  for ( int i = 0; i < 14; i++ ) pins |= digitalRead(pins_array[i]) << i;
  frame.buf[1] = pins >> 8;
  frame.buf[2] = pins;
  Can0.write(frame);
}


void loop() {
  vTaskSuspend(CANBUS_TASK); /* prevents the busy for loop from being overwriting the task's pin writes in onReceive callback */
  for ( int i = 0; i < 14; i++ ) {
    if ( ( pins_millis[i] || pins_toggle[i] ) && (millis() - pins_millis[i] > pins_time[i]) ) {
      digitalWrite(pins_array[i], !digitalRead(pins_array[i]));
      if ( pins_toggle[i] ) pins_millis[i] = millis();
      else pins_millis[i] = 0;
    }
  }
  vTaskResume(CANBUS_TASK);

  static uint32_t t = millis();
  if ( debug && millis() - t > 1000 ) {
    for ( int i = 0; i < 14; i++ ) {
      Serial.print("Pin ");
      Serial.print(i);
      Serial.print("(");
      Serial.print(pins_names[i]);
      Serial.print("):\t");
      Serial.print(digitalRead(pins_array[i]));
      Serial.print("\tToggle: ");
      Serial.print(pins_toggle[i] ? "On" : "Off");
      Serial.print("\tTimer: ");
      Serial.println(pins_millis[i] ? (String)(millis() - pins_millis[i]) + " / " + (String)pins_time[i] : "Off");
    } Serial.println();
    t = millis();
  }
}
