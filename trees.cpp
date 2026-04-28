// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : trees.cpp
// Layer  : Part 4 — Tree-Based Decision Intelligence Layer
//
// Implements DecisionTree and DecisionEngine declared in trees.h
//
// Authors: Partner B
// Course : CL2001 — Data Structures 2026
// ============================================================

#include "trees.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>

// ============================================================
// DecisionTree — private helpers
// ============================================================

// ------------------------------------------------------------
// insertRec
// Recursively inserts node n into BST rooted at cur.
// BST key = riskScore  (left < root, right >= root).
// Time Complexity: O(h) — h is current tree height
// ------------------------------------------------------------
TreeNode* DecisionTree::insertRec(TreeNode* cur, TreeNode* n)
{
    if (cur == nullptr)
        return n;                          // Found empty slot

    if (n->riskScore < cur->riskScore)
        cur->left  = insertRec(cur->left,  n);
    else
        cur->right = insertRec(cur->right, n);

    return cur;
}

// ------------------------------------------------------------
// searchByZone
// Linear scan: zoneId is not the BST key, must visit all nodes.
// Time Complexity: O(n)
// ------------------------------------------------------------
TreeNode* DecisionTree::searchByZone(TreeNode* cur, int zoneId) const
{
    if (cur == nullptr) return nullptr;

    if (cur->zoneId == zoneId) return cur;

    TreeNode* found = searchByZone(cur->left, zoneId);
    if (found) return found;

    return searchByZone(cur->right, zoneId);
}

// ------------------------------------------------------------
// inorderRec
// In-order traversal (ascending riskScore), prints each node.
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionTree::inorderRec(TreeNode* cur, int depth) const
{
    if (cur == nullptr) return;

    inorderRec(cur->left, depth + 1);

    std::cout << "  [" << std::setw(2) << cur->nodeId << "] "
              << std::left << std::setw(28) << cur->label
              << " Zone:" << std::setw(2) << cur->zoneId
              << " Risk:" << std::fixed << std::setprecision(2)
              << cur->riskScore
              << " Val:"  << std::setprecision(1) << cur->value
              << "\n";

    inorderRec(cur->right, depth + 1);
}

// ------------------------------------------------------------
// preorderRec
// Pre-order traversal with indentation for structural display.
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionTree::preorderRec(TreeNode* cur, int depth) const
{
    if (cur == nullptr) return;

    std::string indent(depth * 3, ' ');

    std::cout << indent << "+-- [" << cur->nodeId << "] "
              << cur->label
              << " (risk=" << std::fixed << std::setprecision(2)
              << cur->riskScore << ", zone=" << cur->zoneId << ")\n";

    preorderRec(cur->left,  depth + 1);
    preorderRec(cur->right, depth + 1);
}

// ------------------------------------------------------------
// destroyRec
// Post-order delete — children freed before parent.
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionTree::destroyRec(TreeNode* cur)
{
    if (cur == nullptr) return;
    destroyRec(cur->left);
    destroyRec(cur->right);
    delete cur;
}

// ------------------------------------------------------------
// maxNode
// Walks right until rightmost (highest riskScore).
// Time Complexity: O(h)
// ------------------------------------------------------------
TreeNode* DecisionTree::maxNode(TreeNode* cur) const
{
    if (cur == nullptr)       return nullptr;
    if (cur->right == nullptr) return cur;
    return maxNode(cur->right);
}

// ------------------------------------------------------------
// collectHighRisk
// In-order traversal; collects nodes with riskScore > threshold.
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionTree::collectHighRisk(TreeNode* cur, float threshold,
                                    TreeNode* result[], int& count) const
{
    if (cur == nullptr) return;

    // Left subtree: only visit if max possible > threshold
    collectHighRisk(cur->left, threshold, result, count);

    if (cur->riskScore > threshold)
        result[count++] = cur;

    collectHighRisk(cur->right, threshold, result, count);
}

