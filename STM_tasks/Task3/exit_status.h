// exit_status.h
#ifndef EXIT_STATUS_H
#define EXIT_STATUS_H

typedef enum {
    S_EXIT_SUCCESS                  ,     // Successful completion
    S_EXIT_FAILURE                  ,     // General failure
    S_EXIT_INVALID_COMMAND          ,     // Command is invalid
    S_EXIT_FILE_NOT_FOUND           ,     // File does not exist
    S_EXIT_PERMISSION_DENIED        ,     // Permission denied
    S_EXIT_SAME_OPERANDS            ,     // Operands are the same
    S_EXIT_OPEN_FILE_FAILED         ,     // Openning the file failed
    S_EXIT_READ_FILE_FAIL           ,     // Reading file failed
    S_EXIT_MODIFY_EXISTED_FILE      ,        
    S_EXIT_NO_ARGS                  ,     // No arguments passed
    S_EXIT_DUP_FILE_FAILED          ,     // Failed to duplicate file in redirecting streams    
    S_EXIT_INVALID_VARIABLE_NAME    ,
    S_EXIT_MEM_ALLOC                     // Failed to allocate memory using malloc   
} ExitStatus;

#endif // EXIT_STATUS_H
