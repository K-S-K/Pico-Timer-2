#pragma once

/*
  * Clock Related Types and Structures
    * This header defines the structures and enums used for managing date and time,
    * clock events, and alarm functionality in a system.
*/

struct DateTime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};
