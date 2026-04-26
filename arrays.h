// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : arrays.h
// Layer  : Part 1 — Array-Based Environmental Processing Layer
//
// Structural Instances:
//   A1 — Static Environmental Baseline Array    (float[4])
//   A2 — Dynamic Sensor Stream Array            (SensorReading[MAX_READINGS])
//   A3 — Static Forest Grid Matrix              (float[ROWS][COLS])
//   A4 — Dynamic Terrain Expansion Matrix       (float[ROWS][COLS])
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#ifndef ARRAYS_H
#define ARRAYS_H

#include "constants.h"
#include <string>

// ------------------------------------------------------------
// struct SensorReading
// Represents one timestamped reading from a forest sensor node.
// Used as the element type of A2 (Dynamic Sensor Stream Array).
// Memory: fixed-size struct — O(1) construction and access.
// ------------------------------------------------------------

struct SensorReading
{
    float temperature;   // Temperature reading in Celsius
    float humidity;      // Relative humidity in percentage (%)
    float smokeLevel;    // Smoke concentration (arbitrary units)
    float windSpeed;     // Wind speed in km/h
    int   zoneId;        // Forest zone this reading belongs to (0–NUM_ZONES-1)
    int   timestamp;     // Logical timestamp (incremented per reading)
    bool  isValid;       // False if noise-filtered or out-of-range
    bool  isAnomaly;     // True if flagged by flagAnomaly()
};

// ------------------------------------------------------------
// class ArrayLayer
// Manages all four array instances (A1–A4) for the system.
// Provides data ingestion, validation, spatial analysis, and
// display operations used by the menu and higher layers.
// ------------------------------------------------------------

class ArrayLayer
{
private:

    // --------------------------------------------------------
    // A1: Static Environmental Baseline Array
    // Stores fixed reference values for normal forest state.
    // Index mapping (use BASELINE_* constants from constants.h):
    //   [0] = temperature   [1] = humidity
    //   [2] = smokeLevel    [3] = windSpeed
    // Never modified after loadBaseline(). O(1) access.
    // --------------------------------------------------------
    float baseline[4];

    // --------------------------------------------------------
    // A2: Dynamic Sensor Stream Array
    // Stores live sensor readings as they arrive at runtime.
    // Elements appended via ingestReading(). Grows until MAX_READINGS.
    // Sequential (1D) — tracks time-ordered sensor data.
    // Access: O(1) by index, O(n) full scan.
    // --------------------------------------------------------
    SensorReading sensorStream[MAX_READINGS];
    int           streamCount;      // Current number of stored readings

    // --------------------------------------------------------
    // A3: Static Forest Grid Matrix (2D Spatial)
    // Represents forest zones as a 5x5 spatial grid.
    // Each cell [r][c] holds a baseline temperature value.
    // Loaded once via loadStaticGrid(). Never updated at runtime.
    // Spatial access: O(1) at [row][col].
    // --------------------------------------------------------
    float grid[ROWS][COLS];

    // --------------------------------------------------------
    // A4: Dynamic Terrain Expansion Matrix (2D Spatial)
    // Same layout as A3 but updated in real time with live sensor data.
    // Updated by ingestReading() whenever a new reading arrives.
    // Supports interpolation and boundary detection.
    // Spatial access: O(1) at [row][col].
    // --------------------------------------------------------
    float terrain[ROWS][COLS];

    // --------------------------------------------------------
    // Internal helper — maps zoneId to (row, col) in the grid.
    // zoneId 0..NUM_ZONES-1 maps as: row = zoneId/COLS, col = zoneId%COLS
    // Time Complexity: O(1)
    // --------------------------------------------------------
    void zoneToGrid(int zoneId, int &row, int &col) const;

public:

    // --------------------------------------------------------
    // Constructor
    // Initialises all arrays to safe default values, then calls
    // loadBaseline() and loadStaticGrid() automatically.
    // Time Complexity: O(n*m) for grid initialisation
    // --------------------------------------------------------
    ArrayLayer();

    // ========================================================
    // A1 — Baseline Operations
    // ========================================================

    // Loads fixed reference values into A1 from constants.h.
    // Sets baseline[0..3] = TEMP_NORM, HUM_NORM, SMOKE_NORM, WIND_NORM.
    // Time Complexity: O(1)
    void loadBaseline();