// ============================================================
// DecisionTree — private helper for averageRisk
// ============================================================
static void sumRisk(TreeNode* cur, float& total, int& cnt)
{
    if (cur == nullptr) return;
    total += cur->riskScore;
    cnt++;
    sumRisk(cur->left,  total, cnt);
    sumRisk(cur->right, total, cnt);
}

// ============================================================
// DecisionTree — public methods
// ============================================================

// ------------------------------------------------------------
// Constructor
// Time Complexity: O(1)
// ------------------------------------------------------------
DecisionTree::DecisionTree(const std::string& name)
    : root(nullptr), nodeCount(0), treeName(name)
{}

// ------------------------------------------------------------
// Destructor
// Time Complexity: O(n)
// ------------------------------------------------------------
DecisionTree::~DecisionTree()
{
    destroyRec(root);
    root = nullptr;
}

// ------------------------------------------------------------
// insert
// Allocates new TreeNode, assigns auto-incremented nodeId.
// Time Complexity: O(h)
// ------------------------------------------------------------
void DecisionTree::insert(const std::string& label, float riskScore,
                           float value, int zoneId)
{
    TreeNode* n    = new TreeNode();
    n->nodeId      = nodeCount + 1;   // 1-based auto ID
    n->label       = label;
    n->riskScore   = riskScore;
    n->value       = value;
    n->zoneId      = zoneId;
    n->left        = nullptr;
    n->right       = nullptr;

    root = insertRec(root, n);
    nodeCount++;
}

// ------------------------------------------------------------
// findByZone
// Time Complexity: O(n)
// ------------------------------------------------------------
TreeNode* DecisionTree::findByZone(int zoneId) const
{
    return searchByZone(root, zoneId);
}

// ------------------------------------------------------------
// getMaxRisk
// Time Complexity: O(h)
// ------------------------------------------------------------
TreeNode* DecisionTree::getMaxRisk() const
{
    return maxNode(root);
}

// ------------------------------------------------------------
// displayInOrder
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionTree::displayInOrder() const
{
    std::cout << "\n  Tree: " << treeName
              << "  (" << nodeCount << " nodes, in-order ascending risk)\n";
    std::cout << "  " << std::string(54, '-') << "\n";

    if (root == nullptr)
    {
        std::cout << "  (empty)\n";
        return;
    }
    inorderRec(root, 0);
}

// ------------------------------------------------------------
// displayStructure
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionTree::displayStructure() const
{
    std::cout << "\n  Tree: " << treeName << " — structural view\n";
    std::cout << "  " << std::string(54, '-') << "\n";

    if (root == nullptr)
    {
        std::cout << "  (empty)\n";
        return;
    }
    preorderRec(root, 0);
}

// ------------------------------------------------------------
// averageRisk
// Time Complexity: O(n)
// ------------------------------------------------------------
float DecisionTree::averageRisk() const
{
    if (nodeCount == 0) return 0.0f;

    float total = 0.0f;
    int   cnt   = 0;
    sumRisk(root, total, cnt);

    return (cnt > 0) ? total / cnt : 0.0f;
}

// ------------------------------------------------------------
// getHighRiskNodes
// Time Complexity: O(n)
// ------------------------------------------------------------
int DecisionTree::getHighRiskNodes(float threshold,
                                    TreeNode* result[], int capacity) const
{
    int count = 0;
    collectHighRisk(root, threshold, result, count);
    (void)capacity;   // capacity guard — caller responsible
    return count;
}

// ------------------------------------------------------------
// size / isEmpty / getName
// Time Complexity: O(1)
// ------------------------------------------------------------
int         DecisionTree::size()    const { return nodeCount; }
bool        DecisionTree::isEmpty() const { return nodeCount == 0; }
std::string DecisionTree::getName() const { return treeName; }


// ============================================================
// DecisionEngine — private helpers
// ============================================================

