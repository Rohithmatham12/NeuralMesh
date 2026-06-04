# gRPC, Kafka, perf, and Valgrind Notes

The local build uses a dependency-free framed RPC transport so the project remains easy to verify. The service boundary is intentionally shaped like a gRPC service:

```proto
service NeuralMesh {
  rpc Infer(InferRequest) returns (InferResponse);
}
```

`InferRequest` maps to a length-prefixed binary frame:

- 4-byte little-endian request id.
- 4-byte little-endian feature count.
- `feature_count` little-endian float32 values.

The event log in `src/event_log.cpp` is a Kafka-style append-only record stream. In a production Kafka integration, each `EventRecord` becomes a message keyed by request id and published to a topic such as `neuralmesh.inference.latency`.

Suggested Linux profiling commands after installing tools:

```bash
perf stat ./build/neuralmesh_server
perf record -g ./build/neuralmesh_server
valgrind --leak-check=full ./build/neuralmesh_tests
```

The serialization bottleneck addressed in this implementation is avoiding per-request copies by parsing request payloads through `BufferView` and passing views through the dispatch path.
