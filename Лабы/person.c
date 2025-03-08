#include "person.h"
//Person* new_person()
//{
//	//if (info1==NULL && score1 &&influence1)
//	//{
//	//	perror("ivalid data format");
//	//}
//	//else
//	//{
//	//	Person* human = (Person*)malloc(sizeof(Person));
//	//	human->info = info1;
//	//	human->score = score1;
//	//	human->influence = influence1;
//
//	//}
//	 Person* human = (Person*)malloc(sizeof(Person));
//	 return human;
//}

Person new_person(char* typeInfo, char* firstName, char* middleName, char* lastName,int day, int month, int year )
{
	Person user;
	user.typeInfo = typeInfo;
	user.firstName = firstName;
	user.middleName = middleName;
	user.lastName = lastName;
	struct tm tm;
	tm.tm_mday = day;
	tm.tm_mon = month-1;
	tm.tm_year = year-1900;

	time_t birthDate = mktime(&tm);
	user.birthDate = birthDate;
	return user;
}
void set_firstName(Person* person, char* value)
{
	person->firstName = value;
}
char* get_firstName(Person* person)
{
	return person->firstName;
}

void set_middleName(Person* person, char* value)
{
	person->middleName = value;
}
char* get_middleName(Person* person)
{
	return person->middleName;
}

void set_lastName(Person* person, char* value)
{
	person->lastName = value;
}
char* get_lastName(Person* person)
{
	return person->lastName;
}

void set_birthDate(Person* person, time_t value)
{
	person->birthDate = value;
}
time_t get_birthDate(Person* person)
{
	return person->birthDate;
}

char* get_typeInfo(Person* person)
{
	return person->typeInfo;
}

void print( Person* person)
{
	struct tm* now= localtime(person->birthDate);
	printf("%s %s %s %s % d.% d.% d ", person->typeInfo, person->firstName, person->middleName, 
		person->lastName, now->tm_mday, now->tm_mon + 1, now->tm_year+1900);

}









//char value_check(int value)
//{
//	if (value > 0)
//	{
//		char buf = person1->info + "хорошо побеседовал с" + person2->info;
//		//printf(buf)		
//	}
//	else
//	{
//		char buf = person1->info + "отчитал" + person2->info;
//		//printf(buf)	
//	}
//	return buf;
//}
//
//char have_an_inflence_on(*Person person1, *Person person2, int value)//value >0 || value<0
//{
//	if (value != 0)
//	{
//		if (person1 && person2 && value == false)
//		{
//			perror("ivalid data format");
//		}
//		else
//		{
//			if (person1->influence > person2->influence)
//			{
//				person2->score += (person1->influence- person2->influence) * value;
//				return value_check(int value);
//			}
//			else
//			{
//				person1->score += person2->influence - person1->influence * value;
//				return value_check(int value);
//			}
//			if (person1->score <= 0)
//			{
//				return flag;
//			}
//			if (person2->score <= 0)
//			{
//				return flag;
//			}
//
//		}
//	}
//
//}
//  remove_person(Person* human)
//{
//	if (human==NULL)
//	{
//		//return human->info + "покинул университет";
//		free(human);
//	}
//}
//
