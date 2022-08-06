#include <iostream>
#include <typeinfo>
using namespace std;

typedef string a, b;

int main(int argc, char const *argv[])
{
    a s1;
    b s2;
    cout << typeid(s1).name() << "\n" << typeid(s2).name();
    return 0;
}
