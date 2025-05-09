#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFLEN 1024
#define MAXARGNUM 256
#define MAX_HISTORY 32
#define MAX_COMMAND_LEN 256
#define MAX_EXPANDED_COMMAND_LEN 4096
#define MAX_ALIASES 100
#define DEFAULT_PROMPT "Command : "
#define MAX_DIRS 100
#define MAX_PATH_LEN 1024
#define MAXLENGTH 256


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



void cd(int argc, char *argv[]) {
    const char *dir;
    // 引数が指定されていない場合
    if (argc == 1) {
        // 環境変数HOMEを取得
        dir = getenv("HOME");
        if (dir == NULL) {
            fprintf(stderr, "HOME environment variable is not set.\n");
            return;
        }
    } else if (argc == 2) {
        // 引数が指定されている場合
        dir = argv[1];
    } else {
        // 引数が多すぎる場合、使い方を表示
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        return;
    }

    // 指定されたディレクトリに移動
    if (chdir(dir) != 0) {
        perror("chdir");
    } else {
        // 成功メッセージを表示
        printf("Changed directory to %s\n", dir);
    }
}

void cat(const char *filename) {
    char cc;
    FILE *file1; // ファイルポインタの宣言

    file1 = fopen(filename, "r"); // ファイルのオープン

    if(file1 == NULL) { // ファイルがオープンできたどうかの確認
        fprintf(stderr, "%s is not found.\n", filename);
        exit(1);
    }

    while((cc = getc(file1)) != EOF) { // ファイルの終りまで読み込むためのループ
        if(cc==10||cc>=32){
        putchar(cc); // 文字の出力
        }
    }

    fclose(file1); // ファイルのクローズ
}

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

void execute_script(FILE *script) {
    char line[MAX_COMMAND_LEN];
    char *argv[256];
    int argc=0;
    while (fgets(line, sizeof(line), script) != NULL) {
        // 改行文字を取り除く
        line[strlen(line)-1] = '\0';

            // Check for the exit command

        if (strcmp("exit", line) == 0) {
            printf("the end\n");
            return 0;
        }
        // Tokenize the input line based on spaces
        char *tp;
        tp = strtok(line, " ");
        while (tp != NULL) {
            //puts(tp); // Print each token
            argv[argc] = tp;
            argc++;
            tp = strtok(NULL, " ");
        
        }
        argv[argc]=NULL;
        
        main_command(argv);
        //１回のコマンド処理ごとにカウンタ変数の初期化
        argc=0;
    }
}


void main_command(char *argv[]){
    if (strcmp(argv[0], "!!") != 0 && strncmp(argv[0], "!", 1) != 0) {
        // Add the entire command to history
        char full_command[MAX_COMMAND_LEN] = "";
        for (int i = 0; argv[i] != NULL; i++) {
            if (i > 0) {
                strcat(full_command, " ");
            }
            strcat(full_command, argv[i]);
        }
        add_to_history(full_command);
    }
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(argv[0], aliases[i].alias) == 0) {
            argv[0] = aliases[i].command;
            break;
        }
    }

    if (strcmp(argv[0], "cat") == 0) {
        if (argv[1] != NULL) {
            cat(argv[1]);
        } 
        else {
            fprintf(stderr, "Usage: cat [filename]\n");
        }
    }
    else if (strcmp(argv[0], "cd") == 0) {
        int j=0;
        while(argv[j]!=NULL){
            j++;
        }
        cd(j,argv);
    }

    else if (strcmp(argv[0], "pushd") == 0) {
        pushd();
    } 
    else if (strcmp(argv[0], "dirs") == 0) {
        dirs();
    } 
    else if (strcmp(argv[0], "popd") == 0) {
        popd();
    }
    else if (strcmp(argv[0], "history") == 0) {
        show_history();
    }
    else if (strcmp(argv[0], "!!") == 0) {
        execute_last_command();
    }
    else if (argv[0][0] == '!' && argv[0][1]!='!' && strlen(argv[0]) > 1) {
        execute_command_starting_with(&argv[0][1]);
    }
    else if (strcmp(argv[0], "prompt") == 0) {
        if (argv[1] != NULL) {
            char new_prompt[MAX_COMMAND_LEN] = "";
            for (int i = 1; argv[i] != NULL; i++) {
                if (i > 1) {
                    strcat(new_prompt, " ");
                }
                strcat(new_prompt, argv[i]);
            }
            strcpy(prompt, new_prompt);
        } else {
            strcpy(prompt, DEFAULT_PROMPT);
        }
    } 
    else if (strcmp(argv[0], "alias") == 0) {
        if (argv[1] == NULL) {
            show_aliases();
        } else if (argv[2] != NULL) {
            set_alias(argv[1], argv[2]);
        } else {
            fprintf(stderr, "Usage: alias [alias_name command]\n");
        }
    } else if (strcmp(argv[0], "unalias") == 0) {
        if (argv[1] != NULL) {
            remove_alias(argv[1]);
        }
    }else {
        printf("Unknown command: %s\n", argv[0]);
    }
}

int main(int scr1,char *argv[256]) {
    if (scr1 > 1) {
        FILE *script = fopen(argv[1], "r");
        if (script == NULL) {
            perror("fopen");
            return 1;
        }

        execute_script(script);
        fclose(script);
        return 0;
    }

    for (;;) {
        char line[MAXLENGTH];
        
        int argc = 0;
        printf("Command: ");
        while (fgets(line, sizeof(line), stdin) != NULL) {
            // Remove the newline character from the end of the input
            line[strlen(line) - 1] = '\0';
            if(strlen(line)==0){
                continue;
            }

            // Check for the exit command
            if (strcmp("exit", line) == 0) {
                printf("the end\n");
                return 0;
            }
            // Tokenize the input line based on spaces
            char *tp;
            tp = strtok(line, " ");
            while (tp != NULL) {
                //puts(tp); // Print each token
                argv[argc] = tp;
                argc++;
                tp = strtok(NULL, " ");
            
            }
            argv[argc]=NULL;
            
            main_command(argv);
            //１回のコマンド処理ごとにカウンタ変数の初期化
            argc=0;
            
            printf("%s",prompt);
        }
    }
    for (int i = 0; i <= stack_top; ++i) {
        free(dir_stack[i]);
    }
    return 0;
}

void execute_command_starting_with(const char *prefix) {
    for (int i = history_count - 1; i >= 0; i--) {
        if (strncmp(history[i], prefix, strlen(prefix)) == 0) {
            char *argv[256];
            const char *matched_command = history[i];
            printf("%s\n", matched_command);
            int argc = 0;
            char command_copy[MAX_COMMAND_LEN];
            strcpy(command_copy, history[i]);

            char *tp=strtok(command_copy, " ");
            while (tp != NULL) {
                argv[argc] = tp;
                argc++;
                tp = strtok(NULL, " ");
            }
            argv[argc] = NULL;

            main_command(argv);
            return;
        }
    }
    printf("No command starts with '%s'.\n", prefix);
}

void execute_last_command() {
    if (history_count > 0) {
        const char *last_command = history[history_count - 1];
        printf("%s\n", last_command);

        // トークン化してコマンドを再実行
        char *argv[256];
        int argc = 0;
        char command_copy[MAX_COMMAND_LEN];
        strcpy(command_copy, last_command);

        char *tp = strtok(command_copy, " ");
        while (tp != NULL) {
            argv[argc] = tp;
            argc++;
            tp = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        main_command(argv);
    } else {
        printf("No commands in history.\n");
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
