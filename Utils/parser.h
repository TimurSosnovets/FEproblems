#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <regex>
#include <tuple>
#include <vector>
#include <string>
#include "../Plates/Structs.h"

class Parser {
public:
    static void parse_file(const std::string& filename, LBC& LBC);

private:
    static bool parse_fix_x(const std::string& line, std::tuple<int, bool, bool>& result);
    static bool parse_fix_y(const std::string& line, std::tuple<int, bool, bool>& result);
    static bool parse_load_x(const std::string& line, std::tuple<int, double, double>& result);
    static bool parse_load_y(const std::string& line, std::tuple<int, double, double>& result);
};

#endif // PARSER_H
