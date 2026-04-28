// ============================================================
// IFAMDS -- Intelligent Forest Advisory & Multi-Structure
//           Decision System
// File   : graphs.cpp
// Layer  : Part 5 -- Graph-Based Spatial Routing Layer
//
// Authors: Partner B
// Course : CL2001 -- Data Structures 2026
// ============================================================

#include "graphs.h"
#include <iostream>
#include <iomanip>
#include <cstring>

// ============================================================
// AdjListGraph -- private helpers
// ============================================================

// findIndex: linear scan for zoneId. O(V)
int AdjListGraph::findIndex(int zoneId) const
{
    for (int i = 0; i < zoneCount; i++)
        if (zones[i].zoneId == zoneId) return i;
    return -1;
}

// BFS queue helpers -- circular array. O(1)
void AdjListGraph::bfsEnqueue(int idx) { bfsQueue[bqTail++ % MAX_ZONES] = idx; }
int  AdjListGraph::bfsDequeue()        { return bfsQueue[bqHead++ % MAX_ZONES]; }
bool AdjListGraph::bfsEmpty()  const   { return bqHead == bqTail; }

// freeEdgeList: delete all edges for zone[idx]. O(degree)
void AdjListGraph::freeEdgeList(int idx)
{
    GraphEdge* cur = zones[idx].edgeList;
    while (cur)
    {
        GraphEdge* tmp = cur->next;
        delete cur;
        cur = tmp;
    }
    zones[idx].edgeList = nullptr;
}

// ============================================================
// AdjListGraph -- constructor / destructor
// ============================================================

// Constructor: O(V)
AdjListGraph::AdjListGraph()
    : zoneCount(0), bqHead(0), bqTail(0), dfsTop(-1)
{
    for (int i = 0; i < MAX_ZONES; i++)
    {
        zones[i].zoneId      = -1;
        zones[i].zoneName    = "";
        zones[i].temperature = 25.0f;
        zones[i].smokeLevel  = 0.0f;
        zones[i].fireLevel   = 0.0f;
        zones[i].isBlocked   = false;
        zones[i].edgeList    = nullptr;
        visited[i]           = false;
    }
}

// Destructor: O(V+E)
AdjListGraph::~AdjListGraph()
{
    for (int i = 0; i < zoneCount; i++)
        freeEdgeList(i);
}

// ============================================================
// AdjListGraph -- public methods
// ============================================================

// addZone: O(1)
bool AdjListGraph::addZone(int zoneId, const std::string& name,
                            float temp, float smoke)
{
    if (zoneCount >= MAX_ZONES) return false;
    if (findIndex(zoneId) != -1) return false;   // duplicate

    int i = zoneCount++;
    zones[i].zoneId      = zoneId;
    zones[i].zoneName    = name;
    zones[i].temperature = temp;
    zones[i].smokeLevel  = smoke;
    zones[i].fireLevel   = 0.0f;
    zones[i].isBlocked   = false;
    zones[i].edgeList    = nullptr;
    return true;
}

// addEdge (directed): O(V)
bool AdjListGraph::addEdge(int srcId, int destId,
                            float distance, float danger)
{
    int si = findIndex(srcId);
    if (si == -1) return false;

    GraphEdge* e = new GraphEdge();
    e->dest      = destId;
    e->distance  = distance;
    e->danger    = danger;
    e->fireLevel = 0.0f;
    e->next      = zones[si].edgeList;
    zones[si].edgeList = e;
    return true;
}

// addUndirectedEdge: O(V)
bool AdjListGraph::addUndirectedEdge(int srcId, int destId,
                                      float distance, float danger)
{
    return addEdge(srcId, destId, distance, danger)
        && addEdge(destId, srcId, distance, danger);
}

