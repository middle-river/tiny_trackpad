// Tiny Trackpad
// 2020-12-29  T. Nakagawa

#include <Mouse.h>
#include <SPI.h>

constexpr int INTERVAL = 10;  // 10ms HID polling interval.
constexpr int SCALE = 1;  // Sensitivity of the trackpad.
constexpr int PIN_RESET = 2;
constexpr int PIN_SHTDWN = 3;
constexpr int PIN_CS = 4;
constexpr int PIN_SW = A2;

uint8_t read_register(uint8_t adrs) {
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(adrs);
  const uint8_t data = SPI.transfer(0x00);
  digitalWrite(PIN_CS, HIGH);
  return data;
}

void setup() {
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_SHTDWN, OUTPUT);
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_SHTDWN, LOW);
  digitalWrite(PIN_CS, HIGH);
  pinMode(PIN_SW, INPUT_PULLUP);
  digitalWrite(PIN_RESET, HIGH);
  delay(100);
  digitalWrite(PIN_RESET, LOW);

  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  Mouse.begin();
}

void loop() {
  delay(INTERVAL);

  const uint8_t motion = read_register(0x02);
  if ((motion & 0x80) != 0) {
    const int8_t dx = (int8_t)read_register(0x03);
    const int8_t dy = (int8_t)read_register(0x04);
    if (dx != 0 || dy != 0) Mouse.move(-dy * SCALE, -dx * SCALE);
  }

  static int old_btn = HIGH;
  const bool new_btn = digitalRead(PIN_SW);
  if (old_btn == HIGH && new_btn == LOW) Mouse.press();
  else if (old_btn == LOW && new_btn == HIGH) Mouse.release();
  old_btn = new_btn;
}
