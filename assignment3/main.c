#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>

#define MAX_INPUT 2048
#define MAX_ARGS 512

struct Command {
    char *command;              // Main command 
    char *args[MAX_ARGS];       // Array of arguments
    char *inputFile;            // Input redirection file
    char *outputFile;           // Output redirection file
    bool background;            // Background execution flag
};

void handle_exit(struct Command *cmd);
void handle_cd(struct Command *cmd);
void handle_status(int status);
void execute_command(struct Command *cmd, int *status);
void parse_command(char *input, struct Command *cmd);
void free_command_args(struct Command *cmd);
void input_redirection(struct Command *cmd);
void output_redirection(struct Command *cmd);

int main() {
    // Handle zombie process
    char input[MAX_INPUT];
    int status = 0;

    while (1) {
        // Watch for background processes
        pid_t wpid;
        int bg_status;
        while ((wpid = waitpid(-1, &bg_status, WNOHANG)) > 0) {
            if (WIFEXITED(bg_status)) {
                printf("Background PID %d is done: exit status %d\n", wpid, WEXITSTATUS(bg_status));
            } else if (WIFSIGNALED(bg_status)) {
                printf("Background PID %d is done: terminated by signal %d\n", wpid, WTERMSIG(bg_status));
            }
            fflush(stdout); 
        }

        // Command prompt
        printf(": ");
        fflush(stdout);

        // Read input
        if (!fgets(input, MAX_INPUT, stdin)) {
            // Clear if fgets fails
            clearerr(stdin); 
            continue;
        }

        // Remove newline char \n
        input[strcspn(input, "\n")] = '\0';

        // Ignore blank lines
        if (input[0] == '\0' || input[0] == ' ') {
            continue;
        }

        // Initialize and parse command into struct
        struct Command cmd;
        parse_command(input, &cmd);

        // Execute 3 commands exit, cd, status, or other
        if (strcmp(cmd.command, "exit") == 0) {
            handle_exit(&cmd);
        } else if (strcmp(cmd.command, "cd") == 0) {
            handle_cd(&cmd);
        } else if (strcmp(cmd.command, "status") == 0) {
            handle_status(status);
        } else {
            // Execute other commands
            execute_command(&cmd, &status);

            // Free dynamic string after every command
            free_command_args(&cmd);
        }
    }
    return 0;
}

void parse_command(char *input, struct Command *cmd) {
    cmd->command = NULL;
    cmd->inputFile = NULL;
    cmd->outputFile = NULL;
    cmd->background = 0;

    // Parse and separate arguments by spaces
    int arg_count = 0;
    char *token = strtok(input, " ");

    while (token != NULL && arg_count < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            // Input redirection, NULL allows reinitializing string parsing
            token = strtok(NULL, " ");
            cmd->inputFile = token;
        } else if (strcmp(token, ">") == 0) {
            // Output redirection
            token = strtok(NULL, " ");
            cmd->outputFile = token;
        } else if (strcmp(token, "&") == 0) {
            // Background execution
            cmd->background = true;
        } else if (strcmp(token, "$$") == 0 || strstr(token, "$$") == "$$"){
            // Handle $$ variable expansion
            // get process ID of shell
            int pid = getpid();

            // int to string conversion
            int len = snprintf(NULL, 0, "%d", pid);
            char *pidStr = malloc(len + 1);

            if (pidStr != NULL){
                // Store as string
                sprintf(pidStr, "%d", pid);
                cmd->args[arg_count] = pidStr;
                arg_count++;
            }
        }
        else {
            // Set argument
            cmd->args[arg_count++] = token;
            if (cmd->command == NULL) {
                cmd->command = token;  
            }
        }
        // Move to next token
        token = strtok(NULL, " ");
    }
    // Null-terminate array for execvp() function, indicate ending of args
    cmd->args[arg_count] = NULL; 
}

void free_command_args(struct Command *cmd){
    // for (int i = 0; i < MAX_ARGS && cmd->args[i] != NULL; i++){
    //     free(cmd->args[i]);
    //     cmd->args[i] = NULL;
    // }
}
// Terminate any runing background process before exiting the shell
void handle_exit(struct Command *cmd){
    exit(0);
}

void handle_cd(struct Command *cmd) {
    // Change directory
    if (cmd->args[1] == NULL) {
        // No argument provided; go to HOME directory
        if (chdir(getenv("HOME")) != 0){
            // Error going back to HOME directory
            perror("chdir error.\n");
        }
    } else {
        // Go to specified directory
        if (chdir(cmd->args[1]) != 0) {
            // Error if invalid specified path
            perror("chdir error.\n");
        }
    }
}

// Ignore background commands and only report the most recent foreground command's status
void handle_status(int status) {
    if (WIFEXITED(status)) {
        // Print exit status
        printf("Exit value: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        // Print last foreground process that was terminated
        printf("Terminated by signal: %d\n", WTERMSIG(status));
    }
    fflush(stdout);
}

// Signal handling
// Terminate background process

void execute_command(struct Command *cmd, int *status) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error returning PID.");
        *status = 1;
    } else if (pid == 0) {
        // Child process
        
        if (cmd->inputFile != NULL){
            input_redirection(cmd);
        }
        if (cmd->outputFile != NULL){
            output_redirection(cmd);
        }
        if (execvp(cmd->command, cmd->args) == -1) {
            perror("Error.");
            exit(1);
        }
    } else {
        // Parent process
        if (cmd->background == true){
            printf("Background PID: %d\n", pid);
            fflush(stdout);
        } else {
            pid_t wpid = waitpid(pid, status, 0);
            if (wpid == -1) {
                perror("Error.");
            }
        }
    }
}

void output_redirection(struct Command *cmd){
    int file_desc = open(cmd->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_desc == -1){
        perror("Error.");
        exit(1);
    }
    // Redirect stdout to file
    dup2(file_desc, STDOUT_FILENO);

    // Closing file descriptor
    close(file_desc);
}

void input_redirection(struct Command *cmd){
    int file_desc = open(cmd->inputFile, O_RDONLY);
    if (file_desc == -1){
        perror("Error.");
        exit(1);
    }
    // Redirect stdin to file
    dup2(file_desc, STDIN_FILENO);

    // Closing file descriptor
    close(file_desc);
}