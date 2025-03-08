#include "stdio.h"
#include "dynamic_array.h"
#include "teacher.h"
#include "student.h"
int main()
{

    printf("Hello world!\n");
    Person* human = (Person*)malloc(sizeof(Person));
    set_firstName(human,"Peter");
    printf("%s", get_firstName(human));
    free(human);
    return 0;
}

