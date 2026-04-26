// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : main.cpp
// Purpose: Entry point — constructs all three Partner A layer
//          objects, seeds them with representative demo data so
//          the system is never empty at presentation time, then
//          hands control to the interactive menu loop.
//
// Compile:
//   g++ -std=c++17 -Wall -Wextra -o ifamds
//       main.cpp arrays.cpp linkedlist.cpp queues.cpp menu.cpp
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#include "menu.h"      // brings in arrays.h, linkedlist.h, queues.h
#include <iostream>
#include <string>

// ------------------------------------------------------------
// seedDemoData
// Populates all three layers with realistic sample data so the
// demo can jump straight to any menu option without hitting
// "queue empty" or "no readings" dead ends.
//
// Data injected
//   ArrayLayer   : 8 sensor readings across 4 zones (2 anomalies)
//   EventMemory  : events in every list category (L1–L10)
//   SchedulingEngine : tasks in every queue (Q1–Q4)
//
// Time Complexity: O(n) where n = number of seeded items
// ------------------------------------------------------------
static void seedDemoData(ArrayLayer& arr, EventMemory& em,
                         SchedulingEngine& sched)
{
    // ── Part 1: Sensor readings ──────────────────────────────
    // Normal readings — four zones, varied but within thresholds
    SensorReading readings[] = {
        // { temp,  humidity, smoke, wind, zone, time, valid, anomaly }
        { 28.5f,  62.0f,  0.05f, 12.0f,  0, 100, true, false },
        { 30.1f,  58.5f,  0.08f, 15.0f,  1, 101, true, false },
        { 27.3f,  70.2f,  0.04f,  9.5f,  2, 102, true, false },
        { 29.8f,  60.0f,  0.07f, 11.0f,  3, 103, true, false },
        // Anomalous readings — high temp + smoke spike
        { 54.0f,  35.0f,  0.75f, 22.0f,  1, 104, true, false },
        { 61.2f,  28.5f,  0.90f, 30.0f,  2, 105, true, false },
        // Recovery readings
        { 31.0f,  55.0f,  0.10f, 14.0f,  0, 106, true, false },
        { 33.5f,  52.0f,  0.12f, 16.5f,  3, 107, true, false }
    };

    for (auto& r : readings) {
        arr.ingestReading(r);
        arr.flagAnomaly(r);
    }

    // Initialise the static grid baseline
    arr.loadStaticGrid();

    // ── Part 2: Event memory (all 10 lists) ─────────────────
    // Signature: insertEvent(float val, int time, int zone,
    //                        int listId, bool anomalyFlag)

    // L1 — Raw sensor data (SLL)
    em.insertEvent(28.5f, 100, 0, LIST_RAW,      false);
    em.insertEvent(30.1f, 101, 1, LIST_RAW,      false);
    em.insertEvent(54.0f, 104, 1, LIST_RAW,      true);

    // L2 — Verified sensor data (SLL)
    em.insertEvent(28.5f, 100, 0, LIST_VERIFIED, false);
    em.insertEvent(30.1f, 101, 1, LIST_VERIFIED, false);
    em.insertEvent(27.3f, 102, 2, LIST_VERIFIED, false);

    // L3 — Anomaly log (SLL) — integration pipeline writes here
    em.insertEvent(54.0f, 104, 1, LIST_ANOMALY,  true);
    em.insertEvent(61.2f, 105, 2, LIST_ANOMALY,  true);

    // L4 — Forward event log (DLL)
    em.insertEvent(29.8f, 103, 3, LIST_FORWARD,  false);
    em.insertEvent(31.0f, 106, 0, LIST_FORWARD,  false);

    // L5 — Backward event log (DLL)
    em.insertEvent(33.5f, 107, 3, LIST_BACKWARD, false);
    em.insertEvent(27.3f, 102, 2, LIST_BACKWARD, false);

    // L6 — Sync event log (DLL)
    em.insertEvent(30.0f, 108, 1, LIST_SYNC,     false);
    em.insertEvent(29.0f, 109, 0, LIST_SYNC,     false);

    // L7 — Local patrol events (CLL)
    em.insertEvent(25.0f, 110, 0, LIST_LOCAL,    false);
    em.insertEvent(26.5f, 111, 1, LIST_LOCAL,    false);

    // L8 — System-wide events (CLL)
    em.insertEvent(28.0f, 112, 2, LIST_SYSTEM,   false);
    em.insertEvent(27.5f, 113, 3, LIST_SYSTEM,   false);

    // L9 — Emergency circular log (CLL) — integration pipeline writes here
    em.insertEvent(54.0f, 104, 1, LIST_EMERGENCY, true);
    em.insertEvent(61.2f, 105, 2, LIST_EMERGENCY, true);

    // L10 — Terrain stability events (CLL)
    em.insertEvent(0.3f,  115, 0, LIST_STABILITY, false);
    em.insertEvent(0.5f,  116, 2, LIST_STABILITY, false);

    // ── Part 3: Queue tasks (Q1–Q4) ─────────────────────────
    // Q1 — Routine monitoring tasks (FIFO, low priority)
    sched.addRoutineTask("Inspect Zone-0 moisture sensors",   0);
    sched.addRoutineTask("Log Zone-1 wind speed readings",    1);
    sched.addRoutineTask("Check Zone-2 camera feeds",         2);
    sched.addRoutineTask("Update Zone-3 baseline readings",   3);

    // Q2 — Continuous surveillance tasks (FIFO, medium priority)
    sched.addSurveillanceTask("Monitor Zone-1 smoke levels continuously", 1);
    sched.addSurveillanceTask("Track Zone-2 temperature trend",           2);

    // Q3 — Emergency response tasks (Max-Heap, critical priority)
    sched.addEmergencyTask("FIRE ALERT: Deploy suppression to Zone-1",  1);
    sched.addEmergencyTask("FIRE ALERT: Evacuate personnel from Zone-2", 2);

    // Q4 — Multi-factor decision tasks (FIFO, high priority)
    sched.addMultiFactorTask("Analyse combined temp+smoke risk for Zone-1", 1);
    sched.addMultiFactorTask("Cross-reference wind+humidity for Zone-2",    2);
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main()
{
    // Construct all three Partner A layer objects
    ArrayLayer       arr;
    EventMemory      em;
    SchedulingEngine sched;

    // Pre-load representative data for the demo
    seedDemoData(arr, em, sched);

    // Hand control to the interactive menu loop.
    // runMenu() blocks until the user selects "Exit".
    runMenu(arr, em, sched);

    return 0;
}
