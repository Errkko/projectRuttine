#include <stdio.h>
#include <time.h>
#include <windows.h>

#include "tasks.h"
#include "menu.h"
#include "date.h"

// Display task with status and timestamp
void displayTask(int index, task_t *task) {
    char timeStr[20];
    formatTime(task->lastCompleted, timeStr, sizeof(timeStr));

    //highlight high prio
    const char *priorityStr = 
        task->priority == PRIO_HIGH ? "[HIGH] " :
        task->priority == PRIO_MEDIUM ? "[MED] " : "[LOW] ";

    const char *recurrenceStr = 
        task->recurrence == TASK_DAILY ? "DAILY" :
        task->recurrence == TASK_WEEKLY ? "WEEKLY" : 
        task->recurrence == TASK_BIWEEKLY ? "BIWEEKLY" : "ONCE"; 

    if (taskDueToday(task)) {
        printf("%s[%d] %s (%s) [DUE TODAY]\n", priorityStr, index, task->name, recurrenceStr);
    } else {
        printf("%s[%d] %s (%s) [DONE] Last done: %s\n", priorityStr, index, task->name, recurrenceStr, timeStr);
    }
}

void sortTasksByPriority(task_t tasks[], int count) {
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j< count; j++) {
            if (tasks[i].priority > tasks[j].priority) {
                task_t tmp = tasks[i];
                tasks[i] = tasks[j];
                tasks[j] = tmp;
            }
        }
    }
}

int main() {
    while (1) {
        clearScreen(); // replace with embedded display clear if needed

        // Load tasks
        task_t tasks[MAX_TASKS];
        int taskCount = loadTasks(tasks);
        printf("%d Tasks Loaded\n", taskCount); // debug info

        int today = getTodayDate();
        printf("Ruttine - %d\n\n", today);

        //sort tasks by prio
        sortTasksByPriority(tasks, taskCount);

        // Display all tasks
        for (int i = 0; i < taskCount; i++) {
            displayTask(i, &tasks[i]);
        }

        // Prompt user
        printf("\nMark task as done (-1 to exit): ");
        int choice;

        // Safe input handling
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // flush invalid input
            BufferMessage("Invalid input!", 2000);
            continue;
        }

        // Exit condition
        if (choice == -1) {
            return 0;
        }

        // Mark task as done
        if (choice >= 0 && choice < taskCount) {
            if (taskDueToday(&tasks[choice])) {
                tasks[choice].lastCompleted = time(NULL);
                saveTasks(tasks, taskCount);
            } else {
                BufferMessage("Task already completed today!", 2000);
            }
        } else {
            BufferMessage("Choice out of range!", 1000);
        }
    }

    return 0;
}