// ============================================================
// BFS -- breadth-first search, prints level by level
// Algorithm:
//   1. Enqueue startId, mark visited
//   2. While queue not empty:
//      a. Dequeue u, print it
//      b. For each neighbour v of u (not visited, not blocked):
//         mark v visited, enqueue v
// Time Complexity: O(V + E)
// ============================================================
void AdjListGraph::bfs(int startId)
{
    // Reset
    bqHead = bqTail = 0;
    for (int i = 0; i < zoneCount; i++) visited[i] = false;

    int si = findIndex(startId);
    if (si == -1)
    {
        std::cout << "  Zone " << startId << " not found in G1.\n";
        return;
    }

    std::cout << "\n  BFS from Zone-" << startId
              << " (" << zones[si].zoneName << ")\n";
    std::cout << "  " << std::string(50, '-') << "\n";

    visited[si] = true;
    bfsEnqueue(si);
    int level = 0;

    // We track level boundaries with two counters
    int currentLevel = 1;   // nodes at current level
    int nextLevel    = 0;   // nodes at next level

    std::cout << "  Level " << level << " [Origin]: ";

    while (!bfsEmpty())
    {
        int ui = bfsDequeue();
        currentLevel--;

        std::cout << "Zone-" << zones[ui].zoneId;
        if (zones[ui].isBlocked) std::cout << "[BLOCKED]";
        std::cout << "  ";

        // Traverse edge list
        GraphEdge* e = zones[ui].edgeList;
        while (e)
        {
            int di = findIndex(e->dest);
            if (di != -1 && !visited[di])
            {
                visited[di] = true;
                bfsEnqueue(di);
                nextLevel++;
            }
            e = e->next;
        }

        if (currentLevel == 0 && !bfsEmpty())
        {
            level++;
            std::cout << "\n  Level " << level << " [Spread]:  ";
            currentLevel = nextLevel;
            nextLevel    = 0;
        }
    }
    std::cout << "\n";
}

// ============================================================
// DFS -- depth-first search (iterative, uses stack)
// Algorithm:
//   1. Push startId
//   2. While stack not empty:
//      a. Pop u; if not visited, mark visited and print
//      b. Push all unvisited neighbours of u
// Time Complexity: O(V + E)
// ============================================================
void AdjListGraph::dfs(int startId)
{
    for (int i = 0; i < zoneCount; i++) visited[i] = false;
    dfsTop = -1;

    int si = findIndex(startId);
    if (si == -1)
    {
        std::cout << "  Zone " << startId << " not found in G1.\n";
        return;
    }

    std::cout << "\n  DFS from Zone-" << startId
              << " (" << zones[si].zoneName << ")\n";
    std::cout << "  " << std::string(50, '-') << "\n";
    std::cout << "  Path: ";

    dfsStack[++dfsTop] = si;

    while (dfsTop >= 0)
    {
        int ui = dfsStack[dfsTop--];
        if (visited[ui]) continue;

        visited[ui] = true;
        std::cout << "Zone-" << zones[ui].zoneId
                  << " (" << zones[ui].zoneName << ")";

        if (zones[ui].isBlocked)
            std::cout << "[BLOCKED -- path ends]";

        std::cout << " -> ";

        if (!zones[ui].isBlocked)
        {
            // Push neighbours in reverse order so leftmost is visited first
            GraphEdge* e = zones[ui].edgeList;
            // Collect into temp array then push reversed
            int nbrs[MAX_ZONES];
            int nc = 0;
            while (e) { nbrs[nc++] = e->dest; e = e->next; }
            for (int k = nc - 1; k >= 0; k--)
            {
                int di = findIndex(nbrs[k]);
                if (di != -1 && !visited[di])
                    dfsStack[++dfsTop] = di;
            }
        }
    }
    std::cout << "[END]\n";
}

// ============================================================
// computePathCost
// Greedy: follow BFS parent map, sum distance + danger per hop.
// PathCost = sum(distance_i + danger_i) for each edge on path.
// Time Complexity: O(V + E)
// ============================================================
float AdjListGraph::computePathCost(int srcId, int destId)
{
    // BFS to find parent map
    int parent[MAX_ZONES];
    float edgeCost[MAX_ZONES];
    for (int i = 0; i < MAX_ZONES; i++) { parent[i] = -1; edgeCost[i] = 0; }
    for (int i = 0; i < zoneCount; i++) visited[i] = false;
    bqHead = bqTail = 0;

    int si = findIndex(srcId);
    int di = findIndex(destId);
    if (si == -1 || di == -1)
    {
        std::cout << "  Zone not found.\n";
        return -1.0f;
    }

    visited[si] = true;
    bfsEnqueue(si);

    while (!bfsEmpty())
    {
        int ui = bfsDequeue();
        if (ui == di) break;

        GraphEdge* e = zones[ui].edgeList;
        while (e)
        {
            int ni = findIndex(e->dest);
            if (ni != -1 && !visited[ni])
            {
                visited[ni]  = true;
                parent[ni]   = ui;
                // FireAware cost: distance * (1 + fireLevel) + danger
                float fc = e->distance * (1.0f + e->fireLevel) + e->danger;
                edgeCost[ni] = fc;
                bfsEnqueue(ni);
            }
            e = e->next;
        }
    }

    if (!visited[di])
    {
        std::cout << "  No path from Zone-" << srcId
                  << " to Zone-" << destId << ".\n";
        return -1.0f;
    }

    // Reconstruct path
    int path[MAX_ZONES];
    int plen = 0;
    float total = 0.0f;
    for (int cur = di; cur != -1; cur = parent[cur])
    {
        path[plen++] = cur;
        if (cur != si) total += edgeCost[cur];
    }

    std::cout << "\n  Safe Path: ";
    for (int k = plen - 1; k >= 0; k--)
    {
        std::cout << "Zone-" << zones[path[k]].zoneId;
        if (k > 0) std::cout << " -> ";
    }
    std::cout << "\n  Total Cost: " << std::fixed << std::setprecision(2)
              << total << " units\n";
    return total;
}

