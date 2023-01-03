#ifndef STR_PROC_HH__
#define STR_PROC_HH__
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

template <typename _T, int _SIZE> struct cyclic_array {
    unsigned int idx;
    std::vector<_T> vals;
    cyclic_array(const std::initializer_list<_T> &I) : vals(I) {
        idx = 0;
    }
    const _T get() {
        return vals[(idx++) % _SIZE];
    }
};

std::string readUntil(const char *str, char delim, bool include = false) {
    std::string tmp = std::string(str);
    auto ret = tmp.find(delim);
    if (ret == EOF) {
        return "";
    }
    return std::string(str, ret + include);
}
std::pair<std::string, std::string> getPair(const std::string &str) {
    int id1 = str.find_first_of("id='");
    int id2 = str.find("d=\'", id1 + 3);
    // std::cout << &str.data()[id1] << '\n' << &str.data()[id2] << std::endl;

    return std::make_pair(readUntil(str.c_str() + id1 + 4, '\''), readUntil(str.c_str() + id2 + 3, '\''));
}

bool is_char(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
bool is_number_related(char c) {
    return (c == '.' || c == '-' || c == '+' /* || c == ' ' */);
}
bool is_number(char c) {
    return c >= '0' && c <= '9';
}

std::string convolve(const std::string &str, const float x, const float y) {
    cyclic_array<float, 2> shift({x, y});
    std::stringstream s;
    for (int i = 0; i < str.size(); i++) {
        if (is_number(str[i]) || is_number_related(str[i])) {
            float tmp = atof(str.c_str() + i);
            tmp += shift.get();
            s << tmp << " ";
            while (is_number(str[i + 1]) || str[i + 1] == '.') {
                i++;
            }
        }
        if (is_char(str[i])) {
            s << str[i];
        }
    }
    return s.str();
}

std::string convolve2(const std::string &str, const float x, const float y) {

    cyclic_array<float, 2> shift({x, y});

    std::stringstream s;
    int state = 0;
    int number_idx = 0;
    int number_length = 0;
    for (int i = 0; i < str.size(); i++) {
        // A letter
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || str[i] == ' ' || str[i] == '-') {
            // if a letter follows a number
            // cout << "Reached: " << str[i] << endl;
            if (state == 0) {
                if (str[i] == '-') {
                    state = 2;
                    i--;
                    continue;
                }
                s << str[i];
            } else if (state == 1) {
                std::string old_number_str = std::string(str.c_str() + number_idx, number_length);
                float old_number_float = atof(old_number_str.c_str());
                s << (old_number_float + shift.get());
                s << str[i];
                state = 0;
            }
        } else if (str[i] == '.' || str[i] == '-' || (str[i] >= '0' && str[i] <= '9')) {
            if (state == 0 || state == 2) { // beginning of a number
                state = 1;
                number_idx = i;
                number_length = 1;
            } else if (state == 1) { // reading the digits
                number_length++;
            }
        }
    }
    if (state == 1) {
        std::string old_number_str = std::string(str.c_str() + number_idx, number_length);
        float old_number_float = atof(old_number_str.c_str());
        s << (old_number_float + shift.get());
        state = 0;
    }
    return s.str();
}

int get_next_number(const char *base, int offset, int freedom = 20) {
    for (int i = offset + 1; i < offset + freedom; i++) {
        if (base[i] == '-' || base[i] == ' ') {
            return base[i] == '-' ? i : i + 1;
        }
    }
    printf("ovenfkaejrge\n");
    return -1;
}
typedef double proc_var;
std::string process_it(const std::string &str, const proc_var x, const proc_var y) {
    std::vector<int> nLocations;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == 'm') {
            nLocations.push_back(i + 1);
            break;
        }
    }
    std::stringstream ss;
    for (int k = 0; k < nLocations.size(); k++) {

        int idx = nLocations[k];

        proc_var rx = atof(str.c_str() + idx);

        idx = get_next_number(str.c_str(), idx);

        proc_var ry = atof(str.c_str() + idx);

        for (int i = idx; i < idx + 32; i++) {
            if (is_char(str[i])) {
                idx = i;
                break;
            }
        }
        printf("origin{%f,%f}\tshift({%f,%f}\n", rx, ry, x, y);
        ss << " M " << rx + x << " " << ry + y << " ";
        if (k < nLocations.size() - 1) {
            ss << std::string(str.c_str() + idx, nLocations[k + 1] - idx - 1);
        } else {
            ss << std::string(str.c_str() + idx);
        }
    }
    return ss.str();
}

struct use_class {
    std::string x_coord, y_coord, id;
    std::string find_val(const std::string &str, const char *start, const char *end, int offset, int *found) {
        int ret1 = str.find(start, offset);
        int ret2 = str.find(end, ret1 + strlen(start));
        *found = ret1 + strlen(start);
        return std::string(str.c_str() + *found, ret2 - (*found));
    }
    use_class(const std::string &str) {
        if (strncmp(str.c_str(), "<use", 4) == 0) {
            int offset = 0;
            x_coord = find_val(str, "x=\'", "\'", 0, &offset);
            y_coord = find_val(str, "y=\'", "\'", offset, &offset);
            id = find_val(str, "xlink:href=\'#", "\'", offset, &offset);
        }
    }
    const proc_var Xcoord() const {
        return atof(x_coord.c_str());
    }
    const proc_var Ycoord() const {
        return atof(y_coord.c_str());
    }
};

class ShitWrapper {
  private:
  public:
    std::unordered_map<std::string, std::string> map;
    std::vector<use_class> equations;
    ShitWrapper(const char *filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            if (strncmp(line.c_str(), "<path", 5) == 0) {
                map.insert(getPair(line));
                //                cout << "Pathline: " << line << endl;
            } else if (strncmp(line.c_str(), "<use", 4) == 0) {
                equations.emplace_back(line);
                //                cout << "UseLine: " << line << endl;
            }
        }
        std::cout << map.size() << '\t' << equations.size() << std::endl;
    }
    std::string GenerateSVG() {
        std::string tmp;

        for (const auto &i : equations) {
            std::cout << "Writing " << i.id << "\t" << i.Xcoord() << '\t' << i.Ycoord() << std::endl;
            tmp += process_it(map[i.id], i.Xcoord(), i.Ycoord()) + "\n";
        }
        return tmp;
    }
    std::vector<std::string> GenSVG() {
        std::vector<std::string> tmp;
        for (const auto &i : equations) {
            std::cout << "Writing " << i.id << "\t" << i.Xcoord() << '\t' << i.Ycoord() << std::endl;
            tmp.push_back(process_it(map[i.id], i.Xcoord(), i.Ycoord()));
        }
        return tmp;
    }
};

#endif