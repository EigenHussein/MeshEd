#ifndef SVG_PARSER__HH
#define SVG_PARSER__HH
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <iterator>
#include <memory.h>
#include <rapidxml.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Cycloid {
namespace SVGEquation {

static bool is_char(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
static bool is_number_related(char c) {
    return (c == '.' || c == '-' || c == '+' /* || c == ' ' */);
}
static bool is_number(char c) {
    return c >= '0' && c <= '9';
}

/// @brief Read text file (i.e. XML) line by line
/// @param filename File's path
/// @return A string contain the entire file
std::string read_xml(const char *filename);

/// @brief Read XML and generate SVG command with specific shift
class SVGEq {
  private:
    // loaded XML file
    std::string xml_file;
    // Document Parser
    rapidxml::xml_document<> *document;
    // hash map for every unique path used to form all equations
    std::unordered_map<std::string, std::string> paths;
    // a pair of xlm_node point to the equation pair with the number of symbol in that equation
    std::vector<std::pair<rapidxml::xml_node<> *, int>> equation_ref;
    // number of symbols used in every formula
    // int symbol_per_equation;
    // read transform matrix from xml
    std::vector<double> tarns_mat;
    // header length {contains the main symbols and and the variables}
    int header_length;
    // header nodes
    std::vector<rapidxml::xml_node<> *> header_nodes;
    bool initialized;

    /// @brief Load all the absolute path in key,value pairs (Fills unordered_map member named "paths")
    void load_paths();

    /// @brief Count the number of symbols ("use" node) until certain attribute repeats 
    /// @param use XML node pointer to the reference node
    /// @param attrib The name of considered attribute
    /// @return return the number of nodes until the specific attribute repeats
    int get_stride(rapidxml::xml_node<> *use, const char *attrib);

    /// @brief Gets XML node following the reference
    /// @param use Reference node
    /// @param n Skips
    /// @return return the node after n skips
    rapidxml::xml_node<> *next_node(rapidxml::xml_node<> *use, int n);

    /// @brief Read XML nodes of each equations with the symbol count ( Fills "equation_ref" pairs )
    /// @param mode Always 0
    /// @param eqn_variants Number of variation (different string formats) used to generate equation lines in .tex file
    void fill_offsets(int mode = 0, const std::vector<int> &eqn_variants = {125, 125});

    /// @brief Process a single symbol and shift it
    /// @param str Symbol's PostScript 
    /// @param xy X-Y shift
    /// @param replace 
    /// @return Processed PostScript ( Shifted symbol )
    std::string process_it(const char *str, const glm::vec2 &xy, bool replace = false);

  public:

    /// @brief Get "set" XML node
    /// @param n Equation's index (modulus)
    /// @param m Symbol's index (modulus)
    /// @return An xml_node* pointer that points to the specified signal 
    rapidxml::xml_node<> *get_SET(int n, int m);

    /// @brief Get location of a symbol (in PostScript's .pt)
    /// @param n Equation's index (modulus)
    /// @param m Symbol's index (modulus)
    /// @return A 2D vector with the location ( in pt )
    glm::vec2 get_SET_location(int n, int m);

    /// @brief Shift an equation
    /// @param n Equation's index 
    /// @param point Shift vector
    /// @return A PostScript for the entire shifted equation
    std::string shift_equation(int n, const glm::vec2 &point);

    /// @brief SVG to Equation
    /// @param doc xml_document class
    /// @param svg_filename .svg filename
    /// @param equation_variants Number of equations in every variant
    /// @param header_len if there is autogen header
    SVGEq(rapidxml::xml_document<> &doc, const char *svg_filename,
          const std::vector<int> &equation_variants = {125, 125}, int header_len = 0);

    /// @brief Read in-file XY shift of a specific "set" symbol node
    /// @param symbol A pointer to the symbol
    /// @return A 2D vector of the shift
    glm::vec2 read_shift(rapidxml::xml_node<> *symbol);

    /// @brief Get in-file width (in pt.) of the view, necessary for normalization
    /// @return The width
    float getWidth();

    /// @brief Read the size of equation node references
    /// @return the number of equation nodes (numbe rof equations)
    int number_of_equations() const;

    /// @brief Generate the n-th equation - Close to "shift_equation" member function
    /// @param n Equation's index
    /// @param point Shift point
    /// @return A PostScript for the entire shifted equation
    std::string getEq(int n, const glm::vec2 &point);
    // std::srting getEq2()
};

} // namespace SVGEquation
} // namespace Cycloid

#endif