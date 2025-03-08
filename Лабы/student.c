#include "student.h"
const char const* Student_info = "student";

Student new_student(char* firstName, char* middleName, char* lastName, int day, int month, int year)
{
	return (Student){ .person = new_person(Student_info, firstName, middleName, lastName, day, month, year) };
}