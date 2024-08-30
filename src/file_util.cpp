#include "file_util.h"

bool file_util::exists(const std::string& file_name) {
    std::ifstream infile(file_name);
    return infile.good();
}
