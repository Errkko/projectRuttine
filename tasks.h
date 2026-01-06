#ifndef TASKS_H
#define TASKS_H

#define MAX_TASKS 32
#define NAME_LENGTH 32

typedef enum {
    TASK_ONCE = 0,
    TASK_DAILY,
    TASK_WEEKLY,
    TASK_BIWEEKLY
} recurrence_t;

typedef enum {
    PRIO_HIGH,
    PRIO_MEDIUM,
    PRIO_LOW
}priority_t;

typedef struct {
    char name[NAME_LENGTH];
    recurrence_t recurrence;
    priority_t priority;
    time_t lastCompleted;
    int startDate; //ref for weekly/biweekly
}task_t;

int loadTasks(task_t tasks[]);
int saveTasks(task_t tasks[], int count);
int taskDueToday(task_t *task);
void formatTime(time_t t, char *buffer, size_t size);

#endif
