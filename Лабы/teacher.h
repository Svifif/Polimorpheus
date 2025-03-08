#ifndef TEACHER
#define TEACHER
#include "person.h"

struct Teacher
{
	struct Person person;
};
struct Teacher new_teacher(char* firstName, char* middleName, char* lastName, int day, int month, int year);

#endif