// ============================================================
// IFAMDS -- Intelligent Forest Advisory & Multi-Structure
//           Decision System
// File   : hashtable.h
// Layer  : Part 6 -- Hash-Based Indexing Layer
//
// Hash Instances:
//   H1 -- Primary Index Table     (open-addressing, linear probe)
//   H2 -- Collision Handling Table (separate chaining)
//   H3 -- Fast Retrieval Cache    (LRU cache, fixed capacity)
//
// Hash Function:
//   Index = Key mod TABLE_SIZE
//
// Authors: Partner B
// Course : CL2001 -- Data Structures 2026
// ============================================================

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "constants.h"
#include "arrays.h"    // SensorReading
#include <string>

// ------------------------------------------------------------
// struct HashRecord
// Key-value pair stored in H1 and H2.
// Key   = ZoneID (integer)
// Value = latest SensorReading for that zone
// ------------------------------------------------------------
struct HashRecord
{
    int           key;          // ZoneID (-1 = empty slot)
    SensorReading reading;      // Sensor data for this zone
    bool          isOccupied;   // True if slot holds valid data
    bool          isDeleted;    // True if slot was lazily deleted (H1)
};

// ------------------------------------------------------------
// struct ChainNode
// Linked-list node for H2 (separate chaining).
// Each bucket is a singly-linked chain of ChainNodes.
// ------------------------------------------------------------
struct ChainNode
{
    int           key;
    SensorReading reading;
    ChainNode*    next;
};

// ------------------------------------------------------------
// struct CacheNode
// Doubly-linked node for H3 LRU cache.
// Most-recently-used is at the head of the list.
// ------------------------------------------------------------
struct CacheNode
{
    int           key;
    SensorReading reading;
    CacheNode*    prev;
    CacheNode*    next;
};

// ============================================================
// class PrimaryHashTable  (H1)
// Open-addressing hash table with linear probing.
//
// Hash function : index = key % HASH_TABLE_SIZE
// Collision     : index = (index + 1) % HASH_TABLE_SIZE (linear probe)
// Deletion      : lazy (mark isDeleted, skip on lookup, reuse on insert)
//
// Insert: O(1) average, O(n) worst
// Search: O(1) average, O(n) worst
// Delete: O(1) average, O(n) worst
// ============================================================
class PrimaryHashTable
{
private:
    HashRecord  table[HASH_TABLE_SIZE]; // Fixed-size slot array
    int         itemCount;              // Current number of records

    // Computes initial bucket index.
    // Time Complexity: O(1)
    int hashFunction(int key) const;

public:
    // Initialises all slots to empty.
    // Time Complexity: O(n)
    PrimaryHashTable();

    // Inserts or updates record for key=zoneId.
    // Uses linear probing on collision.
    // Time Complexity: O(1) average
    bool insert(int zoneId, const SensorReading& r);

    // Returns pointer to record for key=zoneId, or nullptr.
    // Time Complexity: O(1) average
    HashRecord* search(int zoneId);

    // Lazily deletes record with key=zoneId.
    // Time Complexity: O(1) average
    bool remove(int zoneId);

    // Returns number of occupied (non-deleted) records.
    // Time Complexity: O(1)
    int  size() const;

    // Prints all occupied slots.
    // Time Complexity: O(n)
    void display() const;
};

// ============================================================
// class ChainHashTable  (H2)
// Separate-chaining hash table.
// Each bucket holds a linked list of ChainNodes.
// Handles collisions without probing -- chains grow as needed.
//
// Insert: O(1) average
// Search: O(1) average, O(k) worst (k = chain length)
// Delete: O(1) average, O(k) worst
// ============================================================
class ChainHashTable
{
private:
    ChainNode*  buckets[HASH_TABLE_SIZE]; // Array of chain heads
    int         itemCount;

    // Time Complexity: O(1)
    int hashFunction(int key) const;

public:
    // Initialises all bucket heads to nullptr.
    // Time Complexity: O(n)
    ChainHashTable();

