#include "teacher.h"
const char const* Teacher_info = "teacher";
struct Teacher new_teacher(char* firstName, char* middleName, char* lastName, int day, int month, int year)
{
	return (struct Teacher) { .person = new_person(Teacher_info, firstName, middleName, lastName, day, month, year) };
}
void printTeacher(struct Teacher* teacher) 
{
	printPerson(&teacher->person);
}
char* get_TeacherInfo(struct Teacher* teacher)
{
	get_typeInfo(&teacher->person);
}



