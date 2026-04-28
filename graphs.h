// ============================================================
// IFAMDS -- Intelligent Forest Advisory & Multi-Structure
//           Decision System
// File   : graphs.h
// Layer  : Part 5 -- Graph-Based Spatial Routing Layer
//
// Graph Instances:
//   G1 -- Adjacency List Graph  (sparse, O(V+E) space)
//   G2 -- Adjacency Matrix Graph (dense,  O(V^2) space)
//
// Operations:
//   BFS  -- Breadth-First Search  (fire spread, level-by-level)
//   DFS  -- Depth-First Search    (deep path analysis)
//   PathCost  = Distance + Danger
//   FireAware = Distance * (1 + FireLevel)
//
// Authors: Partner B
// Course : CL2001 -- Data Structures 2026
// ============================================================

#ifndef GRAPHS_H
#define GRAPHS_H

#include "constants.h"
#include <string>

// ------------------------------------------------------------
// struct GraphEdge
// One directed connection in G1 (adjacency list).
// ------------------------------------------------------------
struct GraphEdge
{
    int        dest;       // Destination zone ID
    float      distance;   // Base path distance
    float      danger;     // Current danger level [0.0 - 1.0]
    float      fireLevel;  // Fire intensity on this path
    GraphEdge* next;       // Next edge in the list
};

// ------------------------------------------------------------
// struct ZoneNode
// One forest zone vertex. Holds metadata + G1 edge list.
// ------------------------------------------------------------
struct ZoneNode
{
    int        zoneId;
    std::string zoneName;
    float      temperature;
    float      smokeLevel;
    float      fireLevel;
    bool       isBlocked;
    GraphEdge* edgeList;   // Head of adjacency list (G1)
};

// ============================================================
// class AdjListGraph  (G1)
// Sparse graph using linked adjacency lists.
// Space O(V+E), BFS/DFS O(V+E).
// ============================================================
class AdjListGraph
{
private:
    ZoneNode zones[MAX_ZONES];
    int      zoneCount;
    bool     visited[MAX_ZONES];

    int  bfsQueue[MAX_ZONES];
    int  bqHead, bqTail;

    int  dfsStack[MAX_ZONES];
    int  dfsTop;

    // Returns index in zones[] for zoneId, or -1.
    // Time Complexity: O(V)
    int  findIndex(int zoneId) const;

    // BFS queue helpers -- O(1) each
    void bfsEnqueue(int idx);
    int  bfsDequeue();
    bool bfsEmpty() const;

    // Frees edge list for zone at index idx. O(degree)
    void freeEdgeList(int idx);

public:
    // O(V)
    AdjListGraph();
    // O(V+E)
    ~AdjListGraph();

    // Adds a new zone vertex. O(1)
    bool addZone(int zoneId, const std::string& name,
                 float temp = 25.0f, float smoke = 0.0f);

    // Adds directed edge src->dest. O(V)
    bool addEdge(int srcId, int destId, float distance, float danger = 0.0f);

    // Adds undirected edge (two directed edges). O(V)
    bool addUndirectedEdge(int srcId, int destId, float distance, float danger = 0.0f);

    // BFS from startId -- prints zones level by level. O(V+E)
    void bfs(int startId);

    // DFS from startId -- depth-first order. O(V+E)
    void dfs(int startId);

    // Greedy path cost = sum of (distance + danger) on path src->dest. O(V+E)
    float computePathCost(int srcId, int destId);

    // Updates fireLevel on all edges incident to zoneId.
    // FireCost = distance * (1 + fireLevel). O(V+E)
    void updateFireLevel(int zoneId, float level);

    // Block / unblock a zone. O(V)
    void blockZone(int zoneId);
    void unblockZone(int zoneId);

    // Print full adjacency list. O(V+E)
    void display() const;

    // O(1)
    int  numZones() const;
};

// ============================================================
// class AdjMatrixGraph  (G2)
// Dense graph using a 2D cost matrix.
// Space O(V^2), BFS/DFS O(V^2).
// ============================================================
class AdjMatrixGraph
{
private:
    float      matrix[MAX_ZONES][MAX_ZONES];      // base costs
    float      fireMatrix[MAX_ZONES][MAX_ZONES];  // fire-aware costs
    std::string zoneNames[MAX_ZONES];
    float      zoneFire[MAX_ZONES];
    bool       blocked[MAX_ZONES];
    int        zoneCount;

    bool       visited[MAX_ZONES];
    int        bfsQueue[MAX_ZONES];
    int        bqHead, bqTail;

    void bfsEnqueue(int idx);
    int  bfsDequeue();
    bool bfsEmpty() const;

public:
    // O(V^2)
    AdjMatrixGraph();

    // Adds zone entry. O(1)
    bool addZone(int zoneId, const std::string& name);

    // Set matrix[src][dest] = distance (directed). O(1)
    bool addEdge(int src, int dest, float distance);

    // Set both directions. O(1)
    bool addUndirectedEdge(int src, int dest, float distance);

    // BFS from startIdx. O(V^2)
    void bfs(int startIdx);

    // DFS from startIdx. O(V^2)
    void dfs(int startIdx);

    // Recompute fire-aware costs for zone zoneIdx. O(V)
    void updateFireLevel(int zoneIdx, float level);

    // Print matrix grid. O(V^2)
    void display() const;

    // O(1)
    int numZones() const;
};

// ============================================================
// class GraphEngine
// Owns G1 and G2. Unified API for Menu 7.
// ============================================================
class GraphEngine
{
private:
    AdjListGraph   g1;
    AdjMatrixGraph g2;

public:
    // Seeds both graphs with the 4-zone forest layout. O(V+E)
    GraphEngine();

    // Menu 7.1 -- display G1 adjacency list. O(V+E)
    void loadAdjList();

    // Menu 7.2 -- display G2 matrix. O(V^2)
    void loadAdjMatrix();

    // Menu 7.3 -- BFS fire spread from startZone. O(V+E)
    void bfsFireSpread(int startZone);

    // Menu 7.4 -- DFS deep analysis from startZone. O(V+E)
    void dfsDeepAnalysis(int startZone);

    // Menu 7.5 -- path cost src->dest. O(V+E)
    void computeSafePath(int srcZone, int destZone);

    // Menu 7.6 -- update fire level, recalculate costs. O(V+E)
    void updateBlockedRoutes(int zoneId, float level);

    // Add zone + edge to G1. O(V)
    void addZoneToGraph(int zoneId, const std::string& name,
                        int connectTo, float distance);

    // One-line summary. O(1)
    void displaySummary() const;
};

#endif // GRAPHS_H
