# Vitalscope — A C++ Linux System Monitor & Device Explorer


> **Educational reference implementation — not a student submission.**
> This repository is a teaching reference for the 20-day Linux / C++ training capstone
> (Module-9). It was produced with AI assistance at the course coordinator's request.
> Per course rules, each student must implement their own individual project **without AI**
> and must **not** submit this repository (or parts of it) as their own work.
> The commit history is organised per project stage (tags `stage-1` … `stage-6`, `v1.0`)
> to model the expected development process.

## Project description

vitalscope is a small, dependency-free system monitor written in modern C++17. It answers
one question end-to-end: **how does the Linux kernel publish hardware and system state to
ordinary userspace programs, and how does a C++ application turn that raw text into
trustworthy numbers?**

It reads the kernel's own information interfaces — `/proc` (CPU counters, memory zones,
per-process records), `/sys/class` (the device tree drivers publish) and `/dev` (device
nodes) — and presents them as a clean console dashboard. No root privileges, no
libraries, no kernel code: just careful parsing, one sampler thread, and polite signal
handling.

**Data flow (one direction, by design):**

```
/proc, /sys, /dev  →  fs_util  →  collectors  →  value structs  →  dashboard / sampler
   (kernel)          (read)      (parse)         (C++ types)        (presentation)
```

## Features at a glance

| Feature | Source interface | What you get |
|---|---|---|
| CPU usage & load | `/proc/stat`, `/proc/loadavg` | usage % over a sample window, 1/5/15-min load |
| Memory | `/proc/meminfo` | total / used / available / cached, usage % |
| Processes | `/proc/<pid>/comm`, `/proc/<pid>/stat` | PID, name, state (sorted by PID) |
| Devices | `/sys/class/*`, `/dev` | subsystem → device enumeration, node count |
| Interactive menu | — | pick any section on demand |
| One-shot mode | `--once` | single snapshot, script/CI friendly |
| Live mode | `--live <secs>` | background sampler thread, periodic lines |
| Clean shutdown | SIGINT / SIGTERM | thread joined, exit code 0 |
| Testability | `--root <path>` | every collector reads from an alternate root (fixtures) |

## Sample output

Interactive menu:

```text
== vitalscope ==
 1) CPU & load
 2) Memory
 3) Processes
 4) Devices
 5) Full snapshot
 0) Quit
choice:
```

One-shot snapshot (`./build/vitalscope --root tests/fixtures --once`, reproducible):

```text
-- CPU & load --
  user=100 system=50 idle=800 iowait=0
  usage over sample window: 0.0 %
  load average: 0.5 0.6 0.6
-- Memory --
  total=1000 kB  used=600 kB  available=400 kB  cached=150 kB
  usage: 60.0 %
-- Processes (2) --
  pid=     7  state=S  sleep
  pid=    42  state=R  gcc
-- Devices (3 in /sys/class) --
  block        sda
  net          eth0
  thermal      thermal_zone0
  device nodes in /dev: 2
```

Live mode on a real system (`./build/procpulse --live 6 --interval 2`, then Ctrl-C):

```text
[seq 1] cpu=0.0%  mem=9.3%  procs=84  devs=253
[seq 2] cpu=0.8%  mem=9.1%  procs=28  devs=253
[seq 3] cpu=0.0%  mem=9.1%  procs=28  devs=253
sampler stopped cleanly
```

## Commands

| Command | Effect |
|---|---|
| `make` | build `build/vitalscope` |
| `make test` | unit tests (fixtures) + integration script |
| `./build/vitalscope` | interactive menu on the live system |
| `./build/vitalscope --once` | single snapshot of the live system |
| `./build/vitalscope --live 10` | sampler thread prints every 2 s for 10 s |
| `./build/vitalscope --root tests/fixtures --once` | snapshot from fixtures (deterministic) |
| `./build/vitalscope --interval 0.5 --live 5` | custom sampling interval |

## Repository layout

```
vitalscope/
├── include/vitalscope/   # public headers (one class per collector)
├── src/                 # implementations + main
├── tests/               # fixture-based unit tests + integration script
│   └── fixtures/        # saved /proc, /sys and /dev samples
├── docs/                # stage documents (intro, PRD, design, devlog, report, slides)
└── Makefile
```

## Project stages (capstone process)

| Tag | Stage | Contents |
|---|---|---|
| `stage-1` | Introduction | `docs/PROJECT_INTRO.md` |
| `stage-2` | Requirements & plan | `docs/PRD.md` |
| `stage-3` | Design & architecture | `docs/DESIGN.md` (UML), headers, build setup |
| `stage-4` | Prototype | working collectors + CLI |
| `stage-5` | Testing & improvement | `tests/`, `docs/DEVLOG.md` |
| `stage-6` / `v1.0` | Final & presentation | `docs/FINAL_REPORT.md`, `docs/PRESENTATION.md` |

## Training-topic coverage

| Topic | Where it appears |
|---|---|
| Linux | procfs / sysfs, processes, permissions, shell usage |
| Computer architecture | CPU time states, memory hierarchy, load average |
| Hardware & software | how kernel and drivers expose hardware via `/sys` and `/dev` |
| System programming | file I/O, threads, signals, timers |
| C++ | classes, RAII, STL containers, streams, exceptions |

## Build & run

Requires: Linux (or WSL), `g++` with C++17 support, `make`. No root privileges needed.

```bash
make                 # builds build/procpulse
make test            # builds and runs unit tests against tests/fixtures
./build/procpulse    # interactive menu (real system)
```

## License

MIT — see [LICENSE](LICENSE).