// ============================================================
// updateFireLevel
// Updates danger + fireLevel on edges incident to zoneId.
// New cost = distance * (1 + fireLevel).
// Time Complexity: O(V + E)
// ============================================================
void AdjListGraph::updateFireLevel(int zoneId, float level)
{
    int zi = findIndex(zoneId);
    if (zi != -1) zones[zi].fireLevel = level;

    // Update outgoing edges from zoneId
    if (zi != -1)
    {
        GraphEdge* e = zones[zi].edgeList;
        while (e) { e->fireLevel = level; e = e->next; }
    }

    // Update incoming edges to zoneId
    for (int i = 0; i < zoneCount; i++)
    {
        GraphEdge* e = zones[i].edgeList;
        while (e)
        {
            if (e->dest == zoneId) e->fireLevel = level;
            e = e->next;
        }
    }

    std::cout << "  G1: Fire level for Zone-" << zoneId
              << " updated to " << std::fixed << std::setprecision(2)
              << level << "\n";
}

// blockZone / unblockZone: O(V)
void AdjListGraph::blockZone(int zoneId)
{
    int i = findIndex(zoneId);
    if (i != -1) { zones[i].isBlocked = true;
                   std::cout << "  Zone-" << zoneId << " BLOCKED.\n"; }
}
void AdjListGraph::unblockZone(int zoneId)
{
    int i = findIndex(zoneId);
    if (i != -1) { zones[i].isBlocked = false;
                   std::cout << "  Zone-" << zoneId << " unblocked.\n"; }
}

// display: O(V+E)
void AdjListGraph::display() const
{
    std::cout << "\n  G1 -- Adjacency List (" << zoneCount << " zones)\n";
    std::cout << "  " << std::string(54, '-') << "\n";
    for (int i = 0; i < zoneCount; i++)
    {
        std::cout << "  Zone-" << std::setw(2) << zones[i].zoneId
                  << " (" << std::left << std::setw(12) << zones[i].zoneName << ") "
                  << (zones[i].isBlocked ? "[BLOCKED] " : "          ")
                  << "fire=" << std::fixed << std::setprecision(2)
                  << zones[i].fireLevel << " | Edges: ";

        GraphEdge* e = zones[i].edgeList;
        if (!e) std::cout << "(none)";
        while (e)
        {
            std::cout << "->Z" << e->dest
                      << "(d=" << e->distance
                      << ",fire=" << e->fireLevel << ") ";
            e = e->next;
        }
        std::cout << "\n";
    }
}

int AdjListGraph::numZones() const { return zoneCount; }


// ============================================================
// AdjMatrixGraph -- implementation
// ============================================================

// BFS helpers: O(1)
void AdjMatrixGraph::bfsEnqueue(int idx) { bfsQueue[bqTail++ % MAX_ZONES] = idx; }
int  AdjMatrixGraph::bfsDequeue()        { return bfsQueue[bqHead++ % MAX_ZONES]; }
bool AdjMatrixGraph::bfsEmpty()  const   { return bqHead == bqTail; }

// Constructor: O(V^2)
AdjMatrixGraph::AdjMatrixGraph() : zoneCount(0), bqHead(0), bqTail(0)
{
    for (int i = 0; i < MAX_ZONES; i++)
    {
        zoneNames[i] = "";
        zoneFire[i]  = 0.0f;
        blocked[i]   = false;
        visited[i]   = false;
        for (int j = 0; j < MAX_ZONES; j++)
        {
            matrix[i][j]     = 0.0f;
            fireMatrix[i][j] = 0.0f;
        }
    }
}

bool AdjMatrixGraph::addZone(int zoneId, const std::string& name)
{
    if (zoneCount >= MAX_ZONES) return false;
    zoneNames[zoneCount++] = name + "(Z" + std::to_string(zoneId) + ")";
    return true;
}

