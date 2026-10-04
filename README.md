# ProcPulse — A C++ Linux System Monitor & Device Explorer

> **Educational reference implementation — not a student submission.**
> This repository is a teaching reference for the 20-day Linux / C++ training capstone
> (Module-9). It was produced with AI assistance at the course coordinator's request.
> Per course rules, each student must implement their own individual project **without AI**
> and must **not** submit this repository (or parts of it) as their own work.
> The commit history is organised per project stage (tags `stage-1` … `stage-6`, `v1.0`)
> to model the expected development process.

ProcPulse is a menu-driven C++17 command-line tool that reads Linux's own information
interfaces (`/proc`, `/sys`, `/dev`) and reports live CPU, memory, process and device
information — a miniature `top` / task-manager built from scratch.

## Features

- **CPU & load** — usage percentage computed from `/proc/stat` deltas, load averages from `/proc/loadavg`
- **Memory** — total / used / available from `/proc/meminfo`
- **Processes** — PID, name and state from `/proc/<pid>/comm` and `/proc/<pid>/stat`
- **Devices** — subsystem/device enumeration from `/sys/class/*`, device-node count from `/dev`
- **Live mode** — background sampler thread (`--live`), clean shutdown on `Ctrl-C`
- **Testable anywhere** — every collector takes a configurable root path (`--root`), so unit
  tests run against saved fixtures even on machines without Linux procfs

## Repository layout

```
procpulse/
├── include/procpulse/   # public headers (one class per collector)
├── src/                 # implementations + main
├── tests/               # fixture-based unit tests + integration script
│   └── fixtures/        # saved /proc and /sys samples
├── docs/                # stage documents (intro, PRD, design, devlog, report)
└── Makefile
```

## Build & run (Linux or WSL)

```bash
make                 # builds build/procpulse
make test            # builds and runs unit tests against tests/fixtures
./build/procpulse                     # interactive menu (real system)
./build/procpulse --once              # single snapshot of the real system
./build/procpulse --live 10           # sampler thread prints every 2 s for 10 s
./build/procpulse --root tests/fixtures --once   # snapshot from fixtures
```

Requires: Linux (or WSL), `g++` with C++17 support, `make`. No root privileges needed.

## Project stages (capstone process)

| Tag | Stage | Contents |
|---|---|---|
| `stage-1` | Introduction | `docs/PROJECT_INTRO.md` |
| `stage-2` | Requirements & plan | `docs/PRD.md` |
| `stage-3` | Design & architecture | `docs/DESIGN.md`, headers, build setup |
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

## License

MIT — see [LICENSE](LICENSE).
