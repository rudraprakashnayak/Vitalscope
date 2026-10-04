#include "procpulse/dev_collector.h"

#include "procpulse/fs_util.h"

namespace procpulse {

DevCollector::DevCollector(std::string root) : root_(std::move(root)) {}

std::vector<DevInfo> DevCollector::list() const {
    std::vector<DevInfo> out;
    const std::string class_root = root_ + "/sys/class";
    for (const std::string& subsystem : list_dirs(class_root)) {
        for (const std::string& device : list_dirs(class_root + "/" + subsystem)) {
            out.push_back(DevInfo{subsystem, device});
        }
    }
    return out;
}

long DevCollector::count_dev_nodes() const {
    return static_cast<long>(list_entries(root_ + "/dev").size());
}

}  // namespace procpulse
