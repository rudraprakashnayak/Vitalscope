#include "procpulse/mem_collector.h"

#include <sstream>

#include "procpulse/fs_util.h"

namespace procpulse {

MemSample parse_meminfo_content(const std::string& content) {
    MemSample m;
    std::istringstream in(content);
    std::string key;
    long long value;
    std::string unit;
    while (in >> key >> value) {
        in >> unit;  // "kB"
        if (key == "MemTotal:") {
            m.total_kb = value;
        } else if (key == "MemFree:") {
            m.free_kb = value;
        } else if (key == "MemAvailable:") {
            m.available_kb = value;
        } else if (key == "Buffers:") {
            m.buffers_kb = value;
        } else if (key == "Cached:") {
            m.cached_kb = value;
        }
    }
    if (m.total_kb < 0) {
        throw std::runtime_error("malformed /proc/meminfo content");
    }
    if (m.available_kb < 0) {
        m.available_kb = m.free_kb;  // very old kernels lack MemAvailable
    }
    return m;
}

MemCollector::MemCollector(std::string root) : root_(std::move(root)) {}

MemSample MemCollector::sample() const {
    return parse_meminfo_content(read_file(root_ + "/proc/meminfo"));
}

}  // namespace procpulse
