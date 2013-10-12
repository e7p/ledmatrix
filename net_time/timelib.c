#include "timelib.h"

#define SECONDS_OF_DAY             86400
#define TIMEZONE                   3600 /* Timezone difference in seconds. Is set to GMT+1 */
#define	IS_LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !(((year) + 300) % 400)))
#define DAYS_OF_YEAR(year)         (IS_LEAPYEAR(year) ? 366 : 365)

uint8_t days_month(uint8_t month, uint8_t year) {
  if(month == 1) {
    return 28 + IS_LEAPYEAR(year);
  }
  if(month > 6) {
    return 30 + month % 2;
  }
  return 31 - month % 2;
}

void gmtime_r(uint32_t time, struct tm* output) {
  time += TIMEZONE;
  uint16_t day_number = time / SECONDS_OF_DAY;
  uint32_t day_seconds = time % SECONDS_OF_DAY;

  output->tm_sec = day_seconds % 60;
  day_seconds /= 60;
  output->tm_min = day_seconds % 60;
  day_seconds /= 60;
  output->tm_hour = day_seconds;

  // 01.01.1900 was a Monday
  output->tm_wday = (day_number + 1) % 7;

  uint8_t year = 0;
  uint16_t days_year;
  while(1) {
    days_year = DAYS_OF_YEAR(year);
    if(day_number < days_year) break;
    day_number -= days_year;
    year++;
  }
  output->tm_year = year;

  output->tm_yday = day_number;
  uint8_t mon = 0;
  while(day_number > days_month(mon, year)) {
    day_number -= days_month(mon, year);
    mon++;
  }
  output->tm_mon = mon;

  output->tm_mday = day_number + 1;

  output->tm_isdst = 0;
}

void correct_dst(struct tm* time) {
  if(time->tm_isdst) return; // We already are set to DST
  uint8_t isdst;
  uint8_t mday = time->tm_mday;
  uint8_t wday = time->tm_wday;
  switch(time->tm_mon) {
    case 0: case 1: case 10: case 11: // Winter
    // Everything okay, we are definitely in standard time
    isdst = 0;
    break;

    case 2: // March
    // Find last Sunday
    while(wday != 0) {
      mday++;
      wday = (wday + 1) % 7;
    }
    // Sunday found
    // Is the next sunday in this month?
    if(mday <= 31) {
      if(mday >= 25 && mday == time->tm_mday) {
        // Today is the change
        isdst = (time->tm_hour > 1);
        break;
      }
      isdst = 0;
    } else {
      isdst = 1;
    }
    break;

    case 9: // October
    // Find last Sunday
    while(wday != 0) {
      mday++;
      wday = (wday + 1) % 7;
    }
    // Sunday found
    // Is the next sunday in this month?
    if(mday <= 31) {
      if(mday >= 25 && mday == time->tm_mday) {
        // Today is the change
        isdst = (time->tm_hour < 2);
        break;
      }
      isdst = 1;
    } else {
      isdst = 0;
    }
    break;

    default: // Summer
    // Everything okay, we are definitely in summer time
    isdst = 1;
  }

  if(!isdst) return; // We are done now, no adjustment.

  // Adjust time
  time->tm_hour++;
  if(time->tm_hour > 23) {
    time->tm_hour = 0;
    time->tm_mday++;
    if(time->tm_mday >= days_month(time->tm_mon, time->tm_year)) {
      time->tm_mday = 1;
      time->tm_mon++;
      // No year change can ever take place in summer time
    }
    time->tm_wday = (time->tm_wday + 1) % 7;
    time->tm_yday++; // See above
  }
  time->tm_isdst = 1;
}
