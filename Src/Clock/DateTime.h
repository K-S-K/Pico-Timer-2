/*
  * Clock Related Types and Structures
    * This header defines the structures and enums used for managing date and time,
    * clock events, and alarm functionality in a system.
*/

#pragma once

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

  void IncrementSeconds(bool propagate = true)
  {
    second++;

    if (second >= 60)
    {
        second = 0;

        // If propagate is true, 
        // we increment the minute
        if (propagate)
        {
          IncrementMinutes(propagate);
        }
    }
  }

  void IncrementMinutes(bool propagate = true)
  {
    minute++;

    if (minute >= 60)
    {
        minute = 0;

        // If propagate is true, 
        // we increment the hour
        if (propagate)
        {
          IncrementHours(propagate);
        }
    }
  }

  void IncrementHours(bool propagate = true)
  {
    hour++;
    
    if (hour >= 24)
    {
        hour = 0;

        // If propagate is true, 
        // we increment the day
        if (propagate)
        {
          IncrementDays();
        }
    }
  }

  void IncrementDays()
  {
    day++;

    int maxDay = DateTime::DaysInMonth(month, year);
    if (day > maxDay)
    {
        day = 1;
        IncrementMonths();
    }
  }

  void IncrementMonths()
  {
    month++;

    if (month > 12)
    {
        month = 1;
        IncrementYears();
    }
  }

  void IncrementYears()
  {
    year++;
    day = DateTime::DaysInMonth(month, year);
  }
  
  void DecrementSeconds(bool propagate = true)
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
          DecrementMinutes(propagate);
        }
    }
  }

  void DecrementMinutes(bool propagate = true)
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
          DecrementHours(propagate);
        }
    }
  }

  void DecrementHours(bool propagate = true)
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
          DecrementDays();
        }
    }
  }

  void DecrementDays()
  {
    if (day > 1)
    {
        day--;
    }
    else
    {
        DecrementMonths();
    }
  }

  void DecrementMonths()
  {
    if (month > 1)
    {
        month--;
    }
    else
    {
        month = 12;
        year--;
    }

    day = DateTime::DaysInMonth(month, year);
  }

  void DecrementYears()
  {
    if (year > 0)
    {
        year--;
    }
    else
    {
        year = 0; // Prevent underflow
    }
    
    day = DateTime::DaysInMonth(month, year);
  }

  private:

  // Helper function to get the number of days in a month
  // considering leap years for February
  // Returns the number of days in the specified month of the specified year
  // month: 1-12 (January to December)
  // year: any valid year (e.g., 2023)
  // Returns: number of days in the month
  static int DaysInMonth(int month, int year) {
    switch (month) {
      case 2: return IsLeapYear(year) ? 29 : 28;
      case 4: case 6: case 9: case 11: return 30;
      default: return 31;
    }
  }

  // Helper function to check if a year is a leap year
  // A year is a leap year if it is divisible by 4,
  // except for end-of-century years, which must be divisible by 400.
  static bool IsLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
  }
};
