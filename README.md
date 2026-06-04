# NeuralMesh

NeuralMesh is a compact C++17/Python systems-programming project for Linux-style distributed inference infrastructure. It includes a multithreaded C++ server core, custom thread pool, lock-free work queue, RAII-managed zero-copy buffers, Kafka-style event logging, and a Python validation/benchmarking framework.

This repository is intentionally dependency-light so it can be cloned and built quickly. The checked-in transport is a framed RPC protocol; `docs/grpc-kafka-notes.md` documents how the same service contract maps to production gRPC and Kafka.

## Quick Start

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
python3 python/benchmark.py --configs 24 --json
```

## What It Demonstrates

- C++17 server architecture with `std::thread`, `std::atomic`, compare-and-swap, and RAII.
- Lock-free multi-producer/multi-consumer queue for request dispatch.
- Zero-copy buffer views for parsing framed requests without extra serialization copies.
- Deterministic inference stub useful for validation and performance tests.
- Kafka-like append-only event log for structured request/latency records.
- Python validation harness that runs many configurations and reports throughput/latency.

## Build Targets

- `neuralmesh`: C++ library.
- `neuralmesh_server`: framed RPC server demo.
- `neuralmesh_tests`: dependency-free C++ test suite.

## Interview Talking Points

The most defensible implementation files are:

- `include/neuralmesh/lock_free_queue.hpp`: CAS loop and memory-ordering choices.
- `src/thread_pool.cpp`: worker lifecycle and shutdown.
- `src/framed_rpc.cpp`: length-prefixed protocol and zero-copy request view.
- `python/benchmark.py`: automated validation across system configurations.
