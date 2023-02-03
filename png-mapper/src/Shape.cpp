#include "Shape.h"

using std::cout;
using std::endl;


Shape::Shape(const std::string fn) {
    /**
     * Data structure for rendering .shp files without the other .shp format files(.shx, .dbf)
     */
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

Shape::~Shape() {
    /**
     * Close the byte stream
     */
    input.close();
}

void Shape::primary_header() const {
    /**
     * Prints the primary header for the .shp file
     */
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
    /**
     * Record Header. Reads the header info for each record header in the .shp file
     */
    input.read((char*)&recnum, 4);
    input.read((char*)&reclen, 4);
    input.read((char*)&rectype, 4);
    input.read((char*)&mbr1, 8);
    input.read((char*)&mbr2, 8);
    input.read((char*)&mbr3, 8);
    input.read((char*)&mbr4, 8);
    input.read((char*)&numparts, 4);
    input.read((char*)&numpoints, 4);
    for (int i = 1; i <= numparts; ++i) {
        input.read((char*)&part, 4);
    }
}

std::vector<std::vector<Coordinate>> Shape::get_shapes() {
    /**
    * This is the core SHP reader. It reads the .shp file as a byte stream and parses each shape into a vector of coordinates
    * Each shape is then stored in an enclosing vector, creating the vector<vector<Coordinate>> structure
    */
    std::vector<std::vector<Coordinate>> shapes {};
    input.seekg(after_header,std::ios::beg);
    while (input.tellg() != -1) {
        rec_header();
        if (input.tellg() == -1){
            break;
        }
        std::vector<Coordinate> shp {};
        while(numpoints){
            Coordinate pair {};
            input.read((char*)&pair.latitude, 8);
            input.read((char*)&pair.longitude, 8);
            shp.push_back(pair);
            --numpoints;
        }
        shapes.push_back(shp);
    }
    return shapes;
}
