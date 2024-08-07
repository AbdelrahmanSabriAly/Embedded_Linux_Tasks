/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        utils.c                *****************************/
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

#include "utils.h"
#include "../cmds_implementations/cmds.h"
#include "../helper_functions/helpers.h"
#include "../exit_status.h"

char **Command_History; 
int num_commands = 0;
int History_index;


int Execute_Command(char **Command_tokens, char *full_command)
{
    int Exit_Status;

    if(strcmp(Command_tokens[0], "shelp") == 0)
    {
        Help_Command();
        add_to_history(full_command, S_EXIT_SUCCESS);
    }

    else if(strcmp(Command_tokens[0], "secho") == 0)
    {
        Exit_Status = Echo_Command(Command_tokens);
        add_to_history(full_command, Exit_Status);
    }

    else if(strcmp(Command_tokens[0], "spwd") == 0)
    {
        Print_Current_Directory();
        Write_syscall(STDOUT, "\n", white);
        add_to_history(full_command, S_EXIT_SUCCESS);
    }

    else if(strcmp(Command_tokens[0], "scp") == 0)
    {
        Exit_Status = Copy_Command(Command_tokens);
        add_to_history(full_command, Exit_Status);
    }

    else if(strcmp(Command_tokens[0], "smv") == 0)
    {
        Exit_Status = Move_Command(Command_tokens);
        add_to_history(full_command, Exit_Status);
    }

    else if(strcmp(Command_tokens[0], "senvir") == 0)
    {
        Print_Environmen_Variables(Command_tokens);
        add_to_history(full_command, S_EXIT_SUCCESS);
    }

    else if(strcmp(Command_tokens[0], "stype") == 0)
    {
        Type_of_Command(Command_tokens);
        add_to_history(full_command, S_EXIT_SUCCESS);
    }

    else if(strcmp(Command_tokens[0], "sphist") == 0)
    {
        print_history();
        add_to_history(full_command, S_EXIT_SUCCESS);
    }

    else if(strcmp(Command_tokens[0], "sfree") == 0)
    {
        Exit_Status = Free_Command();
        add_to_history(full_command, Exit_Status);
    }

    else if(strcmp(full_command, "suptime") == 0)
    {
        Exit_Status = Uptime_Command();
        add_to_history(full_command, Exit_Status);
    }


    else
    {
        Execute_External_Command(Command_tokens);
    }

    return Exit_Status;
}

void Execute_Single_Command(char *full_command, char **Command_tokens, int redirections, char Target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE]) 
{
    int Exit_Status;
    
    /* If the command was scd => execute it here before forking */
    if(strcmp(Command_tokens[0], "scd") == 0)
    {
        Exit_Status = change_Directory_Command(Command_tokens);
        add_to_history(full_command, Exit_Status);
        return;
    }

    /* If the command was sexit => terminate the program before forking */
    else if(strcmp(Command_tokens[0], "sexit") == 0)
    {
        Write_syscall(STDOUT, "Good bye !\n", red);
        add_to_history(full_command, S_EXIT_SUCCESS);
        printLineSeparator();
        exit(S_EXIT_SUCCESS);
    }
    
    pid_t retPID = fork();
    if(retPID > 0) 
    {
        /* Parent */
        int status;

        /* Wait fot the child process to end */
        wait(&status);
        printLineSeparator();
    } 
    
    else if(retPID == 0) 
    {
        /* Child */

        /* Redirect streams if redirection is used */
        if (redirections > 0) {
            if (Redirect(redirections, Target_files) < 0) {
                perror("Redirection failed");
                exit(S_EXIT_FAILURE);
            }
        }


        Exit_Status = Execute_Command(Command_tokens, full_command);
        exit(Exit_Status);
    } 
    
    else 
    {
        perror("fork");
        exit(S_EXIT_FAILURE);
    }
}

int Execute_Piped_Commands(char **commands, int num_pipes, int redirections, char Target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE]) 
{
    /* Array to hold pipe file descriptors. */
    int pipefds[2 * num_pipes];

    /* Create the necessary pipes */
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe"); /* Error creating pipe. */
            return S_EXIT_FAILURE;
        }
    }

    int j = 0; /* Index for accessing pipe file descriptors. */
    
    /* Iterate through each command, forking a new process for each. */
    for (int i = 0; i <= num_pipes; i++) {
        pid_t pid = fork(); /* Fork a new process. */
        
        if (pid == 0) {
            /* In the child process. */

            /* If not the last command, redirect stdout to the next pipe. */
            if (i < num_pipes) {
                if (dup2(pipefds[j + 1], STDOUT) < 0) {
                    perror("dup2"); /* Error duplicating file descriptor. */
                    return S_EXIT_FAILURE;
                }
            }

            /* If not the first command, redirect stdin to the previous pipe. */
            if (i > 0) {
                if (dup2(pipefds[j - 2], STDIN) < 0) {
                    perror("dup2"); /* Error duplicating file descriptor. */
                    return S_EXIT_FAILURE;
                }
            }

            /* Close all pipe file descriptors in the child process. */
            for (int k = 0; k < 2 * num_pipes; k++) {
                close(pipefds[k]);
            }

            /* Tokenize the current command. */
            char *Command_tokens[MAX_TOKENS] = {NULL};
            Parse_Commands(commands[i], Command_tokens);

            /* Handle redirections if specified for the last command. */
            if (redirections > 0 && i == num_pipes) {
                if (Redirect(redirections, Target_files) < 0) {
                    perror("Redirection failed"); /* Error during redirection. */
                    return S_EXIT_FAILURE;
                }
            }

            /* Execute the current command. */
            int Exit_Status = Execute_Command(Command_tokens, commands[i]);
            exit(Exit_Status); /* Exit the child process with the command's exit status. */
        } 
        
        else if (pid < 0) {
            perror("fork"); /* Error forking process. */
            return S_EXIT_FAILURE;
        }


        /* Move to the next set of pipe file descriptors. */
        j += 2; 
    }

    /* Close all pipe file descriptors in the parent process. */
    for (int i = 0; i < 2 * num_pipes; i++) {
        close(pipefds[i]);
    }

    /* Wait for all child processes to complete. */
    for (int i = 0; i <= num_pipes; i++) {
        wait(NULL);
    }

    return S_EXIT_SUCCESS; /* Return success status. */
}


