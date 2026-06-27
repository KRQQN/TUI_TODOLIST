#ifndef TODO_H
#define TODO_H

#include <ncurses.h>
#include <string.h>

typedef struct Task {
    char text[100];
    bool completed;
} Task;

#define MAX_TASKS 50
#define TB_WIDTH 50
#define TB_HEIGHT 25
#define DATA_FILE "tasks.data"

extern void render_tasks(WINDOW *task_win, Task tasks[], int task_count, int highlighted);
extern void handle_navigation(int *current_task, int task_count, int key);
extern int save_tasks(const Task tasks[], int task_count);
extern int load_tasks(Task tasks[], int *task_count);
extern void delete_task(Task tasks[], int *task_count, int *current_task);
extern void add_task(WINDOW *task_win, Task tasks[], int *task_count);

#endif