// addEdge (directed): O(1)
bool AdjMatrixGraph::addEdge(int src, int dest, float distance)
{
    if (src < 0 || src >= zoneCount || dest < 0 || dest >= zoneCount) return false;
    matrix[src][dest]     = distance;
    fireMatrix[src][dest] = distance;   // starts equal to base
    return true;
}

// addUndirectedEdge: O(1)
bool AdjMatrixGraph::addUndirectedEdge(int src, int dest, float distance)
{
    return addEdge(src, dest, distance) && addEdge(dest, src, distance);
}

// ============================================================
// BFS on G2: O(V^2)
// ============================================================
void AdjMatrixGraph::bfs(int startIdx)
{
    bqHead = bqTail = 0;
    for (int i = 0; i < zoneCount; i++) visited[i] = false;

    if (startIdx < 0 || startIdx >= zoneCount)
    { std::cout << "  Invalid start index.\n"; return; }

    std::cout << "\n  G2 BFS from index " << startIdx
              << " (" << zoneNames[startIdx] << ")\n";
    std::cout << "  Order: ";

    visited[startIdx] = true;
    bfsEnqueue(startIdx);

    while (!bfsEmpty())
    {
        int u = bfsDequeue();
        std::cout << u << "(" << zoneNames[u] << ") -> ";

        for (int v = 0; v < zoneCount; v++)
        {
            if (matrix[u][v] > 0.0f && !visited[v])
            {
                visited[v] = true;
                bfsEnqueue(v);
            }
        }
    }
    std::cout << "[END]\n";
}

// ============================================================
// DFS on G2 (iterative): O(V^2)
// ============================================================
void AdjMatrixGraph::dfs(int startIdx)
{
    for (int i = 0; i < zoneCount; i++) visited[i] = false;

    if (startIdx < 0 || startIdx >= zoneCount)
    { std::cout << "  Invalid start index.\n"; return; }

    std::cout << "\n  G2 DFS from index " << startIdx
              << " (" << zoneNames[startIdx] << ")\n";
    std::cout << "  Path: ";

    int stack[MAX_ZONES];
    int top = -1;
    stack[++top] = startIdx;

    while (top >= 0)
    {
        int u = stack[top--];
        if (visited[u]) continue;
        visited[u] = true;
        std::cout << u << "(" << zoneNames[u] << ") -> ";

        // Push neighbours in reverse
        for (int v = zoneCount - 1; v >= 0; v--)
        {
            if (matrix[u][v] > 0.0f && !visited[v])
                stack[++top] = v;
        }
    }
    std::cout << "[END]\n";
}

// updateFireLevel: O(V)
void AdjMatrixGraph::updateFireLevel(int zoneIdx, float level)
{
    if (zoneIdx < 0 || zoneIdx >= zoneCount) return;
    zoneFire[zoneIdx] = level;

    for (int j = 0; j < zoneCount; j++)
    {
        if (matrix[zoneIdx][j] > 0.0f)
            fireMatrix[zoneIdx][j] = matrix[zoneIdx][j] * (1.0f + level);
        if (matrix[j][zoneIdx] > 0.0f)
            fireMatrix[j][zoneIdx] = matrix[j][zoneIdx] * (1.0f + level);
    }
    std::cout << "  G2: Fire level index " << zoneIdx
              << " updated to " << std::fixed << std::setprecision(2)
              << level << "\n";
}

// display: O(V^2)
void AdjMatrixGraph::display() const
{
    std::cout << "\n  G2 -- Adjacency Matrix (" << zoneCount << " zones)\n";
    std::cout << "  " << std::string(54, '-') << "\n";
    std::cout << "       ";
    for (int j = 0; j < zoneCount; j++)
        std::cout << std::setw(6) << j;
    std::cout << "\n  " << std::string(54, '-') << "\n";

    for (int i = 0; i < zoneCount; i++)
    {
        std::cout << "  [" << std::setw(2) << i << "] ";
        for (int j = 0; j < zoneCount; j++)
        {
            if (fireMatrix[i][j] > 0.0f)
                std::cout << std::setw(6) << std::fixed
                          << std::setprecision(1) << fireMatrix[i][j];
            else
                std::cout << std::setw(6) << "  .";
        }
        std::cout << "  " << zoneNames[i] << "\n";
    }
}

int AdjMatrixGraph::numZones() const { return zoneCount; }


