# Stage 6 — Final Implementation & Project Delivery Report

**Project Name:** Vitalscope — A C++ Linux System Monitor & Device Explorer  
**Version:** v1.0 (`stage-6` / `v1.0`)  
**Repository:** [rudraprakashnayak/Vitalscope](https://github.com/rudraprakashnayak/Vitalscope)  
**Author:** Rudra Prakash Nayak  

---

## 1. Executive Summary & Deliverables

Vitalscope is a production-grade, dependency-free C++17 Linux system monitoring and hardware exploration tool. It demonstrates the complete end-to-end software development lifecycle—moving from problem definition and requirement engineering to object-oriented architectural design, modular implementation, fixture-based unit testing, and final delivery.

### Key Project Artifacts Submitted
- **Source Code**: Modern C++17 modular architecture (`src/`, `include/vitalscope/`).
- **Build & Test Automation**: Standardized `Makefile` and automated test runner (`tests/run_integration.sh`).
- **Documentation Suite**:
  - `docs/PROJECT_INTRO.md` (Stage 1 — Scope & Problem Statement)
  - `docs/PRD.md` (Stage 2 — Product Requirements & Specifications)
  - `docs/DESIGN.md` (Stage 3 — Architecture, Component Design & UML)
  - `docs/DEVLOG.md` (Stage 5 — Development Log & Test Records)
  - `docs/FINAL_REPORT.md` (Stage 6 — Final Project Report)
  - `docs/PRESENTATION.md` (Stage 6 — Presentation & Viva Defense Script)
- **Git Version Control**: Clean staged history tagged from `stage-1` through `stage-6` and `v1.0`.

---

## 2. Professional Software Development Lifecycle (Stage 1 → Stage 6)

The project strictly followed a 6-stage software engineering methodology to ensure continuous progress, traceable design decisions, and verifiable quality.

| Stage | Milestone & Description | Primary Deliverables | Git Tag | Evidence & Verification |
|---|---|---|---|---|
| **Stage 1** | **Project Scope & Problem Statement** | Problem formulation, scope definition, expected outcomes | `stage-1` | `docs/PROJECT_INTRO.md` |
| **Stage 2** | **Requirements & Plan** | Functional & non-functional requirements, acceptance criteria | `stage-2` | `docs/PRD.md` |
| **Stage 3** | **Architecture & Design** | UML Class & Sequence diagrams, header definitions, build setup | `stage-3` | `docs/DESIGN.md`, `include/vitalscope/` |
| **Stage 4** | **Prototype & Implementation** | Core collectors (`Cpu`, `Mem`, `Proc`, `Dev`), `Sampler` thread & CLI | `stage-4` | `src/*.cpp`, `Makefile` |
| **Stage 5** | **Testing & Quality Assurance** | Fixture-based unit tests, shell integration tests, devlog | `stage-5` | `tests/`, `docs/DEVLOG.md` |
| **Stage 6** | **Final Delivery & Presentation** | Final report, viva defense script, complete system demonstration | `stage-6` / `v1.0` | `docs/FINAL_REPORT.md`, `docs/PRESENTATION.md` |

---

## 3. System Architecture & Design Overview

Vitalscope follows a unidirectional data flow design: kernel pseudo-files (`/proc`, `/sys`, `/dev`) are abstracted by file utilities, parsed into immutable C++ data structures by specialized collectors, safely processed by a multi-threaded sampler, and presented to the user.

```
/proc, /sys, /dev  →  fs_util  →  Collectors  →  Snapshot Struct  →  Dashboard / Sampler
   (kernel)          (read)      (parse)        (C++ types)        (presentation)
```

### Core Architecture Components
1. **`fs_util`**: Centralized file I/O abstraction (`read_file`, `list_dirs`, `list_entries`). Enables alternate root redirection via `--root` for fixture-based testing.
2. **`CpuCollector`**: Parses `/proc/stat` and `/proc/loadavg` to calculate CPU utilization over sampling windows.
3. **`MemCollector`**: Parses `/proc/meminfo` into structured memory metrics (total, used, available, cached).
4. **`ProcCollector`**: Enumerates active `/proc/<pid>` entries, safely parsing `/proc/<pid>/stat` and handling process termination gracefully.
5. **`DevCollector`**: Scans `/sys/class/*` and counts character/block device nodes in `/dev`.
6. **`Sampler`**: Runs a background POSIX thread with `std::condition_variable` waiting, safely updating an immutable `Snapshot` under a `std::mutex`.
7. **`Dashboard`**: Handles interactive terminal menu navigation and formatted snapshot output.

---

## 4. Verification & Testing Results

Testing was conducted using both deterministic fixture testing (`tests/fixtures`) and live system validation on Ubuntu Linux (WSL2).

### Automated Test Execution Summary
```bash
make test
```
- **Unit Test Suite (`test_collectors`)**:
  - `test_read_file_and_list_entries`: PASSED
  - `test_cpu_collector`: PASSED (Usage: 60.0%)
  - `test_mem_collector`: PASSED (Total: 1000 kB, Usage: 60.0%)
  - `test_proc_collector`: PASSED (Parsed 2 processes: PID 7 & 42)
  - `test_dev_collector`: PASSED (Found 3 devices, 2 dev nodes)
  - `test_sampler_thread`: PASSED (Sequence increment verified)
- **Integration Test Suite (`run_integration.sh`)**:
  - `--once` Mode Test against fixtures: PASSED
  - `--live` Mode Thread Sampling Test: PASSED

---

## 5. System Demonstration & Usage Guide

### Building the Binary
```bash
make clean && make
```

### Running Modes
1. **Interactive Dashboard**:
   ```bash
   ./build/vitalscope
   ```
2. **One-Shot Snapshot Mode**:
   ```bash
   ./build/vitalscope --once
   ```
3. **Live Threaded Sampler Mode**:
   ```bash
   ./build/vitalscope --live 10 --interval 2
   ```
4. **Fixture-Based Test Mode (No Linux Kernel Required)**:
   ```bash
   ./build/vitalscope --root tests/fixtures --once
   ```

---

## 6. Project Achievements, Limitations & Future Roadmap

### Key Achievements
- **Zero External Dependencies**: Standard C++17 library and POSIX syscalls only.
- **Thread Safety & Clean Signal Handling**: Graceful thread join on `SIGINT` / `SIGTERM` with zero memory leaks.
- **Full Testability**: High coverage achieved through filesystem abstraction (`--root`).
- **Complete Software Engineering Lifecycle**: Clear stage-by-stage progression documented with Git tags.

### Technical Limitations
- **Consumer-Only Scope**: Reads kernel-published data; does not include custom kernel module implementation.
- **Sampling Window Requirement**: CPU percentage calculation requires two distinct sampling windows (`--once` yields 0.0% delta by design).
- **Text Console Output**: CLI only; no graphical interface or ncurses terminal library.

### Future Improvements Roadmap
1. **Stage 7 (Planned)**: Develop a companion Linux Character Device Driver (`/dev/vitalscope`) exposing custom metrics via `ioctl`.
2. **Stage 8 (Planned)**: Add per-core CPU breakdown (`/proc/stat` multi-core rows) and JSON/CSV log export capability.
3. **Stage 9 (Planned)**: Implement an interactive `ncurses` Terminal User Interface (TUI) with real-time graphical usage bars.

---

## 7. Conclusion

Vitalscope successfully bridges system programming theory, computer architecture concepts, and modern C++ software design into a practical, well-tested system tool. All project deliverables—including source code, documentation, UML diagrams, build automation, and stage history—have been verified and published.
