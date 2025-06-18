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

void HD44780::init() {

  // Initialize I2C
  i2c_init((_i2c_port == 0) ? i2c0 : i2c1, 100000);

  // Set up I2C pins in respective ports
  gpio_set_function((_i2c_port == 0) ? 4 : 6, GPIO_FUNC_I2C); // SDA
  gpio_set_function((_i2c_port == 0) ? 5 : 7, GPIO_FUNC_I2C); // SCL
  gpio_pull_up((_i2c_port == 0) ? 4 : 6);
  gpio_pull_up((_i2c_port == 0) ? 5 : 7);

  sleep_ms(50);       // Wait for the LCD to power up
  write4Bits(0x30);   // Initialize the LCD in 4-bit mode

  sleep_ms(5);        // Wait for the LCD to initialize
  write4Bits(0x30);   // Second initialization

  sleep_us(150);      // Wait for the LCD to stabilize
  write4Bits(0x30);   // Third initialization
  write4Bits(0x20);   // 4-bit mode

  writeCommand(0x28); // Function set: 2 line, 5x8
  writeCommand(0x08); // Display off
  writeCommand(0x01); // Clear display

  sleep_ms(2);        // Wait for the clear command to complete
  writeCommand(0x06); // Entry mode
  writeCommand(0x0C); // Display on, no cursor
}

void HD44780::clear() {
  writeCommand(0x01);
  sleep_ms(2);
}

void HD44780::setCursor(uint8_t col, uint8_t row) {
  static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  writeCommand(0x80 | (col + row_offsets[row]));
}

void HD44780::print(const char *text) {
  while (*text) {
    printChar(*text++);
  }
}

void HD44780::printChar(char c) { send(c, RS_BIT); }

void HD44780::writeCommand(uint8_t cmd) { send(cmd, 0); }

void HD44780::writeData(uint8_t data) { send(data, RS_BIT); }

void HD44780::setBacklight(bool backlight) {
  _backlight = backlight ? LCD_BACKLIGHT : 0;
}

void HD44780::send(uint8_t value, uint8_t mode) {
  uint8_t high = (value & 0xF0) | _backlight | mode;
  uint8_t low = ((value << 4) & 0xF0) | _backlight | mode;
  write4Bits(high);
  write4Bits(low);
}

void HD44780::write4Bits(uint8_t value) {
  i2c_write_blocking((_i2c_port == 0) ? i2c0 : i2c1, 
                      _i2c_address, &value, 1, true);
  pulseEnable(value);
}

void HD44780::pulseEnable(uint8_t data) {
  uint8_t data_with_enable = data | ENABLE_BIT;
  uint8_t data_without_enable = data & ~ENABLE_BIT;

  i2c_write_blocking((_i2c_port == 0) ? i2c0 : i2c1, _i2c_address,
                     &data_with_enable, 1, true);
  sleep_us(1);
  i2c_write_blocking((_i2c_port == 0) ? i2c0 : i2c1, _i2c_address,
                     &data_without_enable, 1, true);
  sleep_us(50);
}
