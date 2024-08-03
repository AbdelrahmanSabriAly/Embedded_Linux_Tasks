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

#include "exit_status.h"
#include "utils.h"


int main(int argc, char* argv[])
{
    /* The shell message that will be prompted every time */
    char* shell_msg = " $ Go Ahead! > ";

    /* String Carries the typed command */
    char full_command[BUFFER_SIZE]; 


    
    int Exit_Status = 0;
    int retVal = 0;

    /* Prints the welcome message */
    WelcomeMessage(); 

    while(1)
    {
        /* array of strings represents the tokens derived from the command */
        char *Command_tokens[MAX_TOKENS] = {NULL}; 

        Exit_Status = Print_Current_Directory(); // printing working directory before the "Go Ahead message" 
        Write_syscall(shell_msg,white);

        int bytes_read = read(STDIN, full_command, BUFFER_SIZE - 1); //read the command from user

        /* Null-terminate the command */
        full_command[bytes_read - 1] = '\0';  

        /* Remove leading and trailing spaces */
        trim_spaces(full_command);

        /* Remove additional spaces between words*/
        reduce_spaces(full_command);

        /* If Enter is pressed ==> Do nothing */
        // Remove trailing newline
        full_command[strcspn(full_command, "\n")] = 0;

        // Check if the command is empty after trimming spaces
        if (strlen(full_command) == 0) {
            continue;  // Skip empty commands
        }
        
        /* Convert the command into tokens */
        Parse_Commands(full_command, Command_tokens); 

        if(strcmp(Command_tokens[0], "shelp") == 0)
        {
            Help_Command();
            add_to_history("shelp", S_EXIT_SUCCESS);
        }

        else if(strcmp(Command_tokens[0], "secho") == 0)
        {
            Exit_Status = Echo_Command(Command_tokens);
            add_to_history("secho", Exit_Status);
        }

        else if(strcmp(Command_tokens[0], "spwd") == 0)
        {
            Print_Current_Directory();
            Write_syscall("\n",white);
            add_to_history("spwd", S_EXIT_SUCCESS);

        }

        else if(strcmp(Command_tokens[0], "scp") == 0)
        {
            Exit_Status = Copy_Command(Command_tokens);
            add_to_history("scp",Exit_Status);
            
        }

        else if(strcmp(Command_tokens[0], "smv") == 0)
        {
            Exit_Status = Move_Command(Command_tokens);
            add_to_history("smv",Exit_Status);
        }

        else if(strcmp(Command_tokens[0], "sexit") == 0)
        {
            Write_syscall("Good bye !\n",red);
            add_to_history("sexit",S_EXIT_SUCCESS);
            break;
        }

        else if(strcmp(Command_tokens[0], "scd") == 0)
        {
            Exit_Status = change_Directory_Command(Command_tokens);
            add_to_history("scd", Exit_Status);
        }

        else if(strcmp(Command_tokens[0], "senvir") == 0)
        {
            Print_Environmen_Variables();
            add_to_history("senvir", S_EXIT_SUCCESS);
        }

        else if(strcmp(Command_tokens[0], "stype") == 0)
        {
            Type_of_Command(Command_tokens);
            add_to_history("stype",S_EXIT_SUCCESS);
        }

        else if(strcmp(Command_tokens[0], "sphist") == 0)
        {
            print_history();
            add_to_history("phist", S_EXIT_SUCCESS);
        }


        else
        {
            Execute_External_Command(Command_tokens);
        }

        printLineSeparator();
    
    }
    return 0;
}