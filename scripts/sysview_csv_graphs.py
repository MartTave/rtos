#!/usr/bin/env python3
"""
sysview_csv_graphs.py
=====================
Parse a SEGGER SystemView CSV export and generate two graphs:
  1. Periods graph - shows time between consecutive Start Marker events
  2. Durations graph - shows time between Start Marker and Stop Marker

Usage:
  python sysview_csv_graphs.py recording.csv
  python sysview_csv_graphs.py recording.csv --output-dir ./graphs
  python sysview_csv_graphs.py recording.csv --expected-periods 25 50 100
"""

import argparse
import os
import sys

try:
    import matplotlib.pyplot as plt
except ImportError:
    print("ERROR: matplotlib is required. Install with: pip install matplotlib")
    sys.exit(1)

try:
    import numpy as np
except ImportError:
    print("ERROR: numpy is required. Install with: pip install numpy")
    sys.exit(1)

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from csv_parser import (
    build_intervals,
    compute_periods,
    parse_csv,
)
from csv_parser import (
    parse_args as parse_csv_args,
)


def create_periods_graph(
    periods: dict, expected_periods: dict = None, output_dir: str = "./graphs"
):
    """Create a graph showing periods for each marker."""
    fig, ax = plt.subplots(figsize=(12, 6))

    colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]

    for idx, (marker_id, period_values) in enumerate(periods.items()):
        if not period_values:
            continue

        x = range(1, len(period_values) + 1)
        color = colors[idx % len(colors)]

        ax.plot(
            x,
            period_values,
            "o-",
            label=f"Marker {marker_id}",
            color=color,
            markersize=4,
        )

        if expected_periods and marker_id in expected_periods:
            expected = expected_periods[marker_id]
            ax.axhline(
                y=expected,
                color=color,
                linestyle="--",
                alpha=0.5,
                label=f"Marker {marker_id} expected ({expected}ms)",
            )

    ax.set_xlabel("Execution Number", fontsize=12)
    ax.set_ylabel("Period (ms)", fontsize=12)
    ax.set_title(
        "Task Periods - Time Between Consecutive Start Markers", fontsize=14
    )
    ax.legend(loc="upper right")
    ax.grid(True, alpha=0.3)

    output_path = os.path.join(output_dir, "periods_graph.png")
    plt.savefig(output_path, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"Periods graph saved to: {output_path}")

    return output_path


def create_durations_graph(
    intervals: dict,
    expected_durations: dict = None,
    output_dir: str = "./graphs",
):
    """Create a graph showing durations for each marker."""
    fig, ax = plt.subplots(figsize=(12, 6))

    colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]

    for idx, (marker_id, interval_list) in enumerate(intervals.items()):
        if not interval_list:
            continue

        duration_values = [iv.duration_ms for iv in interval_list]
        x = range(1, len(duration_values) + 1)
        color = colors[idx % len(colors)]

        ax.plot(
            x,
            duration_values,
            "s-",
            label=f"Marker {marker_id}",
            color=color,
            markersize=4,
        )

        if expected_durations and marker_id in expected_durations:
            expected = expected_durations[marker_id]
            ax.axhline(
                y=expected,
                color=color,
                linestyle="--",
                alpha=0.5,
                label=f"Marker {marker_id} expected ({expected}ms)",
            )

    ax.set_xlabel("Execution Number", fontsize=12)
    ax.set_ylabel("Duration (ms)", fontsize=12)
    ax.set_title(
        "Task Durations - Time Between Start and Stop Markers", fontsize=14
    )
    ax.legend(loc="upper right")
    ax.grid(True, alpha=0.3)

    output_path = os.path.join(output_dir, "durations_graph.png")
    plt.savefig(output_path, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"Durations graph saved to: {output_path}")

    return output_path


def compute_stats(values):
    """Compute basic statistics for a list of values."""
    if not values:
        return {}
    return {
        "n": len(values),
        "mean": np.mean(values),
        "min": np.min(values),
        "max": np.max(values),
        "std": np.std(values),
    }


def print_summary(periods: dict, intervals: dict):
    """Print a summary of the parsed data."""
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)

    all_markers = sorted(set(list(periods.keys()) + list(intervals.keys())))

    for marker_id in all_markers:
        print(f"\nMarker {marker_id}:")

        period_values = periods.get(marker_id, [])
        if period_values:
            stats = compute_stats(period_values)
            print(
                f"  Periods:  n={stats['n']}, mean={stats['mean']:.3f}ms, "
                f"min={stats['min']:.3f}ms, max={stats['max']:.3f}ms, "
                f"std={stats['std']:.3f}ms"
            )

        interval_list = intervals.get(marker_id, [])
        if interval_list:
            duration_values = [iv.duration_ms for iv in interval_list]
            stats = compute_stats(duration_values)
            print(
                f"  Durations: n={stats['n']}, mean={stats['mean']:.3f}ms, "
                f"min={stats['min']:.3f}ms, max={stats['max']:.3f}ms, "
                f"std={stats['std']:.3f}ms"
            )


def main():
    parser = argparse.ArgumentParser(
        description="Parse SystemView CSV and generate period/duration graphs",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  Basic usage:
    %(prog)s recording.csv

  Specify output directory:
    %(prog)s recording.csv --output-dir ./graphs

  Specify expected periods (ms) for validation lines:
    %(prog)s recording.csv --expected-periods 25 50 100

  Specify expected durations:
    %(prog)s recording.csv --expected-periods 25 50 100 --expected-durations 15 10 5
""",
    )
    parser.add_argument("csv_file", help="Path to SystemView CSV export file")
    parser.add_argument(
        "--output-dir",
        "-o",
        type=str,
        default=".",
        help="Output directory for graphs (default: current directory)",
    )
    parser.add_argument(
        "--expected-periods",
        type=float,
        nargs="+",
        help="Expected periods in ms for each marker (marker 0, 1, 2, ...)",
    )
    parser.add_argument(
        "--expected-durations",
        type=float,
        nargs="+",
        help="Expected durations in ms for each marker (marker 0, 1, 2, ...)",
    )
    parser.add_argument(
        "--no-summary",
        action="store_true",
        help="Suppress summary statistics output",
    )

    args = parser.parse_args()

    if not os.path.isfile(args.csv_file):
        print(f"ERROR: File not found: {args.csv_file}", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.output_dir, exist_ok=True)

    print(f"Parsing {args.csv_file} ...")
    events, warnings = parse_csv(args.csv_file, verbose=False)

    for w in warnings:
        print(f"WARNING: {w}")

    if not events:
        print("No marker events found in CSV.")
        sys.exit(1)

    print(f"Found {len(events)} marker event(s)")

    intervals = build_intervals(events)
    periods = compute_periods(events)

    expected_periods = None
    expected_durations = None

    if args.expected_periods:
        expected_periods = {i: p for i, p in enumerate(args.expected_periods)}
    if args.expected_durations:
        expected_durations = {
            i: d for i, d in enumerate(args.expected_durations)
        }

    create_periods_graph(periods, expected_periods, args.output_dir)
    create_durations_graph(intervals, expected_durations, args.output_dir)

    if not args.no_summary:
        print_summary(periods, intervals)

    print(f"\nGraphs saved to: {args.output_dir}/")


if __name__ == "__main__":
    main()
