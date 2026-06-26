#include <ncurses.h>
#include <string.h>

typedef struct Task {
  char text[100];
  bool completed;
} Task;

void render_tasks(WINDOW *window, Task task[], int task_count);

int main()
{
  int scr_rows, scr_cols;
  int task_box_width = 50;
  int task_box_height = 20;
  bool is_running = true;
  char input_buffer[100];

  Task task = {
    .text = "Create first task",
    .completed = true
  };
  Task tasks[50];
  tasks[0] = task;
  int task_count = 1;

  initscr();
  nodelay(stdscr, TRUE);
  noecho();
  getmaxyx(stdscr, scr_rows, scr_cols);

  WINDOW *win = newwin(task_box_height, task_box_width, (scr_rows - task_box_height) /2, (scr_cols - task_box_width) /2);
  box(win, 0,0);
  

  while (is_running) {
    
    switch(getch()) {
      case 'a':
        WINDOW *add_task_win = newwin(3, task_box_width, (scr_rows + task_box_height) /2, (scr_cols - task_box_width) /2);
        box(add_task_win, 0,0);
        echo();
        mvwprintw(add_task_win, 1,2, "Task: ");
        wgetnstr(add_task_win, input_buffer, sizeof(input_buffer)-1);
        noecho();
        
        Task new_task = {0};
        strncpy(new_task.text, input_buffer, sizeof(new_task.text) -1);
        new_task.text[sizeof(new_task.text)-1] = '\0';
        new_task.completed = false;

        tasks[task_count] = new_task;
        input_buffer[0] = '\0';
        task_count++;

        werase(add_task_win);
        wrefresh(add_task_win);

        break;
      case 'q':
        is_running = false;
        break;
    }
    
    render_tasks(win, tasks, task_count);
    wrefresh(win);
    mvprintw((scr_rows -5), (scr_cols /2) -10 , "[Q]uit [A]dd task");

    napms(100);
  }
  
  endwin();
  return 0;
}

void render_tasks(WINDOW *window, Task tasks[], int task_count) {

    for(int i = 0; i< task_count; i++) {
        mvwprintw(window, i+1,2, "* %s [%s]", tasks[i].text, tasks[i].completed ? "x" : " ");
    }
}
