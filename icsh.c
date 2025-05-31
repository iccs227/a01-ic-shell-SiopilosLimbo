/* ICCS227: Project 1: icsh
 * Name: Scott Waters
 * StudentID: 6580478
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_CMD_BUFFER 255

volatile pid_t foreground_pid = 0;
int last_status = 0;

void handle_sigint(int signo) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
    }
}

void handle_sigtstp(int signo) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);
    }
}

int main(int argc, char *argv[]) {
    char buffer[MAX_CMD_BUFFER];
    char lastCommand[MAX_CMD_BUFFER] = "";
    FILE *infile = stdin;
    int isScript = 0;

    struct sigaction sa_int, sa_tstp;
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
    sa_tstp.sa_handler = handle_sigtstp;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa_tstp, NULL);

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

        if (strcmp(cmd, "echo $?") == 0) {
            printf("%d\n", last_status);
            continue;
        }
        else if (strncmp(cmd, "echo", 4) == 0) {
            if (strcmp(cmd, "echo") == 0) {
                printf("\n");
                last_status = 0;
            } else if (cmd[4] == ' ') {
                printf("%s\n", cmd + 5);
                last_status = 0;
            } else {
                printf("bad command\n");
                last_status = 1;
            }
            continue;
        }
        else if (strncmp(cmd, "exit", 4) == 0) {
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
                last_status = 1;
            }
            continue;
        }

        char *argv_exec[MAX_CMD_BUFFER / 2 + 1];
        int argCount = 0;
        char *token2 = strtok(cmd, " ");
        while (token2 != NULL && argCount < (MAX_CMD_BUFFER / 2)) {
            argv_exec[argCount++] = token2;
            token2 = strtok(NULL, " ");
        }
        argv_exec[argCount] = NULL;

        pid_t pid = fork();
        if (pid < 0) {
            printf("bad command\n");
            last_status = 1;
        } else if (pid == 0) {
            execvp(argv_exec[0], argv_exec);
            printf("bad command\n");
            exit(1);
        } else {
            foreground_pid = pid;
            int status;
            waitpid(pid, &status, WUNTRACED);
            if (WIFEXITED(status)) {
                last_status = WEXITSTATUS(status);
            } else if (WIFSTOPPED(status)) {
                printf("\n");
                last_status = 1;
            } else {
                last_status = 1;
            }
            foreground_pid = 0;
        }
    }

    if (isScript) {
        fclose(infile);
    }
    return 0;
}

