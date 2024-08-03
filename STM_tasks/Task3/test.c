#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void faulty_function(int *ptr, int size) {
    for (int i = 0; i <= size; i++) {
        ptr[i] = i * 2;  // Potential out-of-bounds write
    }
}

int main(int argc, char *argv[]) {
    int *array = (int *)malloc(5 * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    faulty_function(array, 5);  // Off-by-one error in the size parameter

    // Use of uninitialized variable
    int uninitialized_variable;
    printf("Uninitialized variable value: %d\n", uninitialized_variable);

    // Buffer overflow
    char buffer[10];
    strcpy(buffer, "This is a string that is too long for the buffer");

    free(array);
    return 0;
}

