/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        helpers.c              *****************************/
/**************************      Author:     Abdelrahman Sabry      *****************************/
/**************************      Date:       6 Aug                  *****************************/
/**************************      Version:    3                      *****************************/
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
#include <stdbool.h>

#include "helpers.h"
#include "../cmds_implementations/cmds.h"
#include "../exit_status.h"

/*****************************        Global Variables           ********************************/

extern char ** __environ;
static ProcessHistory history[HISTORY_SIZE];
static int history_index = 0;
static int history_count = 0;




/*****************************        Helper Functions           ********************************/

int is_internal_command(const char* command) 
{
    char* internal_commands[] = {"shelp", "secho", "spwd", "scp", "smv", "scd", "senvir", "stype", "sphist","sexit","sfree","suptime"};
    int commands_number = sizeof(internal_commands) / sizeof(internal_commands[0]);
    for (int i = 0; i < commands_number; i++) {
        if (strcmp(internal_commands[i], command) == 0) {
            return EXIT_SUCCESS; // Command is internal
        }
    }
    return S_EXIT_INVALID_COMMAND; // Command is not internal
}

int is_external_command(const char* command) 
{
    const char* path_env = getenv("PATH");
    if (path_env == NULL) return S_EXIT_INVALID_COMMAND; // PATH environment variable not set

    char path_copy[BUFFER_SIZE];
    strncpy(path_copy, path_env, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';

    char* token = strtok(path_copy, ":");
    while (token != NULL) {
        char full_path[BUFFER_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, command);
        if (access(full_path, X_OK) == 0) { // search for the command in each path in the $PATH env
            return EXIT_SUCCESS; // Command is external
        }
        token = strtok(NULL, ":");
    }
    return S_EXIT_INVALID_COMMAND; // Command is not external
}

void Write_syscall(int stream, const char* msg, const char* color_code) 
{
    if (msg != NULL) {
        write(stream, color_code, strlen(color_code));  //Changing text color
        write(stream, msg, strlen(msg));                // Writing the message
        write(stream, white, strlen(white));            // Return to the default text color
    }
}

void printLineSeparator() {
    Write_syscall(STDOUT, "=========================================================================================================\n", blue);
}

void WelcomeMessage() {
    printLineSeparator();
    printLineSeparator();
    Write_syscall(STDOUT, "\n===================================       A~Sabry's Shell       =========================================\n\n", green);
    printLineSeparator();
    printLineSeparator();
}

int is_file(const char *path) 
{
    if (path == NULL) {
        Write_syscall(STDERR, "Error: Passed text pointer is NULL\n",red);
        return S_EXIT_FAILURE; // Invalid path
    }
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return S_EXIT_FILE_NOT_FOUND; // File does not exist
    }
    return S_ISREG(path_stat.st_mode) ? EXIT_SUCCESS : S_EXIT_FAILURE; // Check if it is a regular file
}

