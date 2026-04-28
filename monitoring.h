// ============================================================
// IFAMDS -- Intelligent Forest Advisory & Multi-Structure
//           Decision System
// File   : monitoring.h
// Layer  : System Monitoring & Adaptive Optimization Layer
//
// Responsibilities:
//   - Execution latency tracking  (Latency = Finish - Start)
//   - Subsystem load analysis     (Load = ActiveTasks / Capacity)
//   - Bottleneck detection        (slowest subsystem flagged)
//   - Performance optimization    (rebalance / adjust limits)
//   - System health dashboard     (Menu 9.5)
//
// Subsystems tracked (7 total):
//   ARRAYS | LINKEDLIST | QUEUES | TREES | GRAPHS | HASH | SCENARIOS
//
// Authors: Partner B
// Course : CL2001 -- Data Structures 2026
// ============================================================

#ifndef MONITORING_H
#define MONITORING_H

#include "constants.h"
#include <string>
#include <ctime>

// ------------------------------------------------------------
// Constants
// ------------------------------------------------------------
const int  NUM_SUBSYSTEMS   = 7;
const int  MAX_LATENCY_LOG  = 50;   // rolling log entries per subsystem
const float LOAD_WARNING    = 0.70f; // warn if load exceeds 70%
const float LOAD_CRITICAL   = 0.90f; // critical if load exceeds 90%
const float LATENCY_WARNING = 5.0f;  // warn if latency > 5 ms
const float LATENCY_CRITICAL= 15.0f; // critical if latency > 15 ms

// Subsystem index constants
const int MON_ARRAYS     = 0;
const int MON_LINKEDLIST = 1;
const int MON_QUEUES     = 2;
const int MON_TREES      = 3;
const int MON_GRAPHS     = 4;
const int MON_HASH       = 5;
const int MON_SCENARIOS  = 6;

// ------------------------------------------------------------
// enum HealthStatus
// Overall health state of a subsystem or the whole system.
// ------------------------------------------------------------
enum HealthStatus
{
    HEALTH_OK       = 0,
    HEALTH_WARNING  = 1,
    HEALTH_CRITICAL = 2
};

// ------------------------------------------------------------
// struct LatencyEntry
// One timed operation record for a subsystem.
// ------------------------------------------------------------
struct LatencyEntry
{
    float       latencyMs;      // Time taken in milliseconds
    std::string operation;      // Name of the operation measured
    int         timestamp;      // Logical clock value when recorded
};

// ------------------------------------------------------------
// struct SubsystemMetrics
// Holds all monitoring data for one subsystem.
// ------------------------------------------------------------
struct SubsystemMetrics
{
    std::string   name;                         // Subsystem name
    int           activeTasks;                  // Current task count
    int           capacity;                     // Max task capacity
    float         totalLatencyMs;               // Cumulative latency
    float         peakLatencyMs;                // Highest single latency
    int           operationCount;               // Total ops recorded
    int           errorCount;                   // Errors detected
    HealthStatus  status;                       // Current health state
    LatencyEntry  latencyLog[MAX_LATENCY_LOG];  // Rolling latency log
    int           logCount;                     // Entries in log
};

// ============================================================
// class SystemMonitor
// Tracks metrics for all NUM_SUBSYSTEMS subsystems.
// Provides load analysis, bottleneck detection, and a health
// dashboard.
// ============================================================
class SystemMonitor
{
private:
    SubsystemMetrics subsystems[NUM_SUBSYSTEMS]; // One entry per subsystem
    int              logicalClock;               // Global logical timestamp
    int              totalErrors;                // System-wide error count

    // Returns health status based on load and latency values.
    // Time Complexity: O(1)
    HealthStatus evaluateHealth(float load, float avgLatency) const;

    // Returns the name string for a subsystem index.
    // Time Complexity: O(1)
    std::string statusLabel(HealthStatus s) const;

    // Prints a load bar (e.g. [####......] 40%).
    // Time Complexity: O(1)
    void printLoadBar(float load) const;

public:

    // Constructor: initialises all subsystems with default capacities.
    // Time Complexity: O(n)
    SystemMonitor();

    // ========================================================
    // Menu 9.1 -- Monitor System Load
    // ========================================================

    // Prints load ratio for every subsystem.
    // Formula: Load = activeTasks / capacity
    // Flags WARNING / CRITICAL thresholds.
    // Time Complexity: O(n)
    void monitorLoad();

    // ========================================================
    // Menu 9.2 -- Track Execution Time
    // ========================================================

    // Records a latency measurement for a subsystem.
    // Called internally when an operation completes.
    // Time Complexity: O(1)
    void recordLatency(int subsystemId, float latencyMs,
                       const std::string& operation);

    // Prints the latency log for all subsystems.
    // Time Complexity: O(n)
    void trackExecutionTime();

    // ========================================================
    // Menu 9.3 -- Detect Bottlenecks
    // ========================================================

    // Finds the subsystem with the highest average latency
    // and the subsystem with the highest load.
    // Prints the identified bottleneck and recommended action.
    // Time Complexity: O(n)
    void detectBottlenecks();

    // ========================================================
    // Menu 9.4 -- Optimize Performance
    // ========================================================

    // Rebalances task load: if any subsystem load > LOAD_CRITICAL,
    // moves 20% of its tasks to the least-loaded subsystem.
    // Adjusts latency warnings dynamically.
    // Time Complexity: O(n)
    void optimizePerformance();

    // ========================================================
    // Menu 9.5 -- View System Health Dashboard
    // ========================================================

    // Prints a full health dashboard: all subsystems, their
    // load bars, latency averages, error counts, and status.
    // Time Complexity: O(n)
    void viewSystemHealth();

    // ========================================================
    // Simulation helpers (used by Scenario menu)
    // ========================================================

    // Simulates an operation on a subsystem:
    //   increments activeTasks, records a synthetic latency,
    //   re-evaluates health.
    // Time Complexity: O(1)
    void simulateOperation(int subsystemId, float latencyMs,
                           const std::string& opName);

    // Simulates a burst of load on subsystemId (for overload scenario).
    // Time Complexity: O(k) where k = burstSize
    void simulateBurst(int subsystemId, int burstSize);

    // Resets active task count for a subsystem (post-recovery).
    // Time Complexity: O(1)
    void resetSubsystem(int subsystemId);

    // Returns the overall system health (worst among all subsystems).
    // Time Complexity: O(n)
    HealthStatus overallHealth() const;

    // Returns the index of the most-loaded subsystem.
    // Time Complexity: O(n)
    int bottleneckIndex() const;
};

#endif // MONITORING_H
