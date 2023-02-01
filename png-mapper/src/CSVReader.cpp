#include "CSVReader.h"


std::vector<std::vector<std::string>> CSVReader::read_csv(const std::string& fn){
    /**
     * This is the core CSV reader. It reads the csv as a byte stream and parses each row into a vector of string values
     * Each row is then stored in an enclosing vector, creating the vector<vector<string>> structure
     */
    std::ifstream file(fn);
    std::vector<std::vector<std::string>> data;
    std::string line;

    // read the first line, but don't input it into data because it just contains the fields.
    std::getline(file, line);

    // now we can iterate through the file and push_back all the data
    while(std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::vector<std::string> fields;
        std::string field;

        while(std::getline(lineStream, field, ',')) {
            fields.push_back(field);
        }
        data.push_back(fields);
    }
    return data;
}

std::vector<std::vector<std::string>> CSVReader::get_temp(const std::vector<std::vector<std::string>> &data){
    /**
     * Takes the vector<vector<string>> structure produced by read_csv() and returns only the Temperature values
     */
    std::vector<std::vector<std::string>> temp;
    for (const std::vector<std::string> &line: data) {
        std::vector<std::string> vals;
        vals.push_back(line[0]);
        vals.push_back(line[3]);
        vals.push_back(line[4]);
        temp.push_back(vals);
    }
    return temp;
}

std::vector<std::vector<std::string>> CSVReader::get_wind(const std::vector<std::vector<std::string>> &data){
    /**
     * Takes the vector<vector<string>> structure produced by read_csv() and returns only the WindSpeed values
     */
    std::vector<std::vector<std::string>> wind;
    for (const std::vector<std::string> &line: data) {
        std::vector<std::string> vals;
        vals.push_back(line[1]);
        vals.push_back(line[2]);
        vals.push_back(line[3]);
        vals.push_back(line[4]);
        wind.push_back(vals);
    }
    return wind;
}
