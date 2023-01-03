#include <SVGParser.h>

namespace Cycloid {
namespace SVGEquation {
std::string read_xml(const char *filename) {
    std::stringstream ss;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        ss << line << "\n";
    }
    file.close();
    return ss.str();
}

void SVGEq::load_paths() {
    if (!initialized)
        return;
    rapidxml::xml_node<> *defines = document->first_node("svg")->first_node("defs");
    rapidxml::xml_node<> *paths_defines = defines->first_node("path");
    while (paths_defines != NULL) {
        auto k = paths_defines->first_attribute("id")->value();
        if (*k != '\0') {
            paths.insert(std::make_pair(std::string(k), std::string(paths_defines->first_attribute("d")->value())));
            // std::cout << "found pair: " << k << '\t' << paths[k] << std::endl;
        }
        paths_defines = paths_defines->next_sibling();
    }
}
int SVGEq::get_stride(rapidxml::xml_node<> *use, const char *attrib) {
    auto ptr = use;
    const char *first_x_value = ptr->first_attribute(attrib)->value();
    int counter = 1;
    while ((ptr = ptr->next_sibling()) != 0) {

        if (strcmp(ptr->first_attribute(attrib)->value(), first_x_value) == 0) {
            return counter;
        }
        counter++;
    }
    return counter;
}
rapidxml::xml_node<> *SVGEq::next_node(rapidxml::xml_node<> *use, int n) {
    for (int i = 0; i < n; ++i) {
        use = use->next_sibling();
        if (use == 0) {
            printf("EHNM\n");
            return 0;
        }
    }
    return use;
}
void SVGEq::fill_offsets(int mode, const std::vector<int> &eqn_variants) {

    if (!initialized)
        return;
    // read "use" nodes which correspondes to symbols
    rapidxml::xml_node<> *page = document->first_node("svg")->first_node("g");
    rapidxml::xml_node<> *use = page->first_node("use");

    if (use == 0)
        return;

    // read header nodes
    for (int i = 0; i < header_length; i++) {
        header_nodes.push_back(use);
        use = use->next_sibling();
    }
    if (mode == 0) {
        for (int i = 0; i < 2; i++) {
            int stride = get_stride(use, "x");
            for (int j = 0; j < eqn_variants[i]; j++) {
                if (use != 0) {
                    equation_ref.push_back(std::make_pair(use, stride));
                    use = next_node(use, stride);
                }
            }
        }

    } else {
        // int big_counter = 0;
        // int equation_counter = 0;
        // // new_reference:
        // auto first_equation_use = use;
        // const char *first_x_value = first_equation_use->first_attribute("x")->value();
        // std::cout << "first x: " << first_x_value << std::endl;
        // equation_ref.push_back(use);
        // equation_counter++;
        // while ((use = use->next_sibling()) != 0) {
        //     if (strcmp(use->first_attribute("x")->value(), first_x_value) == 0) {
        //         // std::cout << "x :" << first_x_value << std::endl;
        //         offsets.push_back(use);
        //         // if (++equation_counter == eqn_list[big_counter]) {
        //         //     use = use->next_sibling();
        //         //     equation_counter = 0;
        //         //     big_counter++;
        //         //     goto new_reference;
        //         // }
        //     }
        // }
    }
}
// int estimate_stride() {
//     if (!initialized)
//         return -1;
//     auto k = offsets[0]; // document->first_node("svg")->first_node("g")->first_node("use");
//     const char *start_key = k->first_attribute("x")->value();
//     int counter = 1;

//     while ((k = k->next_sibling()) != 0) {
//         // std::cout << k->first_attribute("x")->value() << std::endl;
//         if (strcmp(k->first_attribute("x")->value(), start_key) == 0) {
//             break;
//         }
//         counter++;
//     }
//     return counter;
// }

std::string SVGEq::process_it(const char *str, const glm::vec2 &xy, bool replace) {
    std::istringstream iss(str + 1);
    std::istream_iterator<float> isi(iss);
    glm::vec2 tmp;
    tmp.x = *isi++;
    tmp.y = *isi;
    tmp = replace ? glm::vec2(0, 0) : tmp + xy;
    int idx = 1;
    while (!is_char(str[idx])) {
        idx++;
    }
    std::stringstream out;
    out << " M " << tmp.x << " " << tmp.y << " " << str + idx;
    return out.str();
}
// int find_transform() {
//     if (!initialized)
//         return 0;
//     auto g_node = document->first_node("svg")->first_node("g");
//     auto matrix = g_node->first_attribute("transform");
// }

rapidxml::xml_node<> *SVGEq::get_SET(int n, int m) {
    // if (n >= equation_ref.size() || m >= symbol_per_equation)
    //     return {};
    auto equation = equation_ref[n]; // equattion_node , symbol_length pair
    n %= equation_ref.size();
    m %= equation.second;
    auto symbol = equation.first;
    for (int i = 0; i < m; i++) {
        symbol = symbol->next_sibling();
    }
    return symbol;
}
glm::vec2 SVGEq::get_SET_location(int n, int m) {
    auto equation = equation_ref[n]; // equattion_node , symbol_length pair
    n %= equation_ref.size();
    m %= equation.second;
    auto symbol = equation.first;
    for (int i = 0; i < m; i++) {
        symbol = symbol->next_sibling();
    }
    return {atof(symbol->first_attribute("x")->value()), atof(symbol->first_attribute("y")->value())};
}

std::string SVGEq::shift_equation(int n, const glm::vec2 &point) {
    // // for (int i = 0; i < 10; i++) {
    // //     std::string tmp = paths[];
    // //     process_it(())
    // // }
    // return
    // process_it(document->first_node("svg")->first_node("defs")->first_node("path")->first_attribute("d")->value(),
    //            {1.f, 2.f});
    // //return "";
    auto n_symbols = equation_ref[n].second;
    std::string tmp;
    for (int i = 0; i < n_symbols; i++) {
        auto symbol = get_SET(n, i);
        glm::vec2 shift = {atof(symbol->first_attribute("x")->value()), atof(symbol->first_attribute("y")->value())};

        auto key = std::string(symbol->first_attribute("xlink:href")->value() + 1);
        auto val = paths[key];
        // std::cout << "Key: " << key << '\t' << "Val: " << val << std::endl;
        tmp += process_it(val.c_str(), shift) + "\n";
    }
    return tmp;
}
SVGEq::SVGEq(rapidxml::xml_document<> &doc, const char *svg_filename, const std::vector<int> &equation_variants,
             int header_len)
    : header_length(header_len) {

    xml_file = read_xml(svg_filename);
    doc.parse<0>((char *)xml_file.c_str());
    document = &doc;
    initialized = true;
    load_paths();
    fill_offsets(0, equation_variants);
    // symbol_per_equation = estimate_stride();

    // std::cout << "stride: " << symbol_per_equation << std::endl;
    std::cout << "number of equations: " << equation_ref.size() << std::endl;
}

glm::vec2 SVGEq::read_shift(rapidxml::xml_node<> *symbol) {
    return {atof(symbol->first_attribute("x")->value()), atof(symbol->first_attribute("y")->value())};
}
float SVGEq::getWidth() {
    auto width_node = document->first_node("svg")->first_attribute("width");
    if (width_node != 0)
        return atof(width_node->value());
    return 0;
}
// int symbols_per_line() const {
//     return symbol_per_equation;
// }
int SVGEq::number_of_equations() const {
    return equation_ref.size();
}
std::string SVGEq::getEq(int n, const glm::vec2 &point) {

    std::string tmp;
    auto main_symbol = get_SET(n, 0);
    auto n_symbols = equation_ref[n].second;
    for (int i = 0; i < n_symbols; i++) {
        auto symbol = get_SET(n, i);

        glm::vec2 shift = read_shift(symbol) - read_shift(main_symbol);

        auto key = std::string(symbol->first_attribute("xlink:href")->value() + 1);
        auto val = paths[key];
        // std::cout << "Key: " << key << '\t' << "Val: " << val << std::endl;
        tmp += process_it(val.c_str(), shift) + "\n";
    }
    return tmp;
}

} // namespace SVGEquation
} // namespace Cycloid
