// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : trees.h
// Layer  : Part 4 — Tree-Based Decision Intelligence Layer
//
// Tree Instances (12 total):
//   Structural  T1  Zone Hierarchy Tree
//               T2  Sub-Zone Decomposition Tree
//               T3  Terrain Classification Tree
//   Resource    T4  Water Resource Tree
//               T5  Fire Control Resource Tree
//               T6  Equipment Allocation Tree
//   Incident    T7  Fire Classification Tree
//               T8  Wildlife Activity Tree
//               T9  Human Activity Tree
//   Decision    T10 Local Decision Tree
//               T11 Regional Escalation Tree
//               T12 Global Emergency Tree
//
// Design:
//   All trees share a single generic TreeNode.
//   DecisionTree  — owns a root, supports insert/search/display
//   DecisionEngine— owns all 12 tree instances, unified API
//
// Authors: Partner B
// Course : CL2001 — Data Structures 2026
// ============================================================

#ifndef TREES_H
#define TREES_H

#include "constants.h"
#include <string>

// ------------------------------------------------------------
// struct TreeNode
// Generic node used by all 12 trees.
//
// Fields:
//   nodeId    — unique identifier within the tree
//   label     — human-readable name (zone name, resource type…)
//   riskScore — composite risk/priority value  [0.0 – 1.0]
//   value     — sensor/resource quantity (temp, water vol…)
//   zoneId    — which forest zone this node represents
//   left/right— BST children (ordered by riskScore)
// ------------------------------------------------------------
struct TreeNode
{
    int         nodeId;      // Unique node ID
    std::string label;       // Descriptive label
    float       riskScore;   // 0.0 (safe) – 1.0 (critical)
    float       value;       // Domain-specific quantity
    int         zoneId;      // Associated forest zone
    TreeNode*   left;        // Left child  (riskScore <  parent)
    TreeNode*   right;       // Right child (riskScore >= parent)
};

// ============================================================
// class DecisionTree
// A binary search tree ordered by riskScore (left < root <= right).
// Supports insert, search by zoneId, in-order traversal, and
// risk-score-based queries.
// ============================================================
class DecisionTree
{
private:
    TreeNode*   root;       // Root of the BST
    int         nodeCount;  // Total nodes currently in the tree
    std::string treeName;   // Human-readable label (e.g. "T1")

    // ── Private helpers ──────────────────────────────────────

    // Recursively inserts n into the subtree rooted at cur.
    // Ordered by riskScore: go left if n.riskScore < cur.riskScore,
    // right otherwise.
    // Time Complexity: O(h) where h = tree height (O(log n) avg)
    TreeNode* insertRec(TreeNode* cur, TreeNode* n);

    // Recursively searches for the first node matching zoneId.
    // Time Complexity: O(n) — zoneId is not the BST key
    TreeNode* searchByZone(TreeNode* cur, int zoneId) const;

    // In-order traversal (ascending riskScore), prints each node.
    // Time Complexity: O(n)
    void inorderRec(TreeNode* cur, int depth) const;

    // Pre-order traversal for structural display.
    // Time Complexity: O(n)
    void preorderRec(TreeNode* cur, int depth) const;

    // Recursively frees all nodes in the subtree.
    // Time Complexity: O(n)
    void destroyRec(TreeNode* cur);

    // Returns the subtree node with the maximum riskScore (rightmost).
    // Time Complexity: O(h)
    TreeNode* maxNode(TreeNode* cur) const;

    // Collects all nodes whose riskScore > threshold into result[].
    // Time Complexity: O(n)
    void collectHighRisk(TreeNode* cur, float threshold,
                         TreeNode* result[], int& count) const;

public:

    // Constructs an empty tree with the given name.
    // Time Complexity: O(1)
    DecisionTree(const std::string& name);

    // Destructor — frees all nodes via destroyRec.
    // Time Complexity: O(n)
    ~DecisionTree();

    // Inserts a new node. nodeId auto-assigned, caller sets fields.
    // Time Complexity: O(h)
    void insert(const std::string& label, float riskScore,
                float value, int zoneId);

    // Returns a pointer to the node for the given zoneId, or nullptr.
    // Time Complexity: O(n)
    TreeNode* findByZone(int zoneId) const;

    // Returns a pointer to the highest-risk node (rightmost in BST).
    // Time Complexity: O(h)
    TreeNode* getMaxRisk() const;

    // Prints all nodes in ascending risk order (in-order).
    // Time Complexity: O(n)
    void displayInOrder() const;

    // Prints tree structure using pre-order with indentation.
    // Time Complexity: O(n)
    void displayStructure() const;

