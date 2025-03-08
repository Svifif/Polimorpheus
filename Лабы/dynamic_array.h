#ifndef DYNAMIC_ARRAY //��������� ���� �� DYNAMIC_ARRAY.H ���� ���� �������� ������ 
#define DYNAMIC_ARRAY // ���������� ���� ������
#include <stdlib.h>
#include "person.h"

//
//union {
//	string svalue;
//	void* fvalue;
//};
//
//template<T>


typedef void* ElementType;//���� � element type ������� ����������� ��������

typedef struct {
	size_t size;
	void (*print)(void*);
	void* (*getType)();
} TypeInfo;

typedef int bool;
#define TRUE 1
#define FALSE 0
typedef ElementType (*LpMapFunction)(ElementType);
typedef bool (*LpWhereFunction)(ElementType);

typedef struct 
{
	TypeInfo typeinfo;
	ElementType* values; // ������ ���������� 
	int size;//������ ��������� ����� 
	int capacity;// ������� ����� ����� ���������
} Dynamic_Array;

//int set_ElementType(void* type, size_t size);
Dynamic_Array* create_Array(int assumed_capacity, TypeInfo typeinfo);
int add_value(Dynamic_Array* array, ElementType value, TypeInfo typeinfo, int index);
int remove_value(Dynamic_Array* array, int index);
Dynamic_Array* map(Dynamic_Array* array, LpMapFunction mapfunc);
Dynamic_Array* where(Dynamic_Array* array, LpWhereFunction predicate);
Dynamic_Array* concatenate(Dynamic_Array* array1, Dynamic_Array* array2);
int remove_Array(Dynamic_Array* array);
void cpy_array(Dynamic_Array* array, int index1, int index2);




#endif// ���� �������� ������ 