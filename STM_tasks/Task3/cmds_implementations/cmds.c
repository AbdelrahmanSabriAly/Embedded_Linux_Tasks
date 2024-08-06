/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        cmds.c                 *****************************/
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
#include <pwd.h>
#include <sys/utsname.h>

#include "cmds.h"
#include "../exit_status.h"
#include "../helper_functions/helpers.h"

/*****************************        Global Variables           ********************************/

extern char ** __environ;
ProcessHistory history[HISTORY_SIZE];
static int history_index = 0;
static int history_count = 0;

/**************************        Internal Commands Implementation           ****************************/

void print_prompt() 
{
    // Get the username
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        perror("Failed to get user info");
        return;
    }

    // Get the hostname
    char hostname[100];
    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("Failed to get hostname");
        return;
    }

    // Print the prompt
    Write_syscall(STDOUT, pw->pw_name,blue);
    Write_syscall(STDOUT, "@", blue);
    Write_syscall(STDOUT, hostname, blue);
    Write_syscall(STDOUT, ":",blue);

}

void Help_Command() {
    Write_syscall(STDOUT, "\nThe supported commands are:\n\n", blue);
    Write_syscall(STDOUT, "1- spwd: print working directory\n\n", blue);
    Write_syscall(STDOUT, "2- secho: print a user input string on stdout\n\n", blue);
    Write_syscall(STDOUT, "3- scp: copy a file to another file (cp {sourcePath} {targetPath})\n", blue);
    Write_syscall(STDOUT, "    options: -a to append the source content to the end of the target file\n\n", green);
    Write_syscall(STDOUT, "4- smv: move a file to another place (mv {sourcePath} {targetPath})\n", blue);
    Write_syscall(STDOUT, "    options: -f to force overwriting the target file if exists\n\n", green);
    Write_syscall(STDOUT, "5- scd: Change directory to the passed path\n\n", blue);
    Write_syscall(STDOUT, "6- senvir: print all the environment variables\n", blue);
    Write_syscall(STDOUT, "    if variable name is passed, it prints the value of this variable alone\n\n",green);
    Write_syscall(STDOUT, "7- stype: print the type of the command (Internal, External or Unsupported\n\n", blue);
    Write_syscall(STDOUT, "8- sphist: print The last 10 commands with their exit status\n\n", blue);
    Write_syscall(STDOUT, "9- shelp: print all the supported command with a brief info about each one\n\n", blue);
    Write_syscall(STDOUT, "10- sexit: terminate the shell\n\n", blue);

    Write_syscall(STDOUT, "11- sfree: prints information about RAM\n\n",blue);
    Write_syscall(STDOUT, "12- suptime: prints the system's uptime and idle time\n\n",blue);

}

int Echo_Command(char** tokens)
{
    if (tokens == NULL) {
        Write_syscall(STDERR, "Error: Text pointer is NULL\n", red);
        return S_EXIT_FAILURE;
    }

    for (int i = 1; tokens[i] != NULL; i++)
    {
        // Print token if no redirection operator is found
        Write_syscall(STDOUT, tokens[i], green);
        Write_syscall(STDOUT, " ", green);
    }

    // Print a newline character at the end of the output
    Write_syscall(STDOUT, "\n", green);
    return S_EXIT_SUCCESS;    
}