// ------------------------------------------------------------
// treeById  (non-const)
// Maps integer 1–12 to the corresponding tree member.
// Time Complexity: O(1)
// ------------------------------------------------------------
DecisionTree& DecisionEngine::treeById(int id)
{
    switch (id)
    {
        case  1: return t1;
        case  2: return t2;
        case  3: return t3;
        case  4: return t4;
        case  5: return t5;
        case  6: return t6;
        case  7: return t7;
        case  8: return t8;
        case  9: return t9;
        case 10: return t10;
        case 11: return t11;
        case 12: return t12;
        default:
            throw std::out_of_range("DecisionEngine: treeId must be 1–12");
    }
}

// const overload
const DecisionTree& DecisionEngine::treeById(int id) const
{
    return const_cast<DecisionEngine*>(this)->treeById(id);
}

// ------------------------------------------------------------
// weightedScore
// Decision Score = 0.4*fire + 0.3*smoke + 0.3*temperature
// Time Complexity: O(1)
// ------------------------------------------------------------
float DecisionEngine::weightedScore(float fire, float smoke,
                                     float temperature) const
{
    return 0.4f * fire + 0.3f * smoke + 0.3f * temperature;
}

// ============================================================
// DecisionEngine — constructor (seeds all 12 trees)
// Time Complexity: O(n) total insertions
// ============================================================
DecisionEngine::DecisionEngine()
    : t1("T1-ZoneHierarchy"),
      t2("T2-SubZoneDecomp"),
      t3("T3-TerrainClass"),
      t4("T4-WaterResource"),
      t5("T5-FireControl"),
      t6("T6-EquipAlloc"),
      t7("T7-FireClass"),
      t8("T8-WildlifeAct"),
      t9("T9-HumanAct"),
      t10("T10-LocalDecision"),
      t11("T11-RegionalEscal"),
      t12("T12-GlobalEmerg")
{
    // ── T1: Zone Hierarchy (4 zones) ────────────────────────
    t1.insert("Forest-Root",   0.10f, 1.0f, -1);
    t1.insert("Zone-North",    0.20f, 0.9f,  0);
    t1.insert("Zone-East",     0.35f, 0.8f,  1);
    t1.insert("Zone-South",    0.55f, 0.7f,  2);
    t1.insert("Zone-West",     0.45f, 0.6f,  3);

    // ── T2: Sub-Zone Decomposition ───────────────────────────
    t2.insert("Zone0-NW",  0.15f, 1.0f, 0);
    t2.insert("Zone0-NE",  0.22f, 1.0f, 0);
    t2.insert("Zone1-SE",  0.40f, 1.0f, 1);
    t2.insert("Zone1-SW",  0.38f, 1.0f, 1);
    t2.insert("Zone2-Full",0.60f, 1.0f, 2);
    t2.insert("Zone3-Full",0.48f, 1.0f, 3);

    // ── T3: Terrain Classification ───────────────────────────
    // TerrainRisk = (slope + dryness + density) / 3
    t3.insert("Flat-Moist",   0.10f, 0.10f, 0);
    t3.insert("Gentle-Dry",   0.35f, 0.35f, 1);
    t3.insert("Steep-Dry",    0.80f, 0.80f, 2);
    t3.insert("Rocky-Medium", 0.50f, 0.50f, 3);

    // ── T4: Water Resource ───────────────────────────────────
    // value = available/required ratio
    t4.insert("Reservoir-A", 0.20f, 0.90f, 0);
    t4.insert("Reservoir-B", 0.50f, 0.60f, 1);
    t4.insert("Reservoir-C", 0.70f, 0.35f, 2);
    t4.insert("Reservoir-D", 0.40f, 0.75f, 3);

    // ── T5: Fire Control Resources ───────────────────────────
    t5.insert("Truck-Unit-1",  0.25f, 3.0f, 0);
    t5.insert("Truck-Unit-2",  0.45f, 2.0f, 1);
    t5.insert("Aerial-Unit-1", 0.65f, 1.0f, 2);
    t5.insert("Ground-Team-1", 0.55f, 4.0f, 3);

    // ── T6: Equipment Allocation  (priority = risk * impact) ─
    t6.insert("Hose-Zone0",   0.18f, 2.0f, 0);
    t6.insert("Pump-Zone1",   0.42f, 3.0f, 1);
    t6.insert("RetardZone2",  0.72f, 5.0f, 2);
    t6.insert("DroneZone3",   0.48f, 4.0f, 3);

    // ── T7: Fire Classification ──────────────────────────────
    // FireLevel = 0.5*temperature + 0.5*smoke
    t7.insert("No-Fire",       0.05f,  0.0f, 0);
    t7.insert("Low-Fire",      0.30f, 30.0f, 1);
    t7.insert("Moderate-Fire", 0.55f, 55.0f, 2);
    t7.insert("High-Fire",     0.80f, 80.0f, 2);
    t7.insert("Extreme-Fire",  0.95f, 95.0f, 3);

    // ── T8: Wildlife Activity ────────────────────────────────
    t8.insert("Normal-Move",    0.10f, 0.10f, 0);
    t8.insert("Unusual-Move",   0.40f, 0.40f, 1);
    t8.insert("Mass-Exodus",    0.75f, 0.75f, 2);
    t8.insert("Critical-Evac",  0.90f, 0.90f, 3);

    // ── T9: Human Activity ───────────────────────────────────
    // HumanRisk = movement * restrictedAreaFactor
    t9.insert("No-Humans",      0.05f, 0.00f, 0);
    t9.insert("Authorized",     0.20f, 0.20f, 1);
    t9.insert("Unauthorized",   0.65f, 0.65f, 2);
    t9.insert("Trespassing",    0.85f, 0.85f, 3);

    // ── T10: Local Decision ──────────────────────────────────
    t10.insert("Zone0-Safe",      0.20f, 0.0f, 0);
    t10.insert("Zone1-Watch",     0.45f, 0.0f, 1);
    t10.insert("Zone2-Alert",     0.70f, 0.0f, 2);
    t10.insert("Zone3-Elevated",  0.55f, 0.0f, 3);

    // ── T11: Regional Escalation ─────────────────────────────
    t11.insert("Region-A-Low",    0.25f, 0.3f, 0);
    t11.insert("Region-B-Med",    0.50f, 0.6f, 1);
    t11.insert("Region-C-High",   0.75f, 0.8f, 2);
    t11.insert("Region-D-Crit",   0.88f, 0.9f, 3);

    // ── T12: Global Emergency ────────────────────────────────
    t12.insert("Global-Safe",     0.15f, 0.0f, -1);
    t12.insert("Global-Warning",  0.40f, 0.0f, -1);
    t12.insert("Global-Alert",    0.65f, 0.0f, -1);
    t12.insert("Global-Critical", 0.90f, 0.0f, -1);
}

