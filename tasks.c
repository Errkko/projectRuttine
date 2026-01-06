#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tasks.h"

//---------- helpers ----------

static recurrence_t parseRecurrence(const char *s) {
    if (strcmp(s, "DAILY") == 0) return TASK_DAILY;
    if (strcmp(s, "WEEKLY") == 0) return TASK_WEEKLY;
    if (strcmp(s, "BIWEEKLY") == 0) return TASK_BIWEEKLY;
    return TASK_ONCE;
}

static priority_t parsePriority(const char *s) {
    if (strcmp(s, "HIGH") == 0) return PRIO_HIGH;
    if (strcmp(s, "MEDIUM") == 0) return PRIO_MEDIUM;
    return PRIO_LOW;
}

//convert YYYYMMDD → struct tm
static void dateIntToTm(int yyyymmdd, struct tm *out) {
    memset(out, 0, sizeof(*out));

    if (yyyymmdd == 0) return;

    int y = yyyymmdd / 10000;
    int m = (yyyymmdd / 100) % 100;
    int d = yyyymmdd % 100;

    out->tm_year = y - 1900;
    out->tm_mon  = m - 1;
    out->tm_mday = d;

    mktime(out); // normalize & calculate weekday
}

//---------- file IO ----------

int loadTasks(task_t tasks[]) {
    FILE *f = fopen("tasks.txt", "r");
    if (!f) return 0;

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), f) && count < MAX_TASKS) {
        char recurr[16], prio[16];

        if (sscanf(line,
                   " %63[^|]|%15[^|]|%15[^|]|%lld|%d",
                   tasks[count].name,
                   recurr,
                   prio,
                   &tasks[count].lastCompleted,
                   &tasks[count].startDate) != 5) {
            continue;
        }

        tasks[count].recurrence = parseRecurrence(recurr);
        tasks[count].priority   = parsePriority(prio);
        count++;
    }

    fclose(f);
    return count;
}

int saveTasks(task_t tasks[], int count) {
    FILE *f = fopen("tasks.txt", "w");
    if (!f) return 0;

    for (int i = 0; i < count; i++) {
        fprintf(f, "%s|%s|%s|%ld|%d\n",
            tasks[i].name,
            tasks[i].recurrence == TASK_DAILY ? "DAILY" :
            tasks[i].recurrence == TASK_WEEKLY ? "WEEKLY" :
            tasks[i].recurrence == TASK_BIWEEKLY ? "BIWEEKLY" : "ONCE",
            tasks[i].priority == PRIO_HIGH ? "HIGH" :
            tasks[i].priority == PRIO_MEDIUM ? "MEDIUM" : "LOW",
            (long)tasks[i].lastCompleted,
            tasks[i].startDate);
    }

    fclose(f);
    return 1;
}

/* ---------- scheduling logic ---------- */

int taskDueToday(task_t *task) {
    time_t now = time(NULL);
    struct tm today = *localtime(&now);

    /* ONCE */
    if (task->recurrence == TASK_ONCE) {
        return task->lastCompleted == 0;
    }

    /* DAILY */
    if (task->recurrence == TASK_DAILY) {
        if (task->lastCompleted == 0) return 1;
        struct tm completed = *localtime(&task->lastCompleted);
        return today.tm_year != completed.tm_year ||
               today.tm_yday != completed.tm_yday;
    }

    /* WEEKLY / BIWEEKLY */
    struct tm start;
    dateIntToTm(task->startDate, &start);

    if (today.tm_wday != start.tm_wday)
        return 0;

    time_t startEpoch = mktime(&start);
    int weeks = (int)(difftime(now, startEpoch) / (7 * 86400));

    if (task->recurrence == TASK_BIWEEKLY && (weeks % 2 != 0))
        return 0;

    if (task->lastCompleted == 0) return 1;

    struct tm completed = *localtime(&task->lastCompleted);
    return today.tm_year != completed.tm_year ||
           today.tm_yday != completed.tm_yday;
}