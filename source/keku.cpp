
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

// Declaration of a variadic template
template <typename... Elems> struct Tuple;

// Recursive specialization of the template
template <typename Head, typename... Rest> struct Tuple<Head, Rest...> {
    Head first;          // First component
    Tuple<Rest...> rest; // Rest of components

    Tuple() : first(), rest() {
    } // Default constructor
};

// Terminal specialization of the template
template <> struct Tuple<> {
    Tuple() {
    }
};

// Let's instantiate it.
Tuple<int, double, char> t;

#include <algorithm>
#include <array>
#include <deque>
#include <iterator>
#include <list>

#include <iostream>
#include <iterator>
#include <math.h>
#include <sstream>
#include <thread>
void grab_doubles(std::istream_iterator<double> &it) {
    std::istream_iterator<double> end;
    for (; it != end; ++it)
        std::cout << ' ' << *it;
    std::cout << std::endl;
}
// auto k = 12ms;
// constexpr long double operator"" _deg_to_rad(long double deg)
// {
//     long double radians = deg * M_PI/ 180;
//     return radians;
// }

// auto l = 10_deg_to_rad;

int main2() {
    std::thread t;

    // array<int, 10> array_ = { 1,2,3,4,5,6,3,3,2 };

    // *std::find(array_.begin(), array_.end(), 6) = 0;
    // for (auto a : array_) {
    //     cout << a << endl;
    // }

    std::istringstream is("1  2 3.14 4 56 321.1 31223.d five 6 ");
    std::istream_iterator<double> it1(is);
    std::istream_iterator<double> it2(is);

    // std::string myString("1 2 3.14 4 56 321.1 31223.d five 6 ");
    // std::istringstream is(myString);
    // std::istream_iterator<double> it1(is);
    // std::istream_iterator<double> it2(it1);
    std::cout << "it1:";
    grab_doubles(it1);
    std::cout << "it2:";
    grab_doubles(it2);

    return 0;
}

#include <svg_wrapping.h>

int main4() {

    // std::string xml_file = read_xml("./tmp/tmpwrt.svg");
    // // std::cout << xml_file << std::endl;
    rapidxml::xml_document<> doc;
    // doc.parse<0>((char *)xml_file.c_str());

    // auto k = doc.first_node("svg")->first_node("defs")->first_node("path")->first_attribute("d");

    // std::cout << k->value() << std::endl;

    // std::istringstream iss(k->value() + 19);
    // std::istream_iterator<double> end__;
    // std::istream_iterator<double> val_iter(iss);
    // while (val_iter != end__) {
    //     std::cout << *(val_iter++) << endl;
    // }
    auto a = std::chrono::high_resolution_clock::now();
    auto svgeq = Cycloid::SVGEquation::SVGEq(doc, "./tmp/tmpwrt.svg");

    // for (int i = 0; i < 2; i++) {
    //     for (int j = 0; j < 32; j++) {
    //         auto k = svgeq.get_SET_location(i, j);
    //         cout << k.x << '\t' << k.y << endl;
    //     }
    // }
    auto tmp = svgeq.shift_symbol(0, {});
    auto b = std::chrono::high_resolution_clock::now();
    system("cls");
    cout << tmp << endl;
    cout << std::chrono::duration_cast<chrono::microseconds>(b - a).count() << endl;
    return 0;
}
int main() {

    const char *test_str = "matrix(-.1  0   0 -.1 0 0)";
    float d[6];
    sscanf(test_str, "matrix( %f %f %f %f %f %f)", d, d + 1, d + 2, d + 3, d + 4, d + 5);
    for (int i = 0; i < 6; i++) {
        cout << d[i] << endl;
    }

    return 0;
}