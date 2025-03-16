#include "stdio.h"
#include "dynamic_array.h"
#include "teacher.h"
#include "student.h"

struct Teacher* mapAndPrintTeacher(struct Teacher* t) {
    printTeacher(t);
    return t;
}

void CreatePersonTest1() {
    printf("Hello world!\n");
    Person* human = (Person*)malloc(sizeof(Person));
    set_firstName(human, "Peter\n");
    printf("%s", get_firstName(human));
    free(human);
}
struct Student* mapAndPrintStudent(struct Student* t) {
    printStudent(t);
    return t;
}
void CreateArrayTest1() {

    TypeInfo ti = new_TypeInfo(sizeof(struct Teacher), &printTeacher,   &get_TeacherInfo);
    struct Teacher t1 = new_teacher("Kuznetsov", "Ivan", "Vladislavovich", 12, 7, 1985);
    struct Teacher t2 = new_teacher("Kuznetsov", "Artem", "Alekseevich", 25, 3, 1992);
    struct Teacher t3 = new_teacher("Kovalev", "Konstantin", "Ruslanovich", 19, 8, 1982);
    struct Teacher t4 = new_teacher("Orlov", "Vladislav", "Ivanovich", 19, 11, 1984);
    struct Teacher t5 = new_teacher("Kovalev", "Artem", "Evgenyevich", 25, 8, 1993);
    struct Teacher t6 = new_teacher("Popov", "Sergey", "Nikolaevich", 10, 7, 1989);
    struct Teacher t7 = new_teacher("Ivanov", "Evgeniy", "Ivanovich", 5, 4, 1999);
    struct Teacher t8 = new_teacher("Nikolaev", "Artem", "Ivanovich", 17, 9, 1998);
    struct Teacher t9 = new_teacher("Kovalev", "Denis", "Sergeyevich", 15, 8, 1985);
    Dynamic_Array arr = create_Array(9, ti);
    add_value(&arr, &t1, 0, ti);
    add_value(&arr, &t2, 1, ti);
    add_value(&arr, &t3, 2, ti);
    add_value(&arr, &t4, 3, ti);
    add_value(&arr, &t5, 4, ti);
    add_value(&arr, &t6, 5, ti);
    add_value(&arr, &t7, 6, ti);
    add_value(&arr, &t8, 7, ti);
    add_value(&arr, &t9, 8, ti);
    Dynamic_Array arr2 = map(&arr, *mapAndPrintTeacher);
    remove_Array(&arr);
    remove_Array(&arr2);
}


