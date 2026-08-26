#include <stdio.h>
#include <stdlib.h>

#include "shared.h"


//gcc -o test myalloctestfile.c myalloc.c -Wall -Wextra

typedef struct student {
    int age;
    char * major;
}student;

int main() {
    student * mike = myalloc(sizeof(student));
    mike->age = 20;
    mike->major = "CS";


    printf("\nStudents age: %d\n", mike->age);
    printf("Students major: %s\n", mike->major);

    return 0;
}