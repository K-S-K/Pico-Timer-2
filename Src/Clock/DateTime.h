#pragma once

/*
  * Clock Related Types and Structures
    * This header defines the structures and enums used for managing date and time,
    * clock events, and alarm functionality in a system.
*/

#include "pico/stdlib.h"

struct DateTime {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  void CopyFrom(const DateTime& other) {
    year = other.year;
    month = other.month;
    day = other.day;
    hour = other.hour;
    minute = other.minute;
    second = other.second;
  }

  void CopyDateFrom(const DateTime& other) {
    year = other.year;
    month = other.month;
    day = other.day;
  }

  void CopyTimeFrom(const DateTime& other) {
    hour = other.hour;
    minute = other.minute;
    second = other.second;
  }

  void incrementSec(bool propagate = true)
  {
    second++;

    if (second >= 60)
    {
        second = 0;

        // If propagate is true, 
        // we increment the minute
        if (propagate)
        {
          incrementMinute(propagate);
        }
    }
  }

  void incrementMinute(bool propagate = true)
  {
    minute++;

    if (minute >= 60)
    {
        minute = 0;

        // If propagate is true, 
        // we increment the hour
        if (propagate)
        {
          incrementHour(propagate);
        }
    }
  }

  void incrementHour(bool propagate = true)
  {
    hour++;
    
    if (hour >= 24)
    {
        hour = 0;

        // If propagate is true, 
        // we increment the day
        if (propagate)
        {
          incrementDay();
        }
    }
  }

  void incrementDay() {
    day++;
    int maxDay = DateTime::daysInMonth(month, year);
    if (day > maxDay) {
        day = 1;
        incrementMonth();
    }
  }

  void incrementMonth() {
    month++;
    if (month > 12) {
        month = 1;
        incrementYear();
    }
  }

  void incrementYear() {
    year++;
    day = DateTime::daysInMonth(month, year);
  }
  
  void DecrementSec(bool propagate = true)
  {
    if (second > 0)
    {
        second--;
    }
    else
    {
        second = 59;

        // If propagate is true,
        // we decrement the minute
        if (propagate)
        {
          DecrementMinute(propagate);
        }
    }
  }

  void DecrementMinute(bool propagate = true)
  {
    if (minute > 0)
    {
        minute--;
    }
    else
    {
        minute = 59;

        // If propagate is true,
        // we decrement the hour
        if (propagate)
        {
          DecrementHour(propagate);
        }
    }
  }

  void DecrementHour(bool propagate = true)
  {
    if (hour > 0)
    {
        hour--;
    }
    else
    {
        hour = 23;

        // If propagate is true,
        // we decrement the day
        if (propagate)
        {
          DecrementDay();
        }
    }
  }

  void DecrementDay() {
    if (day > 1) {
        day--;
    } else {
        DecrementMonth();
    }
  }

  void DecrementMonth() {
    if (month > 1) {
        month--;
    } else {
        month = 12;
        year--;
    }
    day = DateTime::daysInMonth(month, year);
  }

  void DecrementYear() {
    if (year > 0) {
        year--;
    } else {
        year = 0; // Prevent underflow
    }
    day = DateTime::daysInMonth(month, year);
  }

  static int daysInMonth(int month, int year) {
    switch (month) {
      case 2: return isLeapYear(year) ? 29 : 28;
      case 4: case 6: case 9: case 11: return 30;
      default: return 31;
    }
  }

  static bool isLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
  }
};
