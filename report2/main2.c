#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFLEN    1024     /* コマンド入力バッファの最大長 */
#define MAXARGNUM  256     /* 引数の最大個数 */
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

char* dir_stack[MAX_DIRS];
int stack_top = -1;

/* プロトタイプ宣言 */
int parse(char [], char *[]);
void execute_command(char *[], int);
void add_to_history(const char *command);
void show_history();
void execute_last_command();
void execute_command_starting_with(const char *prefix);
void expand_wildcards(const char *command, char *expanded_command);
void set_alias(const char *alias, const char *command);
void remove_alias(const char *alias);
void show_aliases();
int cd(int argc, char *argv[]);
void cat(const char *filename);
void pushd();
void dirs();
void popd();

/*----------------------------------------------------------------------------
 *
 *  関数名   : main
 *
 *  機能概要 : シェルのメインループを実行する
 *
 *  引数     :
 *
 *  戻り値   :
 *
 *  履歴     :
 *
 *--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char command_buffer[BUFLEN]; /* コマンド入力バッファ */
    char *args[MAXARGNUM];       /* 引数を格納する配列 */
    int command_status;          /* コマンドの状態
                                    command_status = 0 : バックグラウンド実行
                                    command_status = 1 : フォアグラウンド実行
                                    command_status = 2 : シェルの終了
                                    command_status = 3 : 無視 */

    /*
     *  無限ループ開始
     */

    for(;;) {

        /*
         *  コマンドプロンプトを表示する
         */

        printf("%s", prompt);

        /*
         *  入力を行い、結果を command_buffer に格納する
         *  入力がなければループを続けてコマンドプロンプトを再表示する
         */

        if(fgets(command_buffer, BUFLEN, stdin) == NULL) {
            printf("\n");
            continue;
        }

        /*
         *  入力されたコマンドを解析する
         *
         *  結果は command_status に格納される
         */

        command_status = parse(command_buffer, args);

        /*
         *  シェルの終了コマンドであればシェルを終了する
         *  無視するコマンドであればコマンドプロンプトを再表示する
         */

        if(command_status == 2) {
            printf("done.\n");
            exit(EXIT_SUCCESS);
        } else if(command_status == 3) {
            continue;
        }

        /*
         *  コマンドを実行する
         */

        execute_command(args, command_status);
    }

    return 0;
}
void execute_last_command() {
    if (history_count > 0) {
        const char *last_command = history[history_count - 1];
        printf("%s\n", last_command);
        char *args[MAXARGNUM];
        parse(last_command, args);
        execute_command(args, 0);
    } else {
        printf("No commands in history.\n");
    }
}

void execute_command_starting_with(const char *prefix) {
    for (int i = history_count - 1; i >= 0; i--) {
        if (strncmp(history[i], prefix, strlen(prefix)) == 0) {
            const char *matched_command = history[i];
            printf("%s\n", matched_command);
            char *args[MAXARGNUM];
            parse(matched_command, args);
            execute_command(args, 0);
            return;
        }
    }
    printf("No command starts with '%s'.\n", prefix);
}


/*----------------------------------------------------------------------------
 *
 *  関数名   : parse
 *
 *  機能概要 : コマンド入力と引数を解析する
 *
 *  引数     :
 *
 *  戻り値   : コマンドの状態 :
 *                0 : バックグラウンド実行
 *                1 : フォアグラウンド実行
 *                2 : シェルの終了
 *                3 : 無視
 *
 *  履歴     :
 *
 *--------------------------------------------------------------------------*/

