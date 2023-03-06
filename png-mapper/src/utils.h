#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#ifndef PNG_MAPPER_UTILS_H
#define PNG_MAPPER_UTILS_H

#endif //PNG_MAPPER_UTILS_H


void init_logger(const std::string);

void make_dirs(const std::string);

int getX(const double);

int getY(const double);

int map_value(int, const int, const int);
