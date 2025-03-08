#ifndef PERSON
#define PERSON
#include "time.h"
struct Person
{
	char* typeInfo;
	char* firstName;
	char* middleName;
	char* lastName;
	time_t birthDate;
	//int score;
	//int influence;
};
typedef struct Person Person;
Person new_person(char* typeInfo, char* firstName, char* middleName, char* lastName, int day, int month, int year);

void set_firstName(Person* person,char* value );
char* get_firstName(Person* person);

void  set_middleName(Person* person, char* value);
char* get_middleName(Person* person);

void set_lastName(Person* person, char* value);
char* get_lastName(Person* person);

void set_birthDate(Person* person, time_t value);
time_t get_birthDate(Person* person);

char* get_typeInfo(Person* person);

void print(Person* person);
//Person* new_person(char* info, int score, int influence);
//char have_an_inflence_on(*Person person1, *Person person2, int value);
//char  remove_person(*Person human);
//char value_check(int value);
#endif