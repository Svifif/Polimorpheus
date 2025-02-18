#include "dynamic_array.h"
dynamic_Array* create_void_Array(int assumed_capacity, size_t values_size)
{
	if (assumed_capacity < 0)
	{
		perror("capacity can`t  be <0");
	}
	//if (values_size <= 0)
	//{
	//	perror("values_size can`t  be <=0");
	//}
	*dynamic_Array array = (dynamic_Array*)malloc(assumed_capacity * values_size);//приведение типа + выделяем память
	array->values = void;
	array->size = sizeof(array);
	array->cells_number = 0;
	array->capacity = assumed_capacity;
}
void Add_value(dynamic_Array* array, void* value, int index)
{

	if (index < 0)
	{
		perror("index can`t  be <0");
	}
	if (index > (array->cells_number)) //+1 не надо так как индексация с 0 (это случай если a, b, c, _ ,хотим добавить)
	{
		perror("list index out of range");
	}
	if (array->capacity <= (array->cells_number+1))//надо увеличить массив +1 добавили новую ячейку
	{
		array->values = (void**)realloc(array->values, array->capacity * sizeof(value));
		array->capacity *= 2;
	else
	}
	{

	}
	array->cells_number++;
}