// talkclient.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
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
    printf("cutted connection with server. \n");
    exit(0);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    pthread_t tid;
    char buf[BUFSIZE];
    char sendbuf[BUFSIZE + 100];

    printf("input your name: ");
    fgets(myname, sizeof(myname), stdin);
    myname[strcspn(myname, "\n")] = '\0'; // 改行除去

    // ソケット作成
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    printf("success connection with server! \n");

    // 受信用スレッド開始
    pthread_create(&tid, NULL, recv_thread, (void *)&sock);

    while (1) {
        printf(">> ");
        fgets(buf, sizeof(buf), stdin);

        if (strncmp(buf, "/exit", 5) == 0) {
            printf("終了します。\n");
            close(sock);
            exit(0);
        }

        char timestamp[20];
        get_timestamp(timestamp, sizeof(timestamp));
        snprintf(sendbuf, sizeof(sendbuf), "%s %s: %s", timestamp, myname, buf);
        write(sock, sendbuf, strlen(sendbuf));
    }
}
