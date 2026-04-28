// ============================================================
// IFAMDS -- Intelligent Forest Advisory & Multi-Structure
//           Decision System
// File   : hashtable.cpp
// Layer  : Part 6 -- Hash-Based Indexing Layer
//
// Authors: Partner B
// Course : CL2001 -- Data Structures 2026
// ============================================================

#include "hashtable.h"
#include <iostream>
#include <iomanip>
#include <cstring>

// ============================================================
// PrimaryHashTable  (H1) -- open addressing, linear probe
// ============================================================

// hashFunction: Index = key % HASH_TABLE_SIZE
// Time Complexity: O(1)
int PrimaryHashTable::hashFunction(int key) const
{
    return (key % HASH_TABLE_SIZE + HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
}

// Constructor: mark every slot as empty.
// Time Complexity: O(n)
PrimaryHashTable::PrimaryHashTable() : itemCount(0)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        table[i].key        = -1;
        table[i].isOccupied = false;
        table[i].isDeleted  = false;
    }
}

// insert: linear-probe until empty or same key found.
// Time Complexity: O(1) average, O(n) worst
bool PrimaryHashTable::insert(int zoneId, const SensorReading& r)
{
    if (itemCount >= HASH_TABLE_SIZE) return false;   // table full

    int idx   = hashFunction(zoneId);
    int probe = 0;

    while (probe < HASH_TABLE_SIZE)
    {
        HashRecord& slot = table[idx];

        // Reuse deleted slot or update existing key
        if (!slot.isOccupied || slot.isDeleted || slot.key == zoneId)
        {
            bool isNew = (!slot.isOccupied || slot.isDeleted);
            slot.key        = zoneId;
            slot.reading    = r;
            slot.isOccupied = true;
            slot.isDeleted  = false;
            if (isNew) itemCount++;
            return true;
        }
        idx = (idx + 1) % HASH_TABLE_SIZE;   // linear probe
        probe++;
    }
    return false;
}

// search: probe until key found or empty slot.
// Time Complexity: O(1) average
HashRecord* PrimaryHashTable::search(int zoneId)
{
    int idx   = hashFunction(zoneId);
    int probe = 0;

    while (probe < HASH_TABLE_SIZE)
    {
        HashRecord& slot = table[idx];
        if (!slot.isOccupied && !slot.isDeleted) return nullptr; // true empty
        if (slot.isOccupied && !slot.isDeleted && slot.key == zoneId)
            return &slot;
        idx = (idx + 1) % HASH_TABLE_SIZE;
        probe++;
    }
    return nullptr;
}

// remove: lazy deletion -- mark isDeleted = true.
// Time Complexity: O(1) average
bool PrimaryHashTable::remove(int zoneId)
{
    HashRecord* rec = search(zoneId);
    if (!rec) return false;
    rec->isDeleted  = true;
    rec->isOccupied = false;
    itemCount--;
    return true;
}

int PrimaryHashTable::size() const { return itemCount; }

// display: print occupied slots.
// Time Complexity: O(n)
void PrimaryHashTable::display() const
{
    std::cout << "\n  H1 -- Primary Index Table (open addressing)\n";
    std::cout << "  " << std::string(58, '-') << "\n";
    std::cout << "  " << std::left
              << std::setw(6)  << "Slot"
              << std::setw(8)  << "ZoneID"
              << std::setw(10) << "Temp"
              << std::setw(10) << "Smoke"
              << std::setw(10) << "Humidity"
              << "Status\n";
    std::cout << "  " << std::string(58, '-') << "\n";

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        std::cout << "  [" << std::setw(2) << i << "] ";
        if (!table[i].isOccupied && !table[i].isDeleted)
        {
            std::cout << "--- empty ---\n";
        }
        else if (table[i].isDeleted)
        {
            std::cout << "--- deleted ---\n";
        }
        else
        {
            std::cout << std::setw(8)  << table[i].key
                      << std::setw(10) << std::fixed << std::setprecision(1)
                      << table[i].reading.temperature
                      << std::setw(10) << table[i].reading.smokeLevel
                      << std::setw(10) << table[i].reading.humidity
                      << "OK\n";
        }
    }
    std::cout << "  Items: " << itemCount << " / " << HASH_TABLE_SIZE << "\n";
}

// ============================================================
// ChainHashTable  (H2) -- separate chaining
// ============================================================

int ChainHashTable::hashFunction(int key) const
{
    return (key % HASH_TABLE_SIZE + HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
}

// Constructor: null all bucket heads.
// Time Complexity: O(n)
ChainHashTable::ChainHashTable() : itemCount(0)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
        buckets[i] = nullptr;
}

// Destructor: free all chain nodes.
// Time Complexity: O(n)
ChainHashTable::~ChainHashTable()
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        ChainNode* cur = buckets[i];
        while (cur)
        {
            ChainNode* tmp = cur->next;
            delete cur;
            cur = tmp;
        }
        buckets[i] = nullptr;
    }
}

