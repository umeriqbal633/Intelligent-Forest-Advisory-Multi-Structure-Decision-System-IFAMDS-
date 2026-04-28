// ============================================================
// IFAMDS -- Intelligent Forest Advisory & Multi-Structure
//           Decision System
// File   : monitoring.cpp
// Layer  : System Monitoring & Adaptive Optimization Layer
//
// Authors: Partner B
// Course : CL2001 -- Data Structures 2026
// ============================================================

#include "monitoring.h"
#include <iostream>
#include <iomanip>
#include <string>

// ============================================================
// Private helpers
// ============================================================

// evaluateHealth: classify based on load and latency. O(1)
HealthStatus SystemMonitor::evaluateHealth(float load,
                                            float avgLatency) const
{
    if (load >= LOAD_CRITICAL || avgLatency >= LATENCY_CRITICAL)
        return HEALTH_CRITICAL;
    if (load >= LOAD_WARNING  || avgLatency >= LATENCY_WARNING)
        return HEALTH_WARNING;
    return HEALTH_OK;
}

// statusLabel: enum -> display string. O(1)
std::string SystemMonitor::statusLabel(HealthStatus s) const
{
    switch (s)
    {
        case HEALTH_OK:       return "[OK]     ";
        case HEALTH_WARNING:  return "[WARN]   ";
        case HEALTH_CRITICAL: return "[CRIT]   ";
        default:              return "[???]    ";
    }
}

// printLoadBar: ASCII load bar of width 20. O(1)
void SystemMonitor::printLoadBar(float load) const
{
    const int W = 20;
    int filled = (int)(load * W);
    if (filled > W) filled = W;

    std::cout << "[";
    for (int i = 0; i < W; i++)
        std::cout << (i < filled ? '#' : '.');
    std::cout << "] "
              << std::fixed << std::setprecision(0)
              << (load * 100.0f) << "%";
}

// ============================================================
// Constructor
// Seeds each subsystem with a name and realistic capacity.
// Time Complexity: O(n)
// ============================================================
SystemMonitor::SystemMonitor() : logicalClock(0), totalErrors(0)
{
    // Names and capacities per subsystem
    const char* names[NUM_SUBSYSTEMS] = {
        "Arrays", "LinkedList", "Queues",
        "Trees",  "Graphs",    "HashTable", "Scenarios"
    };
    const int caps[NUM_SUBSYSTEMS] = { 100, 200, 50, 100, 50, 50, 20 };

    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        subsystems[i].name           = names[i];
        subsystems[i].activeTasks    = 0;
        subsystems[i].capacity       = caps[i];
        subsystems[i].totalLatencyMs = 0.0f;
        subsystems[i].peakLatencyMs  = 0.0f;
        subsystems[i].operationCount = 0;
        subsystems[i].errorCount     = 0;
        subsystems[i].status         = HEALTH_OK;
        subsystems[i].logCount       = 0;
    }

    // Seed realistic baseline activity
    simulateOperation(MON_ARRAYS,     1.2f, "ingestReading");
    simulateOperation(MON_LINKEDLIST, 0.8f, "insertEvent");
    simulateOperation(MON_QUEUES,     0.5f, "enqueue");
    simulateOperation(MON_TREES,      2.1f, "insertNode");
    simulateOperation(MON_GRAPHS,     3.4f, "bfsTraversal");
    simulateOperation(MON_HASH,       0.3f, "hashInsert");
    simulateOperation(MON_SCENARIOS,  4.8f, "scenarioInit");
}

