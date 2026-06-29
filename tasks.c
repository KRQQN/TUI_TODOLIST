#include "todo.h"
#include <ctype.h>
#include <string.h>

void render_tasks(WINDOW *task_win, Task **tasks, int task_count, int highlighted) {
    werase(task_win);
    box(task_win, 0, 0);
    mvwprintw(task_win, 0, 3, " Todo List [%d tasks]", task_count);

    for (int i = 0; i < task_count && i < TB_HEIGHT - 3; i++) {
        if (i == highlighted) {
            wattron(task_win, COLOR_PAIR(1) | A_BOLD);
            mvwprintw(task_win, i + 2, 2, "%s [%s]", tasks[i]->text,
                      tasks[i]->completed ? "X" : " ");
            wattroff(task_win, COLOR_PAIR(1) | A_BOLD);
        } else {
            mvwprintw(task_win, i + 2, 2, "%s [%s]", tasks[i]->text,
                      tasks[i]->completed ? "X" : " ");
        }
    }

    if (task_count == 0) {
        mvwprintw(task_win, TB_HEIGHT / 2, (TB_WIDTH / 2) - 15, "No tasks yet. Press 'A' to add.");
    }

    wrefresh(task_win);
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

Task *create_task(int *task_count, char *text) {
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

void delete_task(Task **tasks, int *task_count, int *current_task) {
    if (*task_count <= 0) return;

    for (int i = *current_task; i < *task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    (*task_count)--;
    if (*current_task >= *task_count) {
        *current_task = (*task_count > 0) ? *current_task - 1 : 0;
    }
}
