#ifndef TODO_H
#define TODO_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    size_t text_len;
    char *text;
    bool completed;
} Task;

#define MAX_TASKS 50
#define TB_WIDTH 50
#define TB_HEIGHT 25
#define DATA_FILE "tasks.data"

extern void render_tasks(WINDOW *task_win, Task **tasks, int task_count, int highlighted);
extern void handle_navigation(int *current_task, int task_count, int key);
extern int save_task(FILE *fp, Task *task);
extern int save_tasks(Task **tasks, int task_count);
extern Task *load_task(FILE *fp);
extern Task **load_tasks(int *task_count);
extern void delete_task(Task **tasks, int *task_count, int *current_task);
extern char *get_input(WINDOW *task_win);
extern Task *create_task(int *task_count, char *buf);

#endif
