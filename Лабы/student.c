#include "student.h"
Student new_student(char* firstName, char* middleName, char* lastName, int day, int month, int year)
{
	return (Student)new_person(Student_info, firstName, middleName, lastName, day, month, year);
}