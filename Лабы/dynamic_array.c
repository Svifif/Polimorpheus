#include "dynamic_array.h"

/* функци€ создает пустой массив с параметром size=0,
 принимает значение  assumed_capacity- предполагаемый размер массива,
 возвращает указатель на массив или пустой указатель в случае ошибки.
*/
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
	Dynamic_Array* array = (Dynamic_Array*)malloc(sizeof(Dynamic_Array));//приведение типа + выдел€ем пам€ть
	array->values = malloc(assumed_capacity * sizeof(ElementType));
	array->size = 0;
	array->capacity = assumed_capacity;
	return array; // пусть возвращает указатель на массив
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
//		if (размер array 2 влезает)
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


/// <summary>
/// ƒобавл€ет элемент в массив
/// </summary>
/// <param name="array">целевой массив</param>
/// <param name="value">элемент дл€ добавлени€</param>
/// <param name="index">куда добавить</param>
/// <returns>возвращает 0 если успешно, -1 если неуспешно</returns>
int add_value(Dynamic_Array* array, ElementType value, int index)
{
	if (index < 0)
	{
		perror("index can`t  be <0");
		return -1;
	}
	if (index > (array->size)) //+1 не надо так как индексаци€ с 0 (это случай если a, b, c, _ ,хотим добавить)
	{
		perror("list index out of range");
		return -1;
	}
	if (array->capacity <= (array->size + 1))//надо увеличить массив +1 добавили новую €чейку
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
		memcpy(dst, src, (array->size) * sizeof(ElementType));
		array->values[index] = value;
	}
	array->size++;
}
/*удал€ет элемент массива по индексу
возвр€щет 0 если успешно, -1 если нет */
int remove_value(Dynamic_Array* array,int index)
{
	if (array->size >= index || index<0) //если array size 0 он будетменьше или равен индекс
	{
		perror("ivalid index");
		return -1;
	}
	auto src = array->values + index * sizeof(ElementType);
	auto dst = src - sizeof(ElementType);
	memcpy(dst, src, (array->size - index - 1) * sizeof(ElementType));
	array->size--;
	return 0;
}
/*примен€ет функцию к элементам массива 
возвращает указатель на новый массив и пустой указатель в случае ошибки*/
Dynamic_Array* map(Dynamic_Array* array, LpMapFunction func)
{
	if (array == NULL || func == NULL)
	{
		perror("ivalid data format");
		return NULL;
	}
	Dynamic_Array* result = create_void_Array(array->size);
	result->size = array->size;
	for (int i = 0; i < array->size; i++)
	{
		result->values[i] = func(array->values[i]);
	}
	return result;

}

/// <summary>
/// примен€ет предикат к  элементам массива
///возвращает указатель на новый массив(в котором элементы дл€ которых предикат выдал true) и пустой указатель в случае ошибки
/// </summary>
/// <param name="array">массив</param>
/// <param name="predicate">предикат</param>
/// <returns>указатель</returns>
Dynamic_Array* where(Dynamic_Array* array, LpWhereFunction predicate)
{
	if (array == NULL || predicate == NULL)
	{
		perror("ivalid data format");
		return NULL;
	}

	Dynamic_Array* result =  create_void_Array(array->size);
	for (int i = 0; i < array->size; i++)
	{
		if (predicate(array->values[i]) == TRUE)
		{
			add_value(result, array->values[i], i);
		}
	}
	return result;
}

/*создЄт новый массив путЄм обьединени€ 2 х исходных
возвращает указатель на новый массив  и пустой указатель в случае ошибки*/
Dynamic_Array* concatenate(Dynamic_Array* array1, Dynamic_Array* array2)
{
	if (array1 == NULL || array2 == NULL)
	{
		perror("ivalid data format");
		return NULL;
	}
	Dynamic_Array* result = create_void_Array(array1->size + array2->size);
	result->size = array1->size + array2->size;
	memcpy(result->values, array1->values, array1->size * sizeof(ElementType));
	memcpy(result->values + array1->size * sizeof(ElementType), array2->values, array2->size * sizeof(ElementType));

	return result;
}
/*удал€ет исходный массив 
возвращ€ет 0 в случае успеха -1 в случае ошибки*/
int remove_Array(Dynamic_Array* array)
{
	if (array == NULL)
	{
		perror("ivalid data format");
		return -1;

	}
	free(array->values);
	free(array);
	return 0;
}
