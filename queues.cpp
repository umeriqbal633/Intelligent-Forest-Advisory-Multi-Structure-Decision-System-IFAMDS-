// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : queues.cpp
// Layer  : Part 3 — Queue-Based Scheduling Engine
//
// Implements:
//   SimpleQueue   — linked-node FIFO (Q1, Q2, Q4)
//   PriorityQueue — array max-heap   (Q3)
//   SchedulingEngine — unified task management API
//
// Key scheduling algorithm:
//   FIFO  (Q1/Q2/Q4): enqueue at tail O(1), dequeue from head O(1)
//   Heap  (Q3)       : insert + bubbleUp O(log n),
//                      extractMax + siftDown O(log n)
//   Priority switching: dequeue from source, insert/enqueue to target
//   Load balancing   : redistribute tasks across Q1 → Q2
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#include "queues.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ============================================================
// Utility — prints a single Task's details
// Time Complexity: O(1)
// ============================================================
static void printTask(const Task& t, const string& prefix = "  ")
{
    cout << prefix
         << "Task#" << setw(3) << t.taskId
         << " | Zone=" << setw(2) << t.zoneId
         << " | Pri=" << t.priority
         << " | Type=" << setw(12) << left << t.type << right
         << " | " << t.description
         << (t.isPaused ? " [PAUSED]" : "")
         << endl;
}

// ============================================================
// class SimpleQueue — Implementation
// ============================================================

// Initialises an empty FIFO queue.
// Time Complexity: O(1)
SimpleQueue::SimpleQueue() : head(nullptr), tail(nullptr),
                              count(0), paused(false) {}