int is_directory(const char *path) 
{
    if (path == NULL) {
        Write_syscall(STDERR, "Error: Passed text pointer is NULL\n",red);
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

int Print_Current_Directory()
{
    char PWD[BUFFER_SIZE];
    if (getcwd(PWD, BUFFER_SIZE) != NULL) {
        // Print the current working directory in green without a newline
        Write_syscall(STDOUT, PWD,green);
        fflush(stdout); // Flush the output buffer
    } else {
        perror("getcwd() error");
    }
}


void Parse_Commands(char* full_command, char** tokens) {
    if (full_command == NULL || tokens == NULL) {
        return;
    }
    char * tempText = strdup(full_command);

    char* token = strtok(tempText, " ");
    int index  = 0;
    while (token != NULL && index < MAX_TOKENS - 1) {
        tokens[index++] = token;
        token = strtok(NULL, " ");
    }
}

int Process_Options(char** tokens, char** source_path, char** target_path)
{
    int flags = O_WRONLY;
    flags |= strcmp(tokens[0], "scp") == 0 ? O_TRUNC : O_CREAT;

    // Iterate through tokens to find options and paths
    for (int i = 1; tokens[i] != NULL; ++i) {
        if (strcmp(tokens[i], "-a") == 0) {
            flags |= O_APPEND; // Add append flag if -a is found
            flags &= ~O_TRUNC; // Remove truncate flag if -a is found
        } 
        
        else if (strcmp(tokens[i], "-f") == 0) {
            flags |= O_TRUNC;
        }

        else if (*source_path == NULL) {
            *source_path = tokens[i];
        } else if (*target_path == NULL) {
            *target_path = tokens[i];
        }
    }

    return flags;
}



// Function to check if the string is a valid variable name
bool is_valid_variable_name(const char *name) {
    if (!name || !(*name)) return false;
    if (!isalpha(*name) && *name != '_') return false;
    while (*name) {
        if (!isalnum(*name) && *name != '_') return false;
        name++;
    }
    return true;
}

// Function to detect variable declarations (VAR_NAME=value)
int contains_variable_declaration(const char *command, char *var_name, char *var_value) 
{
    const char *var_start = strchr(command, '=');
    if (!var_start) {
        return 0; // No variable declaration found
    }

    // Find the start of the variable name
    const char *var_end = var_start;
    while (var_end > command && *(var_end - 1) != ' ' && *(var_end - 1) != '\t' && *(var_end - 1) != '\n') {
        --var_end;
    }

    // Extract the variable name
    size_t name_len = var_start - var_end;
    if (name_len >= MAX_VAR_NAME_LEN) {
        return 0; // Variable name too long
    }
    strncpy(var_name, var_end, name_len);
    var_name[name_len] = '\0';

    // Extract the variable value
    const char *value_start = var_start + 1;
    size_t value_len = strlen(value_start);
    if (value_len >= MAX_VAR_VALUE_LEN) {
        return 0; // Variable value too long
    }
    strncpy(var_value, value_start, value_len);
    var_value[value_len] = '\0';

    return 1; // Variable declaration found
}

// Function to detect variable usages ($VAR_NAME)
int contains_variable_usage(const char *token, char *var_name) {
    const char *ptr = token;
    while ((ptr = strchr(ptr, '$')) != NULL) {
        ptr++;
        if (*ptr && (isalnum(*ptr) || *ptr == '_')) {
            // Extract the variable name
            const char *start = ptr;
            while (*ptr && (isalnum(*ptr) || *ptr == '_')) {
                ptr++;
            }
            size_t var_name_length = ptr - start;
            
            // Use strncpy to copy the variable name
            strncpy(var_name, start, var_name_length);
            var_name[var_name_length] = '\0';  // Null-terminate the string
            
            return 1;
        }
    }
    return 0;
}


// Function to add or update a variable
void set_variable(const char *name, const char *value) {
    int fd = open(VAR_FILE, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        return;
    }

    // Read the entire file content
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char *file_content = malloc(file_size + 1);
    if (!file_content) {
        perror("Failed to allocate memory");
        close(fd);
        return;
    }

    ssize_t bytes_read = read(fd, file_content, file_size);
    if (bytes_read < 0) {
        perror("Failed to read file");
        free(file_content);
        close(fd);
        return;
    }
    file_content[file_size] = '\0'; // Null-terminate the content

    // Prepare to write the new content to a temp file
    int temp_fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd < 0) {
        perror("Failed to open temp file");
        free(file_content);
        close(fd);
        return;
    }

    int found = 0;
    char *line_start = file_content;
    char *line_end;

    while ((line_end = strchr(line_start, '\n')) != NULL) {
        *line_end = '\0';
        char *eq_pos = strchr(line_start, '=');
        if (eq_pos) {
            *eq_pos = '\0'; // Split into name and value
            if (strcmp(line_start, name) == 0) {
                // Update existing variable
                dprintf(temp_fd, "%s=%s\n", name, value);
                found = 1;
                line_start = line_end + 1;
                continue;
            }
        }
        dprintf(temp_fd, "%s\n", line_start);
        line_start = line_end + 1;
    }
    
    // If variable was not found, add it
    if (!found) {
        dprintf(temp_fd, "%s=%s\n", name, value);
    }

    free(file_content);
    close(fd);
    close(temp_fd);

    // Replace the original file with the updated one
    unlink(VAR_FILE);
    rename("temp.txt", VAR_FILE);
}

// Function to get the value of a variable
int get_variable(const char *name, char *variable_value) {
    int fd = open(VAR_FILE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return S_EXIT_OPEN_FILE_FAILED;
    }

    // Read the entire file content
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char *file_content = malloc(file_size + 1);
    if (!file_content) {
        perror("Failed to allocate memory");
        close(fd);
        return S_EXIT_MEM_ALLOC;
    }

    ssize_t bytes_read = read(fd, file_content, file_size);
    if (bytes_read < 0) {
        perror("Failed to read file");
        free(file_content);
        close(fd);
        return S_EXIT_READ_FILE_FAIL;
    }
    file_content[file_size] = '\0'; // Null-terminate the content

    char *line_start = file_content;
    char *line_end;

    while ((line_end = strchr(line_start, '\n')) != NULL) {
        *line_end = '\0';
        char *eq_pos = strchr(line_start, '=');
        if (eq_pos) {
            *eq_pos = '\0'; // Split into name and value
            if (strcmp(line_start, name) == 0) {
                strncpy(variable_value, eq_pos + 1, MAX_VAR_VALUE_LEN - 1);
                variable_value[MAX_VAR_VALUE_LEN - 1] = '\0'; // Ensure null-termination
                free(file_content);
                close(fd);
                return S_EXIT_SUCCESS;
            }
        }
        line_start = line_end + 1;
    }

    free(file_content);
    close(fd);
    return S_EXIT_FAILURE; // Variable not found
}