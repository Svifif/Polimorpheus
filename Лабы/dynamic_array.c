#include "dynamic_array.h"
Dynamic_Array* create_void_Array(int assumed_capacity)
{
	if (assumed_capacity < 0)
	{
		perror("capacity can`t  be <0");
		/*exit();*/
		return NULL;
	}
	//if (values_size <= 0)
	//{
	//	perror("values_size can`t  be <=0");
	//}
	*Dynamic_Array array = (Dynamic_Array*)malloc(sizeof(Dynamic_Array));//приведение типа + выделяем память
	array->values = malloc(assumed_capacity * sizeof(ElementType));
	array->size = 0;
	array->capacity = assumed_capacity;
	return array; // пусть возвращает указатель на массив
}

cpy_array(Dynamic_Array* array, Dynamic_Array * array2, int index1, int index2)
{
	if (index1 > index2)
	{
		perror("ivalid format of indexes");
	}
	else
	{
		if (размер array 2 влезает)
		{

			auto src = &array->values[index1];
			auto dst = &array2->values[index2];
			memcpy(dst, src, array->[index2 - index1] * sizeof(ElementType));
			array->values[index] = value;
		}
		else
		{
			perror ("array 2< array 1")
		}
	}
}

void add_value(Dynamic_Array* array, ElementType value, int index)
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
		//array->values = (ElementType*)realloc(array->values, array->capacity * sizeof(ElementType));//очевидно мы будем передавать указатель поэтому размер одиннаков
		auto tmp=  malloc(array->capacity * sizeof(ElementType));
		memcpy(tmp, array->values, sizeof(ElementType) * array->size);
		free(array->values);
		array->values = tmp;

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
int remove_value(Dynamic_Array* array,int index);
{
	if (array->size >= index || index<0) //если array size 0 он будетменьше или равен индекс
	{
		perror("ivalid index");
		return -1;
	}
	auto src = array->values + index * array->values_size;
	auto dst = src - sizeof(ElementType);
	memcpy(dst, src, (array->values_size - index - 1) * sizeof(ElementType));
	array->size--;
}
Dynamic_Array* map(Dynamic_Array* array, ElementType(*)(ElementType) func)
{
	auto result = Dynamic_Array * create_void_Array(array->size);
	result->size = array->size;
	for (int i = 0; i < array->size; i++)
	{
		result->values[i] = func(array->values[i]);
	}
	//void remove_Array(Dynamic_Array * array); ????
	
	return result;
}
Dynamic_Array* where(Dynamic_Array* array, bool(*)(ElementType) predicate)
{
	auto result = Dynamic_Array * create_void_Array(array->size);
	for (int i = 0; i < array->size; i++)
	{
		if  predicate(array->values[i])
		{
			add_value(result, array->values[i], i);
		}
	}
	return result;
}