// ============================================================
// GraphEngine -- constructor (seeds both graphs)
// Forest layout:
//   Zone-0 (North) <-> Zone-1 (East)   dist=5
//   Zone-1 (East)  <-> Zone-2 (South)  dist=4
//   Zone-2 (South) <-> Zone-3 (West)   dist=6
//   Zone-3 (West)  <-> Zone-0 (North)  dist=7
//   Zone-0 (North) <-> Zone-2 (South)  dist=9  (diagonal)
//   Zone-1 (East)  <-> Zone-3 (West)   dist=8  (diagonal)
// Time Complexity: O(V + E)
// ============================================================
GraphEngine::GraphEngine()
{
    // --- G1 setup ---
    g1.addZone(0, "North", 28.5f, 0.05f);
    g1.addZone(1, "East",  30.1f, 0.08f);
    g1.addZone(2, "South", 54.0f, 0.75f);   // fire zone
    g1.addZone(3, "West",  29.8f, 0.07f);

    g1.addUndirectedEdge(0, 1, 5.0f, 0.1f);
    g1.addUndirectedEdge(1, 2, 4.0f, 0.5f);   // high danger near fire
    g1.addUndirectedEdge(2, 3, 6.0f, 0.6f);
    g1.addUndirectedEdge(3, 0, 7.0f, 0.1f);
    g1.addUndirectedEdge(0, 2, 9.0f, 0.4f);   // diagonal shortcut
    g1.addUndirectedEdge(1, 3, 8.0f, 0.2f);

    // Seed fire level on Zone-2
    g1.updateFireLevel(2, 0.75f);

    // --- G2 setup (index = zoneId) ---
    g2.addZone(0, "North");
    g2.addZone(1, "East");
    g2.addZone(2, "South");
    g2.addZone(3, "West");

    g2.addUndirectedEdge(0, 1, 5.0f);
    g2.addUndirectedEdge(1, 2, 4.0f);
    g2.addUndirectedEdge(2, 3, 6.0f);
    g2.addUndirectedEdge(3, 0, 7.0f);
    g2.addUndirectedEdge(0, 2, 9.0f);
    g2.addUndirectedEdge(1, 3, 8.0f);

    g2.updateFireLevel(2, 0.75f);
}

// Menu 7.1
void GraphEngine::loadAdjList()
{
    std::cout << "\n  Loading G1 -- Adjacency List Graph\n";
    g1.display();
}

// Menu 7.2
void GraphEngine::loadAdjMatrix()
{
    std::cout << "\n  Loading G2 -- Adjacency Matrix Graph\n";
    g2.display();
}

// Menu 7.3
void GraphEngine::bfsFireSpread(int startZone)
{
    std::cout << "\n  Fire Spread Simulation (BFS) from Zone-" << startZone << "\n";
    g1.bfs(startZone);
}

// Menu 7.4
void GraphEngine::dfsDeepAnalysis(int startZone)
{
    std::cout << "\n  Deep Path Analysis (DFS) from Zone-" << startZone << "\n";
    g1.dfs(startZone);
}

// Menu 7.5
void GraphEngine::computeSafePath(int srcZone, int destZone)
{
    std::cout << "\n  Computing safe path Zone-" << srcZone
              << " -> Zone-" << destZone << "\n";
    g1.computePathCost(srcZone, destZone);
}

// Menu 7.6
void GraphEngine::updateBlockedRoutes(int zoneId, float level)
{
    std::cout << "\n  Updating fire routes for Zone-" << zoneId
              << "  fireLevel=" << std::fixed << std::setprecision(2)
              << level << "\n";
    g1.updateFireLevel(zoneId, level);
    g2.updateFireLevel(zoneId, level);

    if (level >= 0.9f)
    {
        g1.blockZone(zoneId);
        std::cout << "  Zone-" << zoneId
                  << " blocked in G1 (fire level critical).\n";
    }
}

// addZoneToGraph
void GraphEngine::addZoneToGraph(int zoneId, const std::string& name,
                                  int connectTo, float distance)
{
    g1.addZone(zoneId, name);
    g1.addUndirectedEdge(zoneId, connectTo, distance);
    g2.addZone(zoneId, name);
    g2.addUndirectedEdge(zoneId, connectTo, distance);
    std::cout << "  Zone-" << zoneId << " (" << name
              << ") added and connected to Zone-" << connectTo << ".\n";
}

// displaySummary
void GraphEngine::displaySummary() const
{
    std::cout << "\n  Graph Engine Summary\n";
    std::cout << "  G1 (Adjacency List) : " << g1.numZones() << " zones\n";
    std::cout << "  G2 (Adjacency Matrix): " << g2.numZones() << " zones\n";
}
