# Task Overview

This task involves creating a simple C program that is split into two files. One file contains a function and one or more global variables, while the other file contains the main function and utilizes the function defined in the first file. Additionally, you will analyze the symbol table and section sizes of the object files and the generated executable.
Program Structure

    File 1 (file1.c):
        Contains two global variables named GLOBAL_1, GLOBAL_2.
        Contains a function of return type void, it prints the values of the two global variables.

    File 2 (file2.c):
        Contains the main function.
        Uses the function defined in file1.c.


## 1- Printing symbol table of:
  ### * Object file of file1.c:
![image](https://github.com/user-attachments/assets/37e3f48d-44de-4979-a877-1870b6022495)

  ### * Object file of file2.c:
![image](https://github.com/user-attachments/assets/aab55432-92d6-4524-8f8b-a966c2e262d1)

  ### * The executable file:
![image](https://github.com/user-attachments/assets/2c239507-a33a-4d82-a69f-c0c6bf60b541)
![image](https://github.com/user-attachments/assets/68d709c5-727d-41a7-8229-0d694eb5cf82)

## 2- Getting the size of .data and .text sections (using size command) :
  ### * Object file of file1.c:
![image](https://github.com/user-attachments/assets/6bb6a786-dd01-45dd-8704-5675e14768d9)

  ### * Object file of file2.c:
![image](https://github.com/user-attachments/assets/38d7898b-539f-4215-a53f-ab16b00aa70c)

  ### * The executable file:
![image](https://github.com/user-attachments/assets/f400d2f7-1837-4f3e-98e6-c4dff3549621)


## 3- Getting the size of .data and .text sections (using readelf command with option -S) :
  ### * Object file of file1.c:
![image](https://github.com/user-attachments/assets/55b75b49-99df-4fd6-8566-a01efa15051e)

  ### * Object file of file2.c:
![image](https://github.com/user-attachments/assets/ab7f9da7-1baa-410e-9df6-d9c899deed57)

  ### * The executable file:
![image](https://github.com/user-attachments/assets/143e0659-5a8c-4ba4-b4cf-e5bddae0e0db)
![image](https://github.com/user-attachments/assets/31073dd8-181e-41cc-9188-d0d326573ee1)

