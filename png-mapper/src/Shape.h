// Created by adam slay on 11/28/22.
#include <string>
#include <vector>
#include <fstream>

#ifndef SHP_PARSER_SHAPE_H
#define SHP_PARSER_SHAPE_H

class Shape {
private:
        std::string filename;
        std::ifstream input;
        // ---Primary Header---
        unsigned int fcode{};  // 0-3
        unsigned int flen{};  // 24-27
        unsigned int version{};  // 28-31
        unsigned int shp_type{};  // 32-35
        double minx{};  // 36-67
        double miny{};  // 36-67
        double maxx{};  // 36-67
        double maxy{};  // 36-67
        double minz{};  // 68-83
        double maxz{};  // 68-83
        double minm{};  // 84-99
        double maxm{};  // 84-99
        unsigned int after_header;
        // ---Record Header---
        unsigned int recnum{};
        unsigned int reclen{};
        unsigned int rectype{};
        double mbr1{};
        double mbr2{};
        double mbr3{};
        double mbr4{};
        unsigned int numparts{};
        unsigned int numpoints{};
        unsigned int part{};
public:
    explicit Shape(const std::string &fn);
    ~Shape();
    void primary_header() const;
    void rec_header();
    std::vector<std::vector<std::vector<double>>> get_shapes();
};

#endif //SHP_PARSER_SHAPE_H
