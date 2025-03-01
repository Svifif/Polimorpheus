#ifndef PERSON
#define PERSON
#include "time.h"
typedef struct 
{
	int id;
	char* firstName;
	char* middleName;
	char* lastName;
	time_t birthDate;
	//int score;
	//int influence;
} Person;
//Person* new_person(char* info, int score, int influence);
//Person* new_person(char* info, int score, int influence);
//char have_an_inflence_on(*Person person1, *Person person2, int value);
//char  remove_person(*Person human);
//char value_check(int value);
#endif