#ifndef TEACHER
#include "person.h"
const char const* Teacher_info = "teacher";
struct Teacher
{
	struct Person person;
};
typedef struct Teacher Teacher;
Teacher new_teacher(char* firstName, char* middleName, char* lastName, int day, int month, int year);

#endif