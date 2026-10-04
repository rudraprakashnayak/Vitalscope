#pragma once

#include <atomic>
#include <ostream>
#include <string>

#include "procpulse/cpu_collector.h"
#include "procpulse/dev_collector.h"
#include "procpulse/mem_collector.h"
#include "procpulse/proc_collector.h"

namespace procpulse {

// Menu-driven console view over the collectors.
class Dashboard {
public:
    Dashboard(std::string root, std::ostream& out, const std::atomic<bool>& stop_flag);

    // Interactive menu loop; returns 0 on clean exit.
    int run();

    // One-shot report of every section (used by --once and the integration test).
    void print_snapshot();

    void print_cpu();
    void print_memory();
    void print_processes();
    void print_devices();

private:
    std::string root_;
    std::ostream& out_;
    const std::atomic<bool>& stop_flag_;
};

}  // namespace procpulse
