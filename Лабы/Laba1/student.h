#ifndef STUDENT
#define STUDENT
#include "person.h"

struct Student
{
	struct Person person;
};
//typedef struct Student Student;
struct Student new_student(char* firstName, char* middleName, char* lastName, int day, int month, int year);

void printStudent(struct Student* student);
char* get_StudentInfo(struct Student* student);
#endif