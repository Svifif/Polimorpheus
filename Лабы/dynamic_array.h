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
typedef ElementType void*;

typedef struct dynamic_Array
{
	ElementType* values; // массив указателей 
	int size;//текуще количесво ячеек 
	int capacity;// сколько ячеек можем поместить
};
dynamic_Array* create_void_Array( int assumed_capacity);
void Add_value(dynamic_Array* array, ElementType value, int index);
//void cpy_array(dynamic_Array* array, int index1, int index2);
void remove_value(dynamic_Array* array,int index);
void map(dynamic_Array* array, int index);
void where(dynamic_Array* array, int index);
void concatinate(dynamic_Array* array1, dynamic_Array* array2)
void remove_yourself(dynamic_Array* array);





#endif// чтоб избежать ошибок 