// insert: prepend to chain (or update existing key in chain).
// Time Complexity: O(k) where k = chain length (O(1) amortised)
bool ChainHashTable::insert(int zoneId, const SensorReading& r)
{
    int idx = hashFunction(zoneId);

    // Check if key already exists -- update in place
    ChainNode* cur = buckets[idx];
    while (cur)
    {
        if (cur->key == zoneId)
        {
            cur->reading = r;
            return true;
        }
        cur = cur->next;
    }

    // New key -- prepend to chain
    ChainNode* node = new ChainNode();
    node->key     = zoneId;
    node->reading = r;
    node->next    = buckets[idx];
    buckets[idx]  = node;
    itemCount++;
    return true;
}

// search: walk chain at bucket until key found.
// Time Complexity: O(k)
ChainNode* ChainHashTable::search(int zoneId) const
{
    int idx = hashFunction(zoneId);
    ChainNode* cur = buckets[idx];
    while (cur)
    {
        if (cur->key == zoneId) return cur;
        cur = cur->next;
    }
    return nullptr;
}

// remove: unlink node from chain.
// Time Complexity: O(k)
bool ChainHashTable::remove(int zoneId)
{
    int idx = hashFunction(zoneId);
    ChainNode* cur  = buckets[idx];
    ChainNode* prev = nullptr;

    while (cur)
    {
        if (cur->key == zoneId)
        {
            if (prev) prev->next   = cur->next;
            else      buckets[idx] = cur->next;
            delete cur;
            itemCount--;
            return true;
        }
        prev = cur;
        cur  = cur->next;
    }
    return false;
}

int ChainHashTable::size() const { return itemCount; }

// display: print every bucket with its chain.
// Time Complexity: O(n + b)
void ChainHashTable::display() const
{
    std::cout << "\n  H2 -- Collision Handling Table (separate chaining)\n";
    std::cout << "  " << std::string(58, '-') << "\n";

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        std::cout << "  Bucket[" << std::setw(2) << i << "]: ";
        if (!buckets[i])
        {
            std::cout << "(empty)\n";
            continue;
        }
        ChainNode* cur = buckets[i];
        while (cur)
        {
            std::cout << "[Z" << cur->key
                      << " T=" << std::fixed << std::setprecision(1)
                      << cur->reading.temperature
                      << " S=" << cur->reading.smokeLevel << "]";
            if (cur->next) std::cout << " -> ";
            cur = cur->next;
        }
        std::cout << "\n";
    }
    std::cout << "  Total items: " << itemCount << "\n";
}

// ============================================================
// LRUCache  (H3) -- doubly linked list + lookup array
// ============================================================

// detach: remove node from its current DLL position. O(1)
void LRUCache::detach(CacheNode* node)
{
    if (node->prev) node->prev->next = node->next;
    else            head             = node->next;
    if (node->next) node->next->prev = node->prev;
    else            tail             = node->prev;
    node->prev = node->next = nullptr;
}

// insertAtHead: add node as the new MRU. O(1)
void LRUCache::insertAtHead(CacheNode* node)
{
    node->next = head;
    node->prev = nullptr;
    if (head) head->prev = node;
    head = node;
    if (!tail) tail = node;
}

// moveToHead: detach + re-insert at head. O(1)
void LRUCache::moveToHead(CacheNode* node)
{
    detach(node);
    insertAtHead(node);
}

// evictLRU: remove tail node, clear from lookup. O(1)
void LRUCache::evictLRU()
{
    if (!tail) return;
    CacheNode* lru = tail;
    int key = lru->key;

    detach(lru);

    int idx = (key % HASH_TABLE_SIZE + HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
    // Walk lookup collision chain is not needed -- direct map
    lookup[idx] = nullptr;

    delete lru;
    currentSize--;
}

// Constructor: O(n)
LRUCache::LRUCache(int cap)
    : head(nullptr), tail(nullptr), capacity(cap), currentSize(0)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
        lookup[i] = nullptr;
}

// Destructor: O(n)
LRUCache::~LRUCache()
{
    CacheNode* cur = head;
    while (cur)
    {
        CacheNode* tmp = cur->next;
        delete cur;
        cur = tmp;
    }
    head = tail = nullptr;
}