int parse(char buffer[],        /* コマンド入力バッファ */
          char *args[])         /* 引数を格納する配列 */
{
    int arg_index;   /* 引数のインデックス */
    int status;      /* コマンドの状態 */

    /*
     *  変数の初期化
     */

    arg_index = 0;
    status = 0;

    /*
     *  コマンド入力バッファの末尾にある改行を削除
     */

    *(buffer + (strlen(buffer) - 1)) = '\0';

    /*
     *  コマンド入力が "exit" であればシェルを終了する
     */

    if(strcmp(buffer, "exit") == 0) {
        status = 2;
        return status;
    }

    /*
     *  コマンド入力が空であれば無視する
     */

    while(*buffer != '\0') {

        /*
         *  空白文字をスキップ
         */

        while(*buffer == ' ' || *buffer == '\t') {
            *(buffer++) = '\0';
        }

        /*
         * コマンドが空であればループを抜ける
         */

        if(*buffer == '\0') {
            break;
        }

        /*
         *  引数を配列に格納する
         */

        args[arg_index] = buffer;
        ++arg_index;

        /*
         *  次の空白文字またはタブ文字までポインタを進める
         */

        while((*buffer != '\0') && (*buffer != ' ') && (*buffer != '\t')) {
            ++buffer;
        }
    }

    /*
     *  引数配列の最後にNULLをセット
     */

    args[arg_index] = NULL;

    /*
     *  引数の最後が "&" であればバックグラウンド実行と判断
     */

    if(arg_index > 0 && strcmp(args[arg_index - 1], "&") == 0) {
        --arg_index;
        args[arg_index] = NULL;
        status = 1;
    } else {
        status = 0;
    }

    /*
     *  引数がなければ無視する
     */

    if(arg_index == 0) {
        status = 3;
    }

    /*
     *  コマンドの状態を返す
     */

    return status;
}

/*----------------------------------------------------------------------------
 *
 *  関数名   : execute_command
 *
 *  機能概要 : 引数として渡されたコマンドを実行する
 *             コマンドがバックグラウンド実行かフォアグラウンド実行かを判断し
 *             フォアグラウンド実行であればプロセスの終了を待つ
 *             バックグラウンド実行であれば待たない
 *
 *  引数     :
 *
 *  戻り値   :
 *
 *  履歴     :
 *
 *--------------------------------------------------------------------------*/

void execute_command(char *args[],    /* 引数リスト */
                     int command_status)     /* コマンドの状態 */
{
    pid_t pid;    /* プロセスID */
    int status;   /* 子プロセスの終了ステータス */

    /*
     *  子プロセスを作成する
     */

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }

    /*
     *  子プロセス内でコマンドを実行する
     */

    if (pid == 0) {
        if (strcmp(args[0], "cd") == 0) {
            cd(command_status, args);
        } else if (strcmp(args[0], "cat") == 0) {
            cat(args[1]);
        } else if (strcmp(args[0], "pushd") == 0) {
            pushd();
        } else if (strcmp(args[0], "popd") == 0) {
            popd();
        } else if (strcmp(args[0], "dirs") == 0) {
            dirs();
        } else if (execvp(args[0], args) < 0) {
            perror("execvp");
            exit(1);
        }
        exit(0);
    }

    /*
     *  親プロセス内で処理を続ける
     *  バックグラウンド実行でなければ子プロセスの終了を待つ
     */

    if (command_status == 0) {
        while (wait(&status) != pid);
    }

    return;
}

/* シェル関連の追加機能 */

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

void execute_last_command() {
    if (history_count > 0) {
        const char *last_command = history[history_count - 1];
        printf("%s\n", last_command);
        char *args[MAXARGNUM];
        parse(last_command, args);
        execute_command(args, 0);
    } else {
        printf("No commands in history.\n");
    }
}

void execute_command_starting_with(const char *prefix) {
    for (int i = history_count - 1; i >= 0; i--) {
        if (strncmp(history[i], prefix, strlen(prefix)) == 0) {
            const char *matched_command = history[i];
            printf("%s\n", matched_command);
            char *args[MAXARGNUM];
            parse(matched_command, args);
            execute_command(args, 0);
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

/* コマンド機能の実装 */

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

void cat(const char *filename) {
    char cc; 
    FILE *file1;                                 /* ファイルポインタの宣言 */

    file1 = fopen(filename, "r");               /* ファイルのオープン */ 

    if(file1 == NULL){                           /* ファイルがオープンできたどうかの確認 */ 
        fprintf(stderr, "%s is not found.\n", filename); 
        exit(1); 
    } 

    while((cc = getc(file1)) != EOF){                            /* ファイルの終りまで読み込むためのループ */
        putchar(cc);                   /* 文字の出力 */ 
    } 

    fclose(file1);                                /* ファイルのクローズ */ 
}

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
