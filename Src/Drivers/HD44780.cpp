/*
  * HD44780 LCD driver for Raspberry Pi Pico using I2C
  * This implementation assumes a 4-bit interface and uses the I2C protocol.
*/

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "HD44780.hpp"

#define LCD_BACKLIGHT 0x08
#define ENABLE_BIT 0x04
#define RS_BIT 0x01

HD44780::HD44780(uint8_t i2c_address, int i2c_port)
    : _i2c_address(i2c_address), _i2c_port(i2c_port),
      _backlight(LCD_BACKLIGHT) {}

void HD44780::Init() {

  // Initialize I2C
  i2c_init((_i2c_port == 0) ? i2c0 : i2c1, 100000);

  // Set up I2C pins in respective ports
  gpio_set_function((_i2c_port == 0) ? 4 : 6, GPIO_FUNC_I2C); // SDA
  gpio_set_function((_i2c_port == 0) ? 5 : 7, GPIO_FUNC_I2C); // SCL
  gpio_pull_up((_i2c_port == 0) ? 4 : 6);
  gpio_pull_up((_i2c_port == 0) ? 5 : 7);

  sleep_ms(50);       // Wait for the LCD to power up
  WriteHalf(0x30);    // Initialize the LCD in 4-bit mode

  sleep_ms(5);        // Wait for the LCD to initialize
  WriteHalf(0x30);    // Second initialization

  sleep_us(150);      // Wait for the LCD to stabilize
  WriteHalf(0x30);    // Third initialization
  WriteHalf(0x20);    // 4-bit mode

  WriteCommand(0x28); // Function set: 2 line, 5x8
  WriteCommand(0x08); // Display off
  WriteCommand(0x01); // Clear display

  sleep_ms(2);        // Wait for the clear command to complete
  WriteCommand(0x06); // Entry mode
  WriteCommand(0x0C); // Display on, no cursor
}

void HD44780::Clear() {
  WriteCommand(0x01);
  sleep_ms(2);
}

void HD44780::SetCursor(uint8_t row, uint8_t col) {
  static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  WriteCommand(0x80 | (col + row_offsets[row]));
}

void HD44780::PrintString(const char *text) {
  while (*text) {
    PrintSymbol(*text++);
  }
}

void HD44780::PrintSymbol(char c) { WriteByte(c, RS_BIT); }

void HD44780::SetBacklight(bool backlight) {
  _backlight = backlight ? LCD_BACKLIGHT : 0;
}

void HD44780::WriteCommand(uint8_t cmd) { WriteByte(cmd, 0); }

void HD44780::WriteData(uint8_t data) { WriteByte(data, RS_BIT); }

void HD44780::WriteByte(uint8_t value, uint8_t mode) {
  uint8_t high = (value & 0xF0) | _backlight | mode;
  uint8_t low = ((value << 4) & 0xF0) | _backlight | mode;
  WriteHalf(high);
  WriteHalf(low);
}

void HD44780::WriteHalf(uint8_t value) {
  i2c_write_blocking((_i2c_port == 0) ? i2c0 : i2c1, 
                      _i2c_address, &value, 1, true);
  PulseEnable(value);
}

void HD44780::PulseEnable(uint8_t data) {
  uint8_t data_with_enable = data | ENABLE_BIT;
  uint8_t data_without_enable = data & ~ENABLE_BIT;

  i2c_write_blocking((_i2c_port == 0) ? i2c0 : i2c1, _i2c_address,
                     &data_with_enable, 1, true);
  sleep_us(1);
  i2c_write_blocking((_i2c_port == 0) ? i2c0 : i2c1, _i2c_address,
                     &data_without_enable, 1, true);
  sleep_us(50);
}
