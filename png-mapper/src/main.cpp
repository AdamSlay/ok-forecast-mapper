#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "pngwriter.h"
#include "spdlog/spdlog.h"

#include "CSVReader.h"
#include "Shape.h"
#include "spdlog/sinks/basic_file_sink.h"

// RGB Colormap values between 0.0-1.0
static const float red[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00588235294117645f,0.02156862745098032f,0.03725490196078418f,0.05294117647058827f,0.06862745098039214f,0.084313725490196f,0.1000000000000001f,0.115686274509804f,0.1313725490196078f,0.1470588235294117f,0.1627450980392156f,0.1784313725490196f,0.1941176470588235f,0.2098039215686274f,0.2254901960784315f,0.2411764705882353f,0.2568627450980392f,0.2725490196078431f,0.2882352941176469f,0.303921568627451f,0.3196078431372549f,0.3352941176470587f,0.3509803921568628f,0.3666666666666667f,0.3823529411764706f,0.3980392156862744f,0.4137254901960783f,0.4294117647058824f,0.4450980392156862f,0.4607843137254901f,0.4764705882352942f,0.4921568627450981f,0.5078431372549019f,0.5235294117647058f,0.5392156862745097f,0.5549019607843135f,0.5705882352941174f,0.5862745098039217f,0.6019607843137256f,0.6176470588235294f,0.6333333333333333f,0.6490196078431372f,0.664705882352941f,0.6803921568627449f,0.6960784313725492f,0.7117647058823531f,0.7274509803921569f,0.7431372549019608f,0.7588235294117647f,0.7745098039215685f,0.7901960784313724f,0.8058823529411763f,0.8215686274509801f,0.8372549019607844f,0.8529411764705883f,0.8686274509803922f,0.884313725490196f,0.8999999999999999f,0.9156862745098038f,0.9313725490196076f,0.947058823529412f,0.9627450980392158f,0.9784313725490197f,0.9941176470588236f,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9862745098039216f,0.9705882352941178f,0.9549019607843139f,0.93921568627451f,0.9235294117647062f,0.9078431372549018f,0.892156862745098f,0.8764705882352941f,0.8607843137254902f,0.8450980392156864f,0.8294117647058825f,0.8137254901960786f,0.7980392156862743f,0.7823529411764705f,0.7666666666666666f,0.7509803921568627f,0.7352941176470589f,0.719607843137255f,0.7039215686274511f,0.6882352941176473f,0.6725490196078434f,0.6568627450980391f,0.6411764705882352f,0.6254901960784314f,0.6098039215686275f,0.5941176470588236f,0.5784313725490198f,0.5627450980392159f,0.5470588235294116f,0.5313725490196077f,0.5156862745098039f,0.5f};
static const float green[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.001960784313725483f,0.01764705882352935f,0.03333333333333333f,0.0490196078431373f,0.06470588235294117f,0.08039215686274503f,0.09607843137254901f,0.111764705882353f,0.1274509803921569f,0.1431372549019607f,0.1588235294117647f,0.1745098039215687f,0.1901960784313725f,0.2058823529411764f,0.2215686274509804f,0.2372549019607844f,0.2529411764705882f,0.2686274509803921f,0.2843137254901961f,0.3f,0.3156862745098039f,0.3313725490196078f,0.3470588235294118f,0.3627450980392157f,0.3784313725490196f,0.3941176470588235f,0.4098039215686274f,0.4254901960784314f,0.4411764705882353f,0.4568627450980391f,0.4725490196078431f,0.4882352941176471f,0.503921568627451f,0.5196078431372548f,0.5352941176470587f,0.5509803921568628f,0.5666666666666667f,0.5823529411764705f,0.5980392156862746f,0.6137254901960785f,0.6294117647058823f,0.6450980392156862f,0.6607843137254901f,0.6764705882352942f,0.692156862745098f,0.7078431372549019f,0.723529411764706f,0.7392156862745098f,0.7549019607843137f,0.7705882352941176f,0.7862745098039214f,0.8019607843137255f,0.8176470588235294f,0.8333333333333333f,0.8490196078431373f,0.8647058823529412f,0.8803921568627451f,0.8960784313725489f,0.9117647058823528f,0.9274509803921569f,0.9431372549019608f,0.9588235294117646f,0.9745098039215687f,0.9901960784313726f,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9901960784313726f,0.9745098039215687f,0.9588235294117649f,0.943137254901961f,0.9274509803921571f,0.9117647058823528f,0.8960784313725489f,0.8803921568627451f,0.8647058823529412f,0.8490196078431373f,0.8333333333333335f,0.8176470588235296f,0.8019607843137253f,0.7862745098039214f,0.7705882352941176f,0.7549019607843137f,0.7392156862745098f,0.723529411764706f,0.7078431372549021f,0.6921568627450982f,0.6764705882352944f,0.6607843137254901f,0.6450980392156862f,0.6294117647058823f,0.6137254901960785f,0.5980392156862746f,0.5823529411764707f,0.5666666666666669f,0.5509803921568626f,0.5352941176470587f,0.5196078431372548f,0.503921568627451f,0.4882352941176471f,0.4725490196078432f,0.4568627450980394f,0.4411764705882355f,0.4254901960784316f,0.4098039215686273f,0.3941176470588235f,0.3784313725490196f,0.3627450980392157f,0.3470588235294119f,0.331372549019608f,0.3156862745098041f,0.2999999999999998f,0.284313725490196f,0.2686274509803921f,0.2529411764705882f,0.2372549019607844f,0.2215686274509805f,0.2058823529411766f,0.1901960784313728f,0.1745098039215689f,0.1588235294117646f,0.1431372549019607f,0.1274509803921569f,0.111764705882353f,0.09607843137254912f,0.08039215686274526f,0.06470588235294139f,0.04901960784313708f,0.03333333333333321f,0.01764705882352935f,0.001960784313725483f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const float blue[] = {0.5f,0.5156862745098039f,0.5313725490196078f,0.5470588235294118f,0.5627450980392157f,0.5784313725490196f,0.5941176470588235f,0.6098039215686275f,0.6254901960784314f,0.6411764705882352f,0.6568627450980392f,0.6725490196078432f,0.6882352941176471f,0.7039215686274509f,0.7196078431372549f,0.7352941176470589f,0.7509803921568627f,0.7666666666666666f,0.7823529411764706f,0.7980392156862746f,0.8137254901960784f,0.8294117647058823f,0.8450980392156863f,0.8607843137254902f,0.8764705882352941f,0.892156862745098f,0.907843137254902f,0.9235294117647059f,0.9392156862745098f,0.9549019607843137f,0.9705882352941176f,0.9862745098039216f,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9941176470588236f,0.9784313725490197f,0.9627450980392158f,0.9470588235294117f,0.9313725490196079f,0.915686274509804f,0.8999999999999999f,0.884313725490196f,0.8686274509803922f,0.8529411764705883f,0.8372549019607844f,0.8215686274509804f,0.8058823529411765f,0.7901960784313726f,0.7745098039215685f,0.7588235294117647f,0.7431372549019608f,0.7274509803921569f,0.7117647058823531f,0.696078431372549f,0.6803921568627451f,0.6647058823529413f,0.6490196078431372f,0.6333333333333333f,0.6176470588235294f,0.6019607843137256f,0.5862745098039217f,0.5705882352941176f,0.5549019607843138f,0.5392156862745099f,0.5235294117647058f,0.5078431372549019f,0.4921568627450981f,0.4764705882352942f,0.4607843137254903f,0.4450980392156865f,0.4294117647058826f,0.4137254901960783f,0.3980392156862744f,0.3823529411764706f,0.3666666666666667f,0.3509803921568628f,0.335294117647059f,0.3196078431372551f,0.3039215686274508f,0.2882352941176469f,0.2725490196078431f,0.2568627450980392f,0.2411764705882353f,0.2254901960784315f,0.2098039215686276f,0.1941176470588237f,0.1784313725490199f,0.1627450980392156f,0.1470588235294117f,0.1313725490196078f,0.115686274509804f,0.1000000000000001f,0.08431372549019622f,0.06862745098039236f,0.05294117647058805f,0.03725490196078418f,0.02156862745098032f,0.00588235294117645f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int getX(const double &lon){
    /**
     * Convert Longitude into X coordinate. The conversion equations I used can be found here:
     * https://stackoverflow.com/questions/4953150/convert-lat-longs-to-x-y-co-ordinates
     */
    const int width = 23040;  // the conversion from lat/lon to x/y is tiny, so this is scaled up by a factor of 32
    const double x = fmod((width*(180 + lon)/360), (width + (width/2)));
    return (int)round(x) - 4850;  // shift transformation to make it fit within the frame
}

int getY(const double &lat){
    /**
     * Convert Latitude into Y coordinate. The conversion equations I used can be found here:
     * https://stackoverflow.com/questions/4953150/convert-lat-longs-to-x-y-co-ordinates
     */
    const double width = 23040;  // the conversion from lat/lon to x/y is tiny, so this is scaled up by a factor of 32
    const double height = 15360;  // the conversion from lat/lon to x/y is tiny, so this is scaled up by a factor of 32
    const double PI = 3.14159265359;  // sufficient approximation for this purpose
    const double latRad = (lat * PI) / 180;
    const double mercN = log(tan((PI / 4) + (latRad / 2)));
    const double y = (height / 2) - (width * mercN / (2 * PI));
    return (int)round(y) - 5000;  // shift transformation to make it fit within the frame
}

int map_value(int value, int min, int max){
    /**
     * Find the RGB colormap value for the given Temp/WindSpeed with it's corresponding min/max values
     */
    value = std::clamp(value, min, max);
    return (value-min) * 255 / (max - min);
}

void make_dirs(const std::string& path) {
    /**
     * Make the required directory paths for temp and wind maps on the host machine
     */
    std::string tempPath = "/base/images/maps/temp/" + path;
    std::string windPath = "/base/images/maps/wind/" + path;

    std::filesystem::path fs_temp = tempPath;
    std::filesystem::path fs_wind = windPath;

    std::filesystem::create_directories(fs_temp);
    std::filesystem::create_directories(fs_wind);
}

void draw_temp(const std::vector<std::vector<std::vector<double>>> &shapes,
               const std::vector<std::vector<std::string>> &temp,
               const std::string& path,
               const std::string& file){
    /**
     * Draw the temperature map and save it to the host machine as a .png file
     */
    std::string tempPath = "/base/images/maps/temp/" + path + file + ".png";
    char* p = const_cast<char *>(tempPath.c_str());
    pngwriter image(720,480,1.0,p);

    // Render blended Circles
    for (const std::vector<std::string> &line: temp) {
        for (const std::string &val: line) {
            int X = getX(std::stod(line[2]));
            int Y = 480 - getY(std::stod(line[1]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
            int temp_int = std::stoi(line[0]);
            if (temp_int != 1000){  // 1000 represents invalid response
                int temp_col = map_value(temp_int, -30, 130);
                auto r = static_cast<double>(red[temp_col]);
                auto g = static_cast<double>(green[temp_col]);
                auto b = static_cast<double>(blue[temp_col]);
                image.filledcircle_blend(X,Y,30,0.5,r,g,b);
            }
        }
    }

    // Render shape outlines
    for (std::vector<std::vector<double>> points: shapes){
        for (int i = 0; i < points.size() - 1; i++){
            std::vector<double> &pair1 = points.at(i);
            std::vector<double> &pair2 = points.at(i+1);
            const int x1 = getX(pair1.front());
            const int y1 = 480 - getY(pair1.back());
            const int x2 = getX(pair2.front());
            const int y2 = 480 - getY(pair2.back());
            image.line(x1,y1,x2,y2,0.3,0.3,0.3);
        }
    }

    // Render text values
    for (const std::vector<std::string> &line: temp) {
        for (const std::string &val: line) {
            char *fontf = const_cast<char *>("/mapper/data/fonts/Nexa-Light.ttf");  // using const_cast because plot_text() won't accept const char*
            int X = getX(std::stod(line[2]));
            int Y = 480 - getY(std::stod(line[1]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
            char *tchar = const_cast<char *>(line[0].c_str());
            if (line[0] != "1000"){  // 1000 represents invalid response
                image.plot_text(fontf, 10, X, Y, 0, tchar, 0.0, 0.0, 0.0);
            }
            else {
                image.filledcircle(X,Y,3,1.0,0.0,0.0);
            }
        }
    }
    image.close();
}

void draw_wind(const std::vector<std::vector<std::vector<double>>> &shapes,
               const std::vector<std::vector<std::string>> &wind,
               const std::string& path,
               const std::string& file){
    /**
     * Draw the wind speed map and save it to the host machine as a .png file
     */
    std::string tempPath = "/base/images/maps/wind/" + path + file + ".png";
    char* p = const_cast<char *>(tempPath.c_str());
    pngwriter image(720,480,1.0,p);

    // Render data - render data first so shp outline goes over it
    for (const std::vector<std::string> &line: wind) {
        for (const std::string &val: line) {
            int X = getX(std::stod(line[3]));
            int Y = 480 - getY(std::stod(line[2]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
            int wind_int = std::stoi(line[0]);
            if (wind_int != 1000){  // 1000 represents invalid response
                int temp_col = map_value(wind_int, -80, 130);
                auto r = static_cast<double>(red[temp_col]);
                auto g = static_cast<double>(green[temp_col]);
                auto b = static_cast<double>(blue[temp_col]);
                image.filledcircle_blend(X,Y,30,0.5,r,g,b);
            }
        }
    }

    // Render shapes
    for (std::vector<std::vector<double>> points: shapes){
        for (int i = 0; i < points.size() - 1; i++){
            std::vector<double> &pair1 = points.at(i);
            std::vector<double> &pair2 = points.at(i+1);
            const int x1 = getX(pair1.front());
            const int y1 = 480 - getY(pair1.back());
            const int x2 = getX(pair2.front());
            const int y2 = 480 - getY(pair2.back());
            image.line(x2,y2, x1,y1,0, 0,0);
        }
    }

    // Render text
    char *fontf = const_cast<char *>("/mapper/data/fonts/Nexa-Light.ttf");
    for (const std::vector<std::string> &line: wind) {
        for (const std::string &val: line) {
            char *tchar = const_cast<char *>(line[0].c_str());  // using const_cast bc plot_text() won't accept const char*
            int X = getX(std::stod(line[3]));
            int Y = 480 - getY(std::stod(line[2]));  // sub Y from 480(frame height) bc PNGwriter uses origin at bottom left rather than top left
            if (line[0] != "1000"){  // 1000 represents invalid response
                image.plot_text(fontf, 10, X, Y, 0, tchar, 0.0, 0.0, 0.0);
            }
            else {
                image.filledcircle(X,Y,3,1.0,0.0,0.0);
            }
            // TODO: Render arrow pointing in wdir
        }
    }

    image.close();
}

int main(int argc, char* argv[]) {
    try {
        auto logger = spdlog::basic_logger_mt("png-mapper: src.main", "/base/images/logs/basic-log.txt");
        spdlog::set_default_logger(logger);
        spdlog::info("TESTING");
        // initialize shp vector
        std::vector<std::vector<std::vector<double>>> shps;
        Shape shape("/mapper/data/okcounties.shp");
        shps = shape.get_shapes();

        // read csv from shared vol & make dirs on host
        std::string path = argv[1];
        std::string file = argv[2];
        std::string csv_full_path = "/mapper/vol/" + path + file + ".csv";
        std::vector<std::vector<std::string>> data;
        data = CSVReader::read_csv(csv_full_path);
        make_dirs(path);

        // draw maps
        std::vector<std::vector<std::string>> temp;
        std::vector<std::vector<std::string>> wind;
        temp = CSVReader::get_temp(data);
        wind = CSVReader::get_wind(data);
        draw_temp(shps, temp, path, file);
        draw_wind(shps, wind, path, file);
    }
    catch (std::exception &e) {
        std::cerr << "There was an error during execution: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
