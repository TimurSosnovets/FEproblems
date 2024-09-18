#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <regex>
#include <tuple>
#include <vector>
#include <string>

class Parser {
public:
    static void parse_file(const std::string& filename,
                           std::vector<std::tuple<int, bool, bool>>& LBC_dof,
                           std::vector<std::tuple<int, double, double>>& LBC_force);

private:
    static bool parse_fix_x(const std::string& line, std::tuple<int, bool, bool>& result);
    static bool parse_fix_y(const std::string& line, std::tuple<int, bool, bool>& result);
    static bool parse_load_x(const std::string& line, std::tuple<int, double, double>& result);
    static bool parse_load_y(const std::string& line, std::tuple<int, double, double>& result);
};

#endif // PARSER_H
