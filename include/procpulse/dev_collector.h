#pragma once

#include <string>
#include <vector>

namespace procpulse {

struct DevInfo {
    std::string subsystem;  // e.g. "net", "block"
    std::string name;       // e.g. "eth0", "sda"
};

class DevCollector {
public:
    explicit DevCollector(std::string root = "/");
    std::vector<DevInfo> list() const;       // from <root>/sys/class/<subsystem>/<device>
    long count_dev_nodes() const;            // entry count of <root>/dev, -1 if unreadable

private:
    std::string root_;
};

}  // namespace procpulse
