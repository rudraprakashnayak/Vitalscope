# Development Log (Stages 4–5)

## Stage 4 — prototype

- **2026-10-01** — Implemented `fs_util` and `CpuCollector` first (thin vertical slice).
  Verified parsing against hand-computed fixture values before touching any other source.
- **2026-10-01** — Issue: first build host was a Windows MinGW (win32 thread model) where
  `std::thread`/`std::mutex` do not exist. Decision: the project targets Linux; all
  verification moved to Ubuntu on WSL2 (`build-essential` installed). No platform shims
  added to the code — the Linux target is a feature of the training, not a limitation.
- **2026-10-01** — Issue: `/proc/<pid>/stat` field 2 (comm) may contain spaces and
  parentheses. Fix: parse name between first `(` and *last* `)`, state = first non-space
  char after it.
- **2026-10-01** — Added remaining collectors, dashboard menu, `--once`. Live numbers
  cross-checked against `top`/`free` on WSL.

## Stage 5 — testing & improvement

- **2026-10-01** — Added fixture tree (`tests/fixtures`) mirroring `/proc`, `/sys`, `/dev`
  so every parser is unit-testable off-Linux and deterministic on Linux.
- **2026-10-01** — Issue: integration script expected `load average: 0.52` but the
  dashboard prints with default float formatting (`0.5 0.6 0.6`). Fix: aligned the
  expectation with the documented output format.
- **2026-10-01** — Issue: processes can exit between listing `/proc` and reading a PID's
  files, causing sporadic `read_file` throws. Fix: per-PID try/catch that skips vanished
  processes (NFR-5).
- **2026-10-01** — Sampler wait changed from plain sleep to `condition_variable::wait_for`
  with a stop predicate, so `stop()` returns immediately instead of after the interval.
- **2026-10-01** — Results: `make test` → ALL UNIT TESTS PASSED, ALL INTEGRATION CHECKS
  PASSED; `--once` and `--live` verified against real WSL procfs (84 processes, 253
  devices observed); clean shutdown on SIGINT confirmed.
