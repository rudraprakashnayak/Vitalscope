# Stage 6 — Presentation Outline & Viva Prep

## Slides (8)

1. **Title** — ProcPulse: a C++ Linux system monitor & device explorer; one-line pitch.
2. **Problem & scope** — why procfs/sysfs; in/out of scope (from `PROJECT_INTRO.md`).
3. **Requirements** — FR/NFR highlights (from `PRD.md`).
4. **Architecture** — the block diagram; "data flows one way: pseudo-files → parsers →
   structs → presentation".
5. **Design details** — class + sequence diagrams; `Snapshot` immutability; why the
   sampler waits on a condition variable.
6. **Implementation highlights** — comm-with-spaces parsing rule; vanishing-process
   tolerance; `--root` testability trick.
7. **Testing** — fixture tree, unit + integration results, live cross-check vs `top`.
8. **Results & future work** — limitations; the character-driver companion idea.

## Demo script (3 minutes)

1. `make && make test` — show both PASSED lines.
2. `./build/procpulse --once` — walk through each section of the snapshot.
3. `./build/procpulse --live 6 --interval 2` — show three sampler lines; `Ctrl-C`;
   show "sampler stopped cleanly".
4. `git log --oneline --decorate | head` — show the staged history and tags.

## Likely viva questions & short answers

- **Where do the numbers come from?** Kernel-maintained pseudo-files; `/proc/stat`
  counters are cumulative jiffies, so usage is a delta between two samples.
- **Why can comm contain spaces and how do you parse it?** Process names are arbitrary;
  take text between the first `(` and the last `)`, state is the next field.
- **Why a thread and not a loop in main?** Live mode must keep sampling while the user
  reads output; the mutex + immutable snapshot keep readers safe.
- **What happens on Ctrl-C?** SIGINT sets a flag; the sampler's condition-variable wait
  wakes, the thread joins, exit code 0.
- **How is this related to device drivers?** `/sys/class/*` and `/dev` are exactly where
  drivers publish devices; ProcPulse consumes that contract. Writing the producer
  (a character driver) is the planned future work.
- **Why fixtures instead of only live tests?** Determinism and portability: parsers are
  proven against known inputs, independent of machine load.
