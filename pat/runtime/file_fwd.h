#pragma once

#include <string>

namespace pat::runtime {

class File;
File NewFile(int file_descriptor, std::string name);

}  // namespace pat::runtime