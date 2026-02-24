*This project has been created as part of the 42 curriculum by nai-sfi.*

## Description

Codexion is a multithreaded simulation of the classic Dining Philosophers concurrency problem. Coders sit around a circular table sharing USB dongles. Each coder must acquire two adjacent dongles to compile quantum code, then debug and refactor before repeating the cycle. The simulation ends when a coder burns out or when all coders complete their required number of compiles.

## Instructions

### Compilation

```
cd coders/
make
```

### Usage

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All arguments are mandatory. Times are in milliseconds. `scheduler` is either `fifo` or `edf`.

### Examples

```
./codexion 5 800 200 100 100 0 0 fifo
```
```
./codexion 3 600 150 80 80 10 50 edf
```
```
./codexion 1 300 500 100 100 0 0 fifo
```

### Output Format

```
timestamp_in_ms coder_id action
```

Actions:
- `has taken a dongle`
- `is compiling`
- `is debugging`
- `is refactoring`
- `burned out`

## Blocking Cases Handled

### Deadlock Prevention

Deadlock requires four Coffman conditions: mutual exclusion, hold and wait, no preemption, and circular wait. The solution breaks circular wait by enforcing ordered resource acquisition — each coder always picks up the lower-numbered dongle first. This prevents the cycle of dependencies that causes deadlock.

### Starvation Prevention

FIFO scheduler uses a min-heap keyed by sequence numbers, ensuring coders are granted dongles in the order they requested them. EDF scheduler uses each coder's burnout deadline as priority, granting access to the coder closest to burning out first.

### Cooldown Handling

Each dongle tracks a `release_time`. A coder attempting to acquire a dongle is blocked until `release_time + cooldown` has passed. This is implemented using `pthread_cond_timedwait` so threads sleep efficiently rather than busy-waiting.

### Burnout Detection

A dedicated monitor thread polls every 0.5ms. It checks whether `now - last_compile_start >= time_to_burnout` for each coder. Detection is accurate within a 10ms tolerance.

### Log Serialization

A `log_mutex` is held whenever writing to stdout, preventing interleaved output from concurrent coder threads.

## Thread Synchronization Mechanisms

### Mutexes

| Mutex | Protects |
|---|---|
| `dongle.mutex` | `available`, `release_time`, queue |
| `running_mutex` | `running` flag |
| `log_mutex` | stdout |
| `counter_mutex` | `fifo_counter` |
| `coder.state_mutex` | `last_compile_start`, `compile_count` |

### Condition Variables

One condition variable per dongle, paired with its dongle mutex. Uses `broadcast` instead of `signal` to wake all waiters when a dongle becomes available. Uses `timedwait` instead of `wait` to handle cooldown expiry and simulation stop detection.

### Race Condition Prevention

All shared data is accessed under the appropriate lock. Lock ordering is enforced globally to prevent deadlocks between mutexes. Per-coder state mutexes allow the monitor thread to safely read coder state.

### Simulation Stop Coordination

`set_running` sets the `running` flag to 0, then `wake_all_dongles` broadcasts on every dongle's condition variable so all blocked threads wake and check the stop condition.

### Custom Priority Queue (Min-Heap)

| Operation | Complexity |
|---|---|
| `heap_push` | O(log n) |
| `heap_pop` | O(log n) |
| `heap_peek_id` | O(1) |
| `heap_remove_coder` | O(n) |

One heap is stored per dongle and protected by that dongle's mutex.

## Resources

- Dijkstra (1965) — *Cooperating Sequential Processes*
- Coffman (1971) — *System Deadlocks*
- Silberschatz — *Operating System Concepts*, Chapter 7
- POSIX Threads Programming Tutorial: https://computing.llnl.gov/tutorials/pthreads/
- `man pthreads`, `man pthread_mutex_lock`, `man pthread_cond_wait`

### AI Usage

AI (Claude) was used as a learning and tutoring tool for: understanding concurrency concepts, learning the POSIX threads API, designing architecture and data structures, understanding min-heap, and debugging compilation and linker errors. All code was written, understood, and tested by the student.
