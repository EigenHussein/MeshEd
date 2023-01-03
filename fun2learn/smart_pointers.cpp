#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class TestClassA;
class TestClassB;

class TestClassA {

  private:
  public:
    int mVal;
    TestClassA(const TestClassA &src) {
        cout << "class{A} Copy Constructor" << endl;
    }
    TestClassA() {
        cout << "class{A} Default Constructor" << endl;
    }
    TestClassA(int val) {
        mVal = val;
        cout << "class{A} Constructor with value: " << val << endl;
    }
    ~TestClassA() {
        cout << "Class{A} Destructor" << endl;
    }
};

class TestClassB {

  private:
    std::shared_ptr<TestClassA> cls;
    TestClassA tmp;

  public:
    TestClassB(const TestClassB &src) {
        cout << "class{B} Copy Constructor" << endl;
    }
    TestClassB() : cls(new TestClassA()) {
        cout << "class{B} Default Constructor" << endl;
    }
    TestClassB(int val) {

        cout << "class{B} Constructor with value: " << val << endl;
        cls = std::unique_ptr<TestClassA>(new TestClassA(34));
    }
    ~TestClassB() {
        cout << "Class{B} Destructor" << endl;
    }
};
TestClassA getClassA() {
    TestClassA tmp;
    return tmp;
}

typedef unique_ptr<TestClassA> smart_testa;

smart_testa test() {
    // TestClassA *Keku = new TestClassA(100);
    auto k = smart_testa(new TestClassA(101));
    return k;
    // delete Keku;
}
int main() {
    // cout << "Simple" << endl;
    // TestClassB B;
    // cout << "Simple Assignment" << endl;
    // TestClassB newB = B;
    // cout << "CPLX construction of B" << endl;
    auto newB = TestClassB(10);

    getchar();
    return 0;
}