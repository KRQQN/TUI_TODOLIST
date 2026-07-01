#include "todo.h"
#include <ctype.h>
#include <string.h>

void render_tasks(WINDOW *main_task_win, Task **tasks, int task_count, int highlighted) {
    werase(main_task_win);
    box(main_task_win, 0, 0);
    mvwprintw(main_task_win, 0, 3, " Todo List [%d tasks]", task_count);

    if (task_count == 0) {
        mvwprintw(main_task_win, TB_HEIGHT / 2 - 1, TB_WIDTH / 2 - 15,
                  "No tasks yet. Press 'A' to add.");
        wrefresh(main_task_win);
        return;
    }

    int container_h = 3;
    int max_visible = (TB_HEIGHT - 4) / container_h;

    for (int i = 0; i < task_count && i < max_visible; i++) {
        int y = 2 + (i * container_h);

        WINDOW *task_cont = subwin(main_task_win, container_h, TB_WIDTH - 2,
                                   getbegy(main_task_win) + y, getbegx(main_task_win) + 1);

        if (!task_cont) continue;

        werase(task_cont);

        if (i == highlighted) {
            wattron(task_cont, COLOR_PAIR(1) | A_BOLD);
            wborder(task_cont, '|', '|', '=', '=', 'x', 'x', 'x', 'x');
            wattroff(task_cont, COLOR_PAIR(1) | A_BOLD);
        } else {
            box(task_cont, 0, 0);
        }

        if (tasks[i]->completed) {
            wbkgd(task_cont, COLOR_PAIR(2));
        }

        mvwprintw(task_cont, 1, 2, "%s", tasks[i]->text);
        mvwprintw(task_cont, 1, TB_WIDTH - 7, "[%s]", tasks[i]->completed ? "X" : " ");

        wrefresh(task_cont);
    }

    wrefresh(main_task_win);
}

void handle_navigation(int *current_task, int task_count, int key) {
    if (task_count == 0) return;
    if (key == 'j') {
        *current_task = (*current_task + 1) % task_count;
    } else if (key == 'k') {
        *current_task = (*current_task - 1 + task_count) % task_count;
    }
}

char *get_input(WINDOW *task_win) {

    int start_y = getbegy(task_win) + getmaxy(task_win);
    int start_x = getbegx(task_win);

    WINDOW *input_win = newwin(3, TB_WIDTH, start_y, start_x);
    box(input_win, 0, 0);

    size_t max_input = 100;
    char *buf = malloc(max_input);

    if (!buf) {
        delwin(input_win);
        return NULL;
    }

    echo();
    mvwprintw(input_win, 1, 2, "New task: ");

    while (1) {
        buf[0] = '\0';
        wgetnstr(input_win, buf, max_input - 1);

        if (strlen(buf) > max_input - 1) {
            // todo: be a bit more serious
            return NULL;
        } else {
            break;
        }
    }

    noecho();
    werase(input_win);
    wrefresh(input_win);
    touchwin(task_win);
    return buf;
}

Task *create_task(char *text) {
    Task *task = malloc(sizeof(Task *));
    if (!task) return NULL;

    task->text = strdup(text);
    task->completed = false;

    if (!task->text) {
        free(task);
        return NULL;
    }

    return task;
}

void free_task(Task *task) {
    if (task) {
        free(task->text);
        free(task);
    }
}
