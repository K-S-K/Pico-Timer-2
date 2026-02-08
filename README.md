# Pico-Timer-2

## A FreeRTOS Raspberry PI Pico 2 Timer

### Brief Description

This project was intended to play with hardware elements using FreeRTOS.

Technically, it is a Raspberry Pi Pico 2 board, connected to an HD44780-20x4 LCD display, a DS3231 RTC, a piezo buzzer, and a rotary encoder as a user interface input device.

Historically, this project is a descendant of [my previous project about a clock based on Raspberry PI and .NET](https://github.com/K-S-K/RPIDBClock).

![Timer Menu](Doc/Pico2-timer-menu.gif)

### Project Features

#### From the external point of view, the timer:

- Displays the current date and time with every second actualization
- Displays the temperature of the RP2350 Microcontroller
- Displays the configuration of the alarm time and duration
- Displays the configuration of relay switching time
- Plays a melody when the alarm is ringing
- Set the GPIO signal of the alarm
- Set the GPIO signal for the relay
- Provide configurability to the user with a menu, including:
  - Set current date
  - Set alarm time and duration, and enable or disable the alarm
  - set relay on and off time, as well as enable or disable the relay
- All user input is available from one singlerotary button

### Technical Brief on the architecture

- Every device has its own dedicated "driver" abstraction
- The menu logic implemented as a state machine has no knowledge of the menu content
- The menu page is implemented as an abstract base class, whose behavior is controlled by polymorphism of its descendants
- The control flow is organized by FreeRTOS Queues, transferring prioritized signals

### Future development directions

For some time, I had the thought that it was the end of the story. But, now I'm thinking about following:

- Add the possibility to connect an external thermo sensor DS18B20
- Add LCD Backlight brightness control with PWM and photo-resistor-based adaptation to the environment
- Connect physical relays to control the lamp, shining on my wife's agricultural experiment
- After enough prototype-using experience, create a dedicated PCB and make it kind of complete device
