/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        utils.c                *****************************/
/**************************      Author:     Abdelrahman Sabry      *****************************/
/**************************      Date:       29 Jul                 *****************************/
/**************************      Version:    2                      *****************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

/*****************************            Includes               ********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "utils.h"
#include "exit_status.h"

/*****************************        Global Variables           ********************************/

extern char ** __environ;
static ProcessHistory history[HISTORY_SIZE];
static int history_index = 0;
static int history_count = 0;



/*****************************        Helper Functions           ********************************/

int is_internal_command(const char* command) {
    char* internal_commands[] = {"shelp", "secho", "spwd", "scp", "smv", "scd", "senvir", "stype", "sphist"};
    int commands_number = sizeof(internal_commands) / sizeof(internal_commands[0]);
    for (int i = 0; i < commands_number; i++) {
        if (strcmp(internal_commands[i], command) == 0) {
            return EXIT_SUCCESS; // Command is internal
        }
    }
    return S_EXIT_INVALID_COMMAND; // Command is not internal
}

int is_external_command(const char* command) {
    const char* path_env = getenv("PATH");
    if (path_env == NULL) return S_EXIT_INVALID_COMMAND; // PATH environment variable not set

    char path_copy[1024];
    strncpy(path_copy, path_env, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';

    char* token = strtok(path_copy, ":");
    while (token != NULL) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, command);
        if (access(full_path, X_OK) == 0) { // search for the command in each path in the $PATH env
            return EXIT_SUCCESS; // Command is external
        }
        token = strtok(NULL, ":");
    }
    return S_EXIT_INVALID_COMMAND; // Command is not external
}

void Write_syscall(const char* msg, const char* color_code) {
    if (msg != NULL) {
        write(STDOUT, color_code, strlen(color_code));  //Changing text color
        write(STDOUT, msg, strlen(msg));                // Writing the message
        write(STDOUT, white, strlen(white));            // Return to the default text color
    }
}

void printLineSeparator() {
    Write_syscall("=========================================================================================================\n", blue);
}

void WelcomeMessage() {
    printLineSeparator();
    printLineSeparator();
    Write_syscall("\n===================================       A~Sabry's Shell       =========================================\n\n", green);
    printLineSeparator();
    printLineSeparator();
}

int is_file(const char *path) {
    if (path == NULL) {
        Write_syscall("Error: Passed text pointer is NULL\n",red);
        return S_EXIT_FAILURE; // Invalid path
    }
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return S_EXIT_FILE_NOT_FOUND; // File does not exist
    }
    return S_ISREG(path_stat.st_mode) ? EXIT_SUCCESS : S_EXIT_FAILURE; // Check if it is a regular file
}

int is_directory(const char *path) {
    if (path == NULL) {
        Write_syscall("Error: Passed text pointer is NULL\n",red);
        return S_EXIT_FAILURE; // Invalid path
    }
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return S_EXIT_FILE_NOT_FOUND; // Directory does not exist
    }
    return S_ISDIR(path_stat.st_mode) ? EXIT_SUCCESS : S_EXIT_FAILURE; // Check if it is a directory
}


// Function to trim leading and trailing spaces
void trim_spaces(char *str) {
    int start = 0;
    int end = strlen(str) - 1;
    int i, j;

    // Trim leading spaces
    while (isspace((unsigned char)str[start])) {
        start++;
    }

    // Trim trailing spaces
    while (end >= start && isspace((unsigned char)str[end])) {
        end--;
    }

    // Shift characters to the beginning of the string
    for (i = start, j = 0; i <= end; i++) {
        str[j++] = str[i];
    }
    str[j] = '\0';
}

// Function to reduce multiple spaces between words to a single space
void reduce_spaces(char *str) {
    int readIndex = 0, writeIndex = 0;
    int inSpace = 0;

    while (str[readIndex] != '\0') {
        if (isspace((unsigned char)str[readIndex])) {
            if (!inSpace) {
                str[writeIndex++] = ' ';
                inSpace = 1;
            }
        } else {
            str[writeIndex++] = str[readIndex];
            inSpace = 0;
        }
        readIndex++;
    }

    // Null-terminate the string
    str[writeIndex] = '\0';

    // Trim trailing space if any
    if (writeIndex > 0 && isspace((unsigned char)str[writeIndex - 1])) {
        str[writeIndex - 1] = '\0';
    }
}


void Parse_Commands(char* full_command, char** tokens) {
    if (full_command == NULL || tokens == NULL) {
        return;
    }
    char* token = strtok(full_command, " ");
    int index  = 0;
    while (token != NULL && index < MAX_TOKENS - 1) {
        tokens[index++] = token;
        token = strtok(NULL, " ");
    }
    // tokens[index] = NULL;  // Null-terminate the tokens array
}


/**************************        Internal Commands Implementation           ****************************/

