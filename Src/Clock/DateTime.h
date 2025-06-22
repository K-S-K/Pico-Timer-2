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

  void CopyFrom(const DateTime& other) {
    year = other.year;
    month = other.month;
    day = other.day;
    hour = other.hour;
    minute = other.minute;
    second = other.second;
  }

  void incrementSec() {
    second++;
    if (second >= 60) {
        second = 0;
        incrementMinute();
    }
  }

  void incrementMinute() {
    minute++;
    if (minute >= 60) {
        minute = 0;
        incrementHour();
    }
  }

  void incrementHour() {
    hour++;
    if (hour >= 24) {
        hour = 0;
        incrementDay();
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
  
  void DecrementSec() {
    if (second > 0) {
        second--;
    } else {
        second = 59;
        DecrementMinute();
    }
  }

  void DecrementMinute() {
    if (minute > 0) {
        minute--;
    } else {
        minute = 59;
        DecrementHour();
    }
  }

  void DecrementHour() {
    if (hour > 0) {
        hour--;
    } else {
        hour = 23;
        DecrementDay();
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
