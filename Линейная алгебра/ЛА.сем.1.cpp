#include <iostream>
using namespace std;

class Base
{
public:
    unsigned int x;
};

class SemiGroup
{
public:
    virtual Base sum(Base b) = 0; 
};

class MySGroup : public SemiGroup, public Base
{
public:
    Base sum(Base b)
    {
        Base tmp;
        tmp.x = x + b.x;
        return tmp;
    }
};

int main()
{
    MySGroup  item1, item2;
    item1.x = 1;
    item2.x = 2;
    cout << (item1.sum(item2)).x << endl;
    SemiGroup* g1 = new MySGroup();
    SemiGroup* g2 = new MySGroup();
    return 0;
}