void Help_Command() {
    Write_syscall("\nThe supported commands are:\n\n", blue);
    Write_syscall("1- spwd: print working directory\n\n", blue);
    Write_syscall("2- secho: print a user input string on stdout\n\n", blue);
    Write_syscall("3- scp: copy a file to another file (cp {sourcePath} {targetPath})\n", blue);
    Write_syscall("    options: -a to append the source content to the end of the target file\n\n", green);
    Write_syscall("4- smv: move a file to another place (mv {sourcePath} {targetPath})\n", blue);
    Write_syscall("    options: -f to force overwriting the target file if exists\n\n", green);
    Write_syscall("5- scd: Change directory to the passed path\n\n", blue);
    Write_syscall("6- senvir: print all the environment variables\n\n", blue);
    Write_syscall("7- stype: print the type of the command (Internal, External or Unsupported\n\n", blue);
    Write_syscall("8- sphist: print The last 10 commands with their exit status\n\n", blue);
    Write_syscall("9- shelp: print all the supported command with a brief info about each one\n\n", blue);
    Write_syscall("10- sexit: terminate the shell\n", blue);
}

int Echo_Command(char** tokens)
{
    if (tokens == NULL) {
        Write_syscall("Error: Text pointer is NULL\n", red);
        return S_EXIT_FAILURE;
    }

    for (int i = 1; tokens[i] != NULL; i++)
    {
        Write_syscall(tokens[i],white);
        Write_syscall(" ",white);
    }
    Write_syscall("\n",white);
}

int Print_Current_Directory()
{
    char *PWD = malloc(BUFFER_SIZE); // Allocate memory for the buffer

    if (PWD == NULL) {
        perror("Failed to allocate memory");
        return S_EXIT_FAILURE;
    }

    if (getcwd(PWD, BUFFER_SIZE) != NULL) {
        // Print the current working directory in green without a newline
        Write_syscall(PWD,green);
        fflush(stdout); // Flush the output buffer
    } else {
        perror("getcwd() error");
    }

    free(PWD); // Free the allocated memory
}

int Copy_Command(char** tokens) {
    if (tokens == NULL) {
        Write_syscall("Error: Invalid tokens array.\n", red);
        return S_EXIT_INVALID_COMMAND; // Error
    }

    int flags = O_WRONLY|O_TRUNC;
    const char *source_path = NULL;
    const char *target_path = NULL;

    // Iterate through tokens to find options and paths
    for (int i = 1; tokens[i] != NULL; ++i) {
        if (strcmp(tokens[i], "-a") == 0) {
            flags |= O_APPEND; // Add append flag if -a is found
            flags &= ~O_TRUNC; // Remove truncate flag if -a is found
        } else if (source_path == NULL) {
            source_path = tokens[i];
        } else if (target_path == NULL) {
            target_path = tokens[i];
        }
    }

    if (source_path == NULL || target_path == NULL) {
        Write_syscall("Error: No arguments passed to the command scp\n", red);
        return S_EXIT_NO_ARGS;
    }

    if (is_file(source_path) != EXIT_SUCCESS) {
        Write_syscall("Error: Source path is not a file or does not exist.\n", red);
        return S_EXIT_FILE_NOT_FOUND; // Error
    }

    if (strcmp(source_path, target_path) == 0) {
        Write_syscall("Error: Source and Target files are the same\n", red);
        return S_EXIT_SAME_OPERANDS; // Error
    }

    if (is_directory(target_path) == EXIT_SUCCESS) {
        char tempPath[BUFFER_SIZE];
        char *source_filename = basename((char *)source_path);
        snprintf(tempPath, sizeof(tempPath), "%s/%s", target_path, source_filename);
        target_path = tempPath;
    } else if (is_file(target_path) == S_EXIT_INVALID_COMMAND) {
        Write_syscall("Error: Target path is not a file or does not exist.\n", red);
        return S_EXIT_FILE_NOT_FOUND; // Error
    }

    int source_fd = open(source_path, O_RDONLY);
    if (source_fd == -1) {
        Write_syscall("Error opening source file\n", red);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    int target_fd = open(target_path, flags,0664);
    if (target_fd == -1) {
        Write_syscall("Error opening target file\n", red);
        close(source_fd);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(source_fd, buffer, sizeof(buffer))) > 0) {
        if (write(target_fd, buffer, bytesRead) != bytesRead) {
            Write_syscall("Error writing to target file\n", red);
            close(source_fd);
            close(target_fd);
            return S_EXIT_OPEN_FILE_FAILED; // Error
        }
    }

    if (bytesRead == -1) {
        Write_syscall("Error reading source file\n", red);
    }

    close(source_fd);
    close(target_fd);

    Write_syscall("File copied successfully.\n", green);
    return EXIT_SUCCESS; // Success
}


