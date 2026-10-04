#include <atomic>
#include <csignal>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

#include "Vitalscope/dashboard.h"
#include "Vitalscope/sampler.h"

namespace {
std::atomic<bool> g_stop{false};

void on_signal(int) { g_stop.store(true); }

void print_usage(std::ostream& out) {
    out << "usage: Vitalscope [--root PATH] [--interval SECS] [--once] [--live SECS] [--help]\n";
}
}  // namespace

int main(int argc, char** argv) {
    std::string root = "/";
    double interval = 2.0;
    bool once = false;
    bool live = false;
    double live_seconds = 10.0;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--root" && i + 1 < argc) {
            root = argv[++i];
        } else if (arg == "--interval" && i + 1 < argc) {
            interval = std::atof(argv[++i]);
        } else if (arg == "--once") {
            once = true;
        } else if (arg == "--live" && i + 1 < argc) {
            live = true;
            live_seconds = std::atof(argv[++i]);
        } else if (arg == "--help") {
            print_usage(std::cout);
            return 0;
        } else {
            std::cerr << "unknown argument: " << arg << "\n";
            print_usage(std::cerr);
            return 2;
        }
    }
    if (interval <= 0.0) {
        interval = 2.0;
    }

    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);

    try {
        if (once) {
            Vitalscope::Dashboard dashboard(root, std::cout, g_stop);
            dashboard.print_snapshot();
            return 0;
        }
        if (live) {
            Vitalscope::Sampler sampler(root, interval);
            sampler.start();
            const auto deadline = std::chrono::steady_clock::now() +
                                  std::chrono::milliseconds(static_cast<long long>(live_seconds * 1000.0));
            while (!g_stop.load() && std::chrono::steady_clock::now() < deadline) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                const Vitalscope::Snapshot snap = sampler.latest();
                if (snap.seq == 0) {
                    continue;
                }
                std::cout << "[seq " << snap.seq << "] cpu=" << std::fixed << std::setprecision(1)
                          << snap.cpu_usage_percent << "%  mem=" << snap.mem.usage_percent()
                          << "%  procs=" << snap.procs.size() << "  devs=" << snap.devs.size() << "\n"
                          << std::flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(interval * 1000.0)));
            }
            sampler.stop();
            std::cout << "sampler stopped cleanly\n";
            return 0;
        }
        Vitalscope::Dashboard dashboard(root, std::cout, g_stop);
        return dashboard.run();
    } catch (const std::exception& e) {
        std::cerr << "Vitalscope: " << e.what() << "\n";
        std::cerr << "note: live system data requires Linux procfs; use --root with fixtures elsewhere\n";
        return 1;
    }
}
