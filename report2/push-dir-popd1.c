#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_DIRS 100
#define MAX_PATH_LEN 1024
#define MAXLENGTH 256

char* dir_stack[MAX_DIRS];
int stack_top = -1;

void pushd() {
    char cwd[MAX_PATH_LEN];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    if (stack_top >= MAX_DIRS - 1) {
        fprintf(stderr, "Directory stack is full\n");
        return;
    }

    dir_stack[++stack_top] = strdup(cwd);
    if (dir_stack[stack_top] == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    printf("Directory pushed: %s\n", dir_stack[stack_top]);
}

void dirs() {
    if (stack_top == -1) {
        printf("Directory stack is empty\n");
        return;
    }

    printf("Directory stack:\n");
    for (int i = stack_top; i >= 0; --i) {
        printf("%d: %s\n", i, dir_stack[i]);
    }
}

void popd() {
    if (stack_top == -1) {
        printf("Directory stack is empty\n");
        return;
    }

    char *dir_to_change = dir_stack[stack_top];
    if (chdir(dir_to_change) != 0) {
        perror("chdir");
        return;
    }

    printf("Changed directory to: %s\n", dir_to_change);

    free(dir_stack[stack_top]);
    stack_top--;
}

int main() {
    char command[256];

    while (1) {
        printf("Enter command (pushd, dirs, popd or exit): ");
        char line[MAXLENGTH];
        while(fgets(line, sizeof(line), stdin) != NULL){
            printf("%s\n",line);
        }
        if (scanf("%255s", command) != 1) {
            fprintf(stderr, "Failed to read command\n");
            continue;
        }

        if (strcmp(command, "pushd") == 0) {
            pushd();
        } else if (strcmp(command, "dirs") == 0) {
            dirs();
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "popd") == 0) {
            popd();
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    // Free allocated memory
    for (int i = 0; i <= stack_top; ++i) {
        free(dir_stack[i]);
    }

    return 0;
}
