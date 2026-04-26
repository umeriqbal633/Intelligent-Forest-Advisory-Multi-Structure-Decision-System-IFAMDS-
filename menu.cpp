// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : menu.cpp
// Purpose: Full implementation of all menu functions declared
//          in menu.h. Provides the interactive console UI that
//          drives all three Partner A layers.
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#include "menu.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

using namespace std;

// ============================================================
// Utility Functions
// ============================================================

// Prints the IFAMDS startup banner.
void printBanner()
{
    cout << "\n";
    printSeparator('=', 60);
    cout << "   IFAMDS — Intelligent Forest Advisory &" << endl;
    cout << "            Multi-Structure Decision System" << endl;
    cout << "   CL2001 Data Structures Project 2026" << endl;
    cout << "   Partner A  |  Parts 1 (Arrays) + 2 (LL) + 3 (Queue)" << endl;
    printSeparator('=', 60);
}

// Prints a separator line of given character and width.
void printSeparator(char c, int width)
{
    for (int i = 0; i < width; i++) cout << c;
    cout << endl;
}

// Clears the screen (cross-platform safe).
void clearScreen()
{
    cout << "\n\n";
    printSeparator('-', 60);
}

// Prompts for an integer in [minVal, maxVal], re-prompts on bad input.
int getIntInput(const string& prompt, int minVal, int maxVal)
{
    int val;
    while (true)
    {
        cout << prompt;
        if (cin >> val && val >= minVal && val <= maxVal)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Enter a number between "
             << minVal << " and " << maxVal << "." << endl;
    }
}

// Prompts for a float value, re-prompts on bad input.
float getFloatInput(const string& prompt)
{
    float val;
    while (true)
    {
        cout << prompt;
        if (cin >> val)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Please enter a number." << endl;
    }
}

// ============================================================
// Integration: Anomaly Pipeline
// Connects Part 1 → Part 2 → Part 3 automatically.
// Called whenever flagAnomaly() returns true.
// ============================================================
void triggerAnomalyPipeline(SensorReading& r, EventMemory& em,
                             SchedulingEngine& sched, int eventTime)
{
    printSeparator('!', 60);
    cout << "  [ANOMALY PIPELINE] Zone " << r.zoneId
         << " — triggering cross-layer response..." << endl;

    // Part 2: Log anomaly event to L3 (Anomaly Event Stream)
    em.insertEvent(r.temperature, eventTime, r.zoneId,
                   LIST_ANOMALY, true);
    cout << "  [->L3] Anomaly event logged to Anomaly Stream." << endl;

    // Part 2: Also log to L9 emergency circular monitor
    em.insertEvent(r.temperature, eventTime, r.zoneId,
                   LIST_EMERGENCY, true);
    cout << "  [->L9] Emergency circular monitor updated." << endl;

    // Part 3: Create emergency task in Q3 priority heap
    string taskDesc = "ANOMALY Zone " + to_string(r.zoneId)
                    + " Temp=" + to_string((int)r.temperature)
                    + "C Smoke=" + to_string((int)r.smokeLevel);
    sched.addEmergencyTask(taskDesc, r.zoneId);
    cout << "  [->Q3] Emergency task queued for immediate processing." << endl;

    printSeparator('!', 60);
}

// ============================================================
// Main Menu Display
// ============================================================
void showMainMenu()
{
    cout << "\n";
    printSeparator('=', 56);
    cout << "  IFAMDS MAIN MENU" << endl;
    printSeparator('-', 56);
    cout << "  1.  Input Environmental Data       [Arrays]" << endl;
    cout << "  2.  View Forest Grid Status        [Arrays]" << endl;
    cout << "  3.  Event Memory System            [Linked Lists]" << endl;
    cout << "  4.  Fire Detection & Control       [Integration]" << endl;
    cout << "  5.  Task Scheduling System         [Queues]" << endl;
    printSeparator('-', 56);
    cout << "  0.  Exit System" << endl;
    printSeparator('=', 56);
    cout << "  Enter choice: ";
}

