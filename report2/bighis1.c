#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_HISTORY 32
#define MAX_COMMAND_LEN 256
#define MAX_EXPANDED_COMMAND_LEN 4096
#define MAX_ALIASES 100
#define DEFAULT_PROMPT "Command : "

char history[MAX_HISTORY][MAX_COMMAND_LEN];
int history_count = 0;
char prompt[MAX_COMMAND_LEN] = DEFAULT_PROMPT;

typedef struct {
    char alias[MAX_COMMAND_LEN];
    char command[MAX_COMMAND_LEN];
} Alias;

Alias aliases[MAX_ALIASES];
int alias_count = 0;

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

void bighis_command(const char *command);

void execute_last_command() {
    if (history_count > 0) {
        const char *last_command = history[history_count - 1];
        printf("%s\n", last_command);
        bighis_command(last_command);
    } else {
        printf("No commands in history.\n");
    }
}

void bighis_command_starting_with(const char *prefix) {
    for (int i = history_count - 1; i >= 0; i--) {
        if (strncmp(history[i], prefix, strlen(prefix)) == 0) {
            const char *matched_command = history[i];
            printf("%s\n", matched_command);
            bighis_command(matched_command);
            return;
        }
    }
    printf("No command starts with '%s'.\n", prefix);
}

void expand_wildcards(const char *command, char *expanded_command) {
    DIR *dir;
    struct dirent *entry;
    char temp[MAX_COMMAND_LEN];
    char *token, *rest;
    int first_token = 1;

    strcpy(temp, command);
    rest = temp;
    expanded_command[0] = '\0';

    while ((token = strtok_r(rest, " ", &rest))) {
        if (strcmp(token, "*") == 0) {
            dir = opendir(".");
            if (dir == NULL) {
                perror("opendir");
                exit(EXIT_FAILURE);
            }
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] != '.') {  // 隠しファイルを無視する
                    if (!first_token) {
                        strcat(expanded_command, " ");
                    }
                    strcat(expanded_command, entry->d_name);
                    first_token = 0;
                }
            }
            closedir(dir);
        } else {
            if (!first_token) {
                strcat(expanded_command, " ");
            }
            strcat(expanded_command, token);
            first_token = 0;
        }
    }
}

void set_alias(const char *alias, const char *command) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, alias) == 0) {
            strcpy(aliases[i].command, command);
            return;
        }
    }
    if (alias_count < MAX_ALIASES) {
        strcpy(aliases[alias_count].alias, alias);
        strcpy(aliases[alias_count].command, command);
        alias_count++;
    } else {
        printf("Alias limit reached.\n");
    }
}

void remove_alias(const char *alias) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, alias) == 0) {
            for (int j = i; j < alias_count - 1; j++) {
                aliases[j] = aliases[j + 1];
            }
            alias_count--;
            return;
        }
    }
    printf("Alias not found: %s\n", alias);
}

void show_aliases() {
    for (int i = 0; i < alias_count; i++) {
        printf("%s\t%s\n", aliases[i].alias, aliases[i].command);
    }
}

void bighis_command(const char *command) {
    if (strcmp(command, "!!") != 0 && strncmp(command, "!", 1) != 0) {
        add_to_history(command);
    }

    if (strcmp(command, "history") == 0) {
        show_history();
    } else if (strcmp(command, "!!") == 0) {
        execute_last_command();
    } else if (command[0] == '!' && strlen(command) > 1) {
        bighis_command_starting_with(command + 1);
    } else if (strncmp(command, "prompt", 6) == 0) {
        if (strlen(command) > 7) {
            strcpy(prompt, command + 7);
        } else {
            strcpy(prompt, DEFAULT_PROMPT);
        }
    } else if (strncmp(command, "alias", 5) == 0) {
        if (strlen(command) == 5) {
            show_aliases();
        } else {
            char alias[MAX_COMMAND_LEN];
            char actual_command[MAX_COMMAND_LEN];
            sscanf(command + 6, "%s %s", alias, actual_command);
            set_aliFileas(alias, actual_command);
        }
    } else if (strncmp(command, "unalias", 7) == 0) {
        char alias[MAX_COMMAND_LEN];
        sscanf(command + 8, "%s", alias);
        remove_alias(alias);
    } else {
        char expanded_command[MAX_EXPANDED_COMMAND_LEN];
        expand_wildcards(command, expanded_command);

        // エイリアスを展開
        for (int i = 0; i < alias_count; i++) {
            if (strcmp(expanded_command, aliases[i].alias) == 0) {
                strcpy(expanded_command, aliases[i].command);
                break;
            }

        }

        printf("Executing: %s\n", expanded_command);
        // 実際のコマンドの実行は system 関数などを用いて行います
        // ここではシンプルさのために print で代用しています
        // system(expanded_command);
    }
}

int main() {
    char command[MAX_COMMAND_LEN];

    while (1) {
        printf("%s", prompt);
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        // 改行文字を取り除く
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            break;
        }

        bighis_command(command);
    }

    return 0;
}
