#include "student.h"
const char const* Student_info = "student";

struct Student new_student(char* firstName, char* middleName, char* lastName, int day, int month, int year)
{
	return (struct Student){ .person = new_person(Student_info, firstName, middleName, lastName, day, month, year) };
}

void printStudent(struct Student* student) {
	printPerson(&student->person);
}
char* get_StudentInfo(struct Student* student)
{
	get_typeInfo(&student->person);
}