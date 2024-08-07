/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        cmds.h                 *****************************/
/**************************      Author:     Abdelrahman Sabry      *****************************/
/**************************      Date:       6 Aug                  *****************************/
/**************************      Version:    3                      *****************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

#ifndef CMDS_H
#define CMDS_H

#define MAX_COMMAND_LENGTH 35  // Adjust this length as needed

/**
 * @brief Prints the shell prompt with the username and hostname.
 */
void print_prompt();

/**
 * @brief Displays help information for the supported commands.
 */
void Help_Command();

/**
 * @brief Prints a user input string to stdout.
 * 
 * @param Command_tokens Array of strings containing the command and its arguments.
 * @return int Status code indicating success or failure.
 */
int Echo_Command(char** Command_tokens);

/**
 * @brief Copies a file to another file.
 * 
 * @param Command_tokens Array of strings containing the command and its arguments.
 * @return int Status code indicating success or failure.
 */
int Copy_Command(char** Command_tokens);

/**
 * @brief Moves a file to another location.
 * 
 * @param Command_tokens Array of strings containing the command and its arguments.
 * @return int Status code indicating success or failure.
 */
int Move_Command(char** Command_tokens);

/**
 * @brief Changes the current working directory.
 * 
 * @param Command_tokens Array of strings containing the command and its arguments.
 * @return int Status code indicating success or failure.
 */
int change_Directory_Command(char** Command_tokens);

/**
 * @brief Prints environment variables.
 * 
 * @param Command_tokens Array of strings containing the command and its arguments.
 */
void Print_Environmen_Variables(char **Command_tokens);

/**
 * @brief Prints the type of a command (Internal, External, or Unsupported).
 * 
 * @param Command_tokens Array of strings containing the command and its arguments.
 */
void Type_of_Command(char** Command_tokens);

/**
 * @brief Adds a command to the history file with its exit status.
 * 
 * @param command The command string.
 * @param exit_status The exit status of the command.
 */
void add_to_history(const char* command, int exit_status);

/**
 * @brief Prints the command history.
 */
void print_history();

/**
 * @brief Executes an external command.
 * 
 * @param Command_tokens Array of strings containing the command and its arguments.
 * @return int Status code indicating success or failure.
 */
int Execute_External_Command(char **Command_tokens);

/**
 * @brief Prints information about RAM usage.
 * 
 * @return int Status code indicating success or failure.
 */
int Free_Command(void);

/**
 * @brief Prints the system's uptime and idle time.
 * 
 * @return int Status code indicating success or failure.
 */
int Uptime_Command(void);




#endif
