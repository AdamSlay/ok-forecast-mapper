#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#ifndef PNG_MAPPER_EXE_CSVREADER_H
#define PNG_MAPPER_EXE_CSVREADER_H


class CSVReader {
public:
    static std::vector<std::vector<std::string>> read_csv(const std::string &fn);

    static std::vector<std::vector<std::string>> get_temp(const std::vector<std::vector<std::string>> &data);

    static std::vector<std::vector<std::string>> get_wind(const std::vector<std::vector<std::string>> &data);
};

#endif //PNG_MAPPER_EXE_CSVREADER_H
