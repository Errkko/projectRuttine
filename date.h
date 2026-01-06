#ifndef DATE_H
#define DATE_H

#include <time.h>

int getTodayDate(void);
void formatTime(time_t t, char *buffer, size_t size);

#endif