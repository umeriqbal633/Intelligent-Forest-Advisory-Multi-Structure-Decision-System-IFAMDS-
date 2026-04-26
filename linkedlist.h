// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : linkedlist.h
// Layer  : Part 2 — Linked Data Event Memory Layer
//
// Structural Instances:
//   L1  — Raw Event Stream            (Singly Linked List)
//   L2  — Verified Event Stream       (Singly Linked List)
//   L3  — Anomaly Event Stream        (Singly Linked List)
//   L4  — Forward Correction Chain    (Doubly Linked List)
//   L5  — Backward Correction Chain   (Doubly Linked List)
//   L6  — State Synchronization Chain (Doubly Linked List)
//   L7  — Local Monitoring Loop       (Circular Linked List)
//   L8  — System-Wide Monitor Loop    (Circular Linked List)
//   L9  — Emergency Monitor Loop      (Circular Linked List)
//   L10 — Stability Monitor Loop      (Circular Linked List)
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "constants.h"
#include <string>

// ------------------------------------------------------------
// struct EventNode
// The fundamental building block for all 10 linked list instances.
// A single node design supports SLL, DLL, and CLL by choosing
// which pointers to use:
//   SLL  — only 'next' is used  (prev = nullptr)
//   DLL  — both 'next' and 'prev' are used
//   CLL  — 'next' used, tail->next points back to head
// Memory: heap-allocated, managed by EventMemory destructor.
// ------------------------------------------------------------

struct EventNode
{
    float       value;       // Sensor value stored in this event
    int         timestamp;   // Logical time of observation
    int         zoneId;      // Forest zone this event belongs to
    bool        isAnomaly;   // True if value was flagged as anomalous
    bool        isNoise;     // True if value was filtered as noise
    bool        isCorrected; // True if value was updated via correction
    std::string eventType;   // "RAW", "VERIFIED", "ANOMALY", "CORRECTION"
    EventNode*  next;        // Forward pointer (all list types)
    EventNode*  prev;        // Backward pointer (DLL only; nullptr for SLL/CLL)
};

// ------------------------------------------------------------
// class EventMemory
// Manages all 10 linked list instances (L1–L10).
// Acts as the memory layer of IFAMDS — stores, retrieves,
// corrects, and continuously monitors forest event history.
//
// Internal structure:
//   sllHeads[3]  — heads of L1, L2, L3 (index 0,1,2)
//   sllSizes[3]  — element counts for each SLL
//   dllHeads[3]  — heads of L4, L5, L6 (index 0,1,2)
//   dllTails[3]  — tails of L4, L5, L6
//   dllSizes[3]  — element counts for each DLL
//   cllHeads[4]  — heads of L7, L8, L9, L10 (index 0,1,2,3)
//   cllSizes[4]  — element counts for each CLL
// ------------------------------------------------------------

class EventMemory
{
private:

    // --------------------------------------------------------
    // Singly Linked Lists: L1 (raw), L2 (verified), L3 (anomaly)
    // Each stores events in time order with head insertion O(1).
    // Traversal is forward-only: node → node->next → nullptr.
    // --------------------------------------------------------
    EventNode* sllHeads[3];   // Index 0=L1, 1=L2, 2=L3
    int        sllSizes[3];   // Count of nodes in each SLL

    // --------------------------------------------------------
    // Doubly Linked Lists: L4 (forward), L5 (backward), L6 (sync)
    // Supports both forward and backward traversal.
    // Tail pointer maintained for O(1) tail insertion.
    // --------------------------------------------------------
    EventNode* dllHeads[3];   // Index 0=L4, 1=L5, 2=L6
    EventNode* dllTails[3];   // Tail pointers for O(1) tail insert
    int        dllSizes[3];   // Count of nodes in each DLL

    // --------------------------------------------------------
    // Circular Linked Lists: L7 (local), L8 (system),
    //                        L9 (emergency), L10 (stability)
    // Last node's next points back to head — never nullptr.
    // --------------------------------------------------------
    EventNode* cllHeads[4];   // Index 0=L7, 1=L8, 2=L9, 3=L10
    int        cllSizes[4];   // Count of nodes in each CLL

    // Active circular mode (7–10) shown during circularScan()
    int activeCircularMode;

    // --------------------------------------------------------
    // Internal helpers
    // --------------------------------------------------------

    // Creates a new heap-allocated EventNode with given values.
    // Both next and prev are set to nullptr by default.
    // Time Complexity: O(1)
    EventNode* createNode(float val, int time, int zone,
                          bool anomaly, std::string type) const;

