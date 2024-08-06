# Simple Shell Program

This repository contains a basic shell program written in C that interprets user input and translates it into commands where possible. The shell includes several built-in commands as detailed below.

![alt text](image.png)
# Supported commands:

### 1. spwd: Prints the working directory.

### 2. secho: Prints a user-provided string to stdout.

    Example: echo Hello World

### 3. scp: Copies a file to another location.

    Usage: 
        cp {sourcePath} {targetPath}

    Notes:

        - The shell prints an error if the target file already exists.
        
        - If a target file name is provided, it will be used; otherwise, if only a target path is provided, the source file name will be used.
        
        - The -a option appends the source content to the end of the target file.

### 4. smv: Moves a file to another location.

    Usage: 
        mv {sourcePath} {targetPath}

    Notes:
        - The shell prints an error if the target file already exists.

        - If a target file name is provided, it will be used; otherwise, if only a target path is provided, the source file name will be used.

        - The -f option forces overwriting the target file if it exists.

### 5. scd: Change directory to the passed path
    Usage: scd {path}

### 6. senvir: Prints all the environment variables

### 7. stype: Prints the type of the command
    Usage: 
        stype {comand}

    types:
        * Internal
        * External
        * Unsupported

### 8. sphist: print The last 10 commands with their exit status

### 9. sexit: Prints "goodbye" and terminates the shell.

### 10. shelp: Prints all supported commands along with a brief description of each.

<br></br>
# Additional features:

The shell supports pressing Enter key similar to a real shell.

<br></br>
# Compilation and Execution

To compile and run the program, use the following commands:
```
gcc main.c utils.c -o output
./output
```
<br></br>
# Output Samples:

### scp Command:
![cp](https://github.com/asabry7/output_samples/blob/main/cp.gif)


<br></br>

### smv Command
![mv](https://github.com/asabry7/output_samples/blob/main/mv.gif)

<br></br>

### Other internal commands:
![internals](https://github.com/asabry7/output_samples/blob/main/other_internals.gif)


<br></br>

### External Commands:
![external](https://github.com/asabry7/output_samples/blob/main/external.gif)

