#pragma once

#include <string>
#include <vector>

namespace procpulse {

struct ProcInfo {
    int pid = -1;
    std::string name;
    char state = '?';
};

// Parses /proc/<pid>/stat content; state is the field after the closing parenthesis.
ProcInfo parse_proc_stat(const std::string& stat_content, const std::string& comm_fallback);

class ProcCollector {
public:
    explicit ProcCollector(std::string root = "/");
    std::vector<ProcInfo> list() const;  // sorted by pid; unreadable pids are skipped

private:
    std::string root_;
};

}  // namespace procpulse
