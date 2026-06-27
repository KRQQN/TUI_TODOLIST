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

#define TB_HEIGHT 20
#define TB_WIDTH 50
#define MAX_TASKS 50
#define DATA_FILE "tasks.data"

int main() {
    int task_count = 0;
    int scr_rows, scr_cols;
    int current_task = 0;
    bool is_running = true;
    char *menu_text = "[Q] quit [A] add [D] delete [SPACE] toggle checkbox";

    Task tasks[50] = {0};
    load_tasks(tasks, &task_count);

    initscr();
    curs_set(0);
    nodelay(stdscr, TRUE);
    noecho();
    getmaxyx(stdscr, scr_rows, scr_cols);
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    int x_start = (scr_cols - TB_WIDTH) / 2;
    int y_start = (scr_rows - TB_HEIGHT) / 2;

    WINDOW *task_win = newwin(TB_HEIGHT, TB_WIDTH, y_start, x_start);

    while (is_running) {

        switch (getch()) {

            case 'a':
                add_task(task_win, tasks, &task_count);
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

        render_tasks(task_win, tasks, task_count, current_task);
        wrefresh(task_win);

        mvprintw((scr_rows - 5), (scr_cols / 2) - strlen(menu_text) / 2, menu_text);
        refresh();
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
    FILE *fp = fopen(DATA_FILE, "wb");
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
    FILE *fp = fopen(DATA_FILE, "rb");
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

void add_task(WINDOW *main_win, Task tasks[], int *task_count) {
    if (*task_count >= MAX_TASKS) return;

    char input_buffer[100] = {0};
    int start_y = getbegy(main_win) + getmaxy(main_win) + 1;
    int start_x = getbegx(main_win) + 2;

    WINDOW *input_win = newwin(3, TB_WIDTH - 4, start_y, start_x);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 2, ": ");

    echo();
    keypad(input_win, TRUE);
    wgetnstr(input_win, input_buffer, sizeof(input_buffer) - 1);
    noecho();

    if (strlen(input_buffer) > 0) {
        Task t = {0};
        strncpy(t.text, input_buffer, sizeof(t.text) - 1);
        t.completed = false;
        tasks[*task_count] = t;
        (*task_count)++;
    }

    delwin(input_win);
    touchwin(main_win);
}

void delete_task(Task tasks[], int *task_count, int *current_task) {
    if (*task_count <= 0) return;

    for (int i = *current_task; i < *task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }

    (*task_count)--;

    if (*current_task >= *task_count) {
        *current_task = *task_count > 0 ? *task_count - 1 : 0;
    }
}
