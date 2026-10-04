#pragma once

#include <string>

namespace procpulse {

struct MemSample {
    long long total_kb = -1;
    long long free_kb = -1;
    long long available_kb = -1;
    long long buffers_kb = -1;
    long long cached_kb = -1;

    long long used_kb() const { return total_kb - available_kb; }
    double usage_percent() const {
        return total_kb > 0 ? 100.0 * static_cast<double>(used_kb()) / static_cast<double>(total_kb) : 0.0;
    }
};

MemSample parse_meminfo_content(const std::string& content);

class MemCollector {
public:
    explicit MemCollector(std::string root = "/");
    MemSample sample() const;

private:
    std::string root_;
};

}  // namespace procpulse
