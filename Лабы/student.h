#ifndef STUDENT
#include "person.h"
const char const* Student_info = "student";
struct Student
{
	struct Person person;
};
typedef struct Student Student;
Student new_student(char* firstName, char* middleName, char* lastName, int day, int month, int year);

#endif