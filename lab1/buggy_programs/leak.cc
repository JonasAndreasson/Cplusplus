#include <iostream>
using std::cout;

struct Foo{
    Foo(int s) :sz{s}, p{new int[s]} {
            for(int i=0; i < s; ++i) p[i]=i+1; // or use std:iota
    }
    void print() const {
        cout << "Foo: ";
        for(int i=0; i<sz; ++i){
            cout << p[i] << " ";
        }
        cout << "\n";;
    }
    int sz{};
    int* p;
};

struct Bar{
    Bar(int s) :f(s) {};
    void print() const {f.print();}
    Foo f;
};

void example1()
{
    Foo f(10);
    {
        cout << "entering inner block\n";
        Bar b(20);
        b.print();
        cout << "leaving inner block\n";
    }
    f.print();
    cout << "leaving example1\n";
}

int main()
{
    example1();
    cout << "leaving main\n";
}
