# Stage 6 — Final Presentation & Viva Defense Guide

**Project Name:** Vitalscope — A C++ Linux System Monitor & Device Explorer  
**Presenter:** Rudra Prakash Nayak  
**Repository:** [rudraprakashnayak/Vitalscope](https://github.com/rudraprakashnayak/Vitalscope)  

---

## 1. Final Presentation Slide Deck Outline (8 Slides)

### Slide 1: Title & Overview
- **Header**: Vitalscope — A C++ Linux System Monitor & Device Explorer
- **Tagline**: End-to-end Linux hardware and state exploration without external dependencies.
- **Presenter**: Rudra Prakash Nayak (Capstoned Project Delivery)

### Slide 2: Problem Statement & Scope
- **Problem**: Beginners learn Linux commands, C++ syntax, and OS theory in isolation without seeing them interact.
- **Solution**: A clean console-based application answering how kernel state (`/proc`, `/sys`, `/dev`) is parsed, converted into C++ data types, and safely presented in real time.
- **In Scope**: CPU, Memory, Process list, Device tree enumeration, interactive CLI, multi-threaded sampler, `--root` fixture engine.

### Slide 3: Software Engineering Process (Stage 1 → Stage 6)
- **Methodology**: Staged software engineering lifecycle.
- **Milestones**:
  - `stage-1`: Scope & Problem Definition (`PROJECT_INTRO.md`)
  - `stage-2`: Requirements & Product Specifications (`PRD.md`)
  - `stage-3`: System Architecture & UML Diagrams (`DESIGN.md`)
  - `stage-4`: Core Collectors & Thread Implementation (`src/`, `include/`)
  - `stage-5`: Automated Testing & Fixtures (`tests/`, `DEVLOG.md`)
  - `stage-6`: Final Delivery & Report (`FINAL_REPORT.md`, `PRESENTATION.md`)

### Slide 4: System Architecture & Data Flow
- **Unidirectional Architecture**:
  ```
  /proc, /sys, /dev → fs_util → Collectors → Snapshot Struct → Dashboard/Sampler
  ```
- **Component Breakdown**: `fs_util` file abstraction layer, `CpuCollector`, `MemCollector`, `ProcCollector`, `DevCollector`, `Sampler` thread, `Dashboard` view.

### Slide 5: Key Technical Highlights
- **File System Abstraction (`--root`)**: Enables 100% deterministic unit testing using fixture directories (`tests/fixtures`) without relying on host system load.
- **Thread Safety**: POSIX thread with `std::mutex` and `std::condition_variable` wait loops.
- **Signal Handling**: Asynchronous `SIGINT` / `SIGTERM` interception for clean thread shutdown and exit code 0.

### Slide 6: Verification & Testing Results
- **Automated Unit Tests**: All parser tests (`Cpu`, `Mem`, `Proc`, `Dev`, `fs_util`) passing against fixtures.
- **Integration Tests**: `tests/run_integration.sh` verifying `--once` and `--live` CLI modes.
- **Live Validation**: Cross-verified metrics against standard Linux utilities (`top`, `free`, `ps`).

### Slide 7: Achievements & Limitations
- **Achievements**: Zero third-party dependencies; complete memory safety (RAII); clear stage-by-stage Git history.
- **Limitations**: Read-only userspace program (no custom kernel driver module); CPU delta requires two sample points.

### Slide 8: Future Improvements & Roadmap
- **Stage 7 (Planned)**: Companion Linux Character Device Driver (`/dev/vitalscope`) with `ioctl` interface.
- **Stage 8 (Planned)**: Multi-core CPU breakdown & CSV/JSON export.
- **Stage 9 (Planned)**: Interactive `ncurses` Terminal UI (TUI).

---

## 2. Live System Demonstration Script (3-Minute Walkthrough)

1. **Build & Verify Environment**:
   ```bash
   make clean && make && make test
   ```
   *Point out*: "Both the unit test suite and shell integration test suite passed cleanly."

2. **Demonstrate One-Shot Snapshot Mode**:
   ```bash
   ./build/vitalscope --once
   ```
   *Point out*: "Prints a single timestamped snapshot of CPU usage, load average, memory utilization, running processes, and kernel devices."

3. **Demonstrate Live Multi-Threaded Sampler**:
   ```bash
   ./build/vitalscope --live 6 --interval 2
   ```
   *Point out*: "The background sampler thread polls pseudo-files every 2 seconds. Pressing `Ctrl-C` triggers polite signal handling, joins the sampler thread, and exits cleanly with code 0."

4. **Demonstrate Fixture-Based Alternate Root Testing**:
   ```bash
   ./build/vitalscope --root tests/fixtures --once
   ```
   *Point out*: "Redirects file operations to saved test fixtures, allowing deterministic execution on any OS or CI/CD environment."

5. **Show Staged Git Commit History**:
   ```bash
   git tag -l
   git log --oneline --decorate -n 10
   ```
   *Point out*: "Every stage of development (`stage-1` through `stage-6`) is documented and tagged in Git."

---

## 3. Comprehensive Viva Defense Q&A

### Q1: Where does Vitalscope get its data, and why doesn't it need root privileges?
**Answer**: Vitalscope reads kernel pseudo-files from `/proc`, `/sys/class`, and `/dev`. In Linux, the kernel exposes hardware state, CPU counters, memory stats, and process metadata as readable pseudo-files accessible to unprivileged userspace programs.

### Q2: How is CPU utilization calculated from `/proc/stat`?
**Answer**: `/proc/stat` provides cumulative time counters (in jiffies) spent in user, system, idle, and iowait states since boot. A single reading cannot yield instantaneous usage; Vitalscope takes two snapshots across a sample window, calculates the delta in busy vs. total time, and computes the percentage:
$$\text{Usage \%} = \frac{\Delta \text{Busy}}{\Delta \text{Total}} \times 100$$

### Q3: How does Vitalscope handle process names containing spaces in `/proc/<pid>/stat`?
**Answer**: `/proc/<pid>/stat` formats process names enclosed in parentheses `(comm)`. Because process names can contain spaces or parentheses, Vitalscope locates the *first* `(` and the *last* `)` to extract the full process name reliably, and reads process state from the subsequent field.

### Q4: How is thread safety guaranteed in Live Sampler mode?
**Answer**: The background `Sampler` thread updates a private `Snapshot` structure. Once complete, it publishes the update to a shared `latest_` instance protected by a `std::mutex`. Readers acquire a `lock_guard` to retrieve an immutable copy, preventing data races.

### Q5: How does `--root` enable deterministic testing without a live Linux kernel?
**Answer**: `fs_util` prepends a configurable root path (`root_`) to all file accesses. When `--root tests/fixtures` is passed, `fs_util` reads saved sample files instead of `/proc` or `/sys`, enabling reproducible unit testing on any environment (including Windows or CI pipelines).

