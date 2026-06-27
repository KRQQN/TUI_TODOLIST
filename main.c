#include "todo.h"
#include <ctype.h>

int main(void) {
    int task_count = 0;
    int scr_rows, scr_cols;
    int current_task = 0;
    bool is_running = true;
    Task tasks[MAX_TASKS] = {0};

    load_tasks(tasks, &task_count);

    initscr();
    curs_set(0);
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    getmaxyx(stdscr, scr_rows, scr_cols);
    int x_start = (scr_cols - TB_WIDTH) / 2;
    int y_start = (scr_rows - TB_HEIGHT) / 2;

    WINDOW *task_win = newwin(TB_HEIGHT, TB_WIDTH, y_start, x_start);

    const char *menu_text = "[Q]uit [A]dd [D]elete [J/K up/down] [SPACE] Toggle";

    while (is_running) {
        int key = getch();

        if (key != ERR) {
            key = tolower(key);

            switch (key) {
                case 'a':
                    add_task(task_win, tasks, &task_count);
                    save_tasks(tasks, task_count);
                    break;
                case 'd':
                    delete_task(tasks, &task_count, &current_task);
                    save_tasks(tasks, task_count);
                    break;
                case 'j':
                case KEY_DOWN:
                    handle_navigation(&current_task, task_count, 'j');
                    break;
                case 'k':
                case KEY_UP:
                    handle_navigation(&current_task, task_count, 'k');
                    break;
                case ' ':
                    if (task_count > 0) {
                        tasks[current_task].completed = !tasks[current_task].completed;
                        save_tasks(tasks, task_count);
                    }
                    break;
                case 'q':
                    is_running = false;
                    break;
            }
        }

        render_tasks(task_win, tasks, task_count, current_task);
        wrefresh(task_win);

        mvprintw(scr_rows - 2, (scr_cols - strlen(menu_text)) / 2, "%s", menu_text);
        refresh();

        napms(100);
    }

    delwin(task_win);
    endwin();
    return 0;
}