int Parse_Pipes(char *command, char **commands) 
{
    /* Initialize the count of commands. */
    int count = 0;               

    /* Tokenize the command string using '|' as a delimiter. */       
    char *token = strtok(command, "|"); 

    /* Iterate through all tokens, storing each command in the commands array. */
    while (token != NULL && count < MAX_PIPES) {

        /* Store the current token (command) in the commands array. */
        commands[count++] = token; 

        /* Get the next token (command). */     
        token = strtok(NULL, "|");      
    }

    /* Null-terminate the commands array. */
    commands[count] = NULL;    

    /* Return the number of pipes (number of commands - 1). */   
    return count - 1;                   
}



int SearchForRedirections(char* command, char target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE]) {
    const char *input_redirect  =   "<";
    const char *output_redirect =   ">";
    const char *error_redirect  =   "2>";

    int redirections = 0;                   /* Bitmask to store redirection types. */
    char *pos;
    char *command_copy = strdup(command);   /* Create a copy of the command to avoid modifying the original. */
    char *temp_command = command_copy;      /* Temporary pointer for modifying the copy. */
    
    /* Initialize target_files to empty strings. */
    for (int i = 0; i < NUM_OF_STREAMS; i++) {
        target_files[i][0] = '\0';
    }

    /* Search for error redirection first to avoid confusion with output redirection. */
    if ((pos = strstr(temp_command, error_redirect)) != NULL) {
        redirections |= STDERR_REDIRECT;   /* Set the error redirection flag. */
        /* Extract the file name for error redirection. */
        pos += strlen(error_redirect);
        sscanf(pos, "%99s", target_files[2]);
        trim_spaces(target_files[2]);
        /* Remove the error redirection from the temporary command. */
        pos -= strlen(error_redirect);
        *pos = '\0';
    }

    /* Search for output redirection. */
    if ((pos = strstr(temp_command, output_redirect)) != NULL) {
        redirections |= STDOUT_REDIRECT;   /* Set the output redirection flag. */
        /* Extract the file name for output redirection. */
        pos += strlen(output_redirect);
        sscanf(pos, "%99s", target_files[1]);
        trim_spaces(target_files[1]);
        /* Remove the output redirection from the temporary command. */
        pos -= strlen(output_redirect);
        *pos = '\0';
    }

    /* Search for input redirection. */
    if ((pos = strstr(temp_command, input_redirect)) != NULL) {
        redirections |= STDIN_REDIRECT;    /* Set the input redirection flag. */
        /* Extract the file name for input redirection. */
        pos += strlen(input_redirect);
        sscanf(pos, "%99s", target_files[0]);
        trim_spaces(target_files[0]);
        /* Remove the input redirection from the temporary command. */
        pos -= strlen(input_redirect);
        *pos = '\0';
    }

    /* Copy the modified command back to the original command string. */
    strncpy(command, temp_command, strlen(temp_command));
    command[strlen(temp_command)] = '\0';  /* Ensure null termination. */

    /* Remove any trailing spaces from the command after redirection removal. */
    trim_spaces(command);

    free(command_copy);  /* Free the copied command. */
    return redirections; /* Return the bitmask of redirections. */
}

int Redirect(int redirections, char target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE])
{
    int oldFD;
    int ret;

    /* Handle input redirection if specified. */
    if (redirections & STDIN_REDIRECT) {
        oldFD = open(target_files[STDIN], O_RDONLY);
        if (oldFD < 0) {
            perror("open"); /* Error opening file for input redirection. */
            return S_EXIT_OPEN_FILE_FAILED;
        }
        ret = dup2(oldFD, STDIN);
        if (ret < 0) {
            perror("dup2"); /* Error duplicating file descriptor for input redirection. */
            close(oldFD);
            return S_EXIT_DUP_FILE_FAILED;
        }
        close(oldFD);
    }

    /* Handle output redirection if specified. */
    if (redirections & STDOUT_REDIRECT) {
        oldFD = open(target_files[STDOUT], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (oldFD < 0) {
            perror("open"); /* Error opening file for output redirection. */
            return S_EXIT_OPEN_FILE_FAILED;
        }
        ret = dup2(oldFD, STDOUT);
        if (ret < 0) {
            perror("dup2"); /* Error duplicating file descriptor for output redirection. */
            close(oldFD);
            return S_EXIT_DUP_FILE_FAILED;
        }
        close(oldFD);
    }

    /* Handle error redirection if specified. */
    if (redirections & STDERR_REDIRECT) {
        oldFD = open(target_files[STDERR], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (oldFD < 0) {
            perror("open"); /* Error opening file for error redirection. */
            return S_EXIT_OPEN_FILE_FAILED;
        }
        ret = dup2(oldFD, STDERR);
        if (ret < 0) {
            perror("dup2"); /* Error duplicating file descriptor for error redirection. */
            close(oldFD);
            return S_EXIT_DUP_FILE_FAILED;
        }
        close(oldFD);
    }

    return S_EXIT_SUCCESS; /* Return success status. */
}