// ============================================================
// Menu 9.1 -- monitorLoad
// Prints load ratio for every subsystem.
// Load = activeTasks / capacity
// Time Complexity: O(n)
// ============================================================
void SystemMonitor::monitorLoad()
{
    std::cout << "\n  == System Load Monitor ==========================\n";
    std::cout << "  " << std::left
              << std::setw(14) << "Subsystem"
              << std::setw(10) << "Tasks"
              << std::setw(10) << "Capacity"
              << "Load\n";
    std::cout << "  " << std::string(56, '-') << "\n";

    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        SubsystemMetrics& sm = subsystems[i];
        float load = (sm.capacity > 0)
                   ? (float)sm.activeTasks / sm.capacity
                   : 0.0f;

        sm.status = evaluateHealth(load,
            sm.operationCount > 0
                ? sm.totalLatencyMs / sm.operationCount
                : 0.0f);

        std::cout << "  " << statusLabel(sm.status)
                  << std::left << std::setw(12) << sm.name
                  << std::setw(6)  << sm.activeTasks
                  << "/" << std::setw(7) << sm.capacity
                  << "  ";
        printLoadBar(load);
        std::cout << "\n";
    }

    std::cout << "  " << std::string(56, '=') << "\n";
    std::cout << "  Thresholds: WARNING >= "
              << (int)(LOAD_WARNING  * 100) << "%   "
              << "CRITICAL >= "
              << (int)(LOAD_CRITICAL * 100) << "%\n";
}

// ============================================================
// recordLatency: store one entry in the rolling log. O(1)
// ============================================================
void SystemMonitor::recordLatency(int id, float ms,
                                   const std::string& op)
{
    if (id < 0 || id >= NUM_SUBSYSTEMS) return;

    SubsystemMetrics& sm = subsystems[id];
    int slot = sm.logCount % MAX_LATENCY_LOG;

    sm.latencyLog[slot].latencyMs  = ms;
    sm.latencyLog[slot].operation  = op;
    sm.latencyLog[slot].timestamp  = logicalClock++;

    sm.totalLatencyMs += ms;
    sm.operationCount++;
    if (ms > sm.peakLatencyMs) sm.peakLatencyMs = ms;
    if (sm.logCount < MAX_LATENCY_LOG) sm.logCount++;
}

// ============================================================
// Menu 9.2 -- trackExecutionTime
// Prints latency summary for all subsystems.
// Time Complexity: O(n)
// ============================================================
void SystemMonitor::trackExecutionTime()
{
    std::cout << "\n  == Execution Latency Tracker ====================\n";
    std::cout << "  " << std::left
              << std::setw(14) << "Subsystem"
              << std::setw(10) << "Ops"
              << std::setw(12) << "Avg(ms)"
              << std::setw(12) << "Peak(ms)"
              << "Last Operation\n";
    std::cout << "  " << std::string(62, '-') << "\n";

    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        SubsystemMetrics& sm = subsystems[i];
        float avg = (sm.operationCount > 0)
                  ? sm.totalLatencyMs / sm.operationCount
                  : 0.0f;

        std::string lastOp = "(none)";
        if (sm.logCount > 0)
        {
            int lastSlot = (sm.logCount - 1) % MAX_LATENCY_LOG;
            lastOp = sm.latencyLog[lastSlot].operation;
        }

        std::string flag = "";
        if (avg >= LATENCY_CRITICAL) flag = " [!!CRITICAL]";
        else if (avg >= LATENCY_WARNING) flag = " [!WARN]";

        std::cout << "  " << std::left
                  << std::setw(14) << sm.name
                  << std::setw(10) << sm.operationCount
                  << std::setw(12) << std::fixed << std::setprecision(2) << avg
                  << std::setw(12) << sm.peakLatencyMs
                  << lastOp << flag << "\n";
    }
    std::cout << "  " << std::string(62, '=') << "\n";
}

