#include "procpulse/cpu_collector.h"

#include <sstream>

#include "procpulse/fs_util.h"

namespace procpulse {

CpuSample parse_stat_content(const std::string& content) {
    CpuSample s;
    std::istringstream in(content);
    std::string tag;
    if (!(in >> tag) || tag != "cpu") {
        throw std::runtime_error("malformed /proc/stat content");
    }
    in >> s.user >> s.nice >> s.system >> s.idle;
    in >> s.iowait >> s.irq >> s.softirq;  // optional trailing fields stay 0
    return s;
}

void parse_loadavg_content(const std::string& content, CpuSample& out) {
    std::istringstream in(content);
    in >> out.load1 >> out.load5 >> out.load15;
}

double usage_between(const CpuSample& earlier, const CpuSample& later) {
    const unsigned long long dt = later.total() - earlier.total();
    if (dt == 0) {
        return 0.0;
    }
    const unsigned long long db = later.busy() - earlier.busy();
    return 100.0 * static_cast<double>(db) / static_cast<double>(dt);
}

CpuCollector::CpuCollector(std::string root) : root_(std::move(root)) {}

CpuSample CpuCollector::sample() const {
    CpuSample s = parse_stat_content(read_file(root_ + "/proc/stat"));
    try {
        parse_loadavg_content(read_file(root_ + "/proc/loadavg"), s);
    } catch (const std::runtime_error&) {
        // loadavg is optional; keep zeros
    }
    return s;
}

}  // namespace procpulse