// get: O(1) -- hit moves to head; miss returns nullptr
SensorReading* LRUCache::get(int key)
{
    int idx = (key % HASH_TABLE_SIZE + HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
    CacheNode* node = lookup[idx];

    // Verify key matches (simple direct map)
    if (!node || node->key != key) return nullptr;

    moveToHead(node);   // mark as MRU
    return &node->reading;
}

// put: O(1) -- insert at head, evict tail if full
void LRUCache::put(int key, const SensorReading& r)
{
    int idx = (key % HASH_TABLE_SIZE + HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
    CacheNode* existing = lookup[idx];

    if (existing && existing->key == key)
    {
        // Update existing entry, move to MRU
        existing->reading = r;
        moveToHead(existing);
        return;
    }

    // Evict LRU if at capacity
    if (currentSize >= capacity)
        evictLRU();

    CacheNode* node = new CacheNode();
    node->key     = key;
    node->reading = r;
    node->prev    = node->next = nullptr;

    insertAtHead(node);
    lookup[idx] = node;
    currentSize++;
}

bool LRUCache::contains(int key) const
{
    int idx = (key % HASH_TABLE_SIZE + HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
    return lookup[idx] && lookup[idx]->key == key;
}

int LRUCache::size() const { return currentSize; }

// display: MRU to LRU order. O(n)
void LRUCache::display() const
{
    std::cout << "\n  H3 -- LRU Cache (capacity=" << capacity
              << ", used=" << currentSize << ")\n";
    std::cout << "  MRU --> LRU\n";
    std::cout << "  " << std::string(58, '-') << "\n";

    if (!head) { std::cout << "  (empty)\n"; return; }

    int rank = 1;
    CacheNode* cur = head;
    while (cur)
    {
        std::cout << "  [" << std::setw(2) << rank++ << "] "
                  << "Zone-" << std::setw(2) << cur->key
                  << "  T=" << std::fixed << std::setprecision(1)
                  << cur->reading.temperature
                  << "  Smoke=" << cur->reading.smokeLevel
                  << "  Hum="   << cur->reading.humidity
                  << (cur == head ? "  <MRU>" : "")
                  << (cur == tail ? "  <LRU>" : "")
                  << "\n";
        cur = cur->next;
    }
}

// ============================================================
// HashEngine -- owns H1, H2, H3
// ============================================================

// Constructor: seed all three tables with zones 0-3.
// Time Complexity: O(n)
HashEngine::HashEngine() : h3(CACHE_SIZE)
{
    // Representative sensor readings per zone
    SensorReading zones[4] = {
        {28.5f, 62.0f, 0.05f, 12.0f, 0, 100, true, false},
        {54.0f, 35.0f, 0.75f, 22.0f, 1, 104, true, true },
        {61.2f, 28.5f, 0.90f, 30.0f, 2, 105, true, true },
        {29.8f, 60.0f, 0.07f, 11.0f, 3, 103, true, false}
    };

    for (int i = 0; i < 4; i++)
    {
        h1.insert(i, zones[i]);
        h2.insert(i, zones[i]);
        h3.put(i, zones[i]);
    }
}

// Menu 8.1 -- insert into H1 + H2, update H3. O(1) avg
void HashEngine::insertData(int zoneId, const SensorReading& r)
{
    bool ok1 = h1.insert(zoneId, r);
    bool ok2 = h2.insert(zoneId, r);
    h3.put(zoneId, r);

    std::cout << "\n  Inserted Zone-" << zoneId << "\n";
    std::cout << "  H1 (primary)  : " << (ok1 ? "OK" : "FULL") << "\n";
    std::cout << "  H2 (chain)    : " << (ok2 ? "OK" : "FAIL") << "\n";
    std::cout << "  H3 (cache)    : updated (size=" << h3.size() << ")\n";
}

// Menu 8.2 -- retrieve: check H3 first, then H1. O(1) avg
void HashEngine::retrieveData(int zoneId)
{
    std::cout << "\n  Retrieving Zone-" << zoneId << "\n";

    // Try cache first
    SensorReading* cached = h3.get(zoneId);
    if (cached)
    {
        std::cout << "  [CACHE HIT]  H3 returned data\n";
        std::cout << "  Temp="    << std::fixed << std::setprecision(1)
                  << cached->temperature
                  << "  Smoke=" << cached->smokeLevel
                  << "  Hum="   << cached->humidity << "\n";
        return;
    }

    // Cache miss -- go to H1
    std::cout << "  [CACHE MISS] Checking H1...\n";
    HashRecord* rec = h1.search(zoneId);
    if (rec)
    {
        std::cout << "  [H1 HIT] Temp="
                  << std::fixed << std::setprecision(1)
                  << rec->reading.temperature
                  << "  Smoke=" << rec->reading.smokeLevel
                  << "  Hum="   << rec->reading.humidity << "\n";
        h3.put(zoneId, rec->reading);   // promote to cache
        std::cout << "  Data promoted to H3 cache.\n";
    }
    else
    {
        std::cout << "  [MISS] Zone-" << zoneId << " not found in H1 or H3.\n";
    }
}

// Menu 8.3 -- show H2 collision chains. O(n)
void HashEngine::handleCollisions()
{
    h2.display();
}

// Menu 8.4 -- update H3 cache with fresh reading. O(1)
void HashEngine::updateCache(int zoneId, const SensorReading& r)
{
    h3.put(zoneId, r);
    std::cout << "\n  H3 cache updated for Zone-" << zoneId
              << "  (cache size=" << h3.size() << ")\n";
    h3.display();
}

// Menu 8.5 -- show H1 index table. O(n)
void HashEngine::viewIndexTable()
{
    h1.display();
}

// displayAll: full dashboard. O(n)
void HashEngine::displayAll() const
{
    h1.display();
    h2.display();
    h3.display();
}
