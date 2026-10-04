#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "procpulse/cpu_collector.h"
#include "procpulse/dev_collector.h"
#include "procpulse/fs_util.h"
#include "procpulse/mem_collector.h"
#include "procpulse/proc_collector.h"
#include "procpulse/sampler.h"
#include "test_framework.h"

namespace {

void test_fs_util(const std::string& fix) {
    const std::string content = procpulse::read_file(fix + "/proc/loadavg");
    CHECK(content.find("0.52") == 0);

    bool threw = false;
    try {
        procpulse::read_file(fix + "/proc/does-not-exist");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    CHECK(threw);

    const auto entries = procpulse::list_entries(fix + "/dev");
    CHECK(entries.size() == 2);
    CHECK(entries[0] == "null");
    CHECK(entries[1] == "zero");

    const auto dirs = procpulse::list_dirs(fix + "/proc");
    CHECK(dirs.size() == 2);  // only the numeric pid dirs, "stat" is a file
}

void test_cpu(const std::string& fix) {
    const procpulse::CpuSample a =
        procpulse::parse_stat_content(procpulse::read_file(fix + "/proc/stat"));
    CHECK(a.user == 100);
    CHECK(a.system == 50);
    CHECK(a.idle == 800);
    CHECK(a.total() == 950);
    CHECK(a.busy() == 150);

    const procpulse::CpuSample b =
        procpulse::parse_stat_content(procpulse::read_file(fix + "/proc/stat2"));
    CHECK_CLOSE(procpulse::usage_between(a, b), 60.0);
    CHECK_CLOSE(procpulse::usage_between(a, a), 0.0);

    procpulse::CpuCollector collector(fix);
    procpulse::CpuSample live = collector.sample();
    CHECK_CLOSE(live.load1, 0.52);
    CHECK_CLOSE(live.load5, 0.58);
    CHECK_CLOSE(live.load15, 0.59);
}

void test_mem(const std::string& fix) {
    const procpulse::MemSample m = procpulse::MemCollector(fix).sample();
    CHECK(m.total_kb == 1000);
    CHECK(m.free_kb == 200);
    CHECK(m.available_kb == 400);
    CHECK(m.buffers_kb == 50);
    CHECK(m.cached_kb == 150);
    CHECK(m.used_kb() == 600);
    CHECK_CLOSE(m.usage_percent(), 60.0);
}

void test_proc(const std::string& fix) {
    const auto procs = procpulse::ProcCollector(fix).list();
    CHECK(procs.size() == 2);
    if (procs.size() == 2) {
        CHECK(procs[0].pid == 7);
        CHECK(procs[0].name == "sleep");
        CHECK(procs[0].state == 'S');
        CHECK(procs[1].pid == 42);
        CHECK(procs[1].name == "gcc");
        CHECK(procs[1].state == 'R');
    }
}

void test_dev(const std::string& fix) {
    const auto devs = procpulse::DevCollector(fix).list();
    CHECK(devs.size() == 3);
    if (devs.size() == 3) {
        CHECK(devs[0].subsystem == "block");
        CHECK(devs[0].name == "sda");
        CHECK(devs[1].subsystem == "net");
        CHECK(devs[1].name == "eth0");
        CHECK(devs[2].subsystem == "thermal");
        CHECK(devs[2].name == "thermal_zone0");
    }
    CHECK(procpulse::DevCollector(fix).count_dev_nodes() == 2);
}

void test_sampler(const std::string& fix) {
    procpulse::Sampler sampler(fix, 0.1);
    sampler.start();
    CHECK(sampler.running());
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    const procpulse::Snapshot snap = sampler.latest();
    CHECK(snap.seq >= 2);
    CHECK_CLOSE(snap.mem.usage_percent(), 60.0);
    CHECK(snap.procs.size() == 2);
    sampler.stop();
    CHECK(!sampler.running());
}

}  // namespace

int main(int argc, char** argv) {
    const std::string fix = argc > 1 ? argv[1] : "tests/fixtures";
    test_fs_util(fix);
    test_cpu(fix);
    test_mem(fix);
    test_proc(fix);
    test_dev(fix);
    test_sampler(fix);
    if (testing::failures() == 0) {
        std::cout << "ALL UNIT TESTS PASSED\n";
        return 0;
    }
    std::cout << testing::failures() << " TEST(S) FAILED\n";
    return 1;
}
