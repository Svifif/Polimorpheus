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
typedef  void* ElementType;

typedef struct Dynamic_Array
{
	ElementType* values; // массив указателей 
	int size;//текуще количесво ячеек 
	int capacity;// сколько ячеек можем поместить
};
Dynamic_Array* create_Array(int assumed_capacity);
Dynamic_Array* add_value(Dynamic_Array* array, ElementType value, int index);
void remove_value(Dynamic_Array* array,int index);
Dynamic_Array* map(Dynamic_Array* array, ElementType(*)(ElementType) func, int index1, int index2);
Dynamic_Array* where(Dynamic_Array* array, bool(*)(ElementType) predicate);
Dynamic_Array* concatenate(Dynamic_Array* array1, Dynamic_Array* array2)
void remove_Array(Dynamic_Array* array);
void cpy_array(Dynamic_Array* array, int index1, int index2);




#endif// чтоб избежать ошибок 