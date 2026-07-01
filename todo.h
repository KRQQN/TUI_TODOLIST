#ifndef TODO_H
#define TODO_H

#include <menu.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef enum { UP, DOWN } Dir;

typedef struct Task {
    size_t text_len;
    char *text;
    bool completed;
} Task;

typedef struct task_c {
    WINDOW *win;
    Task *data;
    int y_pos;
    int x_pos;
    int completed;
    int focused;

} Task_component;

typedef struct task_list_c {
    WINDOW *win;
    Task **tasks_c;
    int task_count;
    int *focused;

    void (*render_taskss)(struct task_list_c *);
    void (*navigate)(struct task_list_c *, Dir dir);
    void (*update_list)(struct task_list_c *);
    void (*delete_task)(struct task_list_c *);
} Task_list_component;

#define MAX_TASKS 50
#define TB_WIDTH 50
#define TB_HEIGHT 25
#define DATA_FILE "tasks.data"

extern void render_taskss(Task_list_component *tl);
extern void navigate(Task_list_component *tl, Dir dir);
extern void update_list(Task_list_component *tl);
extern void delete_task(Task_list_component *tl);
extern Task_list_component *create_task_list(WINDOW *win, Task **tasks, int task_count, int *f);

extern void render_task(WINDOW *tasks_win, Task *task, int pos, bool highlighted);
extern void render_tasks(WINDOW *task_win, Task **tasks, int task_count, int highlighted);
extern void handle_navigation(int *current_task, int task_count, int key);
extern int save_task(FILE *fp, Task *task);
extern int save_tasks(Task **tasks, int task_count);
extern Task *load_task(FILE *fp);
extern Task **load_tasks(int *task_count);
extern char *get_input(WINDOW *task_win);
extern Task *create_task(char *buf);
extern void free_task(Task *task);

#endif
