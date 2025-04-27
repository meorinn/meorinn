#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
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
