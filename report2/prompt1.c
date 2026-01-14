#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_COMMAND_LEN 256
#define MAX_EXPANDED_COMMAND_LEN 4096

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

int main() {
    char command[MAX_COMMAND_LEN];
    char expanded_command[MAX_EXPANDED_COMMAND_LEN];

    printf("Enter command: ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
        perror("fgets");
        return EXIT_FAILURE;
    }

    // 改行文字を取り除く
    command[strcspn(command, "\n")] = '\0';

    expand_wildcards(command, expanded_command);

    printf("Expanded command: %s\n", expanded_command);

    // 実際のコマンドの実行（例として printf で代用）
    printf("Executing: %s\n", expanded_command);
    // system(expanded_command);

    return 0;
}
