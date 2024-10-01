#include <iostream>
#include "../../headers/mechanical/parser.hpp"

void Parser::parse_file(const std::string& filename, LBC& LBC) {
    std::ifstream file(filename);
    std::string line;    

    while (std::getline(file, line)) {
        std::tuple<int, bool, bool> dof_entry;
        std::tuple<int, double, double> force_entry;
        std::pair<int, double> nodal_temp_entry;
        std::pair<int, double> element_temp_entry;

        if (parse_fix_x(line, dof_entry) || parse_fix_y(line, dof_entry)) {
            LBC.DOF.push_back(dof_entry);
        } else if (parse_load_x(line, force_entry) || parse_load_y(line, force_entry)) {
            LBC.Forces.push_back(force_entry);
        } else if (parse_nodal_temp(line, nodal_temp_entry)) {
            LBC.Nodal_Temp.push_back(nodal_temp_entry);
        } else if (parse_element_temp(line, element_temp_entry)) {
            LBC.Element_Temp.push_back(element_temp_entry);
        }
    }
    std::cout << std::endl << "Data from file " << filename << " parsed successfully!" << std::endl;
}

bool Parser::parse_fix_x(const std::string& line, std::tuple<int, bool, bool>& result) {
    std::regex fix_x_re(R"(Fix_x (\d+))");
    std::smatch match;
    
    if (std::regex_match(line, match, fix_x_re)) {
        int node = std::stoi(match[1]);
        result = std::make_tuple(node, true, false);
        return true;
    }
    return false;
}

bool Parser::parse_fix_y(const std::string& line, std::tuple<int, bool, bool>& result) {
    std::regex fix_y_re(R"(Fix_y (\d+))");
    std::smatch match;

    if (std::regex_match(line, match, fix_y_re)) {
        int node = std::stoi(match[1]);
        result = std::make_tuple(node, false, true);
        return true;
    }
    return false;
}

bool Parser::parse_load_x(const std::string& line, std::tuple<int, double, double>& result) {
    std::regex load_x_re(R"(Load_x (\d+) (-?\d+(\.\d+)?))");
    std::smatch match;

    if (std::regex_match(line, match, load_x_re)) {
        int node = std::stoi(match[1]);
        double value = std::stod(match[2]);
        result = std::make_tuple(node, value, 0.0);
        return true;
    }
    return false;
}

bool Parser::parse_load_y(const std::string& line, std::tuple<int, double, double>& result) {
    std::regex load_y_re(R"(Load_y (\d+) (-?\d+(\.\d+)?))");
    std::smatch match;

    if (std::regex_match(line, match, load_y_re)) {
        int node = std::stoi(match[1]);
        double value = std::stod(match[2]);
        result = std::make_tuple(node, 0.0, value);
        return true;
    }
    return false;
}

bool Parser::parse_nodal_temp(const std::string& line, std::pair<int, double>& result) {
    std::regex nodal_temp(R"(Node_temp (\d+) (-?\d+(\.\d+)?))");
    std::smatch match;

    if (std::regex_match(line, match, nodal_temp)) {
        int node = std::stoi(match[1]);
        double value = std::stod(match[2]);
        result = std::pair(node, value);
        return true;
    }
    return false;
}

bool Parser::parse_element_temp(const std::string& line, std::pair<int, double>& result) {
    std::regex element_temp(R"(Element_temp (\d+) (-?\d+(\.\d+)?))");
    std::smatch match;

    if (std::regex_match(line, match, element_temp)) {
        int node = std::stoi(match[1]);
        double value = std::stod(match[2]);
        result = std::pair(node, value);
        return true;
    }
    return false;
}