int Copy_Command(char** tokens) 
{
    if (tokens == NULL) {
        Write_syscall(STDERR, "Error: Invalid tokens array.\n", red);
        return S_EXIT_INVALID_COMMAND; // Error
    }

    int flags = O_WRONLY|O_TRUNC;
    char *source_path = NULL;
    char *target_path = NULL;
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    flags = Process_Options(tokens, &source_path, &target_path);

    if (source_path == NULL || target_path == NULL) {
        Write_syscall(STDERR, "Error: No arguments passed to the command scp\n", red);
        return S_EXIT_NO_ARGS;
    }

    if (is_file(source_path) != EXIT_SUCCESS) {
        Write_syscall(STDERR, "Error: Source path is not a file or does not exist.\n", red);
        return S_EXIT_FILE_NOT_FOUND; // Error
    }

    if (strcmp(source_path, target_path) == 0) {
        Write_syscall(STDERR, "Error: Source and Target files are the same\n", red);
        return S_EXIT_SAME_OPERANDS; // Error
    }

    if (is_directory(target_path) == EXIT_SUCCESS) {
        char tempPath[BUFFER_SIZE];
        char *source_filename = basename((char *)source_path);
        snprintf(tempPath, sizeof(tempPath), "%s/%s", target_path, source_filename);
        target_path = tempPath;
    } else if (is_file(target_path) == S_EXIT_INVALID_COMMAND) {
        Write_syscall(STDERR, "Error: Target path is not a file or does not exist.\n", red);
        return S_EXIT_FILE_NOT_FOUND; // Error
    }

    int source_fd = open(source_path, O_RDONLY);
    if (source_fd == -1) {
        Write_syscall(STDERR, "Error opening source file\n", red);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    int target_fd = open(target_path, flags,0664);
    if (target_fd == -1) {
        Write_syscall(STDERR, "Error opening target file\n", red);
        close(source_fd);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    while ((bytesRead = read(source_fd, buffer, sizeof(buffer))) > 0) {
        if (write(target_fd, buffer, bytesRead) != bytesRead) {
            Write_syscall(STDERR, "Error writing to target file\n", red);
            close(source_fd);
            close(target_fd);
            return S_EXIT_OPEN_FILE_FAILED; // Error
        }
    }

    if (bytesRead == -1) {
        Write_syscall(STDERR, "Error reading source file\n", red);
    }

    close(source_fd);
    close(target_fd);

    Write_syscall(STDOUT, "File copied successfully.\n", green);
    return EXIT_SUCCESS; // Success
}


int Move_Command(char** tokens) {
    if (tokens == NULL) {
        Write_syscall(STDERR, "Error: Invalid tokens array.\n", red);
        return S_EXIT_INVALID_COMMAND; // Error
    }

    char *source_path = NULL;
    char *target_path = NULL;
    char *source_filename;
    int flags;

    flags = Process_Options(tokens, &source_path, &target_path);

    if (source_path == NULL || target_path == NULL) {
        Write_syscall(STDERR, "Error: No arguments passed to the command smv\n", red);
        return S_EXIT_NO_ARGS;
    }

    if (is_directory(target_path) == EXIT_SUCCESS) {
        source_filename = basename((char *)source_path);
        char tempPath[BUFFER_SIZE];
        snprintf(tempPath, sizeof(tempPath), "%s/%s", target_path, source_filename);
        target_path = tempPath;
    }

    if (is_file(target_path) == EXIT_SUCCESS && !(flags & O_TRUNC)) {
        Write_syscall(STDERR, "The target file already exists, use -f to force overwrite.\n", red);
        return S_EXIT_MODIFY_EXISTED_FILE; // Error
    }

    int source_fd = open(source_path, O_RDONLY);
    if (source_fd == -1) {
        Write_syscall(STDERR, "Error opening source file\n", red);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    int target_fd = open(target_path, flags, 0644);
    if (target_fd == -1) {
        Write_syscall(STDERR, "Error opening target file\n", red);
        close(source_fd);
        return S_EXIT_OPEN_FILE_FAILED; // Error
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(source_fd, buffer, sizeof(buffer))) > 0) {
        if (write(target_fd, buffer, bytesRead) != bytesRead) {
            Write_syscall(STDERR, "Error writing to target file\n", red);
            close(source_fd);
            close(target_fd);
            return S_EXIT_OPEN_FILE_FAILED; // Error
        }
    }

    if (bytesRead == -1) {
        Write_syscall(STDERR, "Error reading source file\n", red);
    }

    close(source_fd);
    close(target_fd);

    if (remove(source_path) != 0) {
        perror("Error removing source file");
        return EXIT_FAILURE; // Error
    }

    Write_syscall(STDOUT, "File moved successfully.\n", green);
    return EXIT_SUCCESS; // Success
}


int change_Directory_Command(char** tokens) {
    if (tokens == NULL || tokens[1] == NULL) {
        Write_syscall(STDERR, "Error: No directory specified.\n", red);
        return S_EXIT_INVALID_COMMAND; // Error
    }

    int retValue = chdir(tokens[1]);
    if (retValue < 0) {
        perror("Change directory failed");
        return EXIT_FAILURE; // Error
    }

    return EXIT_SUCCESS; // Success
}

void Print_Environmen_Variables(char **tokens) 
{
    int NumOfTokens = 0;

    while(tokens[NumOfTokens] != NULL)
    {
        NumOfTokens++;
    }
    
    if(NumOfTokens == 1)
    {
        /* No Variable passed => print all variables */
        for (int i = 0; __environ[i] != NULL; i++) 
        {
            Write_syscall(STDOUT, __environ[i], blue);
            Write_syscall(STDOUT, "\n\n", white);
        }   
    }
    else if(NumOfTokens == 2)
    {
        /* Variable name passed => print that variable */
        const char *variable_name = tokens[1];
        const char *variable_value = getenv(variable_name);
        
        if(variable_value != NULL)
        {
            Write_syscall(STDOUT, variable_value, blue);
            Write_syscall(STDOUT, "\n", white);
        }
        else
        {
            Write_syscall(STDERR, "Error: Variable not found\n", red);
        }
    }
}


void Type_of_Command(char** tokens)
{
    if(is_internal_command(tokens[1]) == S_EXIT_SUCCESS)
    {
        Write_syscall(STDOUT, "Internal Command\n",blue);
    }

    else if(is_external_command(tokens[1]) == S_EXIT_SUCCESS)
    {
        Write_syscall(STDOUT, "External Command\n",green);
    }

    else
    {
        Write_syscall(STDOUT, "Unsupported Command\n",red);
    }
}


void add_to_history(const char* command, int exit_status) {
    int fd = open("history.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("Failed to open history file");
        return;
    }

    char buffer[BUFFER_SIZE];
    
    // Format command with padding
    char formatted_command[MAX_COMMAND_LENGTH + 1];
    snprintf(formatted_command, sizeof(formatted_command), "%-*s", MAX_COMMAND_LENGTH, command);

    // Write to buffer
    snprintf(buffer, sizeof(buffer), "%s %d\n", formatted_command, exit_status);
    write(fd, buffer, strlen(buffer));
    close(fd);
}

void print_history() {
    int fd = open("history.txt", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open history file");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    int count = 1;
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';

        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            printf("%d: %s\n", count++, line);
            line = strtok(NULL, "\n");
        }
    }

    if (bytesRead < 0) {
        perror("Failed to read history file");
    }

    close(fd);
}

int Execute_External_Command(char **tokens) {

    char *argv[10]; // Ensure this is large enough for your needs

    pid_t retPID = fork();
    int exit_status;

    if (retPID < 0) {
        perror("fork");
        return S_EXIT_FAILURE;
    } else if (retPID == 0) {
        // In child process
        argv[0] = "sh";
        argv[1] = "-c";

        // Allocate memory for the command string
        size_t total_length = 1; // Start with 1 for the null terminator
        for (int i = 0; tokens[i] != NULL; i++) {
            total_length += strlen(tokens[i]) + 1; // +1 for the space
        }

        char *cmd = malloc(total_length);
        if (cmd == NULL) {
            perror("malloc failed");
            return S_EXIT_FAILURE;
        }

        // Initialize the cmd string
        cmd[0] = '\0';

        // Concatenate tokens
        for (int i = 0; tokens[i] != NULL; i++) {
            strcat(cmd, tokens[i]);
            if (tokens[i + 1] != NULL) {
                strcat(cmd, " "); // Add a space between tokens
            }
        }

        argv[2] = cmd;
        argv[3] = NULL; // Null-terminate the argv array

        if (execvp("sh", argv) < 0) {
            perror("execvp");
            free(cmd); // Free the allocated memory
            return S_EXIT_FAILURE;
        }
    } else {
        // In parent process
        int status;
        waitpid(retPID, &status, 0);

        if (WIFEXITED(status)) {
            exit_status = WEXITSTATUS(status);
        } else {
            exit_status = S_EXIT_FAILURE;
        }

        // Assuming add_to_history is defined elsewhere
        add_to_history(tokens[0], exit_status);
    }

    return exit_status;
}

int Free_Command(void)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    int count = 1;

    int fd = open("/proc/meminfo", O_RDONLY);
    if(fd<0)
    {
        perror("Failed opening meminfo");
    }


    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';

        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            Write_syscall(STDOUT, line,blue);
            
            Write_syscall(STDOUT, "\n",white);
            line = strtok(NULL, "\n");
        }
    }

    if (bytesRead < 0) {
        perror("Failed to read history file");
    }

    close(fd);


}

int Uptime_Command(void)
{
    int fd = open("/proc/uptime", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /proc/uptime");
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read /proc/uptime");
        close(fd);
    }

    buffer[bytes_read] = '\0'; // Null-terminate the buffer

    close(fd);

    double uptime, idle_time;
    if (sscanf(buffer, "%lf %lf", &uptime, &idle_time) != 2) {
        fprintf(stderr, "Failed to parse /proc/uptime data\n");
    }

    printf("System Uptime: %.2f seconds\n", uptime);
    printf("Idle Time: %.2f seconds\n", idle_time);
}