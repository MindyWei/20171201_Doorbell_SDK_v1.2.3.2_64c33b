#ifndef ITP_SYS_TIME_H
#define ITP_SYS_TIME_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timeval {
  time_t      tv_sec;
  suseconds_t tv_usec;
};

struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

int gettimeofday(struct timeval *__p, void *__tz);

#ifdef __cplusplus
}
#endif

#endif // ITP_SYS_TIME_H
