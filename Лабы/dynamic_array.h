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
typedef struct dynamic_Array
{
	T* values; //������ �� ������ ���������� � ��� �����?
	size_t size;// ������ ������ 
	int cells_number;//������ ��������� ����� 
	int capacity;// ������� ����� ����� ���������
};
dynamic_Array* create_void_Array( int assumed_capacity, size_t values_size);
void Add_value(void* value, int index);// �� ������ ��� ���� � ��� � ������� ������� ����� ������ ������ ��� ����� �� �������, ������� ����� ������������� �� ����� ��������
// ������ 2 � ���� ������ ������� ����� � ������� int ��� ���� ���������
void remove_value(int index);
void  remove_yourself(void);
#endif// ���� �������� ������ 