    // Computes the average risk score across all nodes.
    // Time Complexity: O(n)
    float averageRisk() const;

    // Fills result[] with nodes whose riskScore > threshold.
    // Returns the count. result must have capacity >= nodeCount.
    // Time Complexity: O(n)
    int getHighRiskNodes(float threshold,
                         TreeNode* result[], int capacity) const;

    // Returns the number of nodes in the tree.
    // Time Complexity: O(1)
    int size() const;

    // Returns true if the tree has no nodes.
    // Time Complexity: O(1)
    bool isEmpty() const;

    // Returns the tree name string.
    // Time Complexity: O(1)
    std::string getName() const;
};

// ============================================================
// class DecisionEngine
// Owns all 12 tree instances (T1–T12) and provides a unified
// API for risk computation, zone-level and regional decisions,
// and global emergency coordination.
// ============================================================
class DecisionEngine
{
private:
    // ── 12 tree instances ────────────────────────────────────
    DecisionTree t1;   // Zone Hierarchy
    DecisionTree t2;   // Sub-Zone Decomposition
    DecisionTree t3;   // Terrain Classification
    DecisionTree t4;   // Water Resource
    DecisionTree t5;   // Fire Control Resource
    DecisionTree t6;   // Equipment Allocation
    DecisionTree t7;   // Fire Classification
    DecisionTree t8;   // Wildlife Activity
    DecisionTree t9;   // Human Activity
    DecisionTree t10;  // Local Decision
    DecisionTree t11;  // Regional Escalation
    DecisionTree t12;  // Global Emergency

    // Returns a reference to the tree for treeId 1–12.
    // Throws std::out_of_range for invalid id.
    // Time Complexity: O(1)
    DecisionTree& treeById(int treeId);
    const DecisionTree& treeById(int treeId) const;

    // Internal: compute weighted risk score from three sensor inputs.
    // Formula: w1*fire + w2*smoke + w3*temperature  (weights sum to 1)
    // Time Complexity: O(1)
    float weightedScore(float fire, float smoke, float temperature) const;

public:

    // Constructs the engine — initialises all 12 trees with seed
    // zone data so the system is never empty at demo time.
    // Time Complexity: O(n) where n = number of seeded nodes
    DecisionEngine();

    // ========================================================
    // Risk Score Computation  (Menu 6.1)
    // ========================================================

    // Computes and prints the weighted decision score for a zone.
    // Decision Score = 0.4*fire + 0.3*smoke + 0.3*temperature
    // If score > DECISION_THRESHOLD → emergency activated.
    // Time Complexity: O(1)
    float computeRiskScore(int zoneId, float fire,
                           float smoke, float temperature);

    // ========================================================
    // Zone-Level Decision  (Menu 6.2)
    // ========================================================

    // Runs the local decision tree (T10) for a specific zone.
    // Rule: if zone riskScore > 0.6 → activate local response.
    // Prints recommended action.
    // Time Complexity: O(n)
    void zoneDecision(int zoneId, float riskScore);

    // ========================================================
    // Regional Processing  (Menu 6.3)
    // ========================================================

    // Checks T11 (Regional Escalation Tree).
    // Rule: if fireSpreadRate > 0.5 → escalate to neighbours.
    // Prints zones that need escalation.
    // Time Complexity: O(n)
    void regionalDecision(int zoneId, float fireSpreadRate);

    // ========================================================
    // Global Emergency  (Menu 6.4)
    // ========================================================

    // Aggregates risk across all zones in T12.
    // If sum > GLOBAL_RISK_THRESHOLD → prints GLOBAL ALERT.
    // Time Complexity: O(n)
    void globalEmergency();

    // ========================================================
    // Final Action  (Menu 6.5)
    // ========================================================

    // Selects the single highest-risk zone across T10–T12 and
    // prints the recommended final action (evacuate / contain /
    // monitor / stable).
    // Time Complexity: O(n)
    void executeFinalAction();

    // ========================================================
    // Data Insertion helpers (used by menu + seed)
    // ========================================================

    // Inserts a node into tree treeId (1–12).
    // Time Complexity: O(h)
    void insertNode(int treeId, const std::string& label,
                    float riskScore, float value, int zoneId);

    // ========================================================
    // Display helpers
    // ========================================================

    // Prints the full structure of tree treeId.
    // Time Complexity: O(n)
    void displayTree(int treeId) const;

    // Prints a one-line summary of all 12 trees (name + size + avg risk).
    // Time Complexity: O(n * 12)
    void displayAllTrees() const;
};

#endif // TREES_H
