/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        utils.h                *****************************/
/**************************      Author:     Abdelrahman Sabry      *****************************/
/**************************      Date:       29 Jul                 *****************************/
/**************************      Version:    2                      *****************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

#ifndef UTILS_H
#define UTILS_H

/* Important File Dexription */
#define STDIN       0
#define STDOUT      1
#define STDERR      2

/* Text Colors */
#define green       "\033[1;32m"
#define red         "\033[1;31m"
#define blue        "\033[1;34m"
#define white       "\033[0m"

/* Size of array which will carry the most recently used commands*/
#define HISTORY_SIZE 10


/* Size of strings */
#define BUFFER_SIZE 1024

/* Size of the tokens array*/
#define MAX_TOKENS  10

/* Structure used to store a command's name and exit status. 
 * An array of this structure represents the history of the most recently used commands. */
typedef struct {
    char command[256];
    int exit_status;
} ProcessHistory;


/**
 * @brief Writes a colored string message to standard output.
 * 
 * This function wraps the `write` system call to write a message to the standard
 * output (stdout) with a specified color. It handles `NULL` pointers gracefully by not performing any
 * operation if the provided message is `NULL`.
 * 
 * @param msg The message to be written to stdout. If this is `NULL`, no action is performed.
 * @param color_code The color code of the message to be written. Options: red, green, blue, white.
 * 
 * Example usage:
 * @code
 * Write_syscall("Hello, world!\n", green);
 * @endcode
 */
void Write_syscall(const char* msg, const char* color_code);


/**
 * @brief Trims leading and trailing spaces from the given string.
 * 
 * @param str The string to be trimmed. The string is modified in place.
 */
void trim_spaces(char *str);

/**
 * @brief Reduces multiple spaces within the string to a single space.
 * 
 * @param str The string where spaces will be reduced. The string is modified in place.
 */
void reduce_spaces(char *str);



/**
 * @brief Prints a line separator to the console.
 *
 * This function outputs a line separator, typically used for formatting
 * console output.
 */
void printLineSeparator();

/**
 * @brief Displays a welcome message to the user.
 *
 * This function prints a welcome message to the console, usually used
 * to greet the user when the application starts.
 */
void WelcomeMessage();

/**
 * @brief Parses a command into tokens.
 *
 * Splits the provided command string into tokens based on whitespace.
 *
 * @param full_command The command string to be parsed.
 * @param tokens Array of character pointers where the parsed tokens will be stored.
 */
void Parse_Commands(char* full_command, char** tokens);

/**
 * @brief Displays help information for commands.
 *
 * This function prints a list of available commands and their descriptions
 * to the console.
 */
void Help_Command();

/**
 * @brief Echoes the provided tokens to the console.
 *
 * This function prints the tokens received as input to the console.
 *
 * @param tokens Array of character pointers containing the tokens to be echoed.
 */
int Echo_Command(char** tokens);

/**
 * @brief Prints the current working directory.
 *
 * This function retrieves the current working directory and prints it
 * to the standard output. It uses the `getcwd` function to get the
 * current working directory and then writes it using the `write` function.
 * A newline character is also printed to the standard output.
 *
 * @return 0 on success, any other number on failure
 */
int Print_Current_Directory(void);

/**
 * @brief Copies a file from source to destination.
 *
 * This function copies a file from the source path specified in the tokens
 * to the destination path. The buffer size defines how much data can be
 * processed at once.
 *
 * @param tokens Array of character pointers containing the source and destination paths.
 * @return 0 on success, or any other value on failure.
 */
int Copy_Command(char** tokens);

/**
 * @brief Moves a file from source to destination.
 *
 * This function moves a file from the source path specified in the tokens
 * to the destination path. The buffer size defines how much data can be
 * processed at once.
 *
 * @param tokens Array of character pointers containing the source and destination paths.
 * @param BUFFER_SIZE The size of the buffer used for moving the file.
 * @return 0 on success, or any other value on failure.
 */
int Move_Command(char** tokens);

/**
 * @brief Changes the current directory.
 *
 * This function changes the current working directory to the path specified in the tokens.
 *
 * @param tokens Array of character pointers containing the directory path.
 * @return 0 on success, or any other value on failure.
 */
int change_Directory_Command(char** tokens);

/**
 * @brief Prints the environment variables.
 *
 * This function prints all environment variables to the console.
 */
void Print_Environmen_Variables(void);

/**
 * @brief Determines and prints the type of a command.
 *
 * This function determines whether a command is internal, external, or unsupported, and prints the result.
 *
 * @param tokens Array of character pointers containing the command to be checked.
 */
void Type_of_Command(char** tokens);

/**
 * @brief Adds a command to the process history.
 *
 * This function adds the specified command and its exit status to the process history.
 *
 * @param command The command to be added to the history.
 * @param exit_status The exit status of the command.
 */
void add_to_history(const char* command, int exit_status);

/**
 * @brief Prints the process history.
 *
 * This function prints the last HISTORY_SIZE commands and their exit statuses.
 */
void print_history(void);

/**
 * @brief Executes an external command.
 *
 * This function executes an external command specified in the tokens.
 *
 * @param tokens Array of character pointers containing the command and its arguments.
 * @return The exit status of the executed command.
 */
int Execute_External_Command(char ** tokens);

#endif
