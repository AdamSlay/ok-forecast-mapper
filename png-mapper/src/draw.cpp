#include <cmath>
#include <string>
#include <vector>

#include "pngwriter.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "draw.h"
#include "utils.h"
#include "Shape.h"

const std::string MAPPER_PATH_PREFIX {"/home/png-user/mapper"};

void render_data(const std::vector<std::vector<std::string>>& csv, pngwriter& image, const bool isTemp) {
    /**
     * Render a blended circle at (X,Y) position of each data_point. The color of the circle corresponds to it's colormap value
     * @csv: the csv datafile used to make this plot(either temp data or wind data)
     * @image: PNGwriter image object
     * @isTemp: is it the temp csv? else, its the wind csv
     */
    int color_map_min {};
    int color_map_max {};
    int circle_radius {30};
    double circle_opacity {0.5};

    for (const auto& line: csv) {
        int X{},Y{};
        if (isTemp) {
            X = getX(std::stod(line[2]));
            Y = 480 - getY(std::stod(line[1]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
            color_map_min = -30;
            color_map_max = 130;
        }
        else {
            X = getX(std::stod(line[3]));
            Y = 480 - getY(std::stod(line[2]));
            color_map_min = -30;
            color_map_max = 80;
        }

        int data_point {std::stoi(line[0])};
        if (data_point != 1000){  // 1000 represents invalid response
            int color {map_value(data_point, color_map_min, color_map_max)};
            auto r {static_cast<double>(red[color])};
            auto g {static_cast<double>(green[color])};
            auto b {static_cast<double>(blue[color])};
            image.filledcircle_blend(X,Y,circle_radius,circle_opacity,r,g,b);
        }
    }
}

void render_shapes(const std::vector<std::vector<Coordinate>>& shapes, pngwriter& image) {
    /**
     * Render Shape outline via iterating through each shape in shapes and drawing a line from previous point to next point
     * Uses getX() and getY() to convert (lat,lon) coordinate into (X,Y) coordinate that fits into image frame size
     * @shapes: struct from Shapes object
     * @image: PNGwriter image object
     */
    for (auto shape: shapes){
        int x1{},y1{},x2{},y2{};
        for (int i = 0; i < shape.size() - 1; ++i){
            const Coordinate& pair1 {shape.at(i)};
            const Coordinate& pair2 {shape.at(i+1)};
            x1 = getX(pair1.latitude);
            y1 = 480 - getY(pair1.longitude);
            x2 = getX(pair2.latitude);
            y2 = 480 - getY(pair2.longitude);
            image.line(x1,y1,x2,y2,0.3,0.3,0.3);
        }
    }
}

void render_text(const std::vector<std::vector<std::string>>& csv, pngwriter& image, const bool isTemp) {
    /**
     * Render text value of the data_point at the corresponding (X,Y) coordinate
     * @csv: the csv datafile used to make this plot(either temp data or wind data)
     * @image: PNGwriter image object
     * @isTemp: is it the temp csv? else, it's the wind csv-
     */
    int X{},Y{};
    char* fontf {const_cast<char*>("/home/png-user/mapper/data/fonts/Nexa-Light.ttf")};  // using const_cast because plot_text() won't accept const char*
    int circle_radius {3};  // PNGwriter doesn't support const args
    int font_size {10};  // PNGwriter doesn't support const args
    double font_angle {0};  // PNGwriter doesn't support const args

    for (const auto& line: csv) {
        if (isTemp) {
            X = getX(std::stod(line[2]));
            Y = 480 - getY(std::stod(line[1]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
        }
        else {
            X = getX(std::stod(line[3]));
            Y = 480 - getY(std::stod(line[2]));
        }
        char* data_point = const_cast<char *>(line[0].c_str());
        if (line[0] != "1000"){  // 1000 represents invalid response
            image.plot_text(fontf, font_size, X, Y, font_angle, data_point, 0.0, 0.0, 0.0);
        }
        else {
            image.filledcircle(X,Y,circle_radius,1.0,0.0,0.0);
        }
    }
}

void draw_temp(const std::vector<std::vector<Coordinate>>& shapes, const std::vector<std::vector<std::string>>& temp, const std::string path, const std::string file){
    /**
     * Draw the temperature map and save it to the host machine as a .png file
     * @shapes: struct from Shape object
     * @temp: temp csv file
     * @path: path to endpoint on host machine
     * @file: filename at end of path
     */
    spdlog::info("Starting draw_temp()");
    std::string tempPath {"/home/png-user/mapper/base/images/maps/tair/" + path + file + ".png"};
    char* p = const_cast<char *>(tempPath.c_str());
    pngwriter image(720,480,1.0,p);

    render_data(temp, image, true);
    render_shapes(shapes, image);
    render_text(temp,image,true);

    image.close();
}

void draw_wind(const std::vector<std::vector<Coordinate>>& shapes, const std::vector<std::vector<std::string>>& wind, const std::string path, const std::string file){
    /**
     * Draw the wind map and save it to the host machine as a .png file
     * @shapes: struct from Shape object
     * @wind: wind csv file
     * @path: path to endpoint on host machine
     * @file: filename at end of path
     */
    spdlog::info("Starting draw_wind()");
    std::string tempPath {MAPPER_PATH_PREFIX + "/base/images/maps/wspd/" + path + file + ".png"};
    char* p = const_cast<char *>(tempPath.c_str());
    pngwriter image(720,480,1.0,p);

    render_data(wind, image, false);
    render_shapes(shapes, image);
    render_text(wind,image,false);

    image.close();
}

