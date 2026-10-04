#include "procpulse/sampler.h"

#include <chrono>
#include <stdexcept>

namespace procpulse {

Sampler::Sampler(std::string root, double interval_seconds)
    : root_(std::move(root)), interval_(interval_seconds) {}

Sampler::~Sampler() { stop(); }

void Sampler::start() {
    if (running_.exchange(true)) {
        return;
    }
    stop_requested_.store(false);
    thread_ = std::thread([this] { loop(); });
}

void Sampler::stop() {
    if (!running_.exchange(false)) {
        return;
    }
    stop_requested_.store(true);
    cv_.notify_all();
    if (thread_.joinable()) {
        thread_.join();
    }
}

Snapshot Sampler::latest() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return latest_;
}

void Sampler::loop() {
    CpuCollector cpu(root_);
    MemCollector mem(root_);
    ProcCollector procs(root_);
    DevCollector devs(root_);

    while (!stop_requested_.load()) {
        Snapshot snap;
        try {
            snap.cpu = cpu.sample();
            if (have_previous_cpu_) {
                snap.cpu_usage_percent = usage_between(previous_cpu_, snap.cpu);
            }
            previous_cpu_ = snap.cpu;
            have_previous_cpu_ = true;
            snap.mem = mem.sample();
            snap.procs = procs.list();
            snap.devs = devs.list();
            snap.dev_nodes = devs.count_dev_nodes();
        } catch (const std::runtime_error&) {
            // procfs disappeared (non-Linux root): keep previous snapshot
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            snap.seq = latest_.seq + 1;
            latest_ = snap;
        }
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_for(lock, std::chrono::milliseconds(static_cast<long long>(interval_ * 1000.0)),
                     [this] { return stop_requested_.load(); });
    }
}

}  // namespace procpulse
