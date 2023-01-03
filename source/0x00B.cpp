#include <iostream>
#include <stdio.h>



class A {
private:
    const char* name = "A";
public:
    A() {
        printf("Defualt Construction of %s\n", name);
    }
    A(const char* str) {
        printf("Second Construction of %s\n", name);
    }
    ~A() {
        printf("Destruction of %s\n", name);
    }
};


class B {
private:
    const char* name = "B";
    A classA;
public:
    B() {
        classA = A("Hello");
        std::cout << "BlaBla" << std::endl;
        printf("Defualt Construction of %s\n", name);
    }
    B(const char* str) {
        printf("Second Construction of %s\n", name);
    }
    ~B() {
        printf("Destruction of %s\n", name);
    }
};


A getClass() {
    const char* msg = "Heelow";
    return A(msg);
}
int main() {

    B tmp;
    getchar();
    return 0;
}