// ============================================================
// Main Menu Loop
// ============================================================
void runMenu(ArrayLayer& arr, EventMemory& em, SchedulingEngine& sched)
{
    int choice = -1;
    do
    {
        showMainMenu();
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        else
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

        clearScreen();

        switch (choice)
        {
            case 1: menuInputData(arr, em, sched); break;
            case 2: menuViewGrid(arr);              break;
            case 3: menuEventMemory(em);            break;
            case 4: menuFireControl(arr, em, sched);break;
            case 5: menuScheduling(sched);          break;
            case 0:
                cout << "\n  [IFAMDS] System shutting down. Goodbye." << endl;
                printSeparator('=', 56);
                break;
            default:
                cout << "  [!] Invalid choice. Please select 0–5." << endl;
        }

    } while (choice != 0);
}

// ============================================================
// Sub-Menu 1: Input Environmental Data
// ============================================================
void menuInputData(ArrayLayer& arr, EventMemory& em, SchedulingEngine& sched)
{
    int choice = -1;
    do
    {
        cout << "\n";
        printSeparator('-', 56);
        cout << "  MENU 1 — Input Environmental Data" << endl;
        printSeparator('-', 56);
        cout << "  1.1  Add Sensor Reading (manual entry)" << endl;
        cout << "  1.2  Compare Last Reading with Baseline" << endl;
        cout << "  1.3  Filter Noise on Last Two Readings" << endl;
        cout << "  1.4  Check Anomaly on Last Reading" << endl;
        cout << "  1.5  Display Baseline (A1)" << endl;
        cout << "  0.   Back to Main Menu" << endl;
        printSeparator('-', 56);

        choice = getIntInput("  Choice: ", 0, 5);

        switch (choice)
        {
            case 1:
            {
                cout << "\n  [1.1] Enter Sensor Reading:" << endl;
                SensorReading r;
                r.temperature = getFloatInput("    Temperature (°C)  : ");
                r.humidity    = getFloatInput("    Humidity (%)      : ");
                r.smokeLevel  = getFloatInput("    Smoke Level       : ");
                r.windSpeed   = getFloatInput("    Wind Speed (km/h) : ");
                r.zoneId      = getIntInput  ("    Zone ID (0-8)     : ", 0, NUM_ZONES - 1);
                r.isValid     = true;
                r.isAnomaly   = false;

                arr.ingestReading(r);

                // Auto-log raw event to L1 on every ingestion
                em.insertEvent(r.temperature, arr.getStreamCount(),
                               r.zoneId, LIST_RAW);
                cout << "  [->L1] Reading also logged to Raw Event Stream." << endl;
                break;
            }
            case 2:
            {
                cout << "\n  [1.2] Baseline Comparison:" << endl;
                if (arr.getStreamCount() == 0)
                { cout << "  [!] No readings yet. Add a sensor reading first." << endl; break; }
                SensorReading last = arr.getLatestReading();
                arr.compareWithBaseline(last);
                break;
            }
            case 3:
            {
                cout << "\n  [1.3] Noise Filter (last two readings):" << endl;
                int cnt = arr.getStreamCount();
                if (cnt < 2)
                { cout << "  [!] Need at least 2 readings." << endl; break; }
                arr.filterNoise(cnt - 1);
                break;
            }
            case 4:
            {
                cout << "\n  [1.4] Anomaly Detection on Latest Reading:" << endl;
                if (arr.getStreamCount() == 0)
                { cout << "  [!] No readings yet." << endl; break; }
                SensorReading last = arr.getLatestReading();
                bool anomaly = arr.flagAnomaly(last);
                if (anomaly)
                    triggerAnomalyPipeline(last, em, sched,
                                           arr.getStreamCount());
                break;
            }
            case 5:
                arr.displayBaseline();
                break;
            case 0:
                break;
            default:
                cout << "  [!] Invalid option." << endl;
        }

    } while (choice != 0);
}

