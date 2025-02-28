#indef DYNAMIC_ARRAY_realese
#define DYNAMIC_ARRAY_realese
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
	*Dynamic_Array array = (Dynamic_Array*)malloc(sizeof(Dynamic_Array));//���������� ���� + �������� ������
	array->values = malloc(assumed_capacity * sizeof(ElementType));
	array->size = 0;
	array->capacity = assumed_capacity;
	return array; // ����� ���������� ��������� �� ������
}

void cpy_array(Dynamic_Array* array, Dynamic_Array * array2, int index1, int index2)
{
	if (index1 > index2)
	{
		perror("ivalid format of indexes");
	}
	else
	{
		if (������ array 2 �������)
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
	if (index > (array->size)) //+1 �� ���� ��� ��� ���������� � 0 (��� ������ ���� a, b, c, _ ,����� ��������)
	{
		perror("list index out of range");
		exit();
	}
	if (array->capacity <= (array->size + 1))//���� ��������� ������ +1 �������� ����� ������
	{
		array->capacity *= 2;
		//array->values = (ElementType*)realloc(array->values, array->capacity * sizeof(ElementType));//�������� �� ����� ���������� ��������� ������� ������ ���������
		auto tmp=  malloc(array->capacity * sizeof(ElementType));
		memcpy(tmp, array->values, sizeof(ElementType) * array->size);
		free(array->values);
		array->values = tmp;

	}
	if (index == array->size + 1)// � ����� 
	{
		array->values[index] = value;
	}
	else// ����-�� 
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
	if (array->size >= index || index<0) //���� array size 0 �� ����������� ��� ����� ������
	{
		perror("ivalid index");
		return -1;
	}
	auto src = array->values + index * array->values_size;
	auto dst = src - sizeof(ElementType);
	memcpy(dst, src, (array->values_size - index - 1) * sizeof(ElementType));
	array->size--;
}
Dynamic_Array* map(Dynamic_Array* array, ElementType(*)(ElementType) func,int index1, int index2)
{
	if (array != NULL && func != NULL)
	{
		auto result = Dynamic_Array * create_void_Array(array->size);
		result->size = array->size;
		for (int i = index1; i < index2; i++)
		{
			result->values[i] = func(array->values[i]);
		}
		//void remove_Array(Dynamic_Array * array); ????

		return result;
	}
	else
	{
		perror("ivalid data format");
	}
}

Dynamic_Array* where(Dynamic_Array* array, bool(*)(ElementType) predicate)
{
	if (array != NULL && predicate != NULL )
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
	else
	{
		perror("ivalid data format");
	}
}

Dynamic_Array* concatenate(Dynamic_Array* array1, Dynamic_Array* array2)
{
	if (array1 != NULL && array2 != NULL)
	{
		result = create_void_Array(array1->capacity + array2->capacity);
		result->size = array1->size + array2->size;
		cpy_array(array1, result, 0, array1->size - 1);
		cpy_array(array1, result, array1->size - 1, result->size);
		return result*;
	}
	else
	{
		perror("ivalid data format");
	}
}

void remove_Array(Dynamic_Array* array)
{
	if (array != NULL)
	{
		free(array->values);
		free(array);
	}
	else
	{
		perror("ivalid data format");
	}
}
#endif