#include <ncurses.h>

typedef struct Task {
  char text[100];
  bool completed;
} Task;

int main()
{
  int scr_rows, scr_cols;
  int task_box_width = 50;
  int task_box_height = 20;

  Task task = {
    .text = "Create first task",
    .completed = true
  };

  initscr();
  getmaxyx(stdscr, scr_rows, scr_cols);

  WINDOW *win = newwin(task_box_height, task_box_width, (scr_rows - task_box_height) /2, (scr_cols - task_box_width) /2);
  box(win, 0,0);


  mvwprintw(win, 1,2, "* %s [%s]", task.text, task.completed ? "x" : " ");
  
  while (1) {
    wrefresh(win);
    napms(100);
  }
  
  endwin();
  return 0;
}
