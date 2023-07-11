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
#include "utils.h"
#include "draw.h"

std::string MAPPER_PATH_PREFIX {"/home/png-user/mapper"};

int main(int argc, char* argv[]) {
    try {
        // get args and init logger
        std::string path {argv[1]};
        std::string file {argv[2]};
        std::string log_fn {file.substr(0,10)};
        init_logger(log_fn);
        spdlog::info("-----| png-writer started |-----");

        // read csv from shared vol & make dirs on host
        std::string csv_full_path {"/home/png-user/mapper/vol/" + path + file + ".csv"};
        std::vector<std::vector<std::string>> data;
        data = CSVReader::read_csv(csv_full_path);
        make_dirs(path);

        // initialize shps object
        Shape shape(MAPPER_PATH_PREFIX + "/data/okcounties.shp");
        auto shps {shape.get_shapes()};

        // draw maps
        auto temp {CSVReader::get_temp(data)};
        auto wind {CSVReader::get_wind(data)};
        draw_temp(shps, temp, path, file);
        draw_wind(shps, wind, path, file);
    }
    catch (std::exception& e) {
        spdlog::critical("There was an Error in png-mapper: ");
        spdlog::critical(e.what());
        return 1;
    }
    spdlog::info("-----| png-writer finished |-----");
    return 0;
}