// ============================================================
// Sub-Menu 2: View Forest Grid Status
// ============================================================
void menuViewGrid(ArrayLayer& arr)
{
    int choice = -1;
    do
    {
        cout << "\n";
        printSeparator('-', 56);
        cout << "  MENU 2 — View Forest Grid Status" << endl;
        printSeparator('-', 56);
        cout << "  2.1  Display Time Series (A2 Stream)" << endl;
        cout << "  2.2  Display Static Grid (A3)" << endl;
        cout << "  2.3  Display Dynamic Terrain (A4)" << endl;
        cout << "  2.4  Stream Status Dashboard" << endl;
        cout << "  2.5  Interpolate Missing Zone Value" << endl;
        cout << "  2.6  Detect Boundary Between Zones" << endl;
        cout << "  0.   Back to Main Menu" << endl;
        printSeparator('-', 56);

        choice = getIntInput("  Choice: ", 0, 6);

        switch (choice)
        {
            case 1: arr.displayTimeSeries();  break;
            case 2: arr.displayGrid();        break;
            case 3: arr.displayTerrain();     break;
            case 4: arr.displayStreamStatus();break;
            case 5:
            {
                int r = getIntInput("    Row (0-4): ", 0, ROWS - 1);
                int c = getIntInput("    Col (0-4): ", 0, COLS - 1);
                arr.interpolateMissing(r, c);
                break;
            }
            case 6:
            {
                int r = getIntInput("    Row (0-4): ", 0, ROWS - 1);
                int c = getIntInput("    Col (0-4): ", 0, COLS - 1);
                arr.detectBoundary(r, c);
                break;
            }
            case 0: break;
        }

    } while (choice != 0);
}

// ============================================================
// Sub-Menu 3: Event Memory System
// ============================================================
void menuEventMemory(EventMemory& em)
{
    int choice = -1;
    do
    {
        cout << "\n";
        printSeparator('-', 56);
        cout << "  MENU 3 — Event Memory System" << endl;
        printSeparator('-', 56);
        cout << "  3.1  Insert Event into a List" << endl;
        cout << "  3.2  Traverse Forward  (SLL: L1/L2/L3)" << endl;
        cout << "  3.3  Traverse Backward (DLL: L4/L5/L6)" << endl;
        cout << "  3.4  Circular Scan     (CLL: L7/L8/L9/L10)" << endl;
        cout << "  3.5  Restore Last Stable State" << endl;
        cout << "  3.6  Synchronize State (L6)" << endl;
        cout << "  3.7  Detect & Filter Noise in SLL" << endl;
        cout << "  3.8  Correct an Event  (DLL)" << endl;
        cout << "  3.9  Display All Lists" << endl;
        cout << "  0.   Back to Main Menu" << endl;
        printSeparator('-', 56);

        choice = getIntInput("  Choice: ", 0, 9);

        switch (choice)
        {
            case 1:
            {
                float val  = getFloatInput("    Value      : ");
                int   time = getIntInput  ("    Timestamp  : ", 0, 9999);
                int   zone = getIntInput  ("    Zone ID    : ", 0, NUM_ZONES - 1);
                cout << "    List ID (1=Raw,2=Verified,3=Anomaly," << endl;
                cout << "             4=FwdCorr,5=BwdCorr,6=Sync," << endl;
                cout << "             7=Local,8=System,9=Emerg,10=Stab): ";
                int listId = getIntInput  ("", 1, 10);
                em.insertEvent(val, time, zone, listId);
                break;
            }
            case 2:
            {
                int id = getIntInput("    List ID (1-3): ", 1, 3);
                em.traverseForward(id);
                break;
            }
            case 3:
            {
                int id = getIntInput("    List ID (4-6): ", 4, 6);
                em.traverseBackward(id);
                break;
            }
            case 4:
            {
                int id = getIntInput("    List ID (7-10): ", 7, 10);
                em.circularScan(id);
                break;
            }
            case 5: em.restoreLastStable();  break;
            case 6: em.synchronizeState();   break;
            case 7:
            {
                int id = getIntInput("    SLL List ID (1-3): ", 1, 3);
                em.detectAndFilterNoise(id);
                break;
            }
            case 8:
            {
                int   listId = getIntInput  ("    DLL List ID (4-6): ", 4, 6);
                int   zone   = getIntInput  ("    Zone ID to correct: ", 0, NUM_ZONES - 1);
                float newVal = getFloatInput("    New value          : ");
                em.correctEvent(listId, zone, newVal);
                break;
            }
            case 9: em.displayAllLists(); break;
            case 0: break;
        }

    } while (choice != 0);
}

