#indef DYNAMIC_ARRAY //��������� ���� �� DYNAMIC_ARRAY.H ���� ���� �������� ������ 
#define DYNAMIC_ARRAY // ���������� ���� ������
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
	ElementType* values; // ������ ���������� 
	int size;//������ ��������� ����� 
	int capacity;// ������� ����� ����� ���������
};
Dynamic_Array* create_Array(int assumed_capacity);
Dynamic_Array* add_value(Dynamic_Array* array, ElementType value, int index);
void remove_value(Dynamic_Array* array,int index);
Dynamic_Array* map(Dynamic_Array* array, ElementType(*)(ElementType) func, int index1, int index2);
Dynamic_Array* where(Dynamic_Array* array, bool(*)(ElementType) predicate);
Dynamic_Array* concatenate(Dynamic_Array* array1, Dynamic_Array* array2)
void remove_Array(Dynamic_Array* array);
void cpy_array(Dynamic_Array* array, int index1, int index2);




#endif// ���� �������� ������ 