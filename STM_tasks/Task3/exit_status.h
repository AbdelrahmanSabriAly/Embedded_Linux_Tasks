// exit_status.h
#ifndef EXIT_STATUS_H
#define EXIT_STATUS_H

typedef enum {
    S_EXIT_SUCCESS                  = 0,     // Successful completion
    S_EXIT_FAILURE                  = 1,     // General failure
    S_EXIT_INVALID_COMMAND          = 2,     // Command is invalid
    S_EXIT_FILE_NOT_FOUND           = 3,     // File does not exist
    S_EXIT_PERMISSION_DENIED        = 4,     // Permission denied
    S_EXIT_SAME_OPERANDS            = 5,     // Operands are the same
    S_EXIT_OPEN_FILE_FAILED         = 6,     // Openning the file failed
    S_EXIT_MODIFY_EXISTED_FILE      = 7,        
    S_EXIT_NO_ARGS                  = 8      // No arguments passed
       
} ExitStatus;

#endif // EXIT_STATUS_H
