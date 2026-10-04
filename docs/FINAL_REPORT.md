# Stage 6 — Final Report

## What was built

Vitalscope v1.0: a C++17 Linux system monitor and device explorer. One binary with three
modes — interactive menu, `--once` snapshot, `--live <secs>` threaded sampling — reading
exclusively from `/proc`, `/sys` and `/dev`.

## Architecture recap

Four collectors over a shared `fs_util` layer produce value structs; a `Sampler` thread
publishes immutable snapshots under a mutex; a `Dashboard` presents them. See
`docs/DESIGN.md` for diagrams.

## Testing & results

| Suite | Command | Result |
|---|---|---|
| Unit (parsers, fixtures) | `make test` | ALL UNIT TESTS PASSED |
| Integration (binary vs fixtures) | `make test` | ALL INTEGRATION CHECKS PASSED |
| Live procfs | `./build/Vitalscope --once` / `--live 3` | correct values vs `top`/`free`; clean SIGINT shutdown |

Verified on Ubuntu (WSL2), g++ 15.2, `-std=c++17 -Wall -Wextra` with zero warnings.

## Achievements

- Every training topic exercised in explainable code: procfs/sysfs (Linux), counter and
  memory-hierarchy semantics (computer architecture), driver-published interfaces
  (hardware/software), threads + signals + file I/O (system programming), RAII/STL/
  exceptions (C++).
- Deterministic testing without a live system via the `--root` fixture mechanism.
- Staged Git history (`stage-1`…`stage-6`, `v1.0`) modelling the required process.

## Limitations

- Read-only consumer: no kernel module, so driver *implementation* is not demonstrated.
- CPU usage needs two samples; `--once` reports 0.0 % for the window by design.
- No per-core breakdown, no history/plotting, no alerts.

## Future improvements

- Companion character-device driver (`/dev/Vitalscope`) exposing the same stats via ioctl,
  completing the kernel-space half of the training.
- Per-CPU rows from additional `/proc/stat` lines; CSV/JSON export; ncurses TUI.
- Configurable alert thresholds with signal-based notification.

## Reproduction

```bash
make && make test
./build/Vitalscope --once
```
