#include <inttypes.h>

struct tm {
  uint8_t tm_sec;   // Seconds      [0-60] (1 leap second)
  uint8_t tm_min;   // Minutes      [0-59]
  uint8_t tm_hour;  // Hours        [0-23]
  uint8_t tm_mday;  // Day          [1-31]
  uint8_t tm_mon;   // Month        [0-11]
  uint8_t tm_year;  // Year - 1900
  uint8_t tm_wday;  // Day of week  [0-6]
  uint16_t tm_yday; // Days in year [0-365]
  int8_t tm_isdst;  // DST          [-1/0/1]
};

void gmtime_r(uint32_t time, struct tm* output);
void correct_dst(struct tm* time);
