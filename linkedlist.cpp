// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : linkedlist.cpp
// Layer  : Part 2 — Linked Data Event Memory Layer
//
// Implements all methods of class EventMemory declared in linkedlist.h.
// Data structures used:
//   L1–L3   — Singly Linked Lists    (forward traversal only)
//   L4–L6   — Doubly Linked Lists    (forward + backward traversal)
//   L7–L10  — Circular Linked Lists  (continuous loop monitoring)
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#include "linkedlist.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

using namespace std;

// ============================================================
// Internal Helpers
// ============================================================

// Creates a new EventNode on the heap with given parameters.
// next and prev are initialised to nullptr — caller sets links.
// Time Complexity: O(1)
EventNode* EventMemory::createNode(float val, int time, int zone,
                                   bool anomaly, string type) const
{
    EventNode* node  = new EventNode();
    node->value      = val;
    node->timestamp  = time;
    node->zoneId     = zone;
    node->isAnomaly  = anomaly;
    node->isNoise    = false;
    node->isCorrected= false;
    node->eventType  = type;
    node->next       = nullptr;
    node->prev       = nullptr;
    return node;
}

// Maps listId 1–3 to array index 0–2 for sllHeads[].
// Time Complexity: O(1)
int EventMemory::sllIndex(int listId) const { return listId - 1; }

// Maps listId 4–6 to array index 0–2 for dllHeads[].
// Time Complexity: O(1)
int EventMemory::dllIndex(int listId) const { return listId - 4; }

// Maps listId 7–10 to array index 0–3 for cllHeads[].
// Time Complexity: O(1)
int EventMemory::cllIndex(int listId) const { return listId - 7; }

// Prints a single node's data with an index prefix.
// Time Complexity: O(1)
void EventMemory::printNode(const EventNode* node, int idx,
                            const string& prefix) const
{
    cout << "  " << prefix << "[" << setw(2) << idx << "] "
         << "Zone=" << setw(2) << node->zoneId
         << "  Val="   << setw(6) << fixed << setprecision(1) << node->value
         << "  Time="  << setw(3) << node->timestamp
         << "  Type="  << setw(10) << left << node->eventType << right
         << "  Noise=" << (node->isNoise     ? "Y" : "N")
         << "  Anom="  << (node->isAnomaly   ? "Y" : "N")
         << "  Corr="  << (node->isCorrected ? "Y" : "N")
         << endl;
}

// ============================================================
// Constructor
// Initialises all head/tail/size arrays to nullptr/0.
// Time Complexity: O(1)
// ============================================================
EventMemory::EventMemory()
{
    for (int i = 0; i < 3; i++)
    {
        sllHeads[i] = nullptr;
        sllSizes[i] = 0;
        dllHeads[i] = nullptr;
        dllTails[i] = nullptr;
        dllSizes[i] = 0;
    }
    for (int i = 0; i < 4; i++)
    {
        cllHeads[i] = nullptr;
        cllSizes[i] = 0;
    }
    activeCircularMode = LIST_LOCAL;  // Default monitoring mode = L7
    cout << "[EventMemory] Initialised — all 10 lists ready." << endl;
}

