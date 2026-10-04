#include "procpulse/dashboard.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <thread>

namespace procpulse {

Dashboard::Dashboard(std::string root, std::ostream& out, const std::atomic<bool>& stop_flag)
    : root_(std::move(root)), out_(out), stop_flag_(stop_flag) {}

void Dashboard::print_cpu() {
    CpuCollector collector(root_);
    const CpuSample first = collector.sample();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));  // sample window for the delta
    const CpuSample second = collector.sample();
    out_ << "-- CPU & load --\n";
    out_ << "  user=" << second.user << " system=" << second.system << " idle=" << second.idle
         << " iowait=" << second.iowait << "\n";
    out_ << "  usage over sample window: " << std::fixed << std::setprecision(1)
         << usage_between(first, second) << " %\n";
    out_ << "  load average: " << second.load1 << " " << second.load5 << " " << second.load15 << "\n";
}

void Dashboard::print_memory() {
    const MemSample m = MemCollector(root_).sample();
    out_ << "-- Memory --\n";
    out_ << "  total=" << m.total_kb << " kB  used=" << m.used_kb() << " kB  available="
         << m.available_kb << " kB  cached=" << m.cached_kb << " kB\n";
    out_ << "  usage: " << std::fixed << std::setprecision(1) << m.usage_percent() << " %\n";
}

void Dashboard::print_processes() {
    const std::vector<ProcInfo> procs = ProcCollector(root_).list();
    out_ << "-- Processes (" << procs.size() << ") --\n";
    std::size_t shown = 0;
    for (const ProcInfo& p : procs) {
        out_ << "  pid=" << std::setw(6) << p.pid << "  state=" << p.state << "  " << p.name << "\n";
        if (++shown >= 10) {
            out_ << "  ... (" << (procs.size() - shown) << " more)\n";
            break;
        }
    }
}

void Dashboard::print_devices() {
    const DevCollector collector(root_);
    const std::vector<DevInfo> devs = collector.list();
    out_ << "-- Devices (" << devs.size() << " in /sys/class) --\n";
    std::size_t shown = 0;
    for (const DevInfo& d : devs) {
        out_ << "  " << std::setw(12) << std::left << d.subsystem << " " << d.name << "\n";
        if (++shown >= 12) {
            out_ << "  ... (" << (devs.size() - shown) << " more)\n";
            break;
        }
    }
    out_ << std::right << "  device nodes in /dev: " << collector.count_dev_nodes() << "\n";
}

void Dashboard::print_snapshot() {
    print_cpu();
    print_memory();
    print_processes();
    print_devices();
}

int Dashboard::run() {
    while (!stop_flag_.load()) {
        out_ << "\n== ProcPulse ==\n"
             << " 1) CPU & load\n"
             << " 2) Memory\n"
             << " 3) Processes\n"
             << " 4) Devices\n"
             << " 5) Full snapshot\n"
             << " 0) Quit\n"
             << "choice: " << std::flush;
        int choice = -1;
        if (!(std::cin >> choice)) {
            break;  // EOF or bad input
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
            case 1: print_cpu(); break;
            case 2: print_memory(); break;
            case 3: print_processes(); break;
            case 4: print_devices(); break;
            case 5: print_snapshot(); break;
            case 0: return 0;
            default: out_ << "unknown choice\n"; break;
        }
    }
    return 0;
}

}  // namespace procpulse
