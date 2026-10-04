#pragma once

#include <string>
#include <vector>

namespace Vitalscope {

// Throws std::runtime_error if the file cannot be opened.
std::string read_file(const std::string& path);

// Names of sub-directories of `path`, sorted. Empty vector if `path` is not a directory.
std::vector<std::string> list_dirs(const std::string& path);

// Names of all entries (files and directories) of `path`, sorted.
std::vector<std::string> list_entries(const std::string& path);

bool path_exists(const std::string& path);

}  // namespace Vitalscope
