#include <stdio.h>

typedef struct Task {
  char text[100];
  bool completed;
} Task;

int main()
{

  Task task = {
    .text = "Create first task",
    .completed = true
  };

  printf("Task: %s\n", task.text);
  printf("Done: %s", task.completed ? "yes" : "no");

  return 0;
}
