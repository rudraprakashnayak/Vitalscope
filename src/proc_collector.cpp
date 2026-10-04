#include "procpulse/proc_collector.h"

#include <cctype>
#include <cstdlib>
#include <stdexcept>

#include <algorithm>

#include "procpulse/fs_util.h"

namespace procpulse {

ProcInfo parse_proc_stat(const std::string& stat_content, const std::string& comm_fallback) {
    ProcInfo info;
    const std::size_t open = stat_content.find('(');
    const std::size_t close = stat_content.rfind(')');
    if (open == std::string::npos || close == std::string::npos || close < open) {
        throw std::runtime_error("malformed /proc/<pid>/stat content");
    }
    info.pid = std::atoi(stat_content.c_str());
    info.name = stat_content.substr(open + 1, close - open - 1);
    // state is field 3: first non-space char after the closing parenthesis
    std::size_t i = close + 1;
    while (i < stat_content.size() && std::isspace(static_cast<unsigned char>(stat_content[i]))) {
        ++i;
    }
    info.state = i < stat_content.size() ? stat_content[i] : '?';
    if (info.name.empty()) {
        info.name = comm_fallback;
    }
    return info;
}

ProcCollector::ProcCollector(std::string root) : root_(std::move(root)) {}

std::vector<ProcInfo> ProcCollector::list() const {
    std::vector<ProcInfo> out;
    for (const std::string& entry : list_dirs(root_ + "/proc")) {
        if (entry.empty() || !std::isdigit(static_cast<unsigned char>(entry[0]))) {
            continue;
        }
        try {
            std::string comm;
            try {
                comm = read_file(root_ + "/proc/" + entry + "/comm");
                while (!comm.empty() && (comm.back() == '\n' || comm.back() == '\r')) {
                    comm.pop_back();
                }
            } catch (const std::runtime_error&) {
                // comm may vanish for exiting processes
            }
            out.push_back(parse_proc_stat(read_file(root_ + "/proc/" + entry + "/stat"), comm));
        } catch (const std::runtime_error&) {
            continue;  // process exited between listing and reading
        }
    }
    std::sort(out.begin(), out.end(),
              [](const ProcInfo& a, const ProcInfo& b) { return a.pid < b.pid; });
    return out;
}

}  // namespace procpulse
