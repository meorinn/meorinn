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

int main() {
    for (;;) {
        char line[MAXLENGTH];
        char *argv[256];
        int argc = 0;
        printf("Command: ");
        while (fgets(line, sizeof(line), stdin) != NULL) {
            // Remove the newline character from the end of the input
            line[strlen(line) - 1] = '\0';

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
            for(int i=0; i<argc; i++){
                printf("%s",argv[i]);
            }
            
        }
    }

    return 0;
}
