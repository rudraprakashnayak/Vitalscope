# Stage 2 — Project Requirements Document (PRD)

## Functional requirements

| ID | Requirement |
|---|---|
| FR-1 | Read and parse `/proc/stat`; compute CPU usage % between two samples |
| FR-2 | Read `/proc/loadavg`; report 1/5/15-minute load averages |
| FR-3 | Read `/proc/meminfo`; report total, used, available, cached and usage % |
| FR-4 | List processes with PID, name and state from `/proc/<pid>` |
| FR-5 | Enumerate devices per subsystem from `/sys/class/*`; count `/dev` nodes |
| FR-6 | Provide an interactive menu (CPU / memory / processes / devices / snapshot / quit) |
| FR-7 | Provide `--once` (single snapshot) and `--live <secs>` (threaded sampler) modes |
| FR-8 | Accept `--root <path>` so all collectors read from an alternate root (testability) |
| FR-9 | Shut down cleanly on SIGINT/SIGTERM (stop sampler thread, exit 0) |

## Non-functional requirements

| ID | Requirement |
|---|---|
| NFR-1 | No root privileges required; read-only access to procfs/sysfs |
| NFR-2 | Runs on any Linux with g++ (C++17); verified on Ubuntu (WSL2), g++ 15.2 |
| NFR-3 | Unit-testable without a live system (fixtures via `--root`) |
| NFR-4 | Sampler thread must not busy-wait; interval-based sleep with interruptible wait |
| NFR-5 | Missing or vanishing proc entries degrade gracefully (skip, never crash) |

## Modules

1. `fs_util` — file read + directory listing primitives
2. `cpu_collector`, `mem_collector`, `proc_collector`, `dev_collector` — one per source
3. `sampler` — background polling thread producing immutable snapshots
4. `dashboard` — console presentation layer
5. `main` — argument parsing, signal wiring, mode dispatch

## Deliverables per stage

| Stage | Deliverable |
|---|---|
| 1 | This intro + `PROJECT_INTRO.md` |
| 2 | This PRD + development plan (below) |
| 3 | `DESIGN.md`, headers, Makefile, repo + branching setup |
| 4 | Working collectors + CLI prototype, `DEVLOG.md` opened |
| 5 | Unit + integration tests green, devlog closed |
| 6 | `FINAL_REPORT.md`, `PRESENTATION.md`, tag `v1.0` |

## Development plan & timeline

| Week | Stages | Exit criterion |
|---|---|---|
| 1 | 1–3 | Docs approved by self-review; interfaces compile empty |
| 2 | 4 | `--once` prints correct numbers on fixtures and live system |
| 3 | 5 | `make test` green; devlog entries for every fixed bug |
| 3 | 6 | Demo rehearsed; report + slides written; `v1.0` tagged |

## Risks

- procfs field layouts differ across kernels → parsers tolerate missing trailing fields
- processes vanish mid-listing → per-PID read failures are skipped, not fatal
- non-Linux build hosts → fixtures keep tests meaningful; live modes report a clear error
