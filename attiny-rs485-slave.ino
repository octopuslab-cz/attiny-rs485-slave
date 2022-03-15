#define FW_MAJOR 0x01
#define FW_MINOR 0x01
#define FW_PATCH 0x01

#define KEEPALIVE 5000

#include <Adafruit_NeoPixel.h>
#include <NeoSWSerial.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define LED_PIN  PB3
Adafruit_NeoPixel strip(45, LED_PIN, NEO_GRB + NEO_KHZ800);
NeoSWSerial swSerial(PB4, PB2);

void setup() {
  swSerial.begin(19200);
  pinMode(PB1, OUTPUT);
  digitalWrite(PB1, HIGH);

  strip.begin();
  strip.show();
  strip.setBrightness(5);

  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.show();
}


long previousKeepAliveMillis = 0;
void handleKeepAlive() {
  if (millis() - previousKeepAliveMillis >= KEEPALIVE) {
    previousKeepAliveMillis = millis();

    swSerial.println("Running");
  }
}


byte rec_register;
byte rec_systemreg;

void receiveEvent(int howMany) {
  rec_register = swSerial.read();
  rec_systemreg = swSerial.read();

  swSerial.print("Register ");
  swSerial.println(rec_register, HEX);

  swSerial.print("System register ");
  swSerial.println(rec_systemreg, HEX);


  switch (rec_register) {
    //case 0x01:
    //  handle_system(howMany - 2);
    //  break;

    case 0x05:
      handle_strip(howMany - 2);
      break;
  }

  // Flush extra buffer
  while (swSerial.available()) {
    char c = swSerial.read();
  }
}

void handle_strip(int howMany) {
  // we have read request
  if (howMany == 0) {
    return;
  }

  switch (rec_systemreg) {
    case 0x03:
      set_num_leds();
      break;

    /*    case 0x04:
          set_brightness();
          break;

        case 0x05:
          set_led(howMany);
          break;
    */
    case 0x06:
      set_rainbow();
      break;

      //    case 0x08:
      //      fill_led();
      //      break;
  }
}

// 0x03
void set_num_leds() {
  byte len = swSerial.read();
  strip.updateLength(len);
}

// 0x06
void set_rainbow() {
  byte hi = swSerial.read();
  byte lo = swSerial.read();

  uint16_t hue =  hi << 8 | lo;

  strip.rainbow(hue);
  strip.show();
}

void loop() {
  handleKeepAlive();

  uint8_t len = swSerial.available();
  if (len) {
    receiveEvent(len);
  }
}
