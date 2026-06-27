#include "todo.h"
#include <stdio.h>

int save_tasks(const Task tasks[], int task_count) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) return 0;

    fwrite(&task_count, sizeof(int), 1, fp);
    if (task_count > 0) {
        fwrite(tasks, sizeof(Task), task_count, fp);
    }
    fclose(fp);
    return 1;
}

int load_tasks(Task tasks[], int *task_count) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        *task_count = 0;
        return 0;
    }

    if (fread(task_count, sizeof(int), 1, fp) != 1) {
        *task_count = 0;
        fclose(fp);
        return 0;
    }

    if (*task_count < 0 || *task_count > MAX_TASKS) {
        *task_count = 0;
        fclose(fp);
        return 0;
    }

    if (*task_count > 0) {
        fread(tasks, sizeof(Task), *task_count, fp);
    }

    fclose(fp);
    return 1;
}
