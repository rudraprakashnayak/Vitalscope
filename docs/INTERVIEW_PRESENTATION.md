# Vitalscope — Interview Presentation Pitch Deck & Defense Guide

**Project:** Vitalscope — A C++ Linux System Monitor & Hardware Device Explorer  
**Presenter:** Rudra Prakash Nayak  
**Interactive Slides:** [`docs/PRESENTATION_SLIDES.html`](PRESENTATION_SLIDES.html)  
**Repository:** [rudraprakashnayak/Vitalscope](https://github.com/rudraprakashnayak/Vitalscope)  

---

## Slide 1: Title & Elevator Pitch
> **Speaker Notes:**  
> "Good morning/afternoon. Today I'd like to present **Vitalscope**, a modern, dependency-free C++17 system monitoring and device exploration tool built directly on Linux kernel interfaces. The core objective of Vitalscope is to demonstrate how raw hardware state published by the Linux kernel via `/proc` and `/sys` can be parsed, safely managed using C++ RAII and multithreading, and presented in real-time without external dependencies."

- **Core Tech Stack:** C++17, POSIX Sycalls, STL Threads & Mutexes, Linux procfs/sysfs.
- **Key Highlight:** 0 Third-Party Libraries, Unprivileged Execution, Deterministic Testability.

---

## Slide 2: The Interview Hook (Why This Project Matters)
> **Speaker Notes:**  
> "When interviewing for C++ or systems roles, candidates often speak about Linux commands, C++ syntax, and multithreading in separate silos. Vitalscope solves this candidate problem by uniting all three into one real-world system program. It demonstrates how a developer handles string parsing, asynchronous data structures, signal safety, and test automation in C++."

- **Problem:** OS theory, C++ design patterns, and multithreading are rarely combined in entry-level projects.
- **Solution:** A clean, safe console-based system tool with 100% explainable code.

---

## Slide 3: Kernel Information Interfaces (`/proc`, `/sys`, `/dev`)
> **Speaker Notes:**  
> "Vitalscope obtains all its data from the kernel's own information interfaces. We read `/proc/stat` for cumulative CPU jiffies, `/proc/meminfo` for memory zones, `/proc/<pid>` for active process states, `/sys/class` for device driver trees, and `/dev` for device nodes."

| Interface | Data Extracted | Purpose |
|---|---|---|
| `/proc/stat`, `/proc/loadavg` | CPU time states, load averages | Compute CPU usage % over sample windows |
| `/proc/meminfo` | Memory total, available, cached | Calculate memory utilization |
| `/proc/<pid>/stat`, `comm` | PID, command name, process state | Enumerate active running processes |
| `/sys/class/*`, `/dev` | Subsystems, devices, device nodes | Hardware device tree discovery |

---

## Slide 4: System Architecture & Unidirectional Data Flow
> **Speaker Notes:**  
> "Architecturally, Vitalscope follows a unidirectional data flow. Raw pseudo-files pass through an `fs_util` abstraction layer, get parsed by specialized collector classes, are stored in immutable `Snapshot` data structures, and are rendered asynchronously by our `Sampler` thread or `Dashboard` UI."

```
/proc, /sys, /dev  →  fs_util  →  Collectors  →  Snapshot Struct  →  Dashboard / Sampler
   (kernel)          (read)      (parse)        (C++ types)        (presentation)
```

- **`fs_util`**: Centralized file reader supporting `--root` path redirection.
- **Collectors**: Single-responsibility objects (`CpuCollector`, `MemCollector`, `ProcCollector`, `DevCollector`).
- **`Snapshot`**: Immutable data transfer object published under mutex protection.

---

## Slide 5: Multithreading & POSIX Signal Safety
> **Speaker Notes:**  
> "In live monitoring mode, Vitalscope spawns a background `Sampler` thread. To ensure thread safety without data races, the worker thread publishes new snapshots under a `std::mutex`, while readers acquire copies safely. Furthermore, pressing `Ctrl-C` triggers asynchronous signal handling that wakes the condition variable, joins the thread, and exits cleanly with code 0."

```cpp
// Polite Signal Handling & Condition Variable Wait
static std::atomic<bool> g_stop{false};

void on_signal(int sig) {
    g_stop.store(true);
}
```

---

## Slide 6: Professional Software Lifecycle (Stage 1 → Stage 6)
> **Speaker Notes:**  
> "Vitalscope was developed using a strict 6-stage software engineering lifecycle. Every single stage—from problem formulation and requirement engineering to architecture design, prototyping, unit testing, and final release—is tracked with Git commit history and dedicated documentation."

```
Stage 1: Scope & Problem Statement (docs/PROJECT_INTRO.md)
   ↓
Stage 2: Requirements & PRD (docs/PRD.md)
   ↓
Stage 3: System Architecture & UML (docs/DESIGN.md)
   ↓
Stage 4: Prototype & Implementation (src/, Makefile)
   ↓
Stage 5: Fixture-based Testing (tests/, docs/DEVLOG.md)
   ↓
Stage 6: Final Delivery & Presentation (v1.0 Tag)
```

---

## Slide 7: Engineering Highlight: Deterministic Test Fixtures (`--root`)
> **Speaker Notes:**  
> "A major challenge when testing system tools is that host system metrics fluctuate constantly. To solve this, I designed `fs_util` with a `--root` parameter. Passing `--root tests/fixtures` redirects file reads to static mock files, enabling 100% deterministic unit testing on any platform—including Windows or CI/CD pipelines."

```bash
# Deterministic snapshot execution against saved fixtures
./build/vitalscope --root tests/fixtures --once
```

---

## Slide 8: Verification & Quality Assurance Results
> **Speaker Notes:**  
> "Quality assurance was executed through automated unit testing and shell integration scripts. All collectors passed fixture-based unit tests, and CLI modes were validated against live Linux utilities like `top`, `free`, and `ps`."

- **Unit Test Suite (`make test`)**: 100% Passing.
- **Integration Test Suite**: CLI flag handling (`--once`, `--live`, `--root`, `--interval`) verified.
- **Memory Safety**: Clean RAII design with 0 memory leaks.

---

## Slide 9: Interview Viva Defense Q&A Cheatsheet

### Q1: How do you calculate CPU usage % from `/proc/stat`?
**Answer**: `/proc/stat` counters represent cumulative time in jiffies since boot. To compute usage %, Vitalscope takes two readings across a sampling window, calculates the delta in busy time vs. total time, and computes:
$$\text{Usage \%} = \frac{\Delta \text{Busy}}{\Delta \text{Total}} \times 100$$

### Q2: How do you parse process names containing spaces in `/proc/<pid>/stat`?
**Answer**: `/proc/<pid>/stat` formats executable names inside parentheses `(comm)`. Vitalscope locates the *first* `(` and the *last* `)` to safely extract names with spaces or special characters.

### Q3: How do you handle vanishing processes during enumeration?
**Answer**: When reading `/proc`, a process may terminate before its `/proc/<pid>/stat` file is read. `ProcCollector` catches `std::runtime_error` gracefully and skips the terminated process without crashing.

---

## Slide 10: Achievements & Future Roadmap
> **Speaker Notes:**  
> "In summary, Vitalscope is a complete, production-grade system tool that demonstrates modern C++ skills and OS fundamentals. For future work, I have mapped out Stage 7 to build a companion Linux Character Device Driver (`/dev/vitalscope`), Stage 8 for multi-core metrics, and Stage 9 for an `ncurses` terminal UI."

- **Current Achievements**: Zero dependencies, RAII memory safety, deterministic testability, clean staged Git history.
- **Planned Extensions**:
  - 🚀 **Stage 7**: Companion Linux Character Driver (`/dev/vitalscope`) via `ioctl`.
  - 🚀 **Stage 8**: Multi-core CPU breakdown & CSV/JSON export.
  - 🚀 **Stage 9**: Interactive `ncurses` Terminal UI.
