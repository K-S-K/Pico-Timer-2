/*
  * HD44780 LCD driver for Raspberry Pi Pico using I2C
  * This implementation assumes a 4-bit interface and uses the I2C protocol.
*/

#pragma once
#include <stdint.h>

class HD44780 {
public:
  HD44780(uint8_t i2c_address, int i2c_port = 0);

  void init();
  void clear();
  void setCursor(uint8_t col, uint8_t row);
  void print(const char *text);
  void printChar(char c);
  void setBacklight(bool backlight);

private:
  void writeCommand(uint8_t cmd);
  void writeData(uint8_t data);
  void send(uint8_t value, uint8_t mode);
  void pulseEnable(uint8_t data);
  void write4Bits(uint8_t value);

  uint8_t _i2c_address;
  int _i2c_port;
  uint8_t _backlight;
};
