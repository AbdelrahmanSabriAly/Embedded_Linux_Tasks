/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        helpers.h              *****************************/
/**************************      Author:     Abdelrahman Sabry      *****************************/
/**************************      Date:       6 Aug                  *****************************/
/**************************      Version:    3                      *****************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

#ifndef HELPERS_H
#define HELPERS_H

/* Important File Dexription */
#define STDIN               0
#define STDOUT              1
#define STDERR              2

#define STDIN_REDIRECT      0b001
#define STDOUT_REDIRECT     0b010
#define STDERR_REDIRECT     0b100

/* Text Colors */
#define green               "\033[1;32m"
#define red                 "\033[1;31m"
#define blue                "\033[1;34m"
#define white               "\033[0m"

/* Size of array which will carry the most recently used commands*/
#define HISTORY_SIZE        10


/* Size of strings */
#define BUFFER_SIZE         1024

/* Size of the tokens array*/
#define MAX_TOKENS          10

#define MAX_VAR_NAME_LEN    100
#define MAX_VAR_VALUE_LEN   100
#define VAR_FILE            "variables.txt"

#define NUM_OF_STREAMS      3
#define MAX_FILE_NAME_SIZE  100

/* Structure used to store a command's name and exit status. 
 * An array of this structure represents the history of the most recently used commands. */
typedef struct {
    char command[256];
    int exit_status;
} ProcessHistory;



/**
 * @brief Checks if the given command is an internal command.
 * 
 * @param command The command to check.
 * @return int Returns EXIT_SUCCESS if the command is internal, otherwise returns S_EXIT_INVALID_COMMAND.
 */
int is_internal_command(const char* command);

/**
 * @brief Checks if the given command is an external command by searching through the PATH environment variable.
 * 
 * @param command The command to check.
 * @return int Returns EXIT_SUCCESS if the command is external, otherwise returns S_EXIT_INVALID_COMMAND.
 */
int is_external_command(const char* command);

/**
 * @brief Writes a message to a specified stream with a given color code.
 * 
 * @param stream The file descriptor of the stream to write to (e.g., STDOUT or STDERR).
 * @param msg The message to write.
 * @param color_code The color code for the message.
 */
void Write_syscall(int stream, const char* msg, const char* color_code);

/**
 * @brief Prints a line separator to STDOUT.
 */
void printLineSeparator();

/**
 * @brief Prints a welcome message to STDOUT.
 */
void WelcomeMessage();

/**
 * @brief Checks if the given path is a regular file.
 * 
 * @param path The path to check.
 * @return int Returns EXIT_SUCCESS if the path is a regular file, otherwise returns S_EXIT_FAILURE.
 */
int is_file(const char *path);

/**
 * @brief Checks if the given path is a directory.
 * 
 * @param path The path to check.
 * @return int Returns EXIT_SUCCESS if the path is a directory, otherwise returns S_EXIT_FAILURE.
 */
int is_directory(const char *path);

/**
 * @brief Trims leading and trailing spaces from a string.
 * 
 * @param str The string to trim.
 */
void trim_spaces(char *str);

/**
 * @brief Reduces multiple spaces between words in a string to a single space.
 * 
 * @param str The string to process.
 */
void reduce_spaces(char *str);

/**
 * @brief Prints the current working directory to STDOUT.
 * 
 * @return int Returns EXIT_SUCCESS on success, otherwise prints an error message.
 */
int Print_Current_Directory();

/**
 * @brief Parses a full command into tokens.
 * 
 * @param full_command The command string to parse.
 * @param tokens Array of strings to store the parsed tokens.
 */
void Parse_Commands(char* full_command, char** tokens);

/**
 * @brief Processes command-line options and extracts source and target paths.
 * 
 * @param tokens Array of strings containing command and its options.
 * @param source_path Pointer to store the source path.
 * @param target_path Pointer to store the target path.
 * @return int Returns flags to be used for file operations.
 */
int Process_Options(char** tokens, char** source_path, char** target_path);

/**
 * @brief Checks if a string is a valid variable name.
 * 
 * @param name The variable name to check.
 * @return bool Returns true if the variable name is valid, otherwise returns false.
 */
bool is_valid_variable_name(const char *name);

/**
 * @brief Detects variable declarations in the format VAR_NAME=value and extracts the variable name and value.
 * 
 * @param command The command string to check.
 * @param var_name Buffer to store the extracted variable name.
 * @param var_value Buffer to store the extracted variable value.
 * @return int Returns 1 if a variable declaration is found, otherwise returns 0.
 */
int contains_variable_declaration(const char *command, char *var_name, char *var_value);

/**
 * @brief Detects variable usages in the format $VAR_NAME in a token.
 * 
 * @param token The token to check.
 * @param var_name Buffer to store the extracted variable name.
 * @return int Returns 1 if a variable usage is found, otherwise returns 0.
 */
int contains_variable_usage(const char *token, char *var_name);

/**
 * @brief Adds or updates a variable in a file.
 * 
 * @param name The variable name.
 * @param value The variable value.
 */
void set_variable(const char *name, const char *value);

/**
 * @brief Retrieves the value of a variable from a file.
 * 
 * @param name The variable name.
 * @param variable_value Buffer to store the variable value.
 * @return int Returns S_EXIT_SUCCESS if the variable is found, otherwise returns S_EXIT_FAILURE.
 */
int get_variable(const char *name, char *variable_value);

#endif
