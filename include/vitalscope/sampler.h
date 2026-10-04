#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "Vitalscope/cpu_collector.h"
#include "Vitalscope/dev_collector.h"
#include "Vitalscope/mem_collector.h"
#include "Vitalscope/proc_collector.h"

namespace Vitalscope {

struct Snapshot {
    CpuSample cpu;
    double cpu_usage_percent = 0.0;
    MemSample mem;
    std::vector<ProcInfo> procs;
    std::vector<DevInfo> devs;
    long dev_nodes = -1;
    unsigned long long seq = 0;
};

// Polls all collectors on a background thread at a fixed interval.
class Sampler {
public:
    Sampler(std::string root, double interval_seconds);
    ~Sampler();

    void start();
    void stop();
    bool running() const { return running_.load(); }
    Snapshot latest() const;

private:
    void loop();

    std::string root_;
    double interval_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stop_requested_{false};
    Snapshot latest_;
    CpuSample previous_cpu_;
    bool have_previous_cpu_ = false;
    std::thread thread_;
};

}  // namespace Vitalscope
