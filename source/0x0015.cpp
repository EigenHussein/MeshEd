#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gl_init.h>
#include <string>
#include <string_proc.h>

using namespace std;

int main() {
       system("clear");
    ShitWrapper SW = ShitWrapper("./tmp/tmpwrt.svg");
    std::string finally = SW.GenerateSVG();
    cout << finally << endl;
 
    // const char *test_str =
    //     "<path id=\'g2-40\' d='m3.297634 2.391034c0-.029888 "
    //     "0-.049813-.169365-.219178c-1.24533-1.255293-1.564134-3.138232-1.564134-4.662516c0-1.733499 .37858-3.466999 "
    //     "1.603985-4.712329c.129514-.119552 .129514-.139477 "
    //     ".129514-.169365c0-.069738-.039851-.099626-.099626-.099626c-.099626 0-.996264 .67746-1.58406 1.942715c-.508095 "
    //     "1.09589-.627646 2.201743-.627646 3.038605c0 .777086 .109589 1.982565 .657534 3.108344c.597758 1.225405 "
    //     "1.454545 1.872976 1.554172 1.872976c.059776 0 .099626-.029888 .099626-.099626z\'/>";

    // auto tmp = getPair(string(test_str));
    // cout << tmp.first << "\n\n\n" << tmp.second << "\n\n\n" << endl;

    // cout << process_it(tmp.second, 10, 10) << endl;

    return 0;
}
int main33() {

    ifstream file("./tmp/tmpwrt.svg");
    string line;
    int line_number = 7;
    for (int i = 0; i < 7 - 1; i++) {
        file.ignore(0xffff, '\n');
    }
    getline(file, line);
    cout << "Line read: " << line << endl;
    auto k = getPair(line);
    // cout << k.first << '\t' << k.second << endl;

    cout << "The final result:\n" << process_it(k.second, 10.0, 20.0) << endl;

    // float k = atof("-.9093d34");
    // cout << k << endl;
    // stringstream hehe;
    // hehe << k;

    // cout << hehe.str() << endl;

    // cyclic_pair<int> HDHD(10, 20);

    // for (int i = 0; i < 10; i++) {
    //     cout << HDHD.get() << endl;
    // }
    // cout << stof(string("1234556.00"), (size_t *)3) << endl;

    // string test_string("M120.12A3.6");

    // string op = convolve(test_string, 10.f, 30.f);
    // cout << op << endl;

    return 0;
}