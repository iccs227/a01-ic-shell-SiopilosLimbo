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
#include <fcntl.h>

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

        char *input_file = NULL;
        char *output_file = NULL;
        char *tokens[MAX_CMD_BUFFER / 2 + 1];
        int tokenCount = 0;

        char *saveptr;
        char *piece = strtok_r(cmd, " ", &saveptr);
        while (piece != NULL) {
            if (strcmp(piece, "<") == 0) {
                piece = strtok_r(NULL, " ", &saveptr);
                if (piece == NULL) {
                    fprintf(stderr, "Error: no input file specified\n");
                    last_status = 1;
                    break;
                }
                input_file = piece;
            }
            else if (strcmp(piece, ">") == 0) {
                piece = strtok_r(NULL, " ", &saveptr);
                if (piece == NULL) {
                    fprintf(stderr, "Error: no output file specified\n");
                    last_status = 1;
                    break;
                }
                output_file = piece;
            }
            else {
                tokens[tokenCount++] = piece;
            }
            piece = strtok_r(NULL, " ", &saveptr);
        }
        tokens[tokenCount] = NULL;

        if (last_status == 1 && tokenCount == 0) {
            continue;
        }
        if (tokenCount == 0) {
            printf("bad command\n");
            last_status = 1;
            continue;
        }

        if (strcmp(tokens[0], "echo") == 0 && tokenCount == 2 && strcmp(tokens[1], "$?") == 0) {
            int old_stdout = -1;
            int old_stdin = -1;
            if (output_file != NULL) {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    fprintf(stderr, "Error: cannot open output file '%s'\n", output_file);
                    last_status = 1;
                    continue;
                }
                old_stdout = dup(STDOUT_FILENO);
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            if (input_file != NULL) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in < 0) {
                    fprintf(stderr, "Error: cannot open input file '%s'\n", input_file);
                    last_status = 1;
                    continue;
                }
                old_stdin = dup(STDIN_FILENO);
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            printf("%d\n", last_status);
            last_status = 0;
            if (old_stdout >= 0) {
                dup2(old_stdout, STDOUT_FILENO);
                close(old_stdout);
            }
            if (old_stdin >= 0) {
                dup2(old_stdin, STDIN_FILENO);
                close(old_stdin);
            }
            continue;
        }
        else if (strcmp(tokens[0], "echo") == 0) {
            int old_stdout = -1;
            int old_stdin = -1;
            if (output_file != NULL) {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    fprintf(stderr, "Error: cannot open output file '%s'\n", output_file);
                    last_status = 1;
                    continue;
                }
                old_stdout = dup(STDOUT_FILENO);
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            if (input_file != NULL) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in < 0) {
                    fprintf(stderr, "Error: cannot open input file '%s'\n", input_file);
                    last_status = 1;
                    continue;
                }
                old_stdin = dup(STDIN_FILENO);
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (tokenCount == 1) {
                printf("\n");
            } else {
                for (int i = 1; i < tokenCount; i++) {
                    printf("%s", tokens[i]);
                    if (i < tokenCount - 1) printf(" ");
                }
                printf("\n");
            }
            last_status = 0;
            if (old_stdout >= 0) {
                dup2(old_stdout, STDOUT_FILENO);
                close(old_stdout);
            }
            if (old_stdin >= 0) {
                dup2(old_stdin, STDIN_FILENO);
                close(old_stdin);
            }
            continue;
        }
        else if (strcmp(tokens[0], "exit") == 0) {
            int old_stdout = -1;
            int old_stdin = -1;
            if (output_file != NULL) {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    fprintf(stderr, "Error: cannot open output file '%s'\n", output_file);
                    last_status = 1;
                    continue;
                }
                old_stdout = dup(STDOUT_FILENO);
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            if (input_file != NULL) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in < 0) {
                    fprintf(stderr, "Error: cannot open input file '%s'\n", input_file);
                    last_status = 1;
                    continue;
                }
                old_stdin = dup(STDIN_FILENO);
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (tokenCount == 1) {
                printf("bye\n");
                if (isScript) fclose(infile);
                exit(0);
            } else {
                long code = strtol(tokens[1], NULL, 10) & 0xFF;
                printf("bye\n");
                if (isScript) fclose(infile);
                exit((int)code);
            }
            if (old_stdout >= 0) {
                dup2(old_stdout, STDOUT_FILENO);
                close(old_stdout);
            }
            if (old_stdin >= 0) {
                dup2(old_stdin, STDIN_FILENO);
                close(old_stdin);
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            printf("bad command\n");
            last_status = 1;
        }
        else if (pid == 0) {
            if (input_file != NULL) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in < 0) {
                    fprintf(stderr, "Error: cannot open input file '%s'\n", input_file);
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (output_file != NULL) {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    fprintf(stderr, "Error: cannot open output file '%s'\n", output_file);
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            char *exec_argv[tokenCount + 1];
            for (int i = 0; i < tokenCount; i++) {
                exec_argv[i] = tokens[i];
            }
            exec_argv[tokenCount] = NULL;
            execvp(exec_argv[0], exec_argv);
            printf("bad command\n");
            exit(1);
        }
        else {
            foreground_pid = pid;
            int status;
            waitpid(pid, &status, WUNTRACED);
            if (WIFEXITED(status)) {
                last_status = WEXITSTATUS(status);
            }
            else if (WIFSTOPPED(status)) {
                printf("\n");
                last_status = 1;
            }
            else {
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