    // Returns the array index for an SLL list (listId 1–3 → 0–2).
    // Time Complexity: O(1)
    int sllIndex(int listId) const;

    // Returns the array index for a DLL list (listId 4–6 → 0–2).
    // Time Complexity: O(1)
    int dllIndex(int listId) const;

    // Returns the array index for a CLL list (listId 7–10 → 0–3).
    // Time Complexity: O(1)
    int cllIndex(int listId) const;

    // Prints a single EventNode's contents with a prefix label.
    // Time Complexity: O(1)
    void printNode(const EventNode* node, int idx, const std::string& prefix) const;

public:

    // --------------------------------------------------------
    // Constructor — initialises all head/tail/size arrays.
    // Time Complexity: O(1)
    // --------------------------------------------------------
    EventMemory();

    // --------------------------------------------------------
    // Destructor — frees all heap-allocated nodes across all lists.
    // Time Complexity: O(n) total nodes
    // --------------------------------------------------------
    ~EventMemory();

    // ========================================================
    // Core Insertion
    // ========================================================

    // Inserts a new event into the specified list (listId 1–10).
    //   listId 1–3  → SLL head insertion              O(1)
    //   listId 4–6  → DLL tail insertion               O(1)
    //   listId 7–10 → CLL insertion, tail->next=head   O(1)
    // anomalyFlag: pass true when event comes from anomaly detection.
    // Time Complexity: O(1)
    void insertEvent(float val, int time, int zone,
                     int listId, bool anomalyFlag = false);

    // ========================================================
    // Traversal Operations
    // ========================================================

    // Traverses the specified SLL (listId 1–3) forward via ->next.
    // Prints each node with its index and values.
    // Traversal rule: Event_i → Event_(i+1)
    // Time Complexity: O(n)
    void traverseForward(int listId) const;

    // Traverses the specified DLL (listId 4–6) backward via ->prev.
    // Starts at tail, walks to head.
    // Traversal rule: Event_i → Event_(i-1)
    // Time Complexity: O(n)
    void traverseBackward(int listId) const;

    // Performs one full cycle of the specified CLL (listId 7–10).
    // Starts at head, stops when next == head again.
    // Loop rule: Event_n → Event_1 (circular)
    // Time Complexity: O(n)
    void circularScan(int listId) const;

    // ========================================================
    // Correction Operations (DLL — L4, L5, L6)
    // ========================================================

    // Searches the DLL (listId 4–6) for a node in the given zone
    // and updates its value to newVal.
    // Marks node as isCorrected = true.
    // Update rule: Event_(i+1) = f(Event_i_corrected)
    // Time Complexity: O(n) search + O(1) update
    void correctEvent(int listId, int zoneId, float newVal);

    // Walks the specified DLL backward to find and return the
    // most recent node whose |value - TEMP_NORM| < threshold.
    // Returns nullptr if no stable node found.
    // Time Complexity: O(n)
    EventNode* findLastStable(int listId, float threshold) const;

    // Restores the system to the last stable event state by
    // calling findLastStable() on L6 and printing the result.
    // Time Complexity: O(n)
    void restoreLastStable();

    // Ensures all DLL nodes (L6) hold a consistent global value.
    // Walks forward through L6, printing synchronization status.
    // Consistency rule: all nodes confirmed against global average.
    // Time Complexity: O(n)
    void synchronizeState();

    // ========================================================
    // Noise & Anomaly Filtering
    // ========================================================

    // Walks the specified SLL (listId 1–3) and marks nodes as
    // isNoise=true when |val_i - val_(i-1)| >= DELTA.
    // Noise rule: |Value_i - Value_(i-1)| >= DELTA → noise
    // Time Complexity: O(n)
    void detectAndFilterNoise(int listId);

    // ========================================================
    // Display Operations
    // ========================================================

    // Prints all nodes in a specific SLL (listId 1–3).
    // Time Complexity: O(n)
    void displaySLL(int listId) const;

    // Prints all nodes in a specific DLL (listId 4–6).
    // Time Complexity: O(n)
    void displayDLL(int listId) const;

    // Prints all nodes in a specific CLL (listId 7–10).
    // Time Complexity: O(n)
    void displayCLL(int listId) const;

    // Prints a complete summary of all 10 lists with node counts.
    // Time Complexity: O(n) total
    void displayAllLists() const;

    // Returns the number of nodes in a given list (any listId 1–10).
    // Time Complexity: O(1)
    int getListSize(int listId) const;
};

#endif // LINKEDLIST_H
