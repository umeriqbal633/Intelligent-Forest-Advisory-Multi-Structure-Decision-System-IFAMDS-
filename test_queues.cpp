#include "queues.h"
#include <iostream>
using namespace std;
int main()
{
    cout << "=== IFAMDS — Part 3 Queue Scheduling Engine Test ===" << endl;
    SchedulingEngine engine;

    // Add tasks to all four queues
    cout << "\n--- Adding Tasks ---" << endl;
    engine.addRoutineTask("Check temperature Zone 0", 0);
    engine.addRoutineTask("Check humidity Zone 1", 1);
    engine.addRoutineTask("Check smoke Zone 2", 2);
    engine.addSurveillanceTask("Monitor dry zone 4", 4);
    engine.addSurveillanceTask("Frequent scan Zone 5", 5);
    engine.addEmergencyTask("FIRE detected Zone 7", 7);
    engine.addEmergencyTask("High smoke Zone 3", 3);
    engine.addEmergencyTask("Critical temp Zone 6", 6);
    engine.addMultiFactorTask("Evaluate fire+smoke+wind Zone 7", 7);
    engine.addMultiFactorTask("Combined risk Zone 4", 4);

    // Display all queues
    engine.displayAllQueues();

    // Process tasks
    cout << "\n--- Processing Tasks ---" << endl;
    engine.processNextTask(QUEUE_EMERGENCY);     // Should get highest priority
    engine.processNextTask(QUEUE_EMERGENCY);
    engine.processNextTask(QUEUE_ROUTINE);
    engine.processNextTask(QUEUE_MULTIFACTOR);

    // Test priority switch
    cout << "\n--- Priority Switch Test ---" << endl;
    engine.prioritySwitch(QUEUE_ROUTINE, QUEUE_EMERGENCY);

    // Test pause / resume
    cout << "\n--- Pause / Resume Test ---" << endl;
    engine.pauseQueue(QUEUE_ROUTINE);
    engine.processNextTask(QUEUE_ROUTINE);   // Should be blocked
    engine.resumeQueue(QUEUE_ROUTINE);
    engine.processNextTask(QUEUE_ROUTINE);   // Should work now

    // Test load balancing — add 11 tasks to Q1 first
    cout << "\n--- Load Balance Test ---" << endl;
    for (int i = 0; i < 11; i++)
        engine.addRoutineTask("Bulk routine task Zone " + to_string(i), i);
    engine.balanceLoad();

    // Final dashboard
    engine.displayAllQueues();

    cout << "\n=== Part 3 Test Complete ===" << endl;
    return 0;
}
