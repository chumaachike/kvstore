# KVStore

A Redis-inspired in-memory key-value store written in modern C++.

Features include concurrent access, TCP networking, append-only file persistence, snapshotting, automated testing, Docker support, and CI/CD with GitHub Actions.

---

## Features

- Thread-safe key-value storage
- TCP client/server architecture
- Multiple concurrent client connections
- Command parsing and execution layer
- Append Only File (AOF) persistence
- Snapshot persistence
- Unit and integration testing with GoogleTest
- Dockerized deployment
- GitHub Actions CI pipeline

---

## Architecture

Client
   |
TCP Server
   |
Parser
   |
Command Executor
   |
KVStore
   |
Persistence Layer
      ├── AOF
      └── Snapshots

---

## Supported Commands

| Command | Description |
|----------|-------------|
| SET key value | Set a value |
| GET key | Retrieve a value |
| DEL key | Delete one or more keys |
| EXISTS key | Check if key exists |
| INCR key | Increment integer value |
| INCRBY key amount | Increment by amount |
| DECR key | Decrement integer value |
| DECRBY key amount | Decrement by amount |
| APPEND key value | Append string |
| QUIT | Close client connection |

---

## Building

### Requirements

- C++20
- CMake 3.20+
- GCC / Clang

### Build

```bash
cmake -S . -B build
cmake --build build
```
#### Run

```bash
./build/kvstore_server
```

### Docker
```bash
docker compose up --build
```
### Connect
```bash
nc localhost 8080
```
## Persistence

### Snapshot
The entire database state can be written to disk and restored on startup.

### Append Only File (AOF)
Every mutating command is logged to an append-only file and replayed during recovery.

#### Recovery sequence: 
1. Load latest snapshot
2. Replay AOF entries
3. Resume serving requests

## Concurrency
The KVStore uses: 
* std::shared_mutex
* std::shared_lock
* std::lock_guard
Readers may execute concurrently while writers obtain exclusive access.

## Testing

### Run Tests

Execute the test suite with:

```bash
ctest --test-dir build --output-on-failure
```
#### Server Test

```bash
source .venv/bin/activate
pytest -v
```

### Framework

The project uses **GoogleTest** for unit and integration testing.

### Test Coverage

Current tests cover:

- Storage operations
- Parser behavior
- Command execution
- Concurrent access scenarios

## Continuous Integration

GitHub Actions automatically:

- Builds the project
- Runs all tests
- Verifies changes on every push and pull request

## Future Improvements

Planned enhancements include:

- Key expiration (TTL)
- RESP protocol compatibility
- Replication
- Publish/Subscribe messaging
- Benchmark suite