// ============================================================
// Menu 9.3 -- detectBottlenecks
// Identifies the slowest subsystem (highest avg latency)
// and the most loaded.
// Time Complexity: O(n)
// ============================================================
void SystemMonitor::detectBottlenecks()
{
    std::cout << "\n  == Bottleneck Detection =========================\n";

    int   slowestIdx  = 0;
    float slowestAvg  = 0.0f;
    int   heaviestIdx = 0;
    float heaviestLoad= 0.0f;

    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        SubsystemMetrics& sm = subsystems[i];

        float avg  = (sm.operationCount > 0)
                   ? sm.totalLatencyMs / sm.operationCount : 0.0f;
        float load = (sm.capacity > 0)
                   ? (float)sm.activeTasks / sm.capacity   : 0.0f;

        if (avg  > slowestAvg)   { slowestAvg  = avg;  slowestIdx  = i; }
        if (load > heaviestLoad) { heaviestLoad = load; heaviestIdx = i; }
    }

    std::cout << "  Latency bottleneck : "
              << subsystems[slowestIdx].name
              << "  (avg " << std::fixed << std::setprecision(2)
              << slowestAvg << " ms)\n";

    if (slowestAvg >= LATENCY_CRITICAL)
        std::cout << "  Action : [!!] Reduce ops on "
                  << subsystems[slowestIdx].name
                  << " -- consider caching results.\n";
    else if (slowestAvg >= LATENCY_WARNING)
        std::cout << "  Action : [!]  Monitor "
                  << subsystems[slowestIdx].name
                  << " closely -- approaching limit.\n";
    else
        std::cout << "  Action : [OK] No latency bottleneck detected.\n";

    std::cout << "\n";
    std::cout << "  Load bottleneck    : "
              << subsystems[heaviestIdx].name
              << "  (" << std::fixed << std::setprecision(0)
              << (heaviestLoad * 100.0f) << "% load)\n";

    if (heaviestLoad >= LOAD_CRITICAL)
        std::cout << "  Action : [!!] Redistribute tasks from "
                  << subsystems[heaviestIdx].name << " immediately.\n";
    else if (heaviestLoad >= LOAD_WARNING)
        std::cout << "  Action : [!]  Watch "
                  << subsystems[heaviestIdx].name
                  << " -- nearing capacity.\n";
    else
        std::cout << "  Action : [OK] Load balanced across all subsystems.\n";

    std::cout << "  " << std::string(50, '=') << "\n";
}

// ============================================================
// Menu 9.4 -- optimizePerformance
// If any subsystem load > LOAD_CRITICAL, move 20% of its
// tasks to the least-loaded subsystem.
// Time Complexity: O(n)
// ============================================================
void SystemMonitor::optimizePerformance()
{
    std::cout << "\n  == Performance Optimization =====================\n";

    bool anyOptimized = false;

    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        SubsystemMetrics& sm = subsystems[i];
        float load = (sm.capacity > 0)
                   ? (float)sm.activeTasks / sm.capacity : 0.0f;

        if (load < LOAD_CRITICAL) continue;

        // Find least-loaded subsystem (excluding self)
        int   minIdx  = -1;
        float minLoad = 1e9f;
        for (int j = 0; j < NUM_SUBSYSTEMS; j++)
        {
            if (j == i) continue;
            float jl = (subsystems[j].capacity > 0)
                     ? (float)subsystems[j].activeTasks / subsystems[j].capacity
                     : 0.0f;
            if (jl < minLoad) { minLoad = jl; minIdx = j; }
        }

        if (minIdx == -1) continue;

        int transfer = sm.activeTasks / 5;   // 20%
        if (transfer < 1) transfer = 1;

        sm.activeTasks -= transfer;
        subsystems[minIdx].activeTasks += transfer;

        std::cout << "  Moved " << transfer << " tasks: "
                  << sm.name << " --> "
                  << subsystems[minIdx].name << "\n";

        anyOptimized = true;
    }

    if (!anyOptimized)
        std::cout << "  System is balanced -- no rebalancing needed.\n";

    // Re-evaluate health for all subsystems
    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        SubsystemMetrics& sm = subsystems[i];
        float load = (sm.capacity > 0)
                   ? (float)sm.activeTasks / sm.capacity : 0.0f;
        float avg  = (sm.operationCount > 0)
                   ? sm.totalLatencyMs / sm.operationCount : 0.0f;
        sm.status  = evaluateHealth(load, avg);
    }

    std::cout << "  Health status recalculated for all subsystems.\n";
    std::cout << "  " << std::string(50, '=') << "\n";
}