// ============================================================
// Sub-Menu 4: Fire Detection & Control
// ============================================================
void menuFireControl(ArrayLayer& arr, EventMemory& em, SchedulingEngine& sched)
{
    int choice = -1;
    do
    {
        cout << "\n";
        printSeparator('-', 56);
        cout << "  MENU 4 — Fire Detection & Control" << endl;
        printSeparator('-', 56);
        cout << "  4.1  Detect Fire Risk (threshold check)" << endl;
        cout << "  4.2  Trigger Emergency Alert for a Zone" << endl;
        cout << "  4.3  Priority-Based Fire Response" << endl;
        cout << "  4.4  Simulate Fire Spread [BFS — Graph Layer]" << endl;
        cout << "  4.5  Allocate Firefighting Resources" << endl;
        cout << "  0.   Back to Main Menu" << endl;
        printSeparator('-', 56);

        choice = getIntInput("  Choice: ", 0, 5);

        switch (choice)
        {
            case 1:
            {
                // Scan all stored readings and report fire risk zones
                cout << "\n  [4.1] Fire Risk Assessment — scanning all readings:" << endl;
                printSeparator('-', 56);
                bool anyRisk = false;
                for (int i = 0; i < arr.getStreamCount(); i++)
                {
                    SensorReading r = arr.getReading(i);
                    if (r.temperature > TEMP_MAX || r.smokeLevel > SMOKE_MAX
                        || r.humidity < HUM_MIN)
                    {
                        cout << "  [FIRE RISK] Zone " << r.zoneId
                             << "  Temp=" << r.temperature
                             << "C  Smoke=" << r.smokeLevel
                             << "  Hum=" << r.humidity << "%" << endl;
                        anyRisk = true;
                    }
                }
                if (!anyRisk)
                    cout << "  [OK] No fire risk zones detected in current data." << endl;
                break;
            }
            case 2:
            {
                // Manually trigger emergency alert for a given zone
                int zone = getIntInput("  Zone ID to alert (0-8): ", 0, NUM_ZONES - 1);
                float temp  = getFloatInput("  Reported temperature   : ");
                float smoke = getFloatInput("  Reported smoke level   : ");

                cout << "\n  [4.2] EMERGENCY ALERT — Zone " << zone << endl;

                // Log to L9 emergency monitor
                em.insertEvent(temp, arr.getStreamCount(), zone, LIST_EMERGENCY, true);

                // Add critical task to Q3
                string desc = "MANUAL ALERT Zone " + to_string(zone)
                            + " T=" + to_string((int)temp)
                            + " S=" + to_string((int)smoke);
                sched.addEmergencyTask(desc, zone);
                break;
            }
            case 3:
            {
                // Process highest priority emergency tasks
                cout << "\n  [4.3] Priority-Based Fire Response:" << endl;
                cout << "  Processing top 3 emergency tasks from Q3..." << endl;
                for (int i = 0; i < 3; i++)
                    sched.processNextTask(QUEUE_EMERGENCY);
                break;
            }
            case 4:
            {
                // BFS fire spread — stub for Partner B's graph layer
                // Algorithm note: BFS = Breadth First Search
                // Uses a queue to visit each adjacent zone level by level.
                // Full implementation in Partner B's graph module.
                // Time Complexity: O(V + E) where V=zones, E=paths
                cout << "\n  [4.4] BFS Fire Spread Simulation" << endl;
                printSeparator('-', 56);
                int startZone = getIntInput("  Start zone (fire origin, 0-8): ", 0, NUM_ZONES - 1);
                cout << "  BFS from Zone " << startZone << ":" << endl;
                cout << "  Level 0 (origin) : Zone " << startZone << endl;

                // Simple adjacency simulation (4-connected grid)
                int row = startZone / COLS;
                int col = startZone % COLS;
                cout << "  Level 1 (adjacent zones):" << endl;
                int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
                for (auto& d : dirs)
                {
                    int nr = row + d[0], nc = col + d[1];
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
                        cout << "    -> Zone " << (nr * COLS + nc) << endl;
                }
                cout << "  [NOTE] Full multi-level BFS implemented in Partner B graph module." << endl;
                break;
            }
            case 5:
            {
                // Resource allocation based on anomaly count
                cout << "\n  [4.5] Firefighting Resource Allocation:" << endl;
                printSeparator('-', 56);
                int riskZones = 0;
                for (int i = 0; i < arr.getStreamCount(); i++)
                {
                    SensorReading r = arr.getReading(i);
                    if (r.temperature > TEMP_MAX || r.smokeLevel > SMOKE_MAX)
                    {
                        cout << "  Zone " << r.zoneId
                             << " — allocating 2 fire units + water supply." << endl;
                        sched.addMultiFactorTask(
                            "Resource deploy Zone " + to_string(r.zoneId),
                            r.zoneId);
                        riskZones++;
                    }
                }
                if (riskZones == 0)
                    cout << "  [OK] No active fire zones. All resources on standby." << endl;
                else
                    cout << "  Total zones needing resources: " << riskZones << endl;
                break;
            }
            case 0: break;
        }

    } while (choice != 0);
}

