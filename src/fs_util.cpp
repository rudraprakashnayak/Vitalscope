#include "procpulse/fs_util.h"

#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace procpulse {

std::string read_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("cannot open " + path);
    }
    std::ostringstream buf;
    buf << in.rdbuf();
    return buf.str();
}

namespace {
std::vector<std::string> list_impl(const std::string& path, bool dirs_only) {
    std::vector<std::string> out;
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return out;
    }
    while (struct dirent* entry = readdir(dir)) {
        const std::string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }
        if (dirs_only) {
            struct stat st {};
            if (stat((path + "/" + name).c_str(), &st) != 0 || !S_ISDIR(st.st_mode)) {
                continue;
            }
        }
        out.push_back(name);
    }
    closedir(dir);
    std::sort(out.begin(), out.end());
    return out;
}
}  // namespace

std::vector<std::string> list_dirs(const std::string& path) { return list_impl(path, true); }

std::vector<std::string> list_entries(const std::string& path) { return list_impl(path, false); }

bool path_exists(const std::string& path) {
    struct stat st {};
    return stat(path.c_str(), &st) == 0;
}

}  // namespace procpulse
