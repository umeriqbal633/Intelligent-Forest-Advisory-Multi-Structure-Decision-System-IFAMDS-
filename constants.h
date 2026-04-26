// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : constants.h
// Purpose: System-wide constants, thresholds, and enumerations
//
// Time complexity note: all constant lookups are O(1)
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#ifndef CONSTANTS_H
#define CONSTANTS_H

// ------------------------------------------------------------
// SECTION 1: Size Constants
// These define the maximum capacities of all data structures
// used throughout the system. Fixed at compile time — O(1).
// ------------------------------------------------------------

const int MAX_READINGS = 100;   // Max sensor readings in A2 dynamic stream
const int ROWS         = 5;     // Rows in forest grid (A3) and terrain (A4)
const int COLS         = 5;     // Columns in forest grid (A3) and terrain (A4)
const int MAX_EVENTS   = 200;   // Max event nodes across all linked lists
const int MAX_TASKS    = 50;    // Max tasks in priority queue heap array
const int TABLE_SIZE   = 11;    // Hash table size (prime — reduces collisions)
const int NUM_ZONES    = 9;     // Total number of forest monitoring zones

// ------------------------------------------------------------
// SECTION 2: Threshold Constants (Environmental Limits)
// Used by anomaly detection, noise filtering, and boundary
// detection functions. All comparisons are O(1).
// ------------------------------------------------------------

const float TEMP_MAX  = 45.0f;  // Temperature danger threshold (°C)
                                 // Above this → fire risk triggered
const float SMOKE_MAX = 70.0f;  // Smoke level danger threshold (units)
                                 // Above this → possible fire alert
const float HUM_MIN   = 20.0f;  // Minimum safe humidity (%)
                                 // Below this → dry/fire-risk condition
const float DELTA     =  5.0f;  // Noise detection threshold
                                 // |V_i - V_(i-1)| >= DELTA → noise flagged
const float THETA     = 15.0f;  // Anomaly detection threshold
                                 // |V_current - V_normal| > THETA → anomaly

// ------------------------------------------------------------
// SECTION 3: Baseline Constants (Normal Forest Conditions)
// Stored in A1 (Static Environmental Baseline Array).
// These represent ideal/reference forest state — never change
// during execution. Used for comparison with live sensor data.
// ------------------------------------------------------------

const float TEMP_NORM  = 25.0f;  // Normal forest temperature (°C)
const float HUM_NORM   = 60.0f;  // Normal forest humidity (%)
const float SMOKE_NORM =  0.0f;  // Normal smoke level (no smoke)
const float WIND_NORM  = 10.0f;  // Normal wind speed (km/h)

// ------------------------------------------------------------
// SECTION 4: Priority Level Enumeration
// Used by the Queue-Based Scheduling Engine (Part 3).
// Higher value = higher urgency = processed first in Q3.
// ------------------------------------------------------------

enum PriorityLevel
{
    PRIORITY_LOW      = 1,   // Routine monitoring tasks (Q1)
    PRIORITY_MED      = 2,   // Continuous surveillance tasks (Q2)
    PRIORITY_HIGH     = 3,   // Multi-factor decision tasks (Q4)
    PRIORITY_CRITICAL = 5    // Emergency response tasks (Q3)
                             // Note: gap between 3 and 5 is intentional —
                             // leaves room for PRIORITY_URGENT = 4 if needed
};

// ------------------------------------------------------------
// SECTION 5: Baseline Array Index Mapping
// These make baseline[idx] reads self-documenting.
// Usage: baseline[BASELINE_TEMP], baseline[BASELINE_HUM], etc.
// ------------------------------------------------------------

const int BASELINE_TEMP  = 0;   // Index for temperature in A1
const int BASELINE_HUM   = 1;   // Index for humidity in A1
const int BASELINE_SMOKE = 2;   // Index for smoke level in A1
const int BASELINE_WIND  = 3;   // Index for wind speed in A1

// ------------------------------------------------------------
// SECTION 6: Linked List ID Mapping (for EventMemory)
// Passed as listId parameter to insertEvent() and traverse().
// ------------------------------------------------------------

const int LIST_RAW      = 1;   // L1 — raw sensor stream (SLL)
const int LIST_VERIFIED = 2;   // L2 — verified/filtered stream (SLL)
const int LIST_ANOMALY  = 3;   // L3 — anomaly event stream (SLL)
const int LIST_FORWARD  = 4;   // L4 — forward correction chain (DLL)
const int LIST_BACKWARD = 5;   // L5 — backward correction chain (DLL)
const int LIST_SYNC     = 6;   // L6 — state synchronization chain (DLL)
const int LIST_LOCAL    = 7;   // L7 — local zone monitoring loop (CLL)
const int LIST_SYSTEM   = 8;   // L8 — system-wide monitoring loop (CLL)
const int LIST_EMERGENCY= 9;   // L9 — emergency monitoring loop (CLL)
const int LIST_STABILITY= 10;  // L10 — stability monitoring loop (CLL)

// ------------------------------------------------------------
// SECTION 7: Queue ID Mapping (for SchedulingEngine)
// Passed as queueId to processNextTask() and pauseQueue().
// ------------------------------------------------------------

const int QUEUE_ROUTINE     = 1;  // Q1 — routine monitoring (FIFO)
const int QUEUE_SURVEILLANCE= 2;  // Q2 — continuous surveillance (FIFO)
const int QUEUE_EMERGENCY   = 3;  // Q3 — emergency response (Priority)
const int QUEUE_MULTIFACTOR = 4;  // Q4 — multi-factor decision (FIFO)

#endif // CONSTANTS_H