// ============================================================
// Sub-Menu 5: Task Scheduling System
// ============================================================
void menuScheduling(SchedulingEngine& sched)
{
    int choice = -1;
    do
    {
        cout << "\n";
        printSeparator('-', 56);
        cout << "  MENU 5 — Task Scheduling System" << endl;
        printSeparator('-', 56);
        cout << "  5.1  Add Routine Task      → Q1 (FIFO)" << endl;
        cout << "  5.2  Add Surveillance Task → Q2 (FIFO)" << endl;
        cout << "  5.3  Add Emergency Task    → Q3 (Heap)" << endl;
        cout << "  5.4  Add Multi-Factor Task → Q4 (FIFO)" << endl;
        cout << "  5.5  Process Next Task from Queue" << endl;
        cout << "  5.6  Priority Switch (move task between queues)" << endl;
        cout << "  5.7  Balance Load" << endl;
        cout << "  5.8  Pause a Queue" << endl;
        cout << "  5.9  Resume a Queue" << endl;
        cout << "  5.10 Display All Queues" << endl;
        cout << "  0.   Back to Main Menu" << endl;
        printSeparator('-', 56);

        choice = getIntInput("  Choice: ", 0, 10);

        switch (choice)
        {
            case 1:
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "    Task description: ";
                string desc; getline(cin, desc);
                int zone = getIntInput("    Zone ID (0-8): ", 0, NUM_ZONES - 1);
                sched.addRoutineTask(desc, zone);
                break;
            }
            case 2:
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "    Task description: ";
                string desc; getline(cin, desc);
                int zone = getIntInput("    Zone ID (0-8): ", 0, NUM_ZONES - 1);
                sched.addSurveillanceTask(desc, zone);
                break;
            }
            case 3:
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "    Task description: ";
                string desc; getline(cin, desc);
                int zone = getIntInput("    Zone ID (0-8): ", 0, NUM_ZONES - 1);
                sched.addEmergencyTask(desc, zone);
                break;
            }
            case 4:
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "    Task description: ";
                string desc; getline(cin, desc);
                int zone = getIntInput("    Zone ID (0-8): ", 0, NUM_ZONES - 1);
                sched.addMultiFactorTask(desc, zone);
                break;
            }
            case 5:
            {
                int qid = getIntInput("    Queue ID (1-4): ", 1, 4);
                sched.processNextTask(qid);
                break;
            }
            case 6:
            {
                int src = getIntInput("    Source Queue (1-4): ", 1, 4);
                int dst = getIntInput("    Target Queue (1-4): ", 1, 4);
                sched.prioritySwitch(src, dst);
                break;
            }
            case 7:  sched.balanceLoad();       break;
            case 8:
            {
                int qid = getIntInput("    Queue to pause (1,2,4): ", 1, 4);
                sched.pauseQueue(qid);
                break;
            }
            case 9:
            {
                int qid = getIntInput("    Queue to resume (1,2,4): ", 1, 4);
                sched.resumeQueue(qid);
                break;
            }
            case 10: sched.displayAllQueues(); break;
            case 0:  break;
        }

    } while (choice != 0);
}