    // Returns the baseline value for a given index (0–3).
    // Use BASELINE_TEMP / BASELINE_HUM / BASELINE_SMOKE / BASELINE_WIND.
    // Time Complexity: O(1)
    float getBaseline(int index) const;

    // ========================================================
    // A2 — Dynamic Stream Operations
    // ========================================================

    // Appends a new SensorReading to sensorStream (A2).
    // Also updates the corresponding cell in terrain (A4).
    // Rejects if stream is full (streamCount >= MAX_READINGS).
    // Time Complexity: O(1)
    void ingestReading(SensorReading r);

    // Compares reading r against A1 baseline.
    // Returns true if ANY field deviates by more than DELTA.
    // Prints which specific field deviated and by how much.
    // Time Complexity: O(1)
    bool compareWithBaseline(SensorReading r) const;

    // Checks the reading at index idx for noise.
    // Noise rule: |stream[idx].temp - stream[idx-1].temp| >= DELTA
    // Marks stream[idx].isValid = false if noise detected.
    // Returns false (invalid) when noise is found.
    // Time Complexity: O(1)
    bool filterNoise(int idx);

    // Checks reading r against anomaly thresholds:
    //   temperature  > TEMP_MAX   → fire risk
    //   smokeLevel   > SMOKE_MAX  → possible fire
    //   humidity     < HUM_MIN    → dry condition
    //   |temp - TEMP_NORM| > THETA → sudden spike
    // Marks r.isAnomaly = true internally if triggered.
    // Time Complexity: O(1)
    bool flagAnomaly(SensorReading &r);

    // Returns the reading stored at position idx in A2.
    // Time Complexity: O(1)
    SensorReading getReading(int idx) const;

    // Returns the most recently ingested reading.
    // Returns a zeroed reading if stream is empty.
    // Time Complexity: O(1)
    SensorReading getLatestReading() const;

    // Returns current number of stored readings in A2.
    // Time Complexity: O(1)
    int getStreamCount() const;

    // ========================================================
    // A3 — Static Grid Operations
    // ========================================================

    // Populates A3 with a hardcoded 5x5 baseline temperature map.
    // Values represent normal zone temperatures (22–28°C range).
    // Prints the grid after loading.
    // Time Complexity: O(n*m)
    void loadStaticGrid();

    // Returns the static baseline value at grid[row][col].
    // Time Complexity: O(1)
    float getGridValue(int row, int col) const;

    // ========================================================
    // A4 — Dynamic Terrain Operations
    // ========================================================

    // Estimates a missing value at terrain[row][col] using
    // the average of valid neighbouring cells (top/bottom/left/right).
    // Formula: value = sum(valid_neighbours) / count(valid_neighbours)
    // Skips out-of-bounds neighbours automatically.
    // Time Complexity: O(1) — fixed max 4 neighbours
    float interpolateMissing(int row, int col) const;

    // Checks if there is a sharp environmental boundary between
    // terrain[row][col] and its right/bottom neighbour.
    // Boundary condition: |cell - neighbour| > DELTA
    // Prints "Boundary detected" with zone info if true.
    // Time Complexity: O(1)
    bool detectBoundary(int row, int col) const;

    // ========================================================
    // Display Operations
    // ========================================================

    // Prints all readings in A2 as a numbered time series.
    // Shows timestamp, zoneId, temperature, smoke, humidity,
    // validity flag, and anomaly flag for each reading.
    // Time Complexity: O(n)
    void displayTimeSeries() const;

    // Prints A3 (static forest grid) as a formatted 5x5 matrix
    // with row/column labels and zone IDs.
    // Time Complexity: O(n*m)
    void displayGrid() const;

    // Prints A4 (dynamic terrain) as a formatted 5x5 matrix
    // showing live temperature values per zone.
    // Time Complexity: O(n*m)
    void displayTerrain() const;

    // Prints a summary dashboard: stream count, anomaly count,
    // last reading values, and baseline comparison result.
    // Time Complexity: O(n)
    void displayStreamStatus() const;

    // Prints all four baseline values from A1.
    // Time Complexity: O(1)
    void displayBaseline() const;
};

#endif // ARRAYS_H
