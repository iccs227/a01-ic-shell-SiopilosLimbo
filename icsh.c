/* ICCS227: Project 1: icsh
 * Name:
 * StudentID:
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CMD_BUFFER 255

int main() {
    char buffer[MAX_CMD_BUFFER];
    char lastCommand[MAX_CMD_BUFFER] = "";
    printf("Starting IC shell\n");

    while (1) {
        printf("icsh $ ");
        fflush(stdout);
        if (fgets(buffer, MAX_CMD_BUFFER, stdin) == NULL) {
            printf("\n");
            break;	
        }
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        char *cmd = buffer;
        while (*cmd == ' ' || *cmd == '\t') {
            cmd++;
        }
        if (*cmd == '\0') {
            continue;
        }
        if (strcmp(cmd, "!!") == 0) {
            if (lastCommand[0] == '\0') {
                continue;
            } else {
                printf("%s\n", lastCommand);
                strcpy(buffer, lastCommand);
                cmd = buffer;
            }
        } else {
            strcpy(lastCommand, cmd);
        }
        if (strncmp(cmd, "echo", 4) == 0) {
            if (strcmp(cmd, "echo") == 0) {
                printf("\n");
            } else if (cmd[4] == ' ') {
                printf("%s\n", cmd + 5);
            } else {
                printf("bad command\n");
            }
        }
        else if (strncmp(cmd, "exit", 4) == 0) {
            if (strcmp(cmd, "exit") == 0) {
                printf("bye\n");
                exit(0);
            }
            else if (cmd[4] == ' ') {
                int code = atoi(cmd + 5);
                code = code & 0xFF;
                printf("bye\n");
                exit(code);
            } else {
                printf("bad command\n");
            }
        }
        else {
            printf("bad command\n");
        }
    }

    return 0;
}