// ============================================================
// DecisionEngine — public methods
// ============================================================

// ------------------------------------------------------------
// computeRiskScore  (Menu 6.1)
// Time Complexity: O(1)
// ------------------------------------------------------------
float DecisionEngine::computeRiskScore(int zoneId, float fire,
                                        float smoke, float temperature)
{
    float score = weightedScore(fire, smoke, temperature);

    std::cout << "\n  ── Risk Score Computation ──────────────────────────\n";
    std::cout << "  Zone       : " << zoneId << "\n";
    std::cout << "  Fire       : " << std::fixed << std::setprecision(2) << fire       << "\n";
    std::cout << "  Smoke      : " << smoke      << "\n";
    std::cout << "  Temperature: " << temperature << "\n";
    std::cout << "  Formula    : 0.4*fire + 0.3*smoke + 0.3*temp\n";
    std::cout << "  Score      : " << std::setprecision(4) << score << "\n";

    if (score > DECISION_THRESHOLD)
        std::cout << "  Status     : [!] EMERGENCY ACTIVATED  (score > "
                  << DECISION_THRESHOLD << ")\n";
    else
        std::cout << "  Status     : [OK] Normal  (score <= "
                  << DECISION_THRESHOLD << ")\n";

    // Store result in T10 for zone
    TreeNode* existing = t10.findByZone(zoneId);
    if (existing)
        existing->riskScore = score;       // update in-place
    else
        t10.insert("Zone-" + std::to_string(zoneId) + "-Computed",
                   score, 0.0f, zoneId);

    return score;
}

