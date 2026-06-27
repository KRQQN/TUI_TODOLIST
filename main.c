#include <ncurses.h>
#include <string.h>

typedef struct Task {
    char text[100];
    bool completed;
} Task;

void render_tasks(WINDOW *window, Task task[], int task_count, int highlighted);
void handle_current_task_change(int *current_task, int task_count, char dir);
int save_tasks(Task tasks[], int task_count);
int load_tasks(Task tasks[], int *task_count);
void delete_task(Task tasks[], int *task_count, int *current_task);
void add_task(WINDOW *add_task_win, Task tasks[], int *task_count);

int main() {
    int task_count = 0;
    int scr_rows, scr_cols;
    int task_box_width = 50;
    int task_box_height = 20;
    int current_task = 0;
    bool is_running = true;
    char *menu_text = "[Q] quit [A] add [D] delete [SPACE] toggle checkbox";

    Task tasks[50] = {0};
    load_tasks(tasks, &task_count);

    initscr();
    nodelay(stdscr, TRUE);
    noecho();
    getmaxyx(stdscr, scr_rows, scr_cols);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    int x_start = (scr_cols - task_box_width) / 2;
    int y_start = (scr_rows - task_box_height) / 2;

    WINDOW *win = newwin(task_box_height, task_box_width, y_start, x_start);
    WINDOW *add_task_win = NULL;

    while (is_running) {
        switch (getch()) {

        case 'a':
            add_task_win = newwin(3, task_box_width, y_start + task_box_height, x_start);
            add_task(add_task_win, tasks, &task_count);
            save_tasks(tasks, task_count);
            break;
        case 'd':
            delete_task(tasks, &task_count, &current_task);
            break;
        case 'j':
            handle_current_task_change(&current_task, task_count, 'j');
            break;
        case 'k':
            handle_current_task_change(&current_task, task_count, 'k');
            break;
        case ' ':
            tasks[current_task].completed = !tasks[current_task].completed;
            save_tasks(tasks, task_count);
            break;
        case 'q':
            is_running = false;
            break;
        }

        render_tasks(win, tasks, task_count, current_task);
        wrefresh(win);
        mvprintw((scr_rows - 5), (scr_cols / 2) - strlen(menu_text) / 2, menu_text);
        napms(100);
    }

    endwin();
    return 0;
}

void render_tasks(WINDOW *window, Task tasks[], int task_count, int highlighted_task) {
    werase(window);
    box(window, 0, 0);
    for (int i = 0; i < task_count; i++) {
        if (i == highlighted_task) {
            wattron(window, COLOR_PAIR(1));
            mvwprintw(window, i + 1, 2, "* %s [%s]", tasks[i].text, tasks[i].completed ? "x" : " ");
            wattroff(window, COLOR_PAIR(1));
        } else {
            mvwprintw(window, i + 1, 2, "* %s [%s]", tasks[i].text, tasks[i].completed ? "x" : " ");
        }
    }
}

void handle_current_task_change(int *current_task, int task_count, char dir) {

    if (dir == 'j') {
        if (*current_task < task_count) {
            (*current_task)++;
        }

        if (*current_task == task_count) {
            (*current_task) = 0;
        }

    } else if (dir == 'k') {

        if (*current_task == 0) {
            (*current_task) = task_count;
        }

        if (*current_task > 0) {
            (*current_task)--;
        }
    }
}

int save_tasks(Task tasks[], int task_count) {
    FILE *fp = fopen("tasks.data", "wb");
    if (!fp) {
        mvprintw(0, 0, "Error: could not write to file");
        return 0;
    }

    if (fwrite(&task_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }

    if (task_count > 0) {
        if (fwrite(tasks, sizeof(Task), task_count, fp) != (size_t)task_count) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int load_tasks(Task tasks[], int *task_count) {
    FILE *fp = fopen("tasks.data", "rb");
    if (!fp) {
        mvprintw(0, 0, "Error: could not read from file");
        *task_count = 0;
        return 0;
    }

    if (fread(task_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        *task_count = 0;
        return 0;
    }

    if (*task_count < 0) {
        *task_count = 0;
        fclose(fp);
        return 0;
    }

    if (fread(tasks, sizeof(Task), *task_count, fp) != (size_t)*task_count) {
        *task_count = 0;
    }

    fclose(fp);
    return 1;
}

void add_task(WINDOW *win, Task tasks[], int *task_count) {
    Task new_task;
    char input_buffer[100];

    box(win, 0, 0);

    echo();
    mvwprintw(win, 1, 2, ": ");
    wgetnstr(win, input_buffer, 100);
    noecho();

    if (strlen(input_buffer) > 0 && *task_count < 50) {

        strncpy(new_task.text, input_buffer, sizeof(new_task.text));
        new_task.text[sizeof(new_task.text)] = '\0';
        new_task.completed = false;

        tasks[*task_count] = new_task;
        (*task_count)++;
    }

    werase(win);
    wrefresh(win);
}

void delete_task(Task tasks[], int *task_count, int *current_task) {
    if (*task_count <= 0)
        return;

    for (int i = *current_task; i < *task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }

    (*task_count)--;

    if (*current_task >= *task_count) {
        *current_task = *task_count > 0 ? *task_count - 1 : 0;
    }
}
