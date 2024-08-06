/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        utils.h                *****************************/
/**************************      Author:     Abdelrahman Sabry      *****************************/
/**************************      Date:       6 Aug                  *****************************/
/**************************      Version:    3                      *****************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/


#ifndef UTILS_H
#define UTILS_H

#define MAX_PIPES           10
#define MAX_COMMANDS        100

#define ARROW_UP            0x41
#define ARROW_DOWN          0x42
#define ARROW_LEFT          0x44
#define ARROW_RIGHT         0x43
#define TAB                 0x09
#define ESC                 0x1b

#define NUM_OF_STREAMS      3
#define MAX_FILE_NAME_SIZE  100

/**
 * @brief Executes a command based on the provided tokens.
 * 
 * This function checks the first token to determine which command to execute.
 * It handles built-in commands and external commands, and updates the command history.
 * 
 * @param tokens Array of strings containing the command and its arguments.
 * @param full_command The full command string as input by the user.
 * @return int Status code indicating success or failure of the command execution.
 */
int Execute_Command(char **tokens, char *full_command);

/**
 * @brief Executes a single command, handling redirections and command execution.
 * 
 * This function forks a new process to execute the command. It handles redirections 
 * if specified and exits the process with the appropriate status code.
 * 
 * @param full_command The full command string as input by the user.
 * @param Command_tokens Array of strings containing the command and its arguments.
 * @param redirections Bitmask indicating the types of redirections to apply.
 * @param Target_files Array of strings containing filenames for redirection.
 */
void Execute_Single_Command(char *full_command, char **Command_tokens, int redirections, char Target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE]);

/**
 * @brief Executes a sequence of commands connected by pipes.
 * 
 * This function sets up pipes between commands and forks processes to execute them.
 * It handles redirections for the final command in the sequence if specified.
 * 
 * @param commands Array of strings containing commands separated by pipes.
 * @param num_pipes Number of pipes connecting the commands.
 * @param redirections Bitmask indicating the types of redirections to apply.
 * @param Target_files Array of strings containing filenames for redirection.
 */
void Execute_Piped_Commands(char **commands, int num_pipes, int redirections, char Target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE]);

/**
 * @brief Parses a command string into individual commands separated by pipes.
 * 
 * This function splits the command string into separate commands based on the pipe character.
 * 
 * @param command The command string containing multiple commands separated by pipes.
 * @param commands Array of strings to store the parsed commands.
 * @return int Number of commands parsed.
 */
int Parse_Pipes(char *command, char **commands);

/**
 * @brief Searches for and handles redirection operators in the command string.
 * 
 * This function identifies input, output, and error redirection operators in the command string
 * and updates the command string and target files accordingly.
 * 
 * @param command The command string to be checked for redirection operators.
 * @param target_files Array of strings to store filenames for redirection.
 * @return int Bitmask indicating the types of redirections found.
 */
int SearchForRedirections(char* command, char target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE]);

/**
 * @brief Redirects standard input, output, and error streams as specified.
 * 
 * This function applies redirection based on the specified redirections and target files.
 * 
 * @param redirections Bitmask indicating the types of redirections to apply.
 * @param target_files Array of strings containing filenames for redirection.
 * @return int Status code indicating success or failure of the redirection.
 */
int Redirect(int redirections, char target_files[NUM_OF_STREAMS][MAX_FILE_NAME_SIZE]);


#endif