    // Destructor -- frees all chain nodes.
    // Time Complexity: O(n)
    ~ChainHashTable();

    // Inserts or updates key. Prepends to chain on collision.
    // Time Complexity: O(1) average
    bool insert(int zoneId, const SensorReading& r);

    // Returns pointer to ChainNode for key, or nullptr.
    // Time Complexity: O(k) where k = chain length at bucket
    ChainNode* search(int zoneId) const;

    // Removes ChainNode with given key from its chain.
    // Time Complexity: O(k)
    bool remove(int zoneId);

    // Returns total item count.
    // Time Complexity: O(1)
    int  size() const;

    // Prints all buckets and their chains.
    // Time Complexity: O(n + b) where b = HASH_TABLE_SIZE
    void display() const;
};

// ============================================================
// class LRUCache  (H3)
// Fixed-capacity Least-Recently-Used cache.
// Uses a doubly-linked list (head = MRU, tail = LRU)
// plus a direct-mapped lookup array for O(1) access.
//
// get:  O(1) -- move to head if hit, return nullptr if miss
// put:  O(1) -- insert at head; evict tail if at capacity
// ============================================================
class LRUCache
{
private:
    CacheNode*  head;                   // Most-recently-used end
    CacheNode*  tail;                   // Least-recently-used end
    CacheNode*  lookup[HASH_TABLE_SIZE];// Direct map key->node
    int         capacity;               // Max items (= CACHE_SIZE)
    int         currentSize;

    // Moves an existing node to the head (marks as MRU).
    // Time Complexity: O(1)
    void moveToHead(CacheNode* node);

    // Removes the tail node (evicts LRU item).
    // Time Complexity: O(1)
    void evictLRU();

    // Inserts new node at head.
    // Time Complexity: O(1)
    void insertAtHead(CacheNode* node);

    // Detaches node from its current list position.
    // Time Complexity: O(1)
    void detach(CacheNode* node);

public:
    // Constructs empty cache with given capacity.
    // Time Complexity: O(n) -- clears lookup table
    LRUCache(int cap = CACHE_SIZE);

    // Destructor -- frees all nodes.
    // Time Complexity: O(n)
    ~LRUCache();

    // Returns reading for key if in cache (cache hit), moves to MRU.
    // Returns nullptr on cache miss.
    // Time Complexity: O(1)
    SensorReading* get(int key);

    // Inserts key->reading into cache. Evicts LRU if full.
    // Time Complexity: O(1)
    void put(int key, const SensorReading& r);

    // Returns true if key is currently cached.
    // Time Complexity: O(1)
    bool contains(int key) const;

    // Returns current number of cached items.
    // Time Complexity: O(1)
    int  size() const;

    // Prints cache from MRU to LRU.
    // Time Complexity: O(n)
    void display() const;
};

// ============================================================
// class HashEngine
// Owns H1, H2, H3. Unified API for Menu 8.
// ============================================================
class HashEngine
{
private:
    PrimaryHashTable h1;  // H1 -- primary index
    ChainHashTable   h2;  // H2 -- collision handler
    LRUCache         h3;  // H3 -- fast retrieval cache

public:
    // Constructor -- seeds all three tables with demo data.
    // Time Complexity: O(n)
    HashEngine();

    // Menu 8.1 -- Insert into H1 and H2, update H3. O(1) avg
    void insertData(int zoneId, const SensorReading& r);

    // Menu 8.2 -- Retrieve from H3 first (cache), else H1. O(1) avg
    void retrieveData(int zoneId);

    // Menu 8.3 -- Display H2 chain table (collision view). O(n)
    void handleCollisions();

    // Menu 8.4 -- Update cache with latest reading. O(1)
    void updateCache(int zoneId, const SensorReading& r);

    // Menu 8.5 -- Display H1 index table. O(n)
    void viewIndexTable();

    // Displays all three tables (full dashboard). O(n)
    void displayAll() const;
};

#endif // HASHTABLE_H
