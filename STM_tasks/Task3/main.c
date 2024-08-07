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

#include "exit_status.h"
#include "utilities/utils.h"
#include "helper_functions/helpers.h"
#include "cmds_implementations/cmds.h"


int main()
{
    int continue_flag;
    char variable_name[MAX_VAR_NAME_LEN]; 
    char variable_value[MAX_VAR_VALUE_LEN]; 
    char *commands[MAX_PIPES + 1];

    char* shell_msg = " $ Go Ahead! > ";
    char full_command[BUFFER_SIZE];     
    int Exit_Status = 0;
    char Target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE];
    

    WelcomeMessage(); 

    while(1)
    {
        continue_flag = 0;
        char *Command_tokens[MAX_TOKENS] = {NULL}; 
        print_prompt();
        Exit_Status = Print_Current_Directory(); 
        Write_syscall(STDOUT, shell_msg, white);

        int bytes_read = read(STDIN, full_command, BUFFER_SIZE - 1);
        full_command[bytes_read] = '\0';  
        
        trim_spaces(full_command);
        reduce_spaces(full_command);
        full_command[strcspn(full_command, "\n")] = 0;

        if (strlen(full_command) == 0) {
            continue;  
        }

        

        for(int i = 0 ; Command_tokens[i]!=NULL; i++)
        {
            if(contains_variable_usage(Command_tokens[i],variable_name))
            {
                int Exit_Status = get_variable(variable_name, variable_value);
                if(Exit_Status == S_EXIT_SUCCESS)
                {
                    Command_tokens[i] = variable_value;
                }

                else
                {
                    Write_syscall(STDERR,"Error: Variable not found\n", red);
                    continue_flag = 1;
                    break;
                }
            }

            else if(contains_variable_declaration(full_command, variable_name, variable_value))
            {
                set_variable(variable_name, variable_value);
                continue_flag = 1;
                break;
            }
        
        }

        if(continue_flag == 1)
        {
            continue;
        }

        int redirections = SearchForRedirections(full_command, Target_files);

        Parse_Commands(full_command, Command_tokens); 
        
        int num_pipes = Parse_Pipes(full_command, commands);

        if (num_pipes == 0) {
            Execute_Single_Command(full_command, Command_tokens, redirections, Target_files);
        } else {
            Execute_Piped_Commands(commands, num_pipes, redirections, Target_files);
        }
    }

    return 0;
}
