#!/bin/bash
gcc -g main.c utilities/utils.c cmds_implementations/cmds.c helper_functions/helpers.c -o output
./output