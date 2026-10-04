# Stage 3 — System Design & Architecture

## Architecture

```mermaid
flowchart TD
    subgraph userspace ["Userspace (C++17)"]
        MAIN[main: args + signals] --> DASH[Dashboard]
        MAIN --> SAMP[Sampler thread]
        DASH --> CC[CpuCollector]
        DASH --> MC[MemCollector]
        DASH --> PC[ProcCollector]
        DASH --> DC[DevCollector]
        SAMP --> CC
        SAMP --> MC
        SAMP --> PC
        SAMP --> DC
        CC --> FS[fs_util: read_file / list_dirs]
        MC --> FS
        PC --> FS
        DC --> FS
    end
    subgraph kernel ["Kernel-provided interfaces (read-only)"]
        FS --> PROC[/proc/stat, meminfo, loadavg, pid dirs/]
        FS --> SYS[/sys/class/*/]
        FS --> DEV[/dev/]
    end
```

Data flows one way: pseudo-files → parsers → value structs → presentation. The kernel side
is where drivers and subsystems publish hardware state; ProcPulse is a pure consumer,
which is what makes the project safe and portable.

## Component responsibilities

| Component | Responsibility |
|---|---|
| `fs_util` | `read_file`, `list_dirs`, `list_entries`, `path_exists`; single place that touches paths |
| `CpuCollector` | Parse `/proc/stat` + `/proc/loadavg`; `usage_between()` delta math |
| `MemCollector` | Parse `/proc/meminfo` into `MemSample` |
| `ProcCollector` | Enumerate numeric `/proc` dirs; parse `comm` + `stat` per PID |
| `DevCollector` | Walk `/sys/class/<subsystem>/<device>`; count `/dev` entries |
| `Sampler` | Poll all collectors on a thread at a fixed interval; publish immutable `Snapshot` |
| `Dashboard` | Menu loop and formatted output; also used by `--once` |
| `main` | CLI flags (`--root`, `--interval`, `--once`, `--live`), SIGINT/SIGTERM wiring |

## Key data structures

- `CpuSample` — counter set + `total()` / `busy()` helpers; load averages
- `MemSample` — kB fields + derived `used_kb()` / `usage_percent()`
- `ProcInfo` — `{pid, name, state}`
- `DevInfo` — `{subsystem, name}`
- `Snapshot` — immutable bundle published by `Sampler` under a mutex

## UML

### Class diagram

```mermaid
classDiagram
    class CpuCollector { +sample() CpuSample }
    class MemCollector { +sample() MemSample }
    class ProcCollector { +list() vector~ProcInfo~ }
    class DevCollector { +list() vector~DevInfo~ +count_dev_nodes() long }
    class Sampler { -root_ -interval_ -latest_ +start() +stop() +latest() Snapshot }
    class Dashboard { +run() int +print_snapshot() }
    Sampler --> CpuCollector
    Sampler --> MemCollector
    Sampler --> ProcCollector
    Sampler --> DevCollector
    Dashboard --> CpuCollector
    Dashboard --> MemCollector
    Dashboard --> ProcCollector
    Dashboard --> DevCollector
```

### Sequence diagram (live mode)

```mermaid
sequenceDiagram
    participant U as User
    participant M as main
    participant S as Sampler thread
    participant C as Collectors
    participant P as /proc, /sys
    U->>M: procpulse --live 10
    M->>S: start()
    loop every interval
        S->>C: sample()/list()
        C->>P: open+read pseudo-files
        P-->>C: counter text
        C-->>S: value structs
        S->>S: publish Snapshot (mutex)
    end
    M->>S: latest()
    S-->>M: Snapshot
    M-->>U: formatted line
    U->>M: Ctrl-C (SIGINT)
    M->>S: stop()
    S-->>M: thread joined
```

### State machine (Sampler)

```mermaid
stateDiagram-v2
    [*] --> Idle
    Idle --> Running: start()
    Running --> Running: interval elapsed / publish snapshot
    Running --> Stopped: stop() or SIGINT flag
    Stopped --> [*]
```

## Implementation plan

1. `fs_util` + `CpuCollector` end-to-end, verified against fixtures
2. Remaining collectors, each with a unit test added immediately
3. `Dashboard` menu + `--once`
4. `Sampler` thread + `--live` + signal handling
5. Integration script, devlog, report

## Development environment & tooling

- Ubuntu 24.04+ (or WSL2), `build-essential` (g++ ≥ 9, make), Git
- Build: `make`; tests: `make test`
- Editor-agnostic; C++17, `-Wall -Wextra`

## Git repository & branching strategy

- `main` — tagged releases only (`stage-1`…`stage-6`, `v1.0`)
- `develop` — integration branch
- `feature/<topic>` — one per collector/subsystem, merged via pull request in team settings
  (individual work: merged locally after self-review)
- Commit style: imperative subject, e.g. `add meminfo parser with fixture test`
