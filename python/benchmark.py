#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import statistics
import time
from dataclasses import dataclass, asdict
from contextlib import nullcontext


@dataclass(frozen=True)
class Config:
    workers: int
    feature_count: int
    batch_size: int


@dataclass
class Result:
    workers: int
    feature_count: int
    batch_size: int
    latency_us_p50: float
    latency_us_p95: float
    throughput_rps: float


def synthetic_infer(features: list[float]) -> float:
    return sum(value * (idx + 1) for idx, value in enumerate(features))


def run_config(config: Config) -> Result:
    profiler = nullcontext()
    try:
        import torch

        profiler = torch.profiler.profile(activities=[torch.profiler.ProfilerActivity.CPU])
    except Exception:
        profiler = nullcontext()

    latencies: list[float] = []
    start = time.perf_counter()
    with profiler:
        for request_id in range(config.batch_size):
            features = [((request_id + idx) % 17 - 8) / 8.0 for idx in range(config.feature_count)]
            before = time.perf_counter()
            synthetic_infer(features)
            latencies.append((time.perf_counter() - before) * 1_000_000)
    elapsed = time.perf_counter() - start
    ordered = sorted(latencies)
    p95_index = min(len(ordered) - 1, int(len(ordered) * 0.95))
    return Result(
        workers=config.workers,
        feature_count=config.feature_count,
        batch_size=config.batch_size,
        latency_us_p50=statistics.median(ordered),
        latency_us_p95=ordered[p95_index],
        throughput_rps=config.batch_size / elapsed if elapsed else 0.0,
    )


def build_configs(limit: int) -> list[Config]:
    configs: list[Config] = []
    for workers in [1, 2, 4, 8]:
        for feature_count in [8, 32, 128, 512, 1024]:
            configs.append(Config(workers, feature_count, batch_size=250))
    return configs[:limit]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--configs", type=int, default=20)
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()

    results = [run_config(config) for config in build_configs(args.configs)]
    if args.json:
        print(json.dumps([asdict(result) for result in results], indent=2))
    else:
        for result in results:
            print(result)


if __name__ == "__main__":
    main()
