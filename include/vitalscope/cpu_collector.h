#pragma once

#include <string>

namespace Vitalscope {

struct CpuSample {
    unsigned long long user = 0;
    unsigned long long nice = 0;
    unsigned long long system = 0;
    unsigned long long idle = 0;
    unsigned long long iowait = 0;
    unsigned long long irq = 0;
    unsigned long long softirq = 0;
    double load1 = 0.0;
    double load5 = 0.0;
    double load15 = 0.0;

    unsigned long long total() const { return user + nice + system + idle + iowait + irq + softirq; }
    unsigned long long busy() const { return user + nice + system + irq + softirq; }
};

// Parses the aggregate "cpu ..." line of a /proc/stat file.
CpuSample parse_stat_content(const std::string& content);

// Parses "1m 5m 15m running/total lastpid" from /proc/loadavg content.
void parse_loadavg_content(const std::string& content, CpuSample& out);

// Usage percentage between two samples taken at different times.
double usage_between(const CpuSample& earlier, const CpuSample& later);

class CpuCollector {
public:
    explicit CpuCollector(std::string root = "/");
    CpuSample sample() const;  // throws std::runtime_error if procfs is unreadable

private:
    std::string root_;
};

}  // namespace Vitalscope
