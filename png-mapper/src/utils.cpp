#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "utils.h"

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
    const double fm_x { (width * (180 + lon)) / 360 };
    const double fm_y { width + (width / 2) };
    const double x { fmod(fm_x, fm_y) };
    return round(x) - 4850;  // shift transformation to make it fit within the frame
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