// ============================================================
// Menu 9.5 -- viewSystemHealth
// Full health dashboard.
// Time Complexity: O(n)
// ============================================================
void SystemMonitor::viewSystemHealth()
{
    std::cout << "\n";
    std::cout << "  ================================================\n";
    std::cout << "  IFAMDS -- System Health Dashboard\n";
    std::cout << "  ================================================\n";

    // Overall health
    HealthStatus overall = overallHealth();
    std::cout << "  Overall Status : " << statusLabel(overall) << "\n";
    std::cout << "  Logical Clock  : " << logicalClock << "\n";
    std::cout << "  Total Errors   : " << totalErrors  << "\n";
    std::cout << "  ------------------------------------------------\n";

    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        SubsystemMetrics& sm = subsystems[i];
        float load = (sm.capacity > 0)
                   ? (float)sm.activeTasks / sm.capacity : 0.0f;
        float avg  = (sm.operationCount > 0)
                   ? sm.totalLatencyMs / sm.operationCount : 0.0f;

        sm.status = evaluateHealth(load, avg);

        std::cout << "\n  " << statusLabel(sm.status)
                  << sm.name << "\n";
        std::cout << "    Load    : ";
        printLoadBar(load);
        std::cout << "  (" << sm.activeTasks << "/" << sm.capacity << ")\n";
        std::cout << "    Latency : avg="
                  << std::fixed << std::setprecision(2) << avg
                  << " ms   peak=" << sm.peakLatencyMs
                  << " ms   ops=" << sm.operationCount << "\n";
        std::cout << "    Errors  : " << sm.errorCount << "\n";
    }

    std::cout << "\n  ================================================\n";
}

// ============================================================
// simulateOperation: add one task + record latency. O(1)
// ============================================================
void SystemMonitor::simulateOperation(int id, float ms,
                                       const std::string& op)
{
    if (id < 0 || id >= NUM_SUBSYSTEMS) return;
    subsystems[id].activeTasks++;
    recordLatency(id, ms, op);

    float load = (subsystems[id].capacity > 0)
               ? (float)subsystems[id].activeTasks / subsystems[id].capacity
               : 0.0f;
    float avg  = (subsystems[id].operationCount > 0)
               ? subsystems[id].totalLatencyMs / subsystems[id].operationCount
               : 0.0f;
    subsystems[id].status = evaluateHealth(load, avg);
}

// ============================================================
// simulateBurst: rapidly add burstSize tasks to subsystem. O(k)
// ============================================================
void SystemMonitor::simulateBurst(int id, int burstSize)
{
    if (id < 0 || id >= NUM_SUBSYSTEMS) return;
    std::cout << "  Burst: adding " << burstSize
              << " tasks to " << subsystems[id].name << "\n";

    for (int k = 0; k < burstSize; k++)
    {
        float syntheticLatency = 2.0f + k * 0.5f;  // grows with burst
        simulateOperation(id, syntheticLatency, "burstOp");
    }

    float load = (subsystems[id].capacity > 0)
               ? (float)subsystems[id].activeTasks / subsystems[id].capacity
               : 0.0f;
    std::cout << "  After burst: load="
              << std::fixed << std::setprecision(0)
              << (load * 100.0f) << "%  status="
              << statusLabel(subsystems[id].status) << "\n";
}

// ============================================================
// resetSubsystem: clear active tasks post-recovery. O(1)
// ============================================================
void SystemMonitor::resetSubsystem(int id)
{
    if (id < 0 || id >= NUM_SUBSYSTEMS) return;
    subsystems[id].activeTasks = 0;
    subsystems[id].status      = HEALTH_OK;
    std::cout << "  " << subsystems[id].name << " reset to 0 active tasks.\n";
}

// ============================================================
// overallHealth: worst status across all subsystems. O(n)
// ============================================================
HealthStatus SystemMonitor::overallHealth() const
{
    HealthStatus worst = HEALTH_OK;
    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
        if (subsystems[i].status > worst)
            worst = subsystems[i].status;
    return worst;
}

// ============================================================
// bottleneckIndex: index of most-loaded subsystem. O(n)
// ============================================================
int SystemMonitor::bottleneckIndex() const
{
    int   best = 0;
    float max  = 0.0f;
    for (int i = 0; i < NUM_SUBSYSTEMS; i++)
    {
        float load = (subsystems[i].capacity > 0)
                   ? (float)subsystems[i].activeTasks / subsystems[i].capacity
                   : 0.0f;
        if (load > max) { max = load; best = i; }
    }
    return best;
}
