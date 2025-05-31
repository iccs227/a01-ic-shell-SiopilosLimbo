[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/WIXYXthJ)
# ICSH

Milestone 1:

**Assumptions & Implementation**  
- On startup, the program prints `Starting IC shell`.  
- In a `while(1)` loop, it:
  1. Calls `printf("icsh $ ")` and `fflush(stdout)` to show the prompt.
  2. Uses `fgets(buffer, MAX_CMD_BUFFER, stdin)` to read a line.
  3. Strips the trailing `\n`.
  4. Trims any leading spaces.
  5. Checks for:
     - `strcmp(cmd, "!!") == 0`
     - `strncmp(cmd, "echo", 4) == 0`
     - `strncmp(cmd, "exit", 4) == 0`
     - Anything else → `printf("bad command\n")`.
  6. Stores any new command (except `!!`) into `lastCommand` so `!!` can replay it.
- An empty line (`cmd[0] == '\0'`) just goes straight back to the prompt.

Milestone 2:

**Assumptions & Implementation**  
- If `icsh` is run with no arguments, it remains in interactive mode.  
- If given a file, it opens that file and reads it: no prompts or startup message are printed.  
- Lines in a script behave just like interactive commands:  
- Empty lines in the script are skipped.  
- On EOF (end‐of‐file) in script mode, `icsh` just  exits.  
- If the script file cannot be opened, print an error and return `1`.

Milestone 3:
**Assumptions & Implementation**  
- Any other input is treated as an external command:
  1. Split the command line into an argument.  
  2. `fork()` a child process.  
  3. In the child, call `execvp(argv_exec[0], argv_exec)`.  
     - If child fails, print `bad command` and `exit(1)`.  
  4. In the parent, call `waitpid` to wait for the child.  
  5. After the child terminates, display the next prompt or continue to the next line in the script.  
- If `fork()` fails, immediately print `bad command` and return to prompt.
