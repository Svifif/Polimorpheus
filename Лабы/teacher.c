#include "teacher.h"
Teacher new_teacher(char* firstName, char* middleName, char* lastName, int day, int month, int year)
{
	return (Teacher)new_person(Teacher_info, firstName, middleName, lastName, day, month, year);
}