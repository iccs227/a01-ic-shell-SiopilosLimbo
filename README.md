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
 
