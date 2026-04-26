#include "linkedlist.h"
#include <iostream>
using namespace std;
int main()
{
    cout << "=== IFAMDS — Part 2 Linked List Layer Test ===" << endl;
    EventMemory em;

    // Test SLL — L1 raw, L2 verified, L3 anomaly
    cout << "\n--- SLL Tests (L1, L2, L3) ---" << endl;
    em.insertEvent(25.0f, 1, 0, LIST_RAW);
    em.insertEvent(26.0f, 2, 1, LIST_RAW);
    em.insertEvent(50.0f, 3, 7, LIST_RAW, true);
    em.insertEvent(25.5f, 1, 0, LIST_VERIFIED);
    em.insertEvent(26.0f, 2, 2, LIST_VERIFIED);
    em.insertEvent(50.0f, 3, 7, LIST_ANOMALY, true);
    em.insertEvent(80.0f, 4, 4, LIST_ANOMALY, true);

    em.traverseForward(LIST_RAW);
    em.traverseForward(LIST_VERIFIED);
    em.traverseForward(LIST_ANOMALY);

    // Test noise detection on L1
    cout << "\n--- Noise Detection Test ---" << endl;
    em.detectAndFilterNoise(LIST_RAW);

    // Test DLL — L4, L5, L6
    cout << "\n--- DLL Tests (L4, L5, L6) ---" << endl;
    em.insertEvent(25.0f, 1, 0, LIST_FORWARD);
    em.insertEvent(27.0f, 2, 3, LIST_FORWARD);
    em.insertEvent(48.0f, 3, 7, LIST_FORWARD, true);
    em.insertEvent(25.0f, 1, 0, LIST_BACKWARD);
    em.insertEvent(26.0f, 2, 1, LIST_BACKWARD);
    em.insertEvent(25.0f, 1, 0, LIST_SYNC);
    em.insertEvent(26.5f, 2, 3, LIST_SYNC);
    em.insertEvent(30.0f, 3, 6, LIST_SYNC);

    em.traverseBackward(LIST_FORWARD);
    em.traverseBackward(LIST_SYNC);

    // Test correction
    cout << "\n--- Correction Test ---" << endl;
    em.correctEvent(LIST_FORWARD, 7, 28.0f);
    em.traverseBackward(LIST_FORWARD);

    // Test synchronize + restore
    em.synchronizeState();
    em.restoreLastStable();

    // Test CLL — L7, L8, L9, L10
    cout << "\n--- CLL Tests (L7, L8, L9, L10) ---" << endl;
    em.insertEvent(25.0f, 1, 0, LIST_LOCAL);
    em.insertEvent(26.0f, 2, 1, LIST_LOCAL);
    em.insertEvent(27.0f, 3, 2, LIST_LOCAL);
    em.insertEvent(25.0f, 1, 0, LIST_SYSTEM);
    em.insertEvent(50.0f, 2, 7, LIST_EMERGENCY, true);
    em.insertEvent(25.5f, 1, 0, LIST_STABILITY);

    em.circularScan(LIST_LOCAL);
    em.circularScan(LIST_EMERGENCY);
    em.circularScan(LIST_STABILITY);

    // Full summary
    em.displayAllLists();

    cout << "\n=== Part 2 Test Complete ===" << endl;
    return 0;
}
