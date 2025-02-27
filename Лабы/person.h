#indef PERSON
#define PERSON
typedef struct Person
{
	char info;//// профессор преподаватель, студет , ректор...фио, возраст...
	int score;
	int influence;
};
Person* new_person(char info, int score, int influence, char status);
char have_an_inflence_on(*Person person1, *Person person2, int value);
char  remove_person(*Person human);
char value_check(int value);
#endif