#include <ncurses.h>
#include <string.h>

typedef struct Task {
  char text[100];
  bool completed;
} Task;

void render_tasks(WINDOW *window, Task task[], int task_count, int highlighted);
void handle_current_task_change(int *current_task, int task_count, char dir);

int main() {
  int task_count = 3;
  int scr_rows, scr_cols;
  int task_box_width = 50;
  int task_box_height = 20;
  int current_task = 0;
  bool is_running = true;
  char input_buffer[100];

  Task task1 = {
    .text = "Create first task",
    .completed = true
  };
  Task task2 = {
    .text = "Create second task",
    .completed = true
  };
  Task task3 = {
    .text = "Create third task",
    .completed = true
  };
  Task tasks[50] = { task1, task2, task3 };

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

        break;
      case 'j':
        handle_current_task_change(&current_task, task_count, 'j');
        break;
      case 'k': 
        handle_current_task_change(&current_task, task_count, 'k');
        break;
      case 'q':
        is_running = false;
        break;
    }
    
    render_tasks(win, tasks, task_count, current_task);
    wrefresh(win);
    mvprintw((scr_rows -5), (scr_cols /2) -10 , "[Q]uit [A]dd task");

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
      }

      if (dir == 'k') {
        if (*current_task == 0) {
          (*current_task) = task_count;
        }

        if (*current_task > 0) {
          (*current_task)--;
        }
      }
}
