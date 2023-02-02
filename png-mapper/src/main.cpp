#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "pngwriter.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "CSVReader.h"
#include "Shape.h"

// RGB Colormap values between 0.0-1.0
static const float red[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00588235294117645f,0.02156862745098032f,0.03725490196078418f,0.05294117647058827f,0.06862745098039214f,0.084313725490196f,0.1000000000000001f,0.115686274509804f,0.1313725490196078f,0.1470588235294117f,0.1627450980392156f,0.1784313725490196f,0.1941176470588235f,0.2098039215686274f,0.2254901960784315f,0.2411764705882353f,0.2568627450980392f,0.2725490196078431f,0.2882352941176469f,0.303921568627451f,0.3196078431372549f,0.3352941176470587f,0.3509803921568628f,0.3666666666666667f,0.3823529411764706f,0.3980392156862744f,0.4137254901960783f,0.4294117647058824f,0.4450980392156862f,0.4607843137254901f,0.4764705882352942f,0.4921568627450981f,0.5078431372549019f,0.5235294117647058f,0.5392156862745097f,0.5549019607843135f,0.5705882352941174f,0.5862745098039217f,0.6019607843137256f,0.6176470588235294f,0.6333333333333333f,0.6490196078431372f,0.664705882352941f,0.6803921568627449f,0.6960784313725492f,0.7117647058823531f,0.7274509803921569f,0.7431372549019608f,0.7588235294117647f,0.7745098039215685f,0.7901960784313724f,0.8058823529411763f,0.8215686274509801f,0.8372549019607844f,0.8529411764705883f,0.8686274509803922f,0.884313725490196f,0.8999999999999999f,0.9156862745098038f,0.9313725490196076f,0.947058823529412f,0.9627450980392158f,0.9784313725490197f,0.9941176470588236f,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9862745098039216f,0.9705882352941178f,0.9549019607843139f,0.93921568627451f,0.9235294117647062f,0.9078431372549018f,0.892156862745098f,0.8764705882352941f,0.8607843137254902f,0.8450980392156864f,0.8294117647058825f,0.8137254901960786f,0.7980392156862743f,0.7823529411764705f,0.7666666666666666f,0.7509803921568627f,0.7352941176470589f,0.719607843137255f,0.7039215686274511f,0.6882352941176473f,0.6725490196078434f,0.6568627450980391f,0.6411764705882352f,0.6254901960784314f,0.6098039215686275f,0.5941176470588236f,0.5784313725490198f,0.5627450980392159f,0.5470588235294116f,0.5313725490196077f,0.5156862745098039f,0.5f};
static const float green[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.001960784313725483f,0.01764705882352935f,0.03333333333333333f,0.0490196078431373f,0.06470588235294117f,0.08039215686274503f,0.09607843137254901f,0.111764705882353f,0.1274509803921569f,0.1431372549019607f,0.1588235294117647f,0.1745098039215687f,0.1901960784313725f,0.2058823529411764f,0.2215686274509804f,0.2372549019607844f,0.2529411764705882f,0.2686274509803921f,0.2843137254901961f,0.3f,0.3156862745098039f,0.3313725490196078f,0.3470588235294118f,0.3627450980392157f,0.3784313725490196f,0.3941176470588235f,0.4098039215686274f,0.4254901960784314f,0.4411764705882353f,0.4568627450980391f,0.4725490196078431f,0.4882352941176471f,0.503921568627451f,0.5196078431372548f,0.5352941176470587f,0.5509803921568628f,0.5666666666666667f,0.5823529411764705f,0.5980392156862746f,0.6137254901960785f,0.6294117647058823f,0.6450980392156862f,0.6607843137254901f,0.6764705882352942f,0.692156862745098f,0.7078431372549019f,0.723529411764706f,0.7392156862745098f,0.7549019607843137f,0.7705882352941176f,0.7862745098039214f,0.8019607843137255f,0.8176470588235294f,0.8333333333333333f,0.8490196078431373f,0.8647058823529412f,0.8803921568627451f,0.8960784313725489f,0.9117647058823528f,0.9274509803921569f,0.9431372549019608f,0.9588235294117646f,0.9745098039215687f,0.9901960784313726f,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9901960784313726f,0.9745098039215687f,0.9588235294117649f,0.943137254901961f,0.9274509803921571f,0.9117647058823528f,0.8960784313725489f,0.8803921568627451f,0.8647058823529412f,0.8490196078431373f,0.8333333333333335f,0.8176470588235296f,0.8019607843137253f,0.7862745098039214f,0.7705882352941176f,0.7549019607843137f,0.7392156862745098f,0.723529411764706f,0.7078431372549021f,0.6921568627450982f,0.6764705882352944f,0.6607843137254901f,0.6450980392156862f,0.6294117647058823f,0.6137254901960785f,0.5980392156862746f,0.5823529411764707f,0.5666666666666669f,0.5509803921568626f,0.5352941176470587f,0.5196078431372548f,0.503921568627451f,0.4882352941176471f,0.4725490196078432f,0.4568627450980394f,0.4411764705882355f,0.4254901960784316f,0.4098039215686273f,0.3941176470588235f,0.3784313725490196f,0.3627450980392157f,0.3470588235294119f,0.331372549019608f,0.3156862745098041f,0.2999999999999998f,0.284313725490196f,0.2686274509803921f,0.2529411764705882f,0.2372549019607844f,0.2215686274509805f,0.2058823529411766f,0.1901960784313728f,0.1745098039215689f,0.1588235294117646f,0.1431372549019607f,0.1274509803921569f,0.111764705882353f,0.09607843137254912f,0.08039215686274526f,0.06470588235294139f,0.04901960784313708f,0.03333333333333321f,0.01764705882352935f,0.001960784313725483f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const float blue[] = {0.5f,0.5156862745098039f,0.5313725490196078f,0.5470588235294118f,0.5627450980392157f,0.5784313725490196f,0.5941176470588235f,0.6098039215686275f,0.6254901960784314f,0.6411764705882352f,0.6568627450980392f,0.6725490196078432f,0.6882352941176471f,0.7039215686274509f,0.7196078431372549f,0.7352941176470589f,0.7509803921568627f,0.7666666666666666f,0.7823529411764706f,0.7980392156862746f,0.8137254901960784f,0.8294117647058823f,0.8450980392156863f,0.8607843137254902f,0.8764705882352941f,0.892156862745098f,0.907843137254902f,0.9235294117647059f,0.9392156862745098f,0.9549019607843137f,0.9705882352941176f,0.9862745098039216f,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9941176470588236f,0.9784313725490197f,0.9627450980392158f,0.9470588235294117f,0.9313725490196079f,0.915686274509804f,0.8999999999999999f,0.884313725490196f,0.8686274509803922f,0.8529411764705883f,0.8372549019607844f,0.8215686274509804f,0.8058823529411765f,0.7901960784313726f,0.7745098039215685f,0.7588235294117647f,0.7431372549019608f,0.7274509803921569f,0.7117647058823531f,0.696078431372549f,0.6803921568627451f,0.6647058823529413f,0.6490196078431372f,0.6333333333333333f,0.6176470588235294f,0.6019607843137256f,0.5862745098039217f,0.5705882352941176f,0.5549019607843138f,0.5392156862745099f,0.5235294117647058f,0.5078431372549019f,0.4921568627450981f,0.4764705882352942f,0.4607843137254903f,0.4450980392156865f,0.4294117647058826f,0.4137254901960783f,0.3980392156862744f,0.3823529411764706f,0.3666666666666667f,0.3509803921568628f,0.335294117647059f,0.3196078431372551f,0.3039215686274508f,0.2882352941176469f,0.2725490196078431f,0.2568627450980392f,0.2411764705882353f,0.2254901960784315f,0.2098039215686276f,0.1941176470588237f,0.1784313725490199f,0.1627450980392156f,0.1470588235294117f,0.1313725490196078f,0.115686274509804f,0.1000000000000001f,0.08431372549019622f,0.06862745098039236f,0.05294117647058805f,0.03725490196078418f,0.02156862745098032f,0.00588235294117645f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void init_logger(const std::string log_fn) {
    /**
     * Initialize logger and set logging pattern
     * log_fn: the filename for the current day's logfile
     */
    std::string full_path {"/base/images/logs/" + log_fn + ".log"};
    auto logger = spdlog::basic_logger_mt("png-mapper: src.main", full_path);
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S]  [%n]  [%L]  %v");
}

void make_dirs(const std::string path) {
    /**
     * Make the required directory paths for temp and wind maps on the host machine
     */
    std::string tempPath {"/base/images/maps/temp/" + path};
    std::string windPath {"/base/images/maps/wind/" + path};

    std::filesystem::path fs_temp {tempPath};
    std::filesystem::path fs_wind {windPath};

    std::filesystem::create_directories(fs_temp);
    std::filesystem::create_directories(fs_wind);
}

int getX(const double lon){
    /**
     * Convert Longitude into X coordinate. The conversion equations I used can be found here:
     * https://stackoverflow.com/questions/4953150/convert-lat-longs-to-x-y-co-ordinates
     */
    const int width {23040};  // the conversion from lat/lon to x/y is tiny, so this is scaled up by a factor of 32
    const double x { fmod((width*(180 + lon)/360), (width + (width/2))) };
    return (int)round(x) - 4850;  // shift transformation to make it fit within the frame
}

int getY(const double lat){
    /**
     * Convert Latitude into Y coordinate. The conversion equations I used can be found here:
     * https://stackoverflow.com/questions/4953150/convert-lat-longs-to-x-y-co-ordinates
     */
    const double width {23040};  // the conversion from lat/lon to x/y is tiny, so this is scaled up by a factor of 32
    const double height {15360};  // the conversion from lat/lon to x/y is tiny, so this is scaled up by a factor of 32
    const double PI {3.14159265359};  // sufficient approximation for this purpose
    const double latRad { (lat * PI) / 180 };
    const double mercN { log(tan((PI / 4) + (latRad / 2))) };
    const double y { (height / 2) - (width * mercN / (2 * PI)) };

    return (int)round(y) - 5000;  // shift transformation to make it fit within the frame
}

int map_value(int value, const int min, const int max){
    /**
     * Find the RGB colormap value for the given temp/wind with it's corresponding min/max values
     */
    value = std::clamp(value, min, max);
    return (value-min) * 255 / (max - min);
}

void render_data(const auto& csv , pngwriter& image, const bool isTemp) {
    /**
     * Render a blended circle at (X,Y) position of each data_point. The color of the circle corresponds to it's colormap value
     * csv: the csv datafile used to make this plot(either temp data or wind data)
     * image: pngwriter image object
     * isTemp: is it the temp csv? else, its the wind csv
     */
    int color_map_min {};
    int circle_radius {30};
    double circle_opacity {0.5};
    for (const auto& line: csv) {
        int X{},Y{};
        if (isTemp) {
            X = getX(std::stod(line[2]));
            Y = 480 - getY(std::stod(line[1]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
            color_map_min = -30;
        }
        else {
            X = getX(std::stod(line[3]));
            Y = 480 - getY(std::stod(line[2]));
            color_map_min = -80;
        }

        int data_point {std::stoi(line[0])};
        if (data_point != 1000){  // 1000 represents invalid response
            int color {map_value(data_point, color_map_min, 130)};
            auto r {static_cast<double>(red[color])};
            auto g {static_cast<double>(green[color])};
            auto b {static_cast<double>(blue[color])};
            image.filledcircle_blend(X,Y,circle_radius,circle_opacity,r,g,b);
        }
    }
}

void render_shapes(const auto& shapes, pngwriter& image) {
    /**
     * Render Shape outline via iterating through each shape in shapes and drawing a line from previous point to next point
     * Uses getX() and getY() to convert (lat,lon) coordinate into (X,Y) coordinate that fits into image frame size
     * shapes: struct from Shapes object
     * image: pngwriter image object
     */
    for (auto shape: shapes){
        int x1{},y1{},x2{},y2{};
        for (int i = 0; i < shape.size() - 1; ++i){
            const Coordinate &pair1 {shape.at(i)};
            const Coordinate &pair2 {shape.at(i+1)};
            x1 = getX(pair1.latitude);
            y1 = 480 - getY(pair1.longitude);
            x2 = getX(pair2.latitude);
            y2 = 480 - getY(pair2.longitude);
            image.line(x1,y1,x2,y2,0.3,0.3,0.3);
        }
    }
}

void render_text(const auto &csv, pngwriter &image, const bool isTemp) {
    /**
     * Render text value of the data_point at the corresponding (X,Y) coordinate
     * csv: the csv datafile used to make this plot(either temp data or wind data)
     * image: pngwriter image object
     * isTemp: is it the temp csv? else, it's the wind csv
     */
    int X{},Y{};
    char* fontf {const_cast<char*>("/mapper/data/fonts/Nexa-Light.ttf")};  // using const_cast because plot_text() won't accept const char*
    int font_size {10};  // pngwriter doesn't support const args
    double font_angle {0};  // pngwriter doesn't support const args
    int circle_radius {3};  // pngwriter doesn't support const args

    for (const auto& line: csv) {
        if (isTemp) {
            X = getX(std::stod(line[2]));
            Y = 480 - getY(std::stod(line[1]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
        }
        else {
            X = getX(std::stod(line[3]));
            Y = 480 - getY(std::stod(line[2]));
        }
        char *data_point = const_cast<char *>(line[0].c_str());
        if (line[0] != "1000"){  // 1000 represents invalid response
            image.plot_text(fontf, font_size, X, Y, font_angle, data_point, 0.0, 0.0, 0.0);
        }
        else {
            image.filledcircle(X,Y,circle_radius,1.0,0.0,0.0);
        }
    }
}

void draw_temp(const auto& shapes, const auto& temp, const std::string path, const std::string file){
    /**
     * Draw the temperature map and save it to the host machine as a .png file
     * shapes: struct from Shape object
     * temp: temp csv file
     * path: path to endpoint on host machine
     * file: filename at end of path
     */
    spdlog::info("Starting draw_temp()");
    std::string tempPath = "/base/images/maps/temp/" + path + file + ".png";
    char* p = const_cast<char *>(tempPath.c_str());
    pngwriter image(720,480,1.0,p);

    render_data(temp, image, true);
    render_shapes(shapes, image);
    render_text(temp,image,true);

    image.close();
}

void draw_wind(const auto& shapes, const auto& wind, const std::string& path, const std::string& file){
    /**
     * Draw the wind map and save it to the host machine as a .png file
     * shapes: struct from Shape object
     * wind: wind csv file
     * path: path to endpoint on host machine
     * file: filename at end of path
     */
    spdlog::info("Starting draw_wind()");
    std::string tempPath {"/base/images/maps/wind/" + path + file + ".png"};
    char* p = const_cast<char *>(tempPath.c_str());
    pngwriter image(720,480,1.0,p);

    render_data(wind, image, false);
    render_shapes(shapes, image);
    render_text(wind,image,false);

    image.close();
}

int main(int argc, char* argv[]) {
    try {
        // get args and init logger
        std::string path {argv[1]};
        std::string file {argv[2]};
        std::string log_fn {file.substr(0,10)};
        init_logger(log_fn);
        spdlog::info("-----| png-writer started |-----");

        // read csv from shared vol & make dirs on host
        std::string csv_full_path {"/mapper/vol/" + path + file + ".csv"};
        std::vector<std::vector<std::string>> data;
        data = CSVReader::read_csv(csv_full_path);
        make_dirs(path);

        // initialize shps object
        Shape shape("/mapper/data/okcounties.shp");
        auto shps {shape.get_shapes()};

        // draw maps
        auto temp {CSVReader::get_temp(data)};
        auto wind {CSVReader::get_wind(data)};
        draw_temp(shps, temp, path, file);
        draw_wind(shps, wind, path, file);
    }
    catch (std::exception &e) {
        spdlog::critical("There was an Error in png-writer: ");
        spdlog::critical(e.what());
        return 1;
    }
    spdlog::info("-----| png-writer finished |-----");
    return 0;
}
