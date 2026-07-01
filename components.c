#include "todo.h"

void render_task(WINDOW *task_cont, Task *task, int current_focus, int this_index) {
    bool is_focused = (this_index == current_focus);

    if (is_focused) {
        wattron(task_cont, COLOR_PAIR(1) | A_BOLD);
        wborder(task_cont, '|', '|', '=', '=', 'x', 'x', 'x', 'x');
        wattroff(task_cont, COLOR_PAIR(1) | A_BOLD);
    } else {
        box(task_cont, 0, 0);
    }

    if (task->completed) {
        wbkgd(task_cont, COLOR_PAIR(2));
    }

    mvwprintw(task_cont, 1, 2, "%s", task->text);
    mvwprintw(task_cont, 1, TB_WIDTH - 8, "[%s]", task->completed ? "X" : " ");
    wrefresh(task_cont);
}

void render_tasks(Task_list_component *tl) {
    werase(tl->win);
    box(tl->win, 0, 0);
    mvwprintw(tl->win, 0, 3, " Todo List [%d tasks]", tl->task_count);

    if (tl->task_count == 0) {
        mvwprintw(tl->win, TB_HEIGHT / 2 - 1, TB_WIDTH / 2 - 15, "No tasks yet. Press 'A' to add.");
        wrefresh(tl->win);
        return;
    }

    int container_h = 3;
    int max_visible = (TB_HEIGHT - 4) / container_h;
    int y = getbegy(tl->win);
    int x = getbegx(tl->win);

    for (int i = 0; i < tl->task_count && i < max_visible; i++) {
        int y = 2 + (i * container_h);

        WINDOW *task_cont =
            subwin(tl->win, container_h, TB_WIDTH - 4, getbegy(tl->win) + y, getbegx(tl->win) + 2);

        if (!task_cont) continue;
        werase(task_cont);

        create_task_c(task_cont, tl->tasks_c[i], *tl->focused, i);
        delwin(task_cont);
    }
    wrefresh(tl->win);
}

void navigate(Task_list_component *tl, Dir dir) {
    if (tl->task_count == 0) return;

    if (dir == DOWN) {
        *tl->focused = (*tl->focused + 1) % tl->task_count;
    } else if (dir == UP) {
        *tl->focused = (*tl->focused - 1 + tl->task_count) % tl->task_count;
    }
}

void delete_task(Task_list_component *tl) {
    int task_count = tl->task_count;
    int current_task = *tl->focused;

    if (task_count <= 0) return;

    for (int i = current_task; i < task_count - 1; i++) {
        tl->tasks_c[i] = tl->tasks_c[i + 1];
    }
    tl->task_count--;
    (*tl->focused)--;
    if (current_task >= task_count) {
        tl->task_count = (task_count > 0) ? current_task - 1 : 0;
    }
}
void update_list(Task_list_component *tl) { wrefresh(tl->win); }

// todo: component (??)
Task_list_component *create_task_list(WINDOW *win, Task **tasks, int task_count, int *f) {

    Task_list_component *task_list = malloc(sizeof(Task_list_component));
    task_list->win = win;
    task_list->tasks_c = tasks;
    task_list->task_count = task_count;
    task_list->focused = f;
    task_list->render_taskss = render_taskss;
    task_list->navigate = navigate;
    task_list->update_list = update_list;
    task_list->delete_task = delete_task;

    return task_list;
}
