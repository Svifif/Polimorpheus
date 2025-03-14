#include "dynamic_array.h"

TypeInfo new_TypeInfo( size_t size, void (*print)(void*), char* (*getType)())
{
	TypeInfo info;
	info.size = size;
	info.print = print;
	info.getType = getType;
	return info;
}

const Dynamic_Array NullArray = { .values = NULL, .size = 0, .capacity = 0, .typeinfo = NULL };

/* ������� ������� ������ ������ � ���������� size=0,
 ��������� ��������  assumed_capacity- �������������� ������ �������,
 ���������� ��������� �� ������ ��� ������ ��������� � ������ ������.
*/
Dynamic_Array create_Array(int assumed_capacity, TypeInfo typeinfo)
{
	if (assumed_capacity < 0)
	{
		perror("capacity can`t  be <0");
		/*exit();*/
		return NullArray;
	}
	//if (values_size <= 0)
	//{
	//	perror("values_size can`t  be <=0");
	//}
	Dynamic_Array array = {
		.values = malloc(assumed_capacity * sizeof(ElementType)),
		.size = 0,
		.capacity = assumed_capacity,
		.typeinfo = typeinfo
	};
	return array; // ����� ���������� ��������� �� ������
}

//void cpy_array(Dynamic_Array* array, Dynamic_Array * array2, int index1, int index2)
//{
//	if (index1 > index2)
//	{
//		perror("ivalid format of indexes");
//		return
//	}
//	else
//	{
//		if (������ array 2 �������)
//		{
//
//			auto src = &array->values[index1];
//			auto dst = &array2->values[index2];
//			memcpy(dst, src, array->[index2 - index1] * sizeof(ElementType));
//			array->values[index] = value;
//		}
//		else
//		{
//			perror ("array 2< array 1")
//		}
//	}
//}


ElementType get_value(Dynamic_Array* array, int index) {
	if (index < 0)
	{
		perror("index can`t  be <0");
		return NULL;
	}
	if (index > (array->size)) //+1 �� ���� ��� ��� ���������� � 0 (��� ������ ���� a, b, c, _ ,����� ��������)
	{
		perror("list index out of range");
		return NULL;
	}
	return array->values[index];
}

/// <summary>
/// ��������� ������� � ������
/// </summary>
/// <param name="array">������� ������</param>
/// <param name="value">������� ��� ����������</param>
/// <param name="index">���� ��������</param>
/// <returns>���������� 0 ���� �������, -1 ���� ���������</returns>
int add_value(Dynamic_Array* array, ElementType value, int index, TypeInfo typeinfo)
{
	if (array->typeinfo.getType != typeinfo.getType)
	{
		perror("wrong walue type");
		return -1;
	}
	if (index < 0)
	{
		perror("index can`t  be <0");
		return -1;
	}
	if (index > (array->size)) //+1 �� ���� ��� ��� ���������� � 0 (��� ������ ���� a, b, c, _ ,����� ��������)
	{
		perror("list index out of range");
		return -1;
	}
	if (array->capacity <= array->size)//���� ��������� ������ +1 �������� ����� ������
	{
		array->capacity *= 2;
		ElementType* tmp = malloc(array->capacity * sizeof(ElementType));
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
		ElementType* src = array->values + index;
		ElementType* dst = src + 1;
		memcpy(dst, src, (array->size - index) * sizeof(ElementType));
		array->values[index] = value;
	}
	array->size++;
}

/// <summary>
/// ������� ������� ������� �� �������
/// </summary>
/// <param name="array"></param>
/// <param name="index"></param>
/// <returns> ��������� 0 ���� �������, -1 ���� ���</returns>
int remove_value(Dynamic_Array* array,int index)
{
	if (array->size <= index || index<0) //���� array size 0 �� ����������� ��� ����� ������
	{
		perror("ivalid index");
		return -1;
	}
	ElementType* dst = array->values + index;
	ElementType* src = dst + 1;
	memcpy(dst, src, (array->size - index - 1) * sizeof(ElementType));
	array->size--;
	return 0;
}
/*��������� ������� � ��������� ������� 
���������� ��������� �� ����� ������ � ������ ��������� � ������ ������*/
Dynamic_Array map(Dynamic_Array* array, LpMapFunction func)
{
	if (array == NULL || func == NULL)
	{
		perror("ivalid data format");
		return NullArray;
	}
	Dynamic_Array result = create_Array(array->size, array->typeinfo);
	result.size = array->size;
	//memcpy(dst, src, (array->size - index - 1) * sizeof(ElementType));
	for (int i = 0; i < array->size; i++)
	{
		result.values[i] = func(array->values[i]);
	}
	return result;

}

/// <summary>
/// ��������� �������� �  ��������� �������
///���������� ��������� �� ����� ������(� ������� �������� ��� ������� �������� ����� true) � ������ ��������� � ������ ������
/// </summary>
/// <param name="array">������</param>
/// <param name="predicate">��������</param>
/// <returns>���������</returns>
Dynamic_Array where(Dynamic_Array* array, LpWhereFunction predicate)
{
	if (array == NULL || predicate == NULL)
	{
		perror("ivalid data format");
		return NullArray;
	}

	Dynamic_Array result =  create_Array(array->size, array->typeinfo);
	for (int i = 0; i < array->size; i++)
	{
		if (predicate(array->values[i]) == TRUE)
		{
			add_value(&result, array->values[i], i, array->typeinfo);
		}
	}
	return result;
}

/*����� ����� ������ ���� ����������� 2 � ��������
���������� ��������� �� ����� ������  � ������ ��������� � ������ ������*/
Dynamic_Array concatenate(Dynamic_Array* array1, Dynamic_Array* array2)
{
	if (array1 == NULL || array2 == NULL)
	{
		perror("ivalid data format");
		return NullArray;
	}
	if (array1->typeinfo.getType() != array2->typeinfo.getType())
	{
		perror("ivalid array types");
		return NullArray;
	}

	Dynamic_Array result = create_Array(array1->size + array2->size, array1->typeinfo);
	result.size = array1->size + array2->size;
	memcpy(result.values, array1->values, array1->size * sizeof(ElementType));
	memcpy(result.values + array1->size * sizeof(ElementType), array2->values, array2->size * sizeof(ElementType));

	return result;
}

/// <summary>
/// ������� �������� ������ 
/// </summary>
/// <param name="array"></param>
void remove_Array(Dynamic_Array* array)
{
	if (array != NULL && array->values != NULL)
	{
		free(array->values);
	}
}