int Move_Command(char** tokens) {
    if (tokens == NULL) {
        Write_syscall("Error: Invalid tokens array.\n", red);
        return S_EXIT_INVALID_COMMAND; // Error
    }

    int force_overwrite = 0;
    const char *source_path = NULL;
    const char *target_path = NULL;
    char *source_filename;

    // Iterate through tokens to find options and paths
    for (int i = 1; tokens[i] != NULL; ++i) {
        if (strcmp(tokens[i], "-f") == 0) {
            force_overwrite = 1;
        } else if (source_path == NULL) {
            source_path = tokens[i];
        } else if (target_path == NULL) {
            target_path = tokens[i];
        }
    }

    if (source_path == NULL || target_path == NULL) {
        Write_syscall("Error: No arguments passed to the command smv\n", red);
        return S_EXIT_NO_ARGS;
    }

    if (is_directory(target_path) == EXIT_SUCCESS) {
        source_filename = basename((char *)source_path);
        char tempPath[BUFFER_SIZE];
        snprintf(tempPath, sizeof(tempPath), "%s/%s", target_path, source_filename);
        target_path = tempPath;
    }

    if (is_file(target_path) == EXIT_SUCCESS && !force_overwrite) {
        Write_syscall("The target file already exists, use -f to force overwrite.\n", red);
        return S_EXIT_MODIFY_EXISTED_FILE; // Error
    }

    int source_fd = open(source_path, O_RDONLY);
    if (source_fd == -1) {
        Write_syscall("Error opening source file\n", red);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    int target_fd = open(target_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (target_fd == -1) {
        Write_syscall("Error opening target file\n", red);
        close(source_fd);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(source_fd, buffer, sizeof(buffer))) > 0) {
        if (write(target_fd, buffer, bytesRead) != bytesRead) {
            Write_syscall("Error writing to target file\n", red);
            close(source_fd);
            close(target_fd);
            return S_EXIT_OPEN_FILE_FAILED; // Error
        }
    }

    if (bytesRead == -1) {
        Write_syscall("Error reading source file\n", red);
    }

    close(source_fd);
    close(target_fd);

    if (remove(source_path) != 0) {
        perror("Error removing source file");
        return EXIT_FAILURE; // Error
    }

    Write_syscall("File moved successfully.\n", green);
    return EXIT_SUCCESS; // Success
}


int change_Directory_Command(char** tokens) {
    if (tokens == NULL || tokens[1] == NULL) {
        Write_syscall("Error: No directory specified.\n", red);
        return S_EXIT_INVALID_COMMAND; // Error
    }

    int retValue = chdir(tokens[1]);
    if (retValue < 0) {
        perror("Change directory failed");
        return EXIT_FAILURE; // Error
    }

    return EXIT_SUCCESS; // Success
}

void Print_Environmen_Variables(void) {
    for (int i = 0; __environ[i] != NULL; i++) {
        Write_syscall(__environ[i], blue);
        Write_syscall("\n\n", white);
    }
}


void Type_of_Command(char** tokens)
{
    if(is_internal_command(tokens[1]) == S_EXIT_SUCCESS)
    {
        Write_syscall("Internal Command\n",blue);
    }

    else if(is_external_command(tokens[1]) == S_EXIT_SUCCESS)
    {
        Write_syscall("External Command\n",green);
    }

    else
    {
        Write_syscall("Unsupported Command\n",red);
    }
}


void add_to_history(const char* command, int exit_status) {
    strncpy(history[history_index].command, command, sizeof(history[history_index].command));
    history[history_index].command[sizeof(history[history_index].command) - 1] = '\0';
    history[history_index].exit_status = exit_status;
    
    /* Update the Index to point to the next position 
     * This will make a cirular array */
    history_index = (history_index + 1) % HISTORY_SIZE; 
    if (history_count < HISTORY_SIZE) {
        history_count++;
    }
}

int Execute_External_Command(char ** tokens)
{
    pid_t retPID = fork();

    if(retPID > 0)
    {
        /* Parent */
        int status;
        wait(&status);
        add_to_history(tokens[0], WEXITSTATUS(status));
        return WEXITSTATUS(status);
    }

    else if(retPID == 0)
    {
        /* Child */
        if (execvp(tokens[0], tokens) < 0) 
            {
                Write_syscall("Invalid Command: please use shelp command to check the supported commands\n",red);
                return S_EXIT_FAILURE;
            }
    }

}

void print_history() {
    int start_index = (history_index - history_count + HISTORY_SIZE) % HISTORY_SIZE;
    for (int i = 0; i < history_count; i++) {
        int index = (start_index + i) % HISTORY_SIZE;
        printf("%sCommand: %s,          Exit Status: %d\n",blue, history[index].command, history[index].exit_status);
        printf("%s",white);
    }
}