// Destructor — walks from head to tail freeing every QueueNode.
// Time Complexity: O(n)
SimpleQueue::~SimpleQueue()
{
    QueueNode* curr = head;
    while (curr != nullptr)
    {
        QueueNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
    head = tail = nullptr;
}

// Enqueues task t at the tail of the linked list.
// If queue is empty, head and tail both point to the new node.
// FIFO principle: first in, first served.
// Time Complexity: O(1)
void SimpleQueue::enqueue(Task t)
{
    QueueNode* node = new QueueNode();
    node->data = t;
    node->next = nullptr;

    if (tail == nullptr)
    {
        // Queue was empty — new node is both head and tail
        head = tail = node;
    }
    else
    {
        // Append to tail and advance tail pointer
        tail->next = node;
        tail       = node;
    }
    count++;
}

// Dequeues the task at the head of the queue.
// Advances head to the next node; if queue becomes empty,
// tail is also reset to nullptr.
// Time Complexity: O(1)
Task SimpleQueue::dequeue()
{
    if (head == nullptr)
    {
        cout << "  [Queue] ERROR: Cannot dequeue — queue is empty." << endl;
        Task empty = {-1, "EMPTY", 0, -1, "NONE", false, 0};
        return empty;
    }

    QueueNode* temp = head;
    Task       t    = head->data;
    head            = head->next;

    if (head == nullptr)
        tail = nullptr;  // Queue is now empty

    delete temp;
    count--;
    return t;
}

// Returns (without removing) the task at the head.
// Time Complexity: O(1)
Task SimpleQueue::peek() const
{
    if (head == nullptr)
    {
        Task empty = {-1, "EMPTY", 0, -1, "NONE", false, 0};
        return empty;
    }
    return head->data;
}

// Returns true if queue has no tasks.
// Time Complexity: O(1)
bool SimpleQueue::isEmpty() const { return count == 0; }

// Returns current number of tasks in queue.
// Time Complexity: O(1)
int SimpleQueue::size() const { return count; }

// Sets the paused flag on this queue.
// Time Complexity: O(1)
void SimpleQueue::setPaused(bool state) { paused = state; }

// Returns the current paused state.
// Time Complexity: O(1)
bool SimpleQueue::isPausedState() const { return paused; }

// Prints all tasks from head to tail.
// Time Complexity: O(n)
void SimpleQueue::display(const string& queueName) const
{
    cout << "  ┌─ " << queueName
         << " [" << count << " task(s)]"
         << (paused ? " — PAUSED" : " — ACTIVE")
         << endl;

    if (head == nullptr)
    {
        cout << "  │  (empty)" << endl;
    }
    else
    {
        QueueNode* curr = head;
        int i = 0;
        while (curr != nullptr)
        {
            cout << "  │  [" << i++ << "] ";
            printTask(curr->data, "");
            curr = curr->next;
        }
    }
    cout << "  └──────────────────────────────────────────" << endl;
}

// ============================================================
// class PriorityQueue — Max-Heap Implementation
// ============================================================

// Initialises empty heap.
// Time Complexity: O(1)
PriorityQueue::PriorityQueue() : heapSize(0) {}

// Swaps two tasks in the heap array.
// Time Complexity: O(1)
void PriorityQueue::swapTasks(int a, int b)
{
    Task temp = heap[a];
    heap[a]   = heap[b];
    heap[b]   = temp;
}

// Restores heap property upward from index i after insertion.
// Compares child with parent; swaps if child has higher priority.
// Heap property: parent.priority >= child.priority (max-heap)
// Time Complexity: O(log n)
void PriorityQueue::bubbleUp(int i)
{
    while (i > 0)
    {
        int parent = (i - 1) / 2;
        if (heap[i].priority > heap[parent].priority)
        {
            swapTasks(i, parent);
            i = parent;
        }
        else
            break;  // Heap property satisfied
    }
}

// Restores heap property downward from index i after extraction.
// Compares parent with both children; swaps with the larger child.
// Time Complexity: O(log n)
void PriorityQueue::siftDown(int i)
{
    while (true)
    {
        int largest = i;
        int left    = 2 * i + 1;
        int right   = 2 * i + 2;

        if (left  < heapSize && heap[left].priority  > heap[largest].priority)
            largest = left;
        if (right < heapSize && heap[right].priority > heap[largest].priority)
            largest = right;

        if (largest != i)
        {
            swapTasks(i, largest);
            i = largest;
        }
        else
            break;  // Heap property satisfied
    }
}

// Inserts task t at the end of the heap array, then bubbles up.
// Scheduling algorithm: O(log n) — required by project spec
// Time Complexity: O(log n)
void PriorityQueue::insert(Task t)
{
    if (heapSize >= MAX_TASKS)
    {
        cout << "  [Q3] ERROR: Priority queue full (max=" << MAX_TASKS << ")." << endl;
        return;
    }
    heap[heapSize] = t;
    heapSize++;
    bubbleUp(heapSize - 1);  // Restore max-heap property
}

// Extracts the root (highest priority task) from the heap.
// Swaps root with last element, reduces heapSize, sifts down.
// Scheduling algorithm: O(log n) — required by project spec
// Time Complexity: O(log n)
Task PriorityQueue::extractMax()
{
    if (heapSize == 0)
    {
        cout << "  [Q3] ERROR: Cannot extract — priority queue is empty." << endl;
        Task empty = {-1, "EMPTY", 0, -1, "NONE", false, 0};
        return empty;
    }

    Task maxTask  = heap[0];          // Save root (highest priority)
    heap[0]       = heap[heapSize-1]; // Move last element to root
    heapSize--;
    if (heapSize > 0)
        siftDown(0);                  // Restore heap property downward

    return maxTask;
}

// Returns the highest-priority task without removing it.
// Time Complexity: O(1)
Task PriorityQueue::peekMax() const
{
    if (heapSize == 0)
    {
        Task empty = {-1, "EMPTY", 0, -1, "NONE", false, 0};
        return empty;
    }
    return heap[0];
}

// Returns true if heap is empty.
// Time Complexity: O(1)
bool PriorityQueue::isEmpty() const { return heapSize == 0; }

// Returns current heap size.
// Time Complexity: O(1)
int PriorityQueue::size() const { return heapSize; }

// Prints all tasks currently in the heap (array order, not sorted).
// Time Complexity: O(n)
void PriorityQueue::display(const string& queueName) const
{
    cout << "  ┌─ " << queueName
         << " [" << heapSize << " task(s)] — MAX-HEAP (root=highest priority)"
         << endl;

    if (heapSize == 0)
    {
        cout << "  │  (empty)" << endl;
    }
    else
    {
        for (int i = 0; i < heapSize; i++)
        {
            cout << "  │  heap[" << i << "] ";
            printTask(heap[i], "");
        }
    }
    cout << "  └──────────────────────────────────────────" << endl;
}

// ============================================================
// class SchedulingEngine — Implementation
// ============================================================

// Initialises all four queues and task counter.
// Time Complexity: O(1)
SchedulingEngine::SchedulingEngine() : taskCounter(1)
{
    cout << "[SchedulingEngine] Initialised — Q1/Q2/Q3/Q4 ready." << endl;
}

// Builds a Task struct with auto-assigned ID and current counter.
// Time Complexity: O(1)
Task SchedulingEngine::buildTask(const string& desc, int priority,
                                  int zone, const string& type)
{
    Task t;
    t.taskId      = taskCounter++;
    t.description = desc;
    t.priority    = priority;
    t.zoneId      = zone;
    t.type        = type;
    t.isPaused    = false;
    t.timestamp   = taskCounter;  // Logical creation order
    return t;
}

// ── Task Addition ─────────────────────────────────────────

// Creates a PRIORITY_LOW routine task and enqueues to Q1.
// Q1 stores normal sensor-check tasks processed in arrival order.
// Time Complexity: O(1)
void SchedulingEngine::addRoutineTask(const string& desc, int zone)
{
    Task t = buildTask(desc, PRIORITY_LOW, zone, "ROUTINE");
    q1.enqueue(t);
    cout << "[Q1] Routine task added    | Task#" << t.taskId
         << " Zone=" << zone << " | \"" << desc << "\"" << endl;
}

// Creates a PRIORITY_MED surveillance task and enqueues to Q2.
// Q2 stores frequent zone-check tasks from sensitive regions.
// Time Complexity: O(1)
void SchedulingEngine::addSurveillanceTask(const string& desc, int zone)
{
    Task t = buildTask(desc, PRIORITY_MED, zone, "SURVEILLANCE");
    q2.enqueue(t);
    cout << "[Q2] Surveillance task added | Task#" << t.taskId
         << " Zone=" << zone << " | \"" << desc << "\"" << endl;
}

// Creates a PRIORITY_CRITICAL emergency task and inserts to Q3.
// Q3 is a max-heap — highest priority task is always processed first.
// Scheduling algorithm: heap insert + bubbleUp = O(log n)
// Time Complexity: O(log n)
void SchedulingEngine::addEmergencyTask(const string& desc, int zone)
{
    Task t = buildTask(desc, PRIORITY_CRITICAL, zone, "EMERGENCY");
    q3.insert(t);
    cout << "[Q3] Emergency task added  | Task#" << t.taskId
         << " Zone=" << zone << " Pri=" << PRIORITY_CRITICAL
         << " | \"" << desc << "\"" << endl;
}

// Creates a PRIORITY_HIGH multi-factor task and enqueues to Q4.
// Q4 holds tasks that require combined inputs (temp+smoke+wind).
// Time Complexity: O(1)
void SchedulingEngine::addMultiFactorTask(const string& desc, int zone)
{
    Task t = buildTask(desc, PRIORITY_HIGH, zone, "MULTI");
    q4.enqueue(t);
    cout << "[Q4] Multi-factor task added | Task#" << t.taskId
         << " Zone=" << zone << " | \"" << desc << "\"" << endl;
}

// ── Task Processing ───────────────────────────────────────

// Processes (removes and prints) the next task from the given queue.
// Q1/Q2/Q4: dequeue from head — O(1)
// Q3       : extractMax from heap root — O(log n)
// Paused queues are skipped with a warning.
// Time Complexity: O(1) or O(log n) for Q3
void SchedulingEngine::processNextTask(int queueId)
{
    cout << "\n[Process] Executing next task from Q" << queueId << "..." << endl;

    Task t;
    bool isEmpty = false;
    bool isPaused = false;

    if (queueId == QUEUE_ROUTINE)
    {
        isPaused = q1.isPausedState();
        isEmpty  = q1.isEmpty();
        if (!isPaused && !isEmpty) t = q1.dequeue();
    }
    else if (queueId == QUEUE_SURVEILLANCE)
    {
        isPaused = q2.isPausedState();
        isEmpty  = q2.isEmpty();
        if (!isPaused && !isEmpty) t = q2.dequeue();
    }
    else if (queueId == QUEUE_EMERGENCY)
    {
        isEmpty  = q3.isEmpty();
        if (!isEmpty) t = q3.extractMax();
    }
    else if (queueId == QUEUE_MULTIFACTOR)
    {
        isPaused = q4.isPausedState();
        isEmpty  = q4.isEmpty();
        if (!isPaused && !isEmpty) t = q4.dequeue();
    }
    else
    {
        cout << "  [!] Invalid queue ID: " << queueId << endl;
        return;
    }

    if (isPaused)
    {
        cout << "  [!] Q" << queueId << " is PAUSED — task not processed." << endl;
        return;
    }
    if (isEmpty)
    {
        cout << "  [!] Q" << queueId << " is EMPTY — nothing to process." << endl;
        return;
    }

    // ── Task execution output ──
    cout << "  ╔══════════════════════════════════════════╗" << endl;
    cout << "  ║  TASK EXECUTED                           ║" << endl;
    cout << "  ╠══════════════════════════════════════════╣" << endl;
    cout << "  ║  Task ID    : " << setw(4) << t.taskId       << "                       ║" << endl;
    cout << "  ║  Type       : " << setw(12) << left << t.type << right << "                   ║" << endl;
    cout << "  ║  Zone       : " << setw(4) << t.zoneId       << "                       ║" << endl;
    cout << "  ║  Priority   : " << setw(4) << t.priority     << "                       ║" << endl;
    cout << "  ║  Description: " << t.description              << endl;
    cout << "  ╚══════════════════════════════════════════╝" << endl;
}

// Moves the front task from sourceQueue to targetQueue.
// Use case: escalate a routine task to emergency when danger detected.
// Dequeue from source (O(1)), insert/enqueue to target (O(1)/O(log n)).
// Time Complexity: O(1) or O(log n) if target is Q3
void SchedulingEngine::prioritySwitch(int sourceQueue, int targetQueue)
{
    cout << "\n[PrioritySwitch] Moving task: Q" << sourceQueue
         << " → Q" << targetQueue << endl;

    // Check source is not empty
    bool sourceEmpty = false;
    if      (sourceQueue == QUEUE_ROUTINE)     sourceEmpty = q1.isEmpty();
    else if (sourceQueue == QUEUE_SURVEILLANCE)sourceEmpty = q2.isEmpty();
    else if (sourceQueue == QUEUE_EMERGENCY)   sourceEmpty = q3.isEmpty();
    else if (sourceQueue == QUEUE_MULTIFACTOR) sourceEmpty = q4.isEmpty();

    if (sourceEmpty)
    {
        cout << "  [!] Source Q" << sourceQueue << " is empty — nothing to move." << endl;
        return;
    }

    // Dequeue from source
    Task t;
    if      (sourceQueue == QUEUE_ROUTINE)      t = q1.dequeue();
    else if (sourceQueue == QUEUE_SURVEILLANCE)  t = q2.dequeue();
    else if (sourceQueue == QUEUE_EMERGENCY)     t = q3.extractMax();
    else                                          t = q4.dequeue();

    cout << "  Moved: Task#" << t.taskId << " \"" << t.description << "\"" << endl;

    // Update priority to match the target queue's level
    if      (targetQueue == QUEUE_ROUTINE)      { t.priority = PRIORITY_LOW;      q1.enqueue(t); }
    else if (targetQueue == QUEUE_SURVEILLANCE) { t.priority = PRIORITY_MED;      q2.enqueue(t); }
    else if (targetQueue == QUEUE_EMERGENCY)    { t.priority = PRIORITY_CRITICAL; q3.insert(t);  }
    else if (targetQueue == QUEUE_MULTIFACTOR)  { t.priority = PRIORITY_HIGH;     q4.enqueue(t); }

    cout << "  Task#" << t.taskId << " now in Q" << targetQueue
         << " with priority=" << t.priority << endl;
}

// Balances load by moving tasks from an overloaded Q1 to Q2.
// Load balancing rule: if Q1.size() > 10, move 3 tasks to Q2.
// This prevents Q1 from becoming a bottleneck during high sensor load.
// Time Complexity: O(n) in worst case (multiple redistributions)
void SchedulingEngine::balanceLoad()
{
    cout << "\n[LoadBalance] Checking queue loads..." << endl;
    displayLoadSummary();

    const int OVERFLOW_THRESHOLD = 10;
    const int TASKS_TO_MOVE      = 3;

    if (q1.size() > OVERFLOW_THRESHOLD)
    {
        cout << "  [!] Q1 overloaded (" << q1.size()
             << " tasks > " << OVERFLOW_THRESHOLD
             << ") — redistributing " << TASKS_TO_MOVE << " tasks to Q2." << endl;

        int moved = 0;
        while (moved < TASKS_TO_MOVE && !q1.isEmpty())
        {
            Task t    = q1.dequeue();
            t.priority = PRIORITY_MED;      // Upgrade to surveillance level
            t.type     = "SURVEILLANCE";
            q2.enqueue(t);
            cout << "  Moved Task#" << t.taskId << " from Q1 → Q2" << endl;
            moved++;
        }
        cout << "  Redistribution complete." << endl;
    }
    else
    {
        cout << "  [OK] All queues within load limits." << endl;
    }

    displayLoadSummary();
}

// ── Pause / Resume ────────────────────────────────────────

// Pauses the specified queue.
// Time Complexity: O(1)
void SchedulingEngine::pauseQueue(int queueId)
{
    if      (queueId == QUEUE_ROUTINE)      q1.setPaused(true);
    else if (queueId == QUEUE_SURVEILLANCE) q2.setPaused(true);
    else if (queueId == QUEUE_MULTIFACTOR)  q4.setPaused(true);
    else
    {
        cout << "[Pause] Q" << queueId << " cannot be paused (Q3 emergency always active)." << endl;
        return;
    }
    cout << "[Pause] Q" << queueId << " is now PAUSED." << endl;
}

// Resumes the specified queue.
// Time Complexity: O(1)
void SchedulingEngine::resumeQueue(int queueId)
{
    if      (queueId == QUEUE_ROUTINE)      q1.setPaused(false);
    else if (queueId == QUEUE_SURVEILLANCE) q2.setPaused(false);
    else if (queueId == QUEUE_MULTIFACTOR)  q4.setPaused(false);
    else
    {
        cout << "[Resume] Q" << queueId << " is always active." << endl;
        return;
    }
    cout << "[Resume] Q" << queueId << " is now ACTIVE." << endl;
}

// ── Display ───────────────────────────────────────────────

// Prints a one-line load summary across all four queues.
// Time Complexity: O(1)
void SchedulingEngine::displayLoadSummary() const
{
    cout << "  Load → Q1(Routine):" << setw(3) << q1.size()
         << "  Q2(Surv):"  << setw(3) << q2.size()
         << "  Q3(Emerg):" << setw(3) << q3.size()
         << "  Q4(Multi):" << setw(3) << q4.size()
         << "  Total:"     << setw(3) << totalTasks() << endl;
}

// Prints a full dashboard showing all four queues.
// Time Complexity: O(n)
void SchedulingEngine::displayAllQueues() const
{
    cout << "\n╔══════════════════════════════════════════════════╗" << endl;
    cout << "║        Scheduling Engine — Queue Dashboard        ║" << endl;
    cout << "╚══════════════════════════════════════════════════╝" << endl;
    displayLoadSummary();
    cout << endl;
    q1.display("Q1 — Routine Monitoring      (FIFO)");
    q2.display("Q2 — Continuous Surveillance (FIFO)");
    q3.display("Q3 — Emergency Response      (MAX-HEAP)");
    q4.display("Q4 — Multi-Factor Decision   (FIFO)");
}

// Returns total task count across all queues.
// Time Complexity: O(1)
int SchedulingEngine::totalTasks() const
{
    return q1.size() + q2.size() + q3.size() + q4.size();
}
