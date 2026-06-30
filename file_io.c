#include "todo.h"
#include <stdio.h>

int save_task(FILE *fp, Task *task) {
    if (!fp) return 0;

    size_t text_len = strlen(task->text);

    fwrite(&text_len, sizeof(size_t), 1, fp);
    fwrite(task->text, text_len, 1, fp);
    fwrite(&task->completed, sizeof(int), 1, fp);

    return 1;
}

int save_tasks(Task **tasks, int task_count) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) return 0;

    if (task_count > 0) {

        fwrite(&task_count, sizeof(int), 1, fp);
        for (int i = 0; i < task_count; i++) {
            save_task(fp, tasks[i]);
        }
    }
    fclose(fp);
    return 1;
}

Task *load_task(FILE *fp) {
    Task *task = (Task *)malloc(sizeof(Task));
    size_t text_len;

    if (fread(&text_len, sizeof(size_t), 1, fp) != 1) {
        free(task);
        return NULL;
    }

    task->text = (char *)malloc(text_len + 1);
    if (!task->text) {
        free(task);
        return NULL;
    }

    if (fread(task->text, 1, text_len, fp) != text_len) {
        free_task(task);
        return NULL;
    }
    task->text[text_len] = '\0';

    if (fread(&task->completed, sizeof(int), 1, fp) != 1) {
        free_task(task);
        return NULL;
    }

    return task;
}

Task **load_tasks(int *task_count) {
    FILE *fp = fopen(DATA_FILE, "rb");

    if (!fp) {
        return NULL;
    }

    fread(task_count, sizeof(int), 1, fp);

    Task **tasks = (Task **)malloc(*task_count * sizeof(Task *));

    if (!tasks) {
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < *task_count; i++) {
        tasks[i] = load_task(fp);

        if (!tasks[i]) {
            for (int j = 0; j < i; j++) {
                free_task(tasks[j]);
            }
            free(tasks);
            fclose(fp);
            *task_count = 0;
            return NULL;
        }
    }

    fclose(fp);
    return tasks;
}
