/* ICCS227: Project 1: icsh
 * Name: Scott Waters
 * StudentID: 6580478
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CMD_BUFFER 255

int main(int argc, char *argv[]) {
    char buffer[MAX_CMD_BUFFER];
    char lastCommand[MAX_CMD_BUFFER] = "";
    FILE *infile = stdin;
    int isScript = 0;
    if (argc == 2) {
        infile = fopen(argv[1], "r");
        if (infile == NULL) {
            fprintf(stderr, "Error: cannot open script file '%s'\n", argv[1]);
            return 1;
        }
        isScript = 1;
    } else if (argc > 2) {
        fprintf(stderr, "Usage: %s [scriptfile]\n", argv[0]);
        return 1;
    }
    if (!isScript) {
        printf("Starting IC shell\n");
    }
    while (1) {
        if (!isScript) {
            printf("icsh $ ");
            fflush(stdout);
        }
        if (fgets(buffer, MAX_CMD_BUFFER, infile) == NULL) {
            if (!isScript) {
                printf("\n");
            }
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
        } else if (strncmp(cmd, "exit", 4) == 0) {
            if (strcmp(cmd, "exit") == 0) {
                printf("bye\n");
                if (isScript) {
                    fclose(infile);
                }
                exit(0);
            } else if (cmd[4] == ' ') {
                int code = atoi(cmd + 5) & 0xFF;
                printf("bye\n");
                if (isScript) {
                    fclose(infile);
                }
                exit(code);
            } else {
                printf("bad command\n");
            }
        } else {
            printf("bad command\n");
        }
    }
    if (isScript) {
        fclose(infile);
    }
    return 0;
}

