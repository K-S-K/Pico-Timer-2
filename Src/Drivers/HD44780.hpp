/*
  * HD44780 LCD driver for Raspberry Pi Pico using I2C
  * This implementation assumes a 4-bit interface and uses the I2C protocol.
*/

#pragma once

#include <stdint.h>

class HD44780 {
public:
  HD44780(uint8_t i2c_address, int i2c_port = 0);

  void Init();
  void Clear();
  void SetCursor(uint8_t row, uint8_t col);
  void PrintString(const char *text);
  void PrintSymbol(char c);
  void SetBacklight(bool backlight);

private:
  void WriteCommand(uint8_t cmd);
  void WriteData(uint8_t data);
  void WriteByte(uint8_t value, uint8_t mode);
  void WriteHalf(uint8_t value);
  void PulseEnable(uint8_t data);

  uint8_t _i2c_address;
  int _i2c_port;
  uint8_t _backlight;
};