void CreateArrayTest2() {

    TypeInfo ti = new_TypeInfo(sizeof(struct Student), &printStudent, &get_StudentInfo);
    struct Student t0 = new_student("Karpov", "Vladimir", "Pavlovich", 10, 3, 1999);
    struct Student t1 = new_student("Sokolov", "Vladimir", "Andreevich", 25, 7, 2007);
    struct Student t2 = new_student("Sokolov", "Andrei", "Romanovich", 5, 10, 2007);
    struct Student t3 = new_student("Karpov", "Andrei", "Dmitrievich", 2, 4, 2000);
    struct Student t4 = new_student("Gusev", "Roman", "Vadimovich", 9, 10, 2005);
    struct Student t5 = new_student("Gusev", "Andrei", "Vladimirovich", 17, 12, 2005);
    struct Student t6 = new_student("Semenov", "Evgeny", "Olegovich", 25, 2, 2004);
    struct Student t7 = new_student("Dmitriev", "Roman", "Igorevich", 9, 11, 1996);
    struct Student t8 = new_student("Semenov", "Pavel", "Evgenyevich", 2, 11, 1995);
    struct Student t9 = new_student("Kolesnikov", "Sergei", "Vadimovich", 23, 1, 2001);
    Dynamic_Array arr = create_Array(9, ti);
    add_value(&arr, &t0, 0, ti);
    add_value(&arr, &t1, 1, ti);
    add_value(&arr, &t2, 2, ti);
    add_value(&arr, &t3, 3, ti);
    add_value(&arr, &t4, 4, ti);
    add_value(&arr, &t5, 5, ti);
    add_value(&arr, &t6, 6, ti);
    add_value(&arr, &t7, 7, ti);
    add_value(&arr, &t8, 8, ti);
    add_value(&arr, &t9, 2, ti);
    if (arr.size != 10) {
        perror("Test CreateArrayTest2 failed at point 0");
    }
    if (get_value(&arr, 9) != &t8) {
        perror("Test CreateArrayTest2 failed at point 1");
    }
    if (get_value(&arr, 2) != &t9) {
        perror("Test CreateArrayTest2 failed at point 2");
    }
    if (arr.size != 10) {
        perror("Test CreateArrayTest2 failed at point 3");
    }

    //printf("-------------------------------------------------\n");
    //for (int i = 0; i < arr.size; i++) printStudent(get_value(&arr, i));

    remove_value(&arr, 2);

    //printf("-------------------------------------------------\n");
    //for (int i = 0; i < arr.size; i++) printStudent(get_value(&arr, i));

    if (get_value(&arr, 8) != &t8) {
        perror("Test CreateArrayTest2 failed at point 4");
    }
    if (arr.size != 9) {
        perror("Test CreateArrayTest2 failed at point 5");
    }
    if (get_value(&arr, 2) != &t2) {
        perror("Test CreateArrayTest2 failed at point 6");
    }
    remove_Array(&arr);
}
void CreateArrayTest3()
{
    TypeInfo ti1 = new_TypeInfo(sizeof(struct Student), &printStudent, &get_StudentInfo);
    struct Student t0 = new_student("Karpov", "Vladimir", "Pavlovich", 10, 3, 1999);
    TypeInfo ti2 = new_TypeInfo(sizeof(struct Teacher), &printTeacher, &get_TeacherInfo);
    struct Teacher t1 = new_teacher("Kuznetsov", "Ivan", "Vladislavovich", 12, 7, 1985);
    Dynamic_Array arr = create_Array(9, ti1);
    add_value(&arr, &t0, 0, ti1);
    if (add_value(&arr, &t1, 0, ti2) == -1)
    {
        printf("test CreateArrayTest3 succeeded\n");
    }
    remove_Array(&arr);
}
bool predicat(struct Student *pointer)
{
    if (pointer->person.firstName[0] == 'K')
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
void CreateArrayTest4()
{
    TypeInfo ti = new_TypeInfo(sizeof(struct Student), &printStudent, &get_StudentInfo);
    struct Student t0 = new_student("Karpov", "Vladimir", "Pavlovich", 10, 3, 1999);
    struct Student t1 = new_student("Sokolov", "Vladimir", "Andreevich", 25, 7, 2007);
    Dynamic_Array arr1 = create_Array(2, ti);
    add_value(&arr1, &t0, 0, ti);
    add_value(&arr1, &t1, 1, ti);
    Dynamic_Array arr2 = create_Array(2, ti);
    struct Student t2 = new_student("Sokolov", "Andrei", "Romanovich", 5, 10, 2007);
    struct Student t3 = new_student("Karpov", "Andrei", "Dmitrievich", 2, 4, 2000);
    add_value(&arr2, &t2, 0, ti);
    add_value(&arr2, &t3, 1, ti);
    Dynamic_Array arr3 = concatenate(&arr1, &arr2);
    if ((arr3.size == 4) && (get_value(&arr3, 0) == &t0) && (get_value(&arr3, 1) == &t1) && (get_value(&arr3, 2) == &t2) && (get_value(&arr3, 3) == &t3))
    {
        printf("test CreateArrayTest4 succeeded\n");
    }
    else
    {
        printf("test CreateArrayTest4 failed at point 1\n");
    }
    //for (int i = 0; i < arr3.size; i++) printStudent(get_value(&arr3, i));
   
    Dynamic_Array arr4=where(&arr3, &predicat);
    //for (int i = 0; i < arr4.size; i++) printStudent(get_value(&arr4, i));
    if ((arr4.size == 2) && (get_value(&arr4, 0) == &t0) && (get_value(&arr4, 1) == &t3))
    {
        printf("test CreateArrayTest4 succeeded\n");
    }
    else
    {
        printf("test CreateArrayTest4 failed at point 1\n");
    }
    remove_Array(&arr1);
    remove_Array(&arr2);
    remove_Array(&arr3);
    remove_Array(&arr4);
}
int main()
{
    //CreatePersonTest1();
    CreateArrayTest1();
    CreateArrayTest2();
    CreateArrayTest3();
    CreateArrayTest4();
    return 0;
}

