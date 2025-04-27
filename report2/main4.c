#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_HISTORY 32
#define MAX_COMMAND_LEN 256
#define MAX_EXPANDED_COMMAND_LEN 4096
#define MAX_ALIASES 100
#define DEFAULT_PROMPT "Command : "
#define MAXLENGTH 256
#define MAXARGNUM 256

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

// cd機能の実装
int cd(int argc, char *argv[]) {
    const char *dir;

    // 引数が指定されていない場合
    if (argc == 1) {
        // 環境変数HOMEを取得
        dir = getenv("HOME");
        if (dir == NULL) {
            fprintf(stderr, "HOME environment variable is not set.\n");
            return 1;
        }
    } else if (argc == 2) {
        // 引数が指定されている場合
        dir = argv[1];
    } else {
        // 引数が多すぎる場合、使い方を表示
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        return 1;
    }

    // 指定されたディレクトリに移動
    if (chdir(dir) != 0) {
        perror("chdir");
        return 1;
    }

    // 成功メッセージを表示
    printf("Changed directory to %s\n", dir);
    return 0;
}

// cat機能の実装
void cat(const char *filename) {
    char cc;
    FILE *file1; // ファイルポインタの宣言

    file1 = fopen(filename, "r"); // ファイルのオープン

    if(file1 == NULL) { // ファイルがオープンできたどうかの確認
        fprintf(stderr, "%s is not found.\n", filename);
        exit(1);
    }

    while((cc = getc(file1)) != EOF) { // ファイルの終りまで読み込むためのループ
        putchar(cc); // 文字の出力
    }

    fclose(file1); // ファイルのクローズ
}

// ここからpushd，dirs, popdコマンドを実装する
#define MAX_DIRS 100
#define MAX_PATH_LEN 1024

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

    while (fgets(line, sizeof(line), script) != NULL) {
        // 改行文字を取り除く
        line[strcspn(line, "\n")] = '\0';

        // 空行を無視
        if (strlen(line) == 0) {
            continue;
        }

        // コマンドを実行
        printf("%s\n", line);
        char *args[MAXARGNUM];
        parse(line, args);
        execute_command(args, 0);
    }
}

// コマンド解析
int parse(char buffer[], char *args[]) {
    int arg_index = 0;
    int status = 0;

    // 改行を削除
    buffer[strcspn(buffer, "\n")] = '\0';

    // 引数を分解
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        args[arg_index++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_index] = NULL;

    return status;
}

// コマンド実行
void execute_command(char *args[], int command_status) {
    if (strcmp(args[0], "cd") == 0) {
        cd(command_status, args);
    } else if (strcmp(args[0], "cat") == 0) {
        if (args[1] != NULL) {
            cat(args[1]);
        } else {
            fprintf(stderr, "Usage: cat [filename]\n");
        }
    } else if (strcmp(args[0], "pushd") == 0) {
        pushd();
    } else if (strcmp(args[0], "dirs") == 0) {
        dirs();
    } else if (strcmp(args[0], "popd") == 0) {
        popd();
    } else if (strcmp(args[0], "history") == 0) {
        show_history();
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else {
        printf("Unknown command: %s\n", args[0]);
    }
}

// メイン関数
int main(int argc, char *argv[]) {
    char command[MAX_COMMAND_LEN];

    // スクリプトファイルが指定されているかチェック
    if (argc > 1) {
        FILE *script = fopen(argv[1], "r");
        if (script == NULL) {
            perror("fopen");
            return 1;
        }

        execute_script(script);
        fclose(script);
        return 0;
    }

    // インタラクティブモード
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

        add_to_history(command);

        char *args[MAXARGNUM];
        parse(command, args);
        execute_command(args, 0);
    }

    return 0;
}
