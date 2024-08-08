/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        main.c                 *****************************/
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "exit_status.h"
#include "utilities/utils.h"
#include "helper_functions/helpers.h"
#include "cmds_implementations/cmds.h"


int main()
{
    char variable_name[MAX_VAR_SIZE]; 
    char variable_value[MAX_VAR_SIZE]; 
    char *commands[MAX_PIPES + 1];

    char* shell_msg = " $ Go Ahead! > ";
    char* full_command;     
    int Exit_Status = 0;
    char Target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE];
    

    WelcomeMessage(); 

    while(1)
    {
        char *Command_tokens[MAX_TOKENS] = {NULL}; 

        /* Print username and host name */
        print_prompt();

        /* Print the current working directory */
        Print_Current_Directory(); 

        /* Read the command from user */
        full_command = readline(shell_msg);
        add_history(full_command);
        
        /* Remove leading and trailing spaces from the command */
        trim_spaces(full_command);

        /* Reduce number of spaces between words to 1 space */
        reduce_spaces(full_command);

        /* If enter is pressed => Do nothing */
        if (strlen(full_command) == 0) {
            continue;  
        }

        /* If the command was variable declaration (contains =) */
        if(contains_variable_declaration(full_command,variable_name,variable_value) == S_EXIT_SUCCESS)
        {
            /* Add the variable to the variables file */
            set_variable(variable_name,variable_value);
        }

        /* If the command contains variable usage (contains $) => substitute by the its value */
        substitute_variables(full_command);

        /* Check if redirection is used */
        int redirections = SearchForRedirections(full_command, Target_files);

        /* Convert command into tokens */
        Parse_Commands(full_command, Command_tokens); 
        
        /* Check if piping is used (contain |) */
        int num_pipes = Parse_Pipes(full_command, commands);

        if (num_pipes == 0) {
            Execute_Single_Command(full_command, Command_tokens, redirections, Target_files);
        } else {
            Exit_Status = Execute_Piped_Commands(commands, num_pipes, redirections, Target_files);
        }
    }

    return 0;
}