// ------------------------------------------------------------
// zoneDecision  (Menu 6.2)
// Rule: riskScore > 0.6 → local response
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionEngine::zoneDecision(int zoneId, float riskScore)
{
    std::cout << "\n  ── Zone-Level Decision (T10) ───────────────────────\n";
    std::cout << "  Zone " << zoneId
              << "  Risk Score: " << std::fixed << std::setprecision(2)
              << riskScore << "\n";

    if (riskScore > 0.6f)
    {
        std::cout << "  Decision : ACTIVATE LOCAL RESPONSE\n";
        std::cout << "  Action   : Deploy nearest fire unit to Zone "
                  << zoneId << "\n";
        t10.insert("Zone-" + std::to_string(zoneId) + "-LocalResp",
                   riskScore, 0.0f, zoneId);
    }
    else if (riskScore > 0.3f)
    {
        std::cout << "  Decision : ELEVATED WATCH\n";
        std::cout << "  Action   : Increase sensor frequency for Zone "
                  << zoneId << "\n";
    }
    else
    {
        std::cout << "  Decision : NORMAL MONITORING\n";
        std::cout << "  Action   : Continue standard monitoring\n";
    }

    std::cout << "\n  T10 current state:\n";
    t10.displayInOrder();
}

// ------------------------------------------------------------
// regionalDecision  (Menu 6.3)
// Rule: fireSpreadRate > 0.5 → escalate
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionEngine::regionalDecision(int zoneId, float fireSpreadRate)
{
    std::cout << "\n  ── Regional Escalation Decision (T11) ─────────────\n";
    std::cout << "  Source Zone    : " << zoneId << "\n";
    std::cout << "  Fire Spread Rate: " << std::fixed << std::setprecision(2)
              << fireSpreadRate << "\n";

    if (fireSpreadRate > 0.5f)
    {
        std::cout << "  Decision : ESCALATE to neighbouring zones\n";
        // Neighbour zones in a 4-zone grid: (zoneId+1)%4, (zoneId+3)%4
        int n1 = (zoneId + 1) % 4;
        int n2 = (zoneId + 3) % 4;
        std::cout << "  Alert sent to Zone " << n1
                  << " and Zone " << n2 << "\n";

        t11.insert("Escalation-Z" + std::to_string(zoneId),
                   fireSpreadRate, fireSpreadRate, zoneId);
    }
    else
    {
        std::cout << "  Decision : NO ESCALATION — spread rate within limits\n";
    }

    std::cout << "\n  T11 current state:\n";
    t11.displayInOrder();
}

// ------------------------------------------------------------
// globalEmergency  (Menu 6.4)
// Sums riskScore across all T12 nodes.
// If sum > GLOBAL_RISK_THRESHOLD → GLOBAL ALERT
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionEngine::globalEmergency()
{
    std::cout << "\n  ── Global Emergency Evaluation (T12) ──────────────\n";

    float avgRisk = t12.averageRisk();
    float sumRiskVal = avgRisk * (float)t12.size();

    std::cout << "  T12 Nodes       : " << t12.size() << "\n";
    std::cout << "  Average Risk    : " << std::fixed << std::setprecision(3)
              << avgRisk << "\n";
    std::cout << "  Aggregate Score : " << sumRiskVal << "\n";
    std::cout << "  Threshold       : " << GLOBAL_RISK_THRESHOLD << "\n";

    if (sumRiskVal > GLOBAL_RISK_THRESHOLD)
    {
        std::cout << "\n  [!!!] GLOBAL ALERT — Full system emergency mode!\n";
        std::cout << "  Action: Notify all zones, activate T12 response tree.\n";
    }
    else
    {
        std::cout << "\n  [OK]  Global risk within acceptable bounds.\n";
    }

    std::cout << "\n  T12 current state:\n";
    t12.displayInOrder();
}

