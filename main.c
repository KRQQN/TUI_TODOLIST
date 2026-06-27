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

int main() {
  int task_count = 0;
  int scr_rows, scr_cols;
  int task_box_width = 50;
  int task_box_height = 20;
  int current_task = 0;
  bool is_running = true;
  char input_buffer[100];
  char *menu_text = "[Q] quit [A] add [SPACE] toggle checkbox";

  Task tasks[50] = {0};
  load_tasks(tasks, &task_count);

  initscr();
  nodelay(stdscr, TRUE);
  noecho();
  getmaxyx(stdscr, scr_rows, scr_cols);
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  
  WINDOW *win = newwin(task_box_height, task_box_width, (scr_rows - task_box_height) /2, (scr_cols - task_box_width) /2);
  box(win, 0,0);

  while (is_running) {
    
    switch(getch()) {

      case 'a':
        Task new_task;
        WINDOW *add_task_win = newwin(3, task_box_width, (scr_rows + task_box_height) /2, (scr_cols - task_box_width) /2);
        box(add_task_win, 0,0);
        memset(input_buffer, 0, sizeof(input_buffer));

        echo();
        mvwprintw(add_task_win, 1,2, ": ");
        wgetnstr(add_task_win, input_buffer, sizeof(input_buffer));
        noecho();
        
        if (strlen(input_buffer) > 0 && task_count < 50) {

          strncpy(new_task.text, input_buffer, sizeof(new_task.text));
          new_task.text[sizeof(new_task.text)] = '\0';
          new_task.completed = false;

          tasks[task_count] = new_task;
          task_count++;
        }

        werase(add_task_win);
        wrefresh(add_task_win);
        save_tasks(tasks,task_count);
        break;
      case 'j':
        handle_current_task_change(&current_task, task_count, 'j');
        break;
      case 'k': 
        handle_current_task_change(&current_task, task_count, 'k');
        break;
      case ' ':
        tasks[current_task].completed = !tasks[current_task].completed;
        save_tasks(tasks,task_count);
        break;
      case 'q':
        is_running = false;
        break;
    }
    
    render_tasks(win, tasks, task_count, current_task);
    wrefresh(win);
    mvprintw((scr_rows -5), (scr_cols /2) - strlen(menu_text)/2 , menu_text);

    napms(100);
  }
  
  endwin();
  return 0;
}

void render_tasks(WINDOW *window, Task tasks[], int task_count, int highlighted_task) {

    for(int i = 0; i< task_count; i++) {
      if(i == highlighted_task) {
        wattron(window,COLOR_PAIR(1));
        mvwprintw(window, i+1,2, "* %s [%s]", tasks[i].text, tasks[i].completed ? "x" : " ");
        wattroff(window,COLOR_PAIR(1));
      } else {
        mvwprintw(window, i+1,2, "* %s [%s]", tasks[i].text, tasks[i].completed ? "x" : " ");
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

