#include "todo.h"
#include <ctype.h>

int main(void) {
    int task_count = 0;
    int scr_rows, scr_cols;
    int current_task = 0;
    char *text_buf;
    bool is_running = true;
    const char *menu_text = "[Q]uit [A]dd [D]elete [J/K up/down] [SPACE] Toggle";
    // todo: dynamically handle tasks & update capacity
    Task **tasks = (Task **)malloc(20 * sizeof(Task *));

    initscr();
    curs_set(0);
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    getmaxyx(stdscr, scr_rows, scr_cols);

    int x_start = (scr_cols - TB_WIDTH) / 2;
    int y_start = (scr_rows - TB_HEIGHT) / 2;

    WINDOW *task_win = newwin(TB_HEIGHT, TB_WIDTH, y_start, x_start);

    tasks = load_tasks(&task_count);
    Task_list_component *task_list = create_task_list(task_win, tasks, task_count, &current_task);

    while (is_running) {
        int key = getch();

        if (key != ERR) {
            key = tolower(key);

            switch (key) {
                case 'a':
                    text_buf = get_input(task_win);
                    Task *task = create_task(text_buf);
                    tasks[task_list->task_count] = task;
                    task_list->task_count++;
                    free(text_buf);
                    save_tasks(tasks, task_list->task_count);
                    break;
                case 'd':
                    task_list->delete_task(task_list);
                    save_tasks(tasks, task_list->task_count);
                    break;
                case 'j':
                    task_list->navigate(task_list, DOWN);
                    break;
                case 'k':
                    task_list->navigate(task_list, UP);
                    break;
                case ' ':
                    if (task_count > 0) {
                        tasks[current_task]->completed = !tasks[current_task]->completed;
                        save_tasks(tasks, task_count);
                    }
                    break;
                case 'q':
                    is_running = false;
                    break;
            }
        }

        task_list->render_taskss(task_list);
        mvprintw(2, 2, "%d", task_list->task_count);
        //  render_tasks(task_win, tasks, task_count, current_task);

        mvprintw(scr_rows - 2, (scr_cols - strlen(menu_text)) / 2, "%s", menu_text);
        refresh();
        napms(100);
    }

    for (int i = 0; i < task_count; i++) {
        free_task(tasks[i]);
    }

    delwin(task_win);
    endwin();
    return 0;
}
