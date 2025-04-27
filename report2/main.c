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

int dir3(){
    char command[256];
    while (1) {
        printf("Enter command: ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = '\0'; // 改行文字を取り除く

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

//ここからhistory機能(!!とか)の実装




int main(){

}


