#indef DYNAMIC_ARRAY //проверяет есть ли DYNAMIC_ARRAY.H если есть избегаем ошибки 
#define DYNAMIC_ARRAY // определяем этот макрос
#include <stdlib.h>
//
//union {
//	string svalue;
//	void* fvalue;
//};
//
//template<T>
typedef struct dynamic_Array
{
	T* values; //ссылка на массив указателей а как проще?
	size_t size;// размер масива 
	int cells_number;//текуще количесво ячеек 
	int capacity;// сколько ячеек можем поместить
};
dynamic_Array* create_void_Array( int assumed_capacity, size_t values_size);
void Add_value(void* value, int index);// не забыть что если у нас в матрице массивы будут разной длинны это будет не матрица, матрица будет наследоваться от этого маассива
// момент 2 я могу ввести столбец строк и столбец int это надо учитывать
void remove_value(int index);
void  remove_yourself(void);
#endif// чтоб избежать ошибок 