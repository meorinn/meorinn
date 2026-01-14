#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_HISTORY 32
#define MAX_COMMAND_LEN 256

char history[MAX_HISTORY][MAX_COMMAND_LEN];
int history_count = 0;

void add_to_history(const char *command) {
    if (history_count < MAX_HISTORY) {
        strcpy(history[history_count], command);
        history_count++;
    } else {
        for (int i = 1; i < MAX_HISTORY; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[MAX_HISTORY - 1], command);
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void execute_command(const char *command);

void execute_last_command() {
    if (history_count > 0) {
        const char *last_command = history[history_count - 1];
        printf("%s\n", last_command);
        execute_command(last_command);  // ここで履歴に追加しないように
    } else {
        printf("No commands in history.\n");
    }
}

void execute_command_starting_with(const char *prefix) {
    for (int i = history_count - 1; i >= 0; i--) {
        if (strncmp(history[i], prefix, strlen(prefix)) == 0) {
            const char *matched_command = history[i];
            printf("%s\n", matched_command);
            execute_command(matched_command);  // ここで履歴に追加しないように
            return;
        }
    }
    printf("No command starts with '%s'.\n", prefix);
}

void execute_command(const char *command) {
    if (strcmp(command, "!!") != 0 && strncmp(command, "!", 1) != 0) {
        add_to_history(command);
    }

    if (strcmp(command, "history") == 0) {
        show_history();
    } else if (strcmp(command, "!!") == 0) {
        execute_last_command();
    } else if (command[0] == '!' && strlen(command) > 1) {
        execute_command_starting_with(command + 1);
    } else {
        printf("Executing: %s\n", command);
        // 実際のコマンドの実行は system 関数などを用いて行います
        // ここではシンプルさのために print で代用しています
        // system(command);
    }
}

int main() {
    char command[MAX_COMMAND_LEN];

    while (1) {
        printf("Enter command: ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        // 改行文字を取り除く
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            break;
        }

        execute_command(command);
    }

    return 0;
}
