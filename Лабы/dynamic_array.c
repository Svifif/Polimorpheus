#include "dynamic_array.h"
dynamic_Array* create_void_Array(int assumed_capacity)
{
	if (assumed_capacity < 0)
	{
		perror("capacity can`t  be <0");
		exit();
	}
	//if (values_size <= 0)
	//{
	//	perror("values_size can`t  be <=0");
	//}
	*dynamic_Array array = (dynamic_Array*)malloc(sizeof(dynamic_Array));//приведение типа + выделяем память
	array->values = malloc(assumed_capacity * sizeof(ElementType));
	array->size = 0;
	array->capacity = assumed_capacity;
	return array; // пусть возвращает указатель на массив
}

void Add_value(dynamic_Array* array, ElementType value, int index)
{

	if (index < 0)
	{
		perror("index can`t  be <0");
		exit();
	}
	if (index > (array->size)) //+1 не надо так как индексация с 0 (это случай если a, b, c, _ ,хотим добавить)
	{
		perror("list index out of range");
		exit();
	}
	if (array->capacity <= (array->size + 1))//надо увеличить массив +1 добавили новую ячейку
	{
		array->capacity *= 2;
		array->values = (ElementType*)realloc(array->values, array->capacity * sizeof(ElementType));//очевидно мы будем передавать указатель поэтому размер одиннаков
	}
	if (index == array->size + 1)// в конец 
	{
		array->values[index] = value;
	}
	else// куда-то 
	{
		auto src = &array->values[index];
		auto dst = src+sizeof(ElementType);
		memcpy(dst, src, array->values_size * sizeof(ElementType));
		array->values[index] = value;
	}
	array->size++;
}
void remove_value(dynamic_Array* array,int index);
{
	auto src = array->values + index * array->values_size;
	auto dst = src + sizeof(ElementType);
	memcpy(dst, src, array->values_size * sizeof(ElementType));
	array->values[index] = value;
}
