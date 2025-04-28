// talkserver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#define PORT 10121
#define BUFSIZE 1024

char myname[50];

void get_timestamp(char *buf, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buf, size, "[%H:%M]", tm_info);
}

void *recv_thread(void *arg) {
    int sock = *(int *)arg;
    char buf[BUFSIZE];
    int n;

    while ((n = read(sock, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("\n%s %s", "", buf);  // 相手のメッセージ表示
        printf(">> ");
        fflush(stdout);
    }
    printf("接続が切れました。\n");
    exit(0);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    pthread_t tid;
    char buf[BUFSIZE];
    char sendbuf[BUFSIZE + 100];

    printf("あなたの名前を入力してください: ");
    fgets(myname, sizeof(myname), stdin);
    myname[strcspn(myname, "\n")] = '\0'; // 改行除去

    // ソケット作成
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_sock, 1) < 0) {
        perror("listen");
        exit(1);
    }

    printf("待機中...\n");
    len = sizeof(client_addr);
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len)) < 0) {
        perror("accept");
        exit(1);
    }
    printf("接続されました！\n");

    // 受信用スレッド開始
    pthread_create(&tid, NULL, recv_thread, (void *)&client_sock);

    while (1) {
        printf(">> ");
        fgets(buf, sizeof(buf), stdin);

        if (strncmp(buf, "/exit", 5) == 0) {
            printf("終了します。\n");
            close(client_sock);
            close(server_sock);
            exit(0);
        }

        char timestamp[20];
        get_timestamp(timestamp, sizeof(timestamp));
        snprintf(sendbuf, sizeof(sendbuf), "%s %s: %s", timestamp, myname, buf);
        write(client_sock, sendbuf, strlen(sendbuf));
    }
}
