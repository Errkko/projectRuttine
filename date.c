#include <time.h>
#include <stdio.h>
#include <stddef.h>

int getTodayDate(void) {
    time_t t = time(NULL);

    struct tm *tm = localtime(&t);

    return (tm->tm_year+1900) *10000 +
            (tm->tm_mon+1) *100 +
            tm->tm_mday;
}

void formatTime(time_t t, char *buffer, size_t size)
{
    if (t == 0) {
        snprintf(buffer, size, "Never");
        return;
    }

    struct tm *tm = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d %H:%M", tm);
}