// ------------------------------------------------------------
// executeFinalAction  (Menu 6.5)
// Finds highest-risk node across T10–T12, prints final action.
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionEngine::executeFinalAction()
{
    std::cout << "\n  ── Final Action Execution ──────────────────────────\n";

    TreeNode* best = nullptr;
    float     bestRisk = -1.0f;

    // Check T10, T11, T12 for maximum risk
    for (int id : {10, 11, 12})
    {
        TreeNode* candidate = treeById(id).getMaxRisk();
        if (candidate && candidate->riskScore > bestRisk)
        {
            bestRisk = candidate->riskScore;
            best     = candidate;
        }
    }

    if (best == nullptr)
    {
        std::cout << "  No decision nodes available.\n";
        return;
    }

    std::cout << "  Highest-Risk Node : " << best->label << "\n";
    std::cout << "  Risk Score        : " << std::fixed << std::setprecision(2)
              << best->riskScore << "\n";
    std::cout << "  Zone              : " << best->zoneId << "\n\n";

    if (bestRisk >= 0.85f)
    {
        std::cout << "  [!!!] FINAL ACTION: FULL EVACUATION + FIRE SUPPRESSION\n";
        std::cout << "        Deploy all units. Close access roads. Alert authorities.\n";
    }
    else if (bestRisk >= 0.60f)
    {
        std::cout << "  [!!]  FINAL ACTION: CONTAIN FIRE + DEPLOY UNITS\n";
        std::cout << "        Send fire trucks and water units to high-risk zones.\n";
    }
    else if (bestRisk >= 0.30f)
    {
        std::cout << "  [!]   FINAL ACTION: ELEVATED MONITORING\n";
        std::cout << "        Increase sensor frequency and standby resources.\n";
    }
    else
    {
        std::cout << "  [OK]  FINAL ACTION: NORMAL OPERATIONS\n";
        std::cout << "        Continue standard monitoring cycle.\n";
    }
}

// ------------------------------------------------------------
// insertNode  (Menu 6 — manual data insertion)
// Time Complexity: O(h)
// ------------------------------------------------------------
void DecisionEngine::insertNode(int treeId, const std::string& label,
                                 float riskScore, float value, int zoneId)
{
    treeById(treeId).insert(label, riskScore, value, zoneId);

    std::cout << "  Node inserted into " << treeById(treeId).getName()
              << " — label=\"" << label
              << "\" risk=" << std::fixed << std::setprecision(2) << riskScore
              << " zone=" << zoneId << "\n";
}

// ------------------------------------------------------------
// displayTree  (single tree)
// Time Complexity: O(n)
// ------------------------------------------------------------
void DecisionEngine::displayTree(int treeId) const
{
    treeById(treeId).displayStructure();
}

// ------------------------------------------------------------
// displayAllTrees  (summary dashboard)
// Time Complexity: O(n * 12)
// ------------------------------------------------------------
void DecisionEngine::displayAllTrees() const
{
    std::cout << "\n  ══ Decision Intelligence Layer — All 12 Trees ══════\n";
    std::cout << "  " << std::left
              << std::setw(22) << "Tree"
              << std::setw(8)  << "Nodes"
              << std::setw(12) << "Avg Risk"
              << "Max Risk Node\n";
    std::cout << "  " << std::string(56, '-') << "\n";

    for (int id = 1; id <= 12; id++)
    {
        const DecisionTree& dt = treeById(id);
        TreeNode* mx = dt.getMaxRisk();

        std::cout << "  " << std::left
                  << std::setw(22) << dt.getName()
                  << std::setw(8)  << dt.size()
                  << std::setw(12) << std::fixed << std::setprecision(3)
                  << dt.averageRisk();

        if (mx)
            std::cout << mx->label
                      << " (" << std::setprecision(2) << mx->riskScore << ")";
        else
            std::cout << "(empty)";

        std::cout << "\n";
    }
    std::cout << "  " << std::string(56, '=') << "\n";
}
