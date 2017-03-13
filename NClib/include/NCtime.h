#ifndef _NCtime_H
#define _NCtime_H

#include<NC.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include<udunits2.h>

typedef enum {
    NCtimeYear = 0,
    NCtimeMonth = 1,
    NCtimeDay = 2,
    NCtimeHour = 3,
    NCtimeMinute = 4,
    NCtimeSecond = 5
} NCtimeStep;

bool NCtimeLeapYear(int);

size_t NCtimeMonthLength(int, size_t);

size_t NCtimeDayOfYear(int, size_t, size_t);

NCstate NCtimeParse(const char *, NCtimeStep, ut_unit *, double *);

NCstate NCtimePrint(NCtimeStep, ut_unit *, double, char *);

const char *NCtimeStepString(NCtimeStep);

#if defined(__cplusplus)
}
#endif

#endif /* _NCtime_H */
