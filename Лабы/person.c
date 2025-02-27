#indef PERSON_rel
#define PERSON_rel
#include "person.h"
Person* new_person(char info1, int score1, int influence1)
{
	if (info1 && score1 &&influence1)
	{
		perror("ivalid data format");
	}
	else
	{
		Person* human = (Dynamic_Array*)malloc(sizeof(Person));
		human->info = info1;
		human->score = score1;
		human->influence = influence1;

	}
}
char value_check(int value)
{
	if (value > 0)
	{
		char buf = person1->info + "хорошо побеседовал с" + person2->info;
		//printf(buf)		
	}
	else
	{
		char buf = person1->info + "отчитал" + person2->info;
		//printf(buf)	
	}
	return buf;
}
char have_an_inflence_on(*Person person1, *Person person2, int value)//value >0 || value<0
{
	if (value != 0)
	{
		if (person1 && person2 && value == false)
		{
			perror("ivalid data format");
		}
		else
		{
			if (person1->influence > person2->influence)
			{
				person2->score += (person1->influence- person2->influence) * value;
				return value_check(int value);
			}
			else
			{
				person1->score += person2->influence - person1->influence * value;
				return value_check(int value);
			}
			if (person1->score <= 0)
			{
				remove_person(person1);
			}
			if (person2->score <= 0)
			{
				remove_person(person2);
			}

		}
	}

}
char  remove_person(*Person human)
{
	if (human)
	{
		return human->info + "покинул университет";
		free(human);
	}
}


#endif