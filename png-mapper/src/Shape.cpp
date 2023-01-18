// Created by adam slay on 11/28/22.
#include <bit>
#include <iostream>
#include "Shape.h"

using std::cout;
using std::endl;

Shape::Shape(const std::string &fn) {
    filename = fn;
    input = std::ifstream(filename, std::ios::in|std::ios::binary);
    input.read((char*)&fcode, 4);
    input.seekg(24, std::ios::beg);  // bytes between 3 and 24 are unused, per documentation
    input.read((char*)&flen, 4);  // file length in 16-bit words(2 bytes)
    input.read((char*)&version, 4);
    input.read((char*)&shp_type, 4);
    input.read((char*)&minx, 8);
    input.read((char*)&miny, 8);
    input.read((char*)&maxx, 8);
    input.read((char*)&maxy, 8);
    input.read((char*)&minz, 8);
    input.read((char*)&maxz, 8);
    input.read((char*)&minm, 8);
    input.read((char*)&maxm, 8);
    after_header = 100;  // byte at which the first record starts
}

void Shape::primary_header() const {
    cout << "---Primary Header---" << endl;
    cout << "file code: " << fcode << endl;
    cout << "file length: " << flen << endl;
    cout << "version: " << version << endl;
    cout << "shp type: " << shp_type << endl;
    cout << "minx: " << minx << endl;
    cout << "miny: " << miny << endl;
    cout << "maxx: " << maxx << endl;
    cout << "maxy: " << maxy << endl;
    cout << "minz: " << minz << endl;
    cout << "maxz: " << maxz << endl;
    cout << "minm: " << minm << endl;
    cout << "maxm: " << maxm << endl;
}

void Shape::rec_header() {
    input.read((char*)&recnum, 4);
    input.read((char*)&reclen, 4);
    input.read((char*)&rectype, 4);
    input.read((char*)&mbr1, 8);
    input.read((char*)&mbr2, 8);
    input.read((char*)&mbr3, 8);
    input.read((char*)&mbr4, 8);
    input.read((char*)&numparts, 4);
    input.read((char*)&numpoints, 4);
    for (int i = 1; i <= numparts; i++) {
        input.read((char*)&part, 4);
    }
}

std::vector<std::vector<std::vector<double>>> Shape::get_shapes() {
    std::vector<std::vector<std::vector<double>>> shapes;
    input.seekg(after_header,std::ios::beg);
    while (input.tellg() != -1) {
        rec_header();
        if (input.tellg() == -1){
            break;
        }
        std::vector<std::vector<double>> shp;
        while(numpoints){
            std::vector<double> pair;
            for (int i = 0; i < 2; i++) {
                double point;
                input.read((char*)&point, 8);
                pair.push_back(point);
            }
            shp.push_back(pair);
            numpoints--;
        }
        shapes.push_back(shp);
    }
    return shapes;
}

Shape::~Shape() {
    input.close();
}