// ============================================================
// Destructor
// Frees all heap-allocated nodes across every list.
// Time Complexity: O(n) total across all lists
// ============================================================
EventMemory::~EventMemory()
{
    // Free SLL nodes (L1–L3)
    for (int i = 0; i < 3; i++)
    {
        EventNode* curr = sllHeads[i];
        while (curr != nullptr)
        {
            EventNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        sllHeads[i] = nullptr;
    }

    // Free DLL nodes (L4–L6)
    for (int i = 0; i < 3; i++)
    {
        EventNode* curr = dllHeads[i];
        while (curr != nullptr)
        {
            EventNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        dllHeads[i] = nullptr;
        dllTails[i] = nullptr;
    }

    // Free CLL nodes (L7–L10) — must break the circle first
    for (int i = 0; i < 4; i++)
    {
        if (cllHeads[i] == nullptr) continue;

        // Break the circular link so we can walk with a standard loop
        EventNode* tail = cllHeads[i];
        while (tail->next != cllHeads[i])
            tail = tail->next;
        tail->next = nullptr;  // Break circle

        EventNode* curr = cllHeads[i];
        while (curr != nullptr)
        {
            EventNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        cllHeads[i] = nullptr;
    }
}

// ============================================================
// Core Insertion
// ============================================================

// Inserts a new event into the specified list (listId 1–10).
//
// SLL (L1–L3): head insertion — new node becomes new head.
//   Traversal rule: head → node → node → nullptr
//   Time Complexity: O(1)
//
// DLL (L4–L6): tail insertion — maintains time-order (oldest at head).
//   Navigation rule: Event_i ↔ Event_(i-1), Event_(i+1)
//   Time Complexity: O(1)
//
// CLL (L7–L10): tail insertion, tail->next = head (circle maintained).
//   Loop rule: Event_n → Event_1
//   Time Complexity: O(1)
//
void EventMemory::insertEvent(float val, int time, int zone,
                               int listId, bool anomalyFlag)
{
    // Determine event type label based on list
    string typeLabel;
    if      (listId == LIST_RAW)       typeLabel = "RAW";
    else if (listId == LIST_VERIFIED)  typeLabel = "VERIFIED";
    else if (listId == LIST_ANOMALY)   typeLabel = "ANOMALY";
    else if (listId == LIST_FORWARD)   typeLabel = "FWD_CORR";
    else if (listId == LIST_BACKWARD)  typeLabel = "BWD_CORR";
    else if (listId == LIST_SYNC)      typeLabel = "SYNC";
    else if (listId == LIST_LOCAL)     typeLabel = "LOCAL_MON";
    else if (listId == LIST_SYSTEM)    typeLabel = "SYS_MON";
    else if (listId == LIST_EMERGENCY) typeLabel = "EMRG_MON";
    else if (listId == LIST_STABILITY) typeLabel = "STAB_MON";
    else
    {
        cout << "[EventMemory] ERROR: Invalid listId=" << listId << endl;
        return;
    }

    EventNode* node = createNode(val, time, zone, anomalyFlag, typeLabel);

    // ── SLL insertion (L1, L2, L3) ──────────────────────────
    if (listId >= LIST_RAW && listId <= LIST_ANOMALY)
    {
        int idx = sllIndex(listId);
        // Head insertion: new node → old head
        node->next    = sllHeads[idx];
        sllHeads[idx] = node;
        sllSizes[idx]++;

        cout << "[L" << listId << "] Event inserted (SLL head) | "
             << "Zone=" << zone << " Val=" << val
             << " Time=" << time
             << " | Size=" << sllSizes[idx] << endl;
        return;
    }

    // ── DLL insertion (L4, L5, L6) ──────────────────────────
    if (listId >= LIST_FORWARD && listId <= LIST_SYNC)
    {
        int idx = dllIndex(listId);

        if (dllHeads[idx] == nullptr)
        {
            // First node — both head and tail point to it
            dllHeads[idx] = node;
            dllTails[idx] = node;
        }
        else
        {
            // Tail insertion: old tail ↔ new node
            node->prev           = dllTails[idx];
            dllTails[idx]->next  = node;
            dllTails[idx]        = node;
        }
        dllSizes[idx]++;

        cout << "[L" << listId << "] Event inserted (DLL tail) | "
             << "Zone=" << zone << " Val=" << val
             << " Time=" << time
             << " | Size=" << dllSizes[idx] << endl;
        return;
    }

    // ── CLL insertion (L7, L8, L9, L10) ─────────────────────
    if (listId >= LIST_LOCAL && listId <= LIST_STABILITY)
    {
        int idx = cllIndex(listId);

        if (cllHeads[idx] == nullptr)
        {
            // First node — points to itself (circle of one)
            node->next    = node;
            cllHeads[idx] = node;
        }
        else
        {
            // Find the current tail (node whose next == head)
            EventNode* tail = cllHeads[idx];
            while (tail->next != cllHeads[idx])
                tail = tail->next;

            // Insert new node between tail and head
            tail->next    = node;
            node->next    = cllHeads[idx];
        }
        cllSizes[idx]++;

        cout << "[L" << listId << "] Event inserted (CLL) | "
             << "Zone=" << zone << " Val=" << val
             << " Time=" << time
             << " | Size=" << cllSizes[idx] << endl;
        return;
    }
}

// ============================================================
// Traversal Operations
// ============================================================

// Traverses a Singly Linked List (L1, L2, or L3) forward.
// Traversal rule: Event_i → Event_(i+1) until nullptr.
// Time Complexity: O(n)
void EventMemory::traverseForward(int listId) const
{
    if (listId < LIST_RAW || listId > LIST_ANOMALY)
    {
        cout << "[TraverseForward] ERROR: listId " << listId
             << " is not an SLL (use L1–L3)." << endl;
        return;
    }

    int        idx  = sllIndex(listId);
    EventNode* curr = sllHeads[idx];

    cout << "\n[L" << listId << "] Forward Traversal ("
         << sllSizes[idx] << " nodes):" << endl;
    cout << "  HEAD";

    if (curr == nullptr)
    {
        cout << " → (empty)" << endl;
        return;
    }

    cout << endl;
    int i = 0;
    while (curr != nullptr)
    {
        printNode(curr, i++, "→ ");
        curr = curr->next;
    }
    cout << "  → NULL (end of list)" << endl;
}

// Traverses a Doubly Linked List (L4, L5, or L6) backward from tail.
// Traversal rule: Event_i → Event_(i-1) via ->prev until nullptr.
// Time Complexity: O(n)
void EventMemory::traverseBackward(int listId) const
{
    if (listId < LIST_FORWARD || listId > LIST_SYNC)
    {
        cout << "[TraverseBackward] ERROR: listId " << listId
             << " is not a DLL (use L4–L6)." << endl;
        return;
    }

    int        idx  = dllIndex(listId);
    EventNode* curr = dllTails[idx];

    cout << "\n[L" << listId << "] Backward Traversal ("
         << dllSizes[idx] << " nodes, tail → head):" << endl;
    cout << "  TAIL";

    if (curr == nullptr)
    {
        cout << " → (empty)" << endl;
        return;
    }

    cout << endl;
    int i = dllSizes[idx] - 1;
    while (curr != nullptr)
    {
        printNode(curr, i--, "← ");
        curr = curr->prev;
    }
    cout << "  ← NULL (reached head)" << endl;
}

// Performs one complete cycle of a Circular Linked List (L7–L10).
// Starts at head, stops when next wraps back to head.
// Loop rule: Event_n → Event_1 (tail->next == head)
// Time Complexity: O(n)
void EventMemory::circularScan(int listId) const
{
    if (listId < LIST_LOCAL || listId > LIST_STABILITY)
    {
        cout << "[CircularScan] ERROR: listId " << listId
             << " is not a CLL (use L7–L10)." << endl;
        return;
    }

    int        idx  = cllIndex(listId);
    EventNode* head = cllHeads[idx];

    string modeLabel;
    if      (listId == LIST_LOCAL)     modeLabel = "LOCAL ZONE MONITOR";
    else if (listId == LIST_SYSTEM)    modeLabel = "SYSTEM-WIDE MONITOR";
    else if (listId == LIST_EMERGENCY) modeLabel = "EMERGENCY MONITOR";
    else                               modeLabel = "STABILITY MONITOR";

    cout << "\n[L" << listId << "] Circular Scan — " << modeLabel
         << " (" << cllSizes[idx] << " nodes):" << endl;

    if (head == nullptr)
    {
        cout << "  (list is empty — no events to scan)" << endl;
        return;
    }

    EventNode* curr = head;
    int i = 0;
    do
    {
        printNode(curr, i++, "↻ ");
        curr = curr->next;
    } while (curr != head);    // Stop when we wrap back to head

    cout << "  ↻ (cycle complete — back to head)" << endl;
}

// ============================================================
// Correction Operations
// ============================================================

// Searches a DLL (L4, L5, or L6) for a node in the given zone
// and updates its value. Marks the node as corrected.
// Update rule: Event_(corrected) = newVal
// Time Complexity: O(n) search + O(1) update
void EventMemory::correctEvent(int listId, int zoneId, float newVal)
{
    if (listId < LIST_FORWARD || listId > LIST_SYNC)
    {
        cout << "[CorrectEvent] ERROR: listId " << listId
             << " is not a DLL (use L4–L6)." << endl;
        return;
    }

    int        idx  = dllIndex(listId);
    EventNode* curr = dllHeads[idx];

    while (curr != nullptr)
    {
        if (curr->zoneId == zoneId)
        {
            float oldVal       = curr->value;
            curr->value        = newVal;
            curr->isCorrected  = true;

            cout << "[L" << listId << "] Correction applied | "
                 << "Zone=" << zoneId
                 << "  OldVal=" << oldVal
                 << " → NewVal=" << newVal << endl;
            return;
        }
        curr = curr->next;
    }

    cout << "[L" << listId << "] No node found for Zone="
         << zoneId << " — correction not applied." << endl;
}

// Walks a DLL backward from tail to find the last node
// whose |value - TEMP_NORM| < threshold (stable condition).
// Returns a pointer to that node, or nullptr if none found.
// Backtracking rule: Event_(i-1) = f(Event_new_i)
// Time Complexity: O(n)
EventNode* EventMemory::findLastStable(int listId, float threshold) const
{
    if (listId < LIST_FORWARD || listId > LIST_SYNC)
        return nullptr;

    int        idx  = dllIndex(listId);
    EventNode* curr = dllTails[idx];

    while (curr != nullptr)
    {
        if (fabs(curr->value - TEMP_NORM) < threshold)
            return curr;
        curr = curr->prev;
    }
    return nullptr;
}

// Restores system to the last known stable event state.
// Uses L6 (State Synchronization Chain) as reference.
// Stability condition: |value - TEMP_NORM| < THETA
// Time Complexity: O(n)
void EventMemory::restoreLastStable()
{
    cout << "\n[RestoreLastStable] Searching L6 for last stable event..." << endl;

    EventNode* stable = findLastStable(LIST_SYNC, THETA);

    if (stable == nullptr)
    {
        cout << "  [!] No stable state found in L6. System cannot restore." << endl;
        return;
    }

    cout << "  [OK] Last stable event found:" << endl;
    printNode(stable, 0, "  RESTORED → ");
    cout << "  System state restored to Zone=" << stable->zoneId
         << " Val=" << stable->value << "C  Time=" << stable->timestamp << endl;
}

// Walks L6 (State Synchronization Chain) forward and confirms
// all nodes agree with the global average value.
// Consistency rule: all nodes = global average
// Time Complexity: O(n)
void EventMemory::synchronizeState()
{
    int        idx  = dllIndex(LIST_SYNC);
    EventNode* curr = dllHeads[idx];

    if (curr == nullptr)
    {
        cout << "[Synchronize] L6 is empty — nothing to synchronize." << endl;
        return;
    }

    // Compute global average of all L6 values
    float sum   = 0.0f;
    int   count = 0;
    EventNode* temp = curr;
    while (temp != nullptr) { sum += temp->value; count++; temp = temp->next; }
    float avg = (count > 0) ? (sum / count) : TEMP_NORM;

    cout << "\n[Synchronize] L6 State Synchronization (avg=" << avg << "C):" << endl;

    int i = 0;
    while (curr != nullptr)
    {
        float deviation = fabs(curr->value - avg);
        cout << "  Node[" << i++ << "] Zone=" << curr->zoneId
             << " Val=" << curr->value
             << " Dev=" << deviation
             << (deviation < DELTA ? "  [SYNC OK]" : "  [OUT OF SYNC]") << endl;
        curr = curr->next;
    }
    cout << "  Synchronization complete." << endl;
}

// ============================================================
// Noise & Anomaly Filtering
// ============================================================

// Scans a SLL (L1, L2, or L3) and marks nodes as noise
// when consecutive value difference >= DELTA.
// Noise rule: |Val_i - Val_(i-1)| >= DELTA → isNoise = true
// Time Complexity: O(n)
void EventMemory::detectAndFilterNoise(int listId)
{
    if (listId < LIST_RAW || listId > LIST_ANOMALY)
    {
        cout << "[NoiseFilter] ERROR: listId " << listId
             << " is not an SLL (use L1–L3)." << endl;
        return;
    }

    int        idx   = sllIndex(listId);
    EventNode* curr  = sllHeads[idx];
    int        noisy = 0;

    cout << "\n[L" << listId << "] Noise Detection Scan:" << endl;

    EventNode* prev = nullptr;
    int i = 0;
    while (curr != nullptr)
    {
        if (prev != nullptr)
        {
            float diff = fabs(curr->value - prev->value);
            if (diff >= DELTA)
            {
                curr->isNoise = true;
                noisy++;
                cout << "  [NOISE] Node[" << i << "] Zone=" << curr->zoneId
                     << " |" << curr->value << " - " << prev->value
                     << "| = " << diff << " >= DELTA(" << DELTA << ")" << endl;
            }
            else
            {
                cout << "  [CLEAN] Node[" << i << "] Zone=" << curr->zoneId
                     << " diff=" << diff << endl;
            }
        }
        else
        {
            cout << "  [FIRST] Node[" << i << "] Zone=" << curr->zoneId
                 << " (no previous — skip)" << endl;
        }
        prev = curr;
        curr = curr->next;
        i++;
    }

    cout << "  Scan complete: " << noisy << " noisy node(s) marked in L"
         << listId << "." << endl;
}

// ============================================================
// Display Operations
// ============================================================

// Prints all nodes in a specific SLL (L1, L2, or L3).
// Time Complexity: O(n)
void EventMemory::displaySLL(int listId) const
{
    if (listId < LIST_RAW || listId > LIST_ANOMALY)
    {
        cout << "[displaySLL] Invalid listId " << listId << endl;
        return;
    }

    int        idx  = sllIndex(listId);
    EventNode* curr = sllHeads[idx];

    string name;
    if      (listId == LIST_RAW)      name = "Raw Event Stream";
    else if (listId == LIST_VERIFIED) name = "Verified Event Stream";
    else                              name = "Anomaly Event Stream";

    cout << "\n── L" << listId << ": " << name
         << " (" << sllSizes[idx] << " nodes) ──" << endl;

    if (curr == nullptr) { cout << "  (empty)" << endl; return; }

    int i = 0;
    while (curr != nullptr)
    {
        printNode(curr, i++, "→ ");
        curr = curr->next;
    }
}

// Prints all nodes in a specific DLL (L4, L5, or L6).
// Time Complexity: O(n)
void EventMemory::displayDLL(int listId) const
{
    if (listId < LIST_FORWARD || listId > LIST_SYNC)
    {
        cout << "[displayDLL] Invalid listId " << listId << endl;
        return;
    }

    int        idx  = dllIndex(listId);
    EventNode* curr = dllHeads[idx];

    string name;
    if      (listId == LIST_FORWARD)  name = "Forward Correction Chain";
    else if (listId == LIST_BACKWARD) name = "Backward Correction Chain";
    else                              name = "State Synchronization Chain";

    cout << "\n── L" << listId << ": " << name
         << " (" << dllSizes[idx] << " nodes) ──" << endl;

    if (curr == nullptr) { cout << "  (empty)" << endl; return; }

    cout << "  HEAD ↔";
    int i = 0;
    while (curr != nullptr)
    {
        cout << endl;
        printNode(curr, i++, "↔ ");
        curr = curr->next;
    }
    cout << "  ↔ TAIL" << endl;
}

// Prints all nodes in a specific CLL (L7, L8, L9, or L10).
// Time Complexity: O(n)
void EventMemory::displayCLL(int listId) const
{
    if (listId < LIST_LOCAL || listId > LIST_STABILITY)
    {
        cout << "[displayCLL] Invalid listId " << listId << endl;
        return;
    }

    int        idx  = cllIndex(listId);
    EventNode* head = cllHeads[idx];

    string name;
    if      (listId == LIST_LOCAL)     name = "Local Monitoring Loop";
    else if (listId == LIST_SYSTEM)    name = "System-Wide Monitor Loop";
    else if (listId == LIST_EMERGENCY) name = "Emergency Monitor Loop";
    else                               name = "Stability Monitor Loop";

    cout << "\n── L" << listId << ": " << name
         << " (" << cllSizes[idx] << " nodes) ──" << endl;

    if (head == nullptr) { cout << "  (empty)" << endl; return; }

    EventNode* curr = head;
    int i = 0;
    do
    {
        printNode(curr, i++, "↻ ");
        curr = curr->next;
    } while (curr != head);
    cout << "  ↻ (circular — tail links back to head)" << endl;
}

// Prints a complete summary of all 10 lists.
// Time Complexity: O(n) total
void EventMemory::displayAllLists() const
{
    cout << "\n╔══════════════════════════════════════════════════╗" << endl;
    cout << "║     Event Memory — All 10 Lists Summary          ║" << endl;
    cout << "╠══════════════════╦═══════════╦════════════════════╣" << endl;
    cout << "║ List             ║ Type      ║ Nodes              ║" << endl;
    cout << "╠══════════════════╬═══════════╬════════════════════╣" << endl;
    cout << "║ L1  Raw Stream   ║ SLL       ║ " << setw(3) << sllSizes[0] << "                ║" << endl;
    cout << "║ L2  Verified     ║ SLL       ║ " << setw(3) << sllSizes[1] << "                ║" << endl;
    cout << "║ L3  Anomaly      ║ SLL       ║ " << setw(3) << sllSizes[2] << "                ║" << endl;
    cout << "║ L4  Fwd Correct  ║ DLL       ║ " << setw(3) << dllSizes[0] << "                ║" << endl;
    cout << "║ L5  Bwd Correct  ║ DLL       ║ " << setw(3) << dllSizes[1] << "                ║" << endl;
    cout << "║ L6  State Sync   ║ DLL       ║ " << setw(3) << dllSizes[2] << "                ║" << endl;
    cout << "║ L7  Local Mon    ║ CLL       ║ " << setw(3) << cllSizes[0] << "                ║" << endl;
    cout << "║ L8  System Mon   ║ CLL       ║ " << setw(3) << cllSizes[1] << "                ║" << endl;
    cout << "║ L9  Emergency    ║ CLL       ║ " << setw(3) << cllSizes[2] << "                ║" << endl;
    cout << "║ L10 Stability    ║ CLL       ║ " << setw(3) << cllSizes[3] << "                ║" << endl;
    cout << "╚══════════════════╩═══════════╩════════════════════╝" << endl;

    // Print each list's contents
    for (int id = LIST_RAW; id <= LIST_ANOMALY; id++)   displaySLL(id);
    for (int id = LIST_FORWARD; id <= LIST_SYNC; id++)   displayDLL(id);
    for (int id = LIST_LOCAL; id <= LIST_STABILITY; id++) displayCLL(id);
}

// Returns the number of nodes in a given list (any listId 1–10).
// Time Complexity: O(1)
int EventMemory::getListSize(int listId) const
{
    if (listId >= LIST_RAW      && listId <= LIST_ANOMALY)   return sllSizes[sllIndex(listId)];
    if (listId >= LIST_FORWARD  && listId <= LIST_SYNC)      return dllSizes[dllIndex(listId)];
    if (listId >= LIST_LOCAL    && listId <= LIST_STABILITY)  return cllSizes[cllIndex(listId)];
    return -1;
}
