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


void trim_spaces(char *str) {
    int start = 0;                  /* Index to mark the start of the string after trimming leading spaces. */
    int end = strlen(str) - 1;      /* Index to mark the end of the string after trimming trailing spaces. */
    int i, j;                       /* Loop indices for character shifting. */

    /* Trim leading spaces. */
    while (isspace((unsigned char)str[start])) {
        start++;                    /* Move the start index forward while leading spaces are found. */
    }

    /* Trim trailing spaces. */
    while (end >= start && isspace((unsigned char)str[end])) {
        end--;                      /* Move the end index backward while trailing spaces are found. */
    }

    /* Shift characters to the beginning of the string to remove trimmed spaces. */
    for (i = start, j = 0; i <= end; i++) {
        str[j++] = str[i];          /* Copy characters from the trimmed portion to the start of the string. */
    }
    str[j] = '\0';                  /* Null-terminate the string after shifting characters. */
}

void reduce_spaces(char *str) {
    int readIndex = 0, writeIndex = 0;  /* Initialize read and write indices. */
    int inSpace = 0;                    /* Flag to track if the current character is a space. */

    /* Iterate through the string until the null terminator is encountered. */
    while (str[readIndex] != '\0') {
        if (isspace((unsigned char)str[readIndex])) {  /* Check if the current character is a space. */
            if (!inSpace) {                            /* If not already in a space sequence, */
                str[writeIndex++] = ' ';               /* write a single space to the write index. */
                inSpace = 1;                           /* Set the flag indicating we are in a space sequence. */
            }
        } else {
            str[writeIndex++] = str[readIndex];        /* Write non-space characters to the write index. */
            inSpace = 0;                               /* Reset the flag indicating we are not in a space sequence. */
        }
        readIndex++;                                   /* Move to the next character in the read index. */
    }

    /* Null-terminate the string. */
    str[writeIndex] = '\0';

    /* Trim trailing space if any. */
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


void Parse_Commands(char* full_command, char** Command_tokens) {
    if (full_command == NULL || Command_tokens == NULL) {
        return;  /* Return if input is invalid. */
    }
    char *tempText = strdup(full_command);  /* Create a copy of the command string to avoid modifying the original. */

    char* token = strtok(tempText, " ");  /* Tokenize the copied string using space as a delimiter. */
    int index = 0;
    while (token != NULL && index < MAX_TOKENS - 1) {
        Command_tokens[index++] = token;  /* Store each token in the Command_tokens array. */
        token = strtok(NULL, " ");        /* Get the next token. */
    }
    
    /* Ensure the last element of Command_tokens is NULL to mark the end of tokens. */
    Command_tokens[index] = NULL;

}

int Process_Options(char** Command_tokens, char** source_path, char** target_path)
{
    int flags = O_WRONLY;  /* Initialize flags with write-only access. */
    flags |= strcmp(Command_tokens[0], "scp") == 0 ? O_TRUNC : O_CREAT;  /* Set flags based on command type. */

    /* Iterate through Command_tokens to find options and paths. */
    for (int i = 1; Command_tokens[i] != NULL; ++i) {
        if (strcmp(Command_tokens[i], "-a") == 0) {
            flags |= O_APPEND;        /* Add append flag if '-a' option is found. */
            flags &= ~O_TRUNC;        /* Remove truncate flag if '-a' option is found. */
        } 
        
        else if (strcmp(Command_tokens[i], "-f") == 0) {
            flags |= O_TRUNC;         /* Add truncate flag if '-f' option is found. */
        }

        else if (*source_path == NULL) {
            *source_path = Command_tokens[i];  /* Set the source path if not already set. */
        } else if (*target_path == NULL) {
            *target_path = Command_tokens[i];  /* Set the target path if not already set. */
        }
    }

    return flags;  /* Return the accumulated flags for file access modes. */
}



// Function to detect variable declarations (VAR_NAME=value)
int contains_variable_declaration(const char *command, char *var_name, char *var_value) 
{
    /* Find the position of the '=' character which separates variable name and value */
    const char *var_start = strchr(command, '=');
    if (!var_start) {
        return S_EXIT_FAILURE; /* No '=' found, so no variable declaration */
    }

    /* Find the start of the variable name by moving backwards from '=' */
    const char *var_end = var_start;
    while (var_end > command && *(var_end - 1) != ' ' && *(var_end - 1) != '\t' && *(var_end - 1) != '\n') {
        --var_end;
    }

    /* Extract the variable name */
    size_t name_len = var_start - var_end;
    if (name_len >= MAX_VAR_SIZE) {
        return S_EXIT_FAILURE; /* Variable name exceeds maximum size */
    }
    strncpy(var_name, var_end, name_len);
    var_name[name_len] = '\0'; /* Null-terminate the variable name */

    /* Extract the variable value */
    const char *value_start = var_start + 1; /* Move past '=' */
    size_t value_len = strlen(value_start);
    if (value_len >= MAX_VAR_SIZE) {
        return S_EXIT_FAILURE; /* Variable value exceeds maximum size */
    }
    strncpy(var_value, value_start, value_len);
    var_value[value_len] = '\0'; /* Null-terminate the variable value */

    return S_EXIT_SUCCESS; /* Successfully extracted variable name and value */
}


// Function to add or update a variable
void set_variable(const char *name, const char *value) {
    int fd = open(VAR_FILE, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        return;
    }

    /* Read the entire file content */
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
    file_content[file_size] = '\0'; /* Null-terminate the file content */

    /* Prepare to write updated content to a temporary file */
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

    while (line_start && *line_start != '\0') {
        line_end = strchr(line_start, '\n');
        if (line_end) {
            *line_end = '\0';
        }

        char *eq_pos = strchr(line_start, '=');
        if (eq_pos) {
            *eq_pos = '\0'; /* Split the line into name and value */
            if (strcmp(line_start, name) == 0) {
                /* Update the existing variable with the new value */
                dprintf(temp_fd, "%s=%s\n", name, value);
                found = 1;
            } else {
                dprintf(temp_fd, "%s=%s\n", line_start, eq_pos + 1);
            }
        }

        if (line_end) {
            line_start = line_end + 1;
        } else {
            break;
        }
    }

    /* If the variable was not found, add it to the end of the file */
    if (!found) {
        dprintf(temp_fd, "%s=%s\n", name, value);
    }

    free(file_content);
    close(fd);
    close(temp_fd);

    /* Replace the original file with the updated file */
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

    /* Read the entire file content */
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
    file_content[file_size] = '\0'; /* Null-terminate the file content */

    char *line_start = file_content;
    char *line_end;

    while ((line_end = strchr(line_start, '\n')) != NULL) {
        *line_end = '\0'; /* Null-terminate the current line */
        char *eq_pos = strchr(line_start, '=');
        if (eq_pos) {
            *eq_pos = '\0'; /* Split the line into name and value */
            if (strcmp(line_start, name) == 0) {
                strncpy(variable_value, eq_pos + 1, MAX_VAR_SIZE - 1);
                variable_value[MAX_VAR_SIZE - 1] = '\0'; /* Null-terminate the variable value */
                free(file_content);
                close(fd);
                return S_EXIT_SUCCESS; /* Variable found */
            }
        }
        line_start = line_end + 1;
    }

    free(file_content);
    close(fd);
    return S_EXIT_FAILURE; /* Variable not found */
}


// Function to substitute variables in a command string
int substitute_variables(char *command) {
    char *result = malloc(strlen(command) + 1);
    if (!result) {
        perror("Failed to allocate memory");
        return S_EXIT_MEM_ALLOC;
    }
    strcpy(result, command);

    char *var_start = NULL;
    while ((var_start = strchr(result, '$')) != NULL) {
        char *var_end = var_start + 1;
        while (*var_end && (isalnum(*var_end) || *var_end == '_')) {
            var_end++;
        }

        size_t var_name_len = var_end - (var_start + 1);
        char var_name[var_name_len + 1];
        strncpy(var_name, var_start + 1, var_name_len);
        var_name[var_name_len] = '\0'; /* Null-terminate the variable name */

        char var_value[MAX_VAR_SIZE];
        if (get_variable(var_name, var_value) != S_EXIT_SUCCESS) {
            const char *tempChar = getenv(var_name);
            if(tempChar != NULL)
            {
                strcpy(var_value, tempChar);
            }
            else
            {
                var_value[0] = '\0'; /* Use empty string if variable not found */
            }
        }

        size_t result_len = strlen(result);
        size_t var_value_len = strlen(var_value);
        size_t new_result_len = result_len - var_name_len - 1 + var_value_len;

        char *new_result = malloc(new_result_len + 1);
        if (!new_result) {
            perror("Failed to allocate memory");
            free(result);
            return S_EXIT_MEM_ALLOC;
        }

        size_t prefix_len = var_start - result;
        strncpy(new_result, result, prefix_len);
        strncpy(new_result + prefix_len, var_value, var_value_len);
        strcpy(new_result + prefix_len + var_value_len, var_start + var_name_len + 1);

        free(result);
        result = new_result;
    }

    strcpy(command, result);
    free(result);

    return S_EXIT_SUCCESS; /* Successfully substituted variables */
}