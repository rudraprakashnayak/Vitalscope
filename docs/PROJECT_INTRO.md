# Stage 1 — Project Introduction

**Project:** Vitalscope — A C++ Linux System Monitor & Device Explorer
**Type:** Individual capstone, 20-day Linux / C++ training (Module-9)

## Idea

Vitalscope is a menu-driven command-line tool written in C++17 that reports live system
health: CPU usage and load averages, memory utilisation, running processes, and the
devices the kernel currently knows about. It obtains every number from Linux's own
information interfaces — `/proc`, `/sys` and `/dev` — the same interfaces that tools like
`top`, `free` and `lsdev` use.

## Problem to be solved

Beginners learn Linux commands, C++ syntax and architecture theory in isolation and never
see them interact. There is no small, safe project that answers, concretely:

- How does the kernel expose hardware state (CPU counters, memory zones, device trees)
  to ordinary userspace programs?
- How does a C++ program turn raw pseudo-files like `/proc/stat` into trustworthy numbers?
- How are threads, signals and file I/O combined in a real, testable program?

Vitalscope solves exactly this: one small program in which every training topic has a
visible, explainable role.

## Scope

**In scope**

- CPU usage (delta of `/proc/stat` counters) and load averages (`/proc/loadavg`)
- Memory totals and usage (`/proc/meminfo`)
- Process list: PID, name, state (`/proc/<pid>/comm`, `/proc/<pid>/stat`)
- Device enumeration (`/sys/class/*`) and device-node count (`/dev`)
- Interactive menu, one-shot mode (`--once`) and threaded live mode (`--live`)
- Fixture-based unit tests and a shell integration test
- Full stage documentation and a staged Git history

**Out of scope**

- Writing kernel modules or device drivers (the project *consumes* driver-provided
  interfaces; a driver add-on is listed as future work)
- Graphical UI, network services, persistence, root privileges

## Expected outcome

A single binary, `Vitalscope`, that on any Linux machine (or WSL) prints a correct live
snapshot of CPU, memory, processes and devices without privileges, shuts down cleanly on
`Ctrl-C`, and passes its test suite against saved fixtures.

## Application

- Teaching aid: a readable reference for procfs/sysfs parsing, threads and signals.
- Base for extensions: alerting, CSV logging, a real character-driver companion module.
- Interview artefact: every line is small enough to explain fully in a viva.
