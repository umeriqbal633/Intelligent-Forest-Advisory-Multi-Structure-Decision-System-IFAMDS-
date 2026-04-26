// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : arrays.cpp
// Layer  : Part 1 — Array-Based Environmental Processing Layer
//
// Implements all methods of class ArrayLayer declared in arrays.h.
// Data structures used:
//   A1 — float[4]                     static baseline
//   A2 — SensorReading[MAX_READINGS]  dynamic sensor stream
//   A3 — float[ROWS][COLS]            static forest grid
//   A4 — float[ROWS][COLS]            dynamic terrain matrix
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#include "arrays.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

using namespace std;

// ============================================================
// Internal Utility
// ============================================================

// Maps a flat zoneId (0..NUM_ZONES-1) to 2D (row, col) indices.
// Formula: row = zoneId / COLS,  col = zoneId % COLS
// Time Complexity: O(1)
void ArrayLayer::zoneToGrid(int zoneId, int &row, int &col) const
{
    row = zoneId / COLS;
    col = zoneId % COLS;
}

// ============================================================
// Constructor
// Initialises all arrays to safe defaults before loading data.
// Time Complexity: O(n*m) — grid initialisation dominates
// ============================================================
ArrayLayer::ArrayLayer()
{
    // Initialise A1 baseline to zero
    for (int i = 0; i < 4; i++)
        baseline[i] = 0.0f;

    // Initialise A2 stream
    streamCount = 0;
    for (int i = 0; i < MAX_READINGS; i++)
    {
        sensorStream[i] = {0.0f, 0.0f, 0.0f, 0.0f, 0, 0, true, false};
    }

    // Initialise A3 and A4 grids to zero
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
        {
            grid[r][c]    = 0.0f;
            terrain[r][c] = 0.0f;
        }

    // Auto-load baseline and static grid
    loadBaseline();
    loadStaticGrid();
}

// ============================================================
// A1 — Baseline Operations
// ============================================================

// Loads fixed reference values from constants into A1.
// These represent normal, healthy forest conditions.
// Time Complexity: O(1)
void ArrayLayer::loadBaseline()
{
    baseline[BASELINE_TEMP]  = TEMP_NORM;   // 25.0°C
    baseline[BASELINE_HUM]   = HUM_NORM;    // 60.0%
    baseline[BASELINE_SMOKE] = SMOKE_NORM;  // 0.0 units
    baseline[BASELINE_WIND]  = WIND_NORM;   // 10.0 km/h

    cout << "\n[A1] Static baseline loaded successfully." << endl;
    cout << "     Normal: Temp=" << TEMP_NORM << "C  Humidity=" << HUM_NORM
         << "%  Smoke=" << SMOKE_NORM << "  Wind=" << WIND_NORM << " km/h" << endl;
}

// Returns the baseline value at the given index.
// Time Complexity: O(1)
float ArrayLayer::getBaseline(int index) const
{
    if (index < 0 || index > 3)
        return -1.0f;
    return baseline[index];
}

// Prints all four A1 baseline reference values.
// Time Complexity: O(1)
void ArrayLayer::displayBaseline() const
{
    cout << "\n╔══════════════════════════════════════╗" << endl;
    cout << "║   A1 — Static Baseline Reference     ║" << endl;
    cout << "╠══════════════════════════════════════╣" << endl;
    cout << "║  Temperature  : " << setw(6) << baseline[BASELINE_TEMP]  << " °C               ║" << endl;
    cout << "║  Humidity     : " << setw(6) << baseline[BASELINE_HUM]   << " %                ║" << endl;
    cout << "║  Smoke Level  : " << setw(6) << baseline[BASELINE_SMOKE] << " units            ║" << endl;
    cout << "║  Wind Speed   : " << setw(6) << baseline[BASELINE_WIND]  << " km/h             ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;
}

// ============================================================
// A3 — Static Grid Operations
// ============================================================

// Populates A3 with a hardcoded 5x5 baseline temperature map.
// Represents the normal temperature distribution across forest zones.
// Time Complexity: O(n*m)
void ArrayLayer::loadStaticGrid()
{
    // Hardcoded baseline zone temperatures (°C) — normal conditions
    // Slight variation simulates natural forest temperature gradient
    float staticValues[ROWS][COLS] = {
        { 24.0f, 25.0f, 25.5f, 24.5f, 23.5f },
        { 25.5f, 26.0f, 26.5f, 25.0f, 24.0f },
        { 26.0f, 27.0f, 27.5f, 26.0f, 25.0f },
        { 25.0f, 25.5f, 26.0f, 25.5f, 24.5f },
        { 23.5f, 24.0f, 24.5f, 24.0f, 23.0f }
    };

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
        {
            grid[r][c]    = staticValues[r][c];
            terrain[r][c] = staticValues[r][c];  // A4 starts from A3 baseline
        }

    cout << "\n[A3] Static forest grid (5x5) loaded." << endl;
    cout << "[A4] Dynamic terrain matrix initialised from grid." << endl;
}

// Returns the static baseline value at grid[row][col].
// Time Complexity: O(1)
float ArrayLayer::getGridValue(int row, int col) const
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        return -1.0f;
    return grid[row][col];
}

// ============================================================
// A2 — Dynamic Stream Operations
// ============================================================

// Appends a new SensorReading to A2 and updates A4 terrain.
// Also auto-runs noise filter and anomaly check on arrival.
// Time Complexity: O(1)
void ArrayLayer::ingestReading(SensorReading r)
{
    // Capacity check
    if (streamCount >= MAX_READINGS)
    {
        cout << "[A2] ERROR: Sensor stream is full (max " << MAX_READINGS << " readings)." << endl;
        return;
    }

    // Assign timestamp equal to current stream position
    r.timestamp = streamCount;
    r.isValid   = true;
    r.isAnomaly = false;

    // Store in A2 stream — O(1) append
    sensorStream[streamCount] = r;

    // Update A4 terrain with live temperature at zone location — O(1)
    int row, col;
    zoneToGrid(r.zoneId, row, col);
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
        terrain[row][col] = r.temperature;

    cout << "\n[A2] Reading #" << streamCount << " ingested:"
         << "  Zone=" << r.zoneId
         << "  Temp=" << r.temperature << "C"
         << "  Smoke=" << r.smokeLevel
         << "  Humidity=" << r.humidity << "%" << endl;
    cout << "[A4] Terrain updated at [" << row << "][" << col << "] = "
         << r.temperature << "C" << endl;

    streamCount++;
}

// Compares a reading against A1 baseline.
// Returns true if any field deviates by more than DELTA (noise threshold).
// Time Complexity: O(1)
bool ArrayLayer::compareWithBaseline(SensorReading r) const
{
    bool deviated = false;

    cout << "\n[A1 vs A2] Baseline Comparison for Zone " << r.zoneId << ":" << endl;

    float tempDiff  = fabs(r.temperature - baseline[BASELINE_TEMP]);
    float humDiff   = fabs(r.humidity    - baseline[BASELINE_HUM]);
    float smokeDiff = fabs(r.smokeLevel  - baseline[BASELINE_SMOKE]);
    float windDiff  = fabs(r.windSpeed   - baseline[BASELINE_WIND]);

    if (tempDiff > DELTA)
    {
        cout << "  [!] Temperature deviation: " << r.temperature
             << "C (baseline=" << baseline[BASELINE_TEMP]
             << "C, diff=" << tempDiff << ")" << endl;
        deviated = true;
    }
    if (humDiff > DELTA)
    {
        cout << "  [!] Humidity deviation: " << r.humidity
             << "% (baseline=" << baseline[BASELINE_HUM]
             << "%, diff=" << humDiff << ")" << endl;
        deviated = true;
    }
    if (smokeDiff > DELTA)
    {
        cout << "  [!] Smoke deviation: " << r.smokeLevel
             << " (baseline=" << baseline[BASELINE_SMOKE]
             << ", diff=" << smokeDiff << ")" << endl;
        deviated = true;
    }
    if (windDiff > DELTA)
    {
        cout << "  [!] Wind deviation: " << r.windSpeed
             << " km/h (baseline=" << baseline[BASELINE_WIND]
             << " km/h, diff=" << windDiff << ")" << endl;
        deviated = true;
    }

    if (!deviated)
        cout << "  [OK] All values within normal range (delta=" << DELTA << ")." << endl;

    return deviated;
}

// Checks reading at index idx for sudden noise spike.
// Noise rule: |stream[idx].temperature - stream[idx-1].temperature| >= DELTA
// Marks reading as invalid if noise is detected.
// Time Complexity: O(1)
bool ArrayLayer::filterNoise(int idx)
{
    // Need at least two readings to detect noise
    if (idx <= 0 || idx >= streamCount)
    {
        cout << "[Noise Filter] Cannot check idx=" << idx
             << " (need at least 2 readings)." << endl;
        return true;
    }

    float diff = fabs(sensorStream[idx].temperature - sensorStream[idx - 1].temperature);

    if (diff >= DELTA)
    {
        sensorStream[idx].isValid = false;
        cout << "[Noise Filter] NOISE detected at reading #" << idx
             << ": |" << sensorStream[idx].temperature
             << " - " << sensorStream[idx - 1].temperature
             << "| = " << diff << " >= DELTA(" << DELTA << ")" << endl;
        cout << "  Reading #" << idx << " marked INVALID." << endl;
        return false;
    }

    cout << "[Noise Filter] Reading #" << idx << " is CLEAN (diff=" << diff << ")." << endl;
    return true;
}

// Checks if a reading triggers any anomaly condition.
// Conditions checked (any one is sufficient):
//   temperature  > TEMP_MAX          → fire risk
//   smokeLevel   > SMOKE_MAX         → fire smoke
//   humidity     < HUM_MIN           → dangerous dryness
//   |temp-TEMP_NORM| > THETA         → sudden spike
// Time Complexity: O(1)
bool ArrayLayer::flagAnomaly(SensorReading &r)
{
    bool anomaly = false;

    cout << "\n[Anomaly Check] Zone " << r.zoneId << ":" << endl;

    if (r.temperature > TEMP_MAX)
    {
        cout << "  [ALERT] Temperature " << r.temperature
             << "C exceeds TEMP_MAX(" << TEMP_MAX << "C) — FIRE RISK!" << endl;
        anomaly = true;
    }
    if (r.smokeLevel > SMOKE_MAX)
    {
        cout << "  [ALERT] Smoke level " << r.smokeLevel
             << " exceeds SMOKE_MAX(" << SMOKE_MAX << ") — POSSIBLE FIRE!" << endl;
        anomaly = true;
    }
    if (r.humidity < HUM_MIN)
    {
        cout << "  [ALERT] Humidity " << r.humidity
             << "% below HUM_MIN(" << HUM_MIN << "%) — DRY CONDITION!" << endl;
        anomaly = true;
    }
    if (fabs(r.temperature - TEMP_NORM) > THETA)
    {
        cout << "  [ALERT] Temperature spike: |" << r.temperature
             << " - " << TEMP_NORM << "| = "
             << fabs(r.temperature - TEMP_NORM)
             << " > THETA(" << THETA << ") — SUDDEN SPIKE!" << endl;
        anomaly = true;
    }

    if (!anomaly)
        cout << "  [OK] No anomaly detected." << endl;
    else
    {
        r.isAnomaly = true;
        // Update the stored reading's anomaly flag in stream
        for (int i = 0; i < streamCount; i++)
            if (sensorStream[i].zoneId    == r.zoneId &&
                sensorStream[i].timestamp == r.timestamp)
            {
                sensorStream[i].isAnomaly = true;
                break;
            }
    }

    return anomaly;
}

// Returns the reading at index idx in A2.
// Time Complexity: O(1)
SensorReading ArrayLayer::getReading(int idx) const
{
    if (idx < 0 || idx >= streamCount)
    {
        SensorReading empty = {0.0f, 0.0f, 0.0f, 0.0f, -1, -1, false, false};
        return empty;
    }
    return sensorStream[idx];
}

// Returns the most recently ingested reading.
// Time Complexity: O(1)
SensorReading ArrayLayer::getLatestReading() const
{
    if (streamCount == 0)
    {
        SensorReading empty = {0.0f, 0.0f, 0.0f, 0.0f, -1, -1, false, false};
        return empty;
    }
    return sensorStream[streamCount - 1];
}

// Returns current number of readings stored in A2.
// Time Complexity: O(1)
int ArrayLayer::getStreamCount() const
{
    return streamCount;
}

// ============================================================
// A4 — Dynamic Terrain Operations
// ============================================================

// Estimates missing value at terrain[row][col] by averaging
// valid neighbouring cells (top, bottom, left, right).
// Boundary neighbours (out-of-range) are simply skipped.
// Formula: value = sum(valid_neighbours) / count(valid_neighbours)
// Time Complexity: O(1) — at most 4 neighbours checked
float ArrayLayer::interpolateMissing(int row, int col) const
{
    float sum   = 0.0f;
    int   count = 0;

    // Top neighbour
    if (row - 1 >= 0)    { sum += terrain[row-1][col]; count++; }
    // Bottom neighbour
    if (row + 1 < ROWS)  { sum += terrain[row+1][col]; count++; }
    // Left neighbour
    if (col - 1 >= 0)    { sum += terrain[row][col-1]; count++; }
    // Right neighbour
    if (col + 1 < COLS)  { sum += terrain[row][col+1]; count++; }

    if (count == 0)
    {
        cout << "[Interpolate] No valid neighbours for ["
             << row << "][" << col << "]." << endl;
        return TEMP_NORM;  // Fall back to baseline
    }

    float estimated = sum / count;
    cout << "[Interpolate] Missing value at [" << row << "][" << col
         << "] estimated = " << estimated
         << "C (avg of " << count << " neighbours)" << endl;

    return estimated;
}

// Checks for a sharp temperature boundary between terrain[row][col]
// and its right/bottom neighbour.
// Boundary condition: |cell - neighbour| > DELTA
// Time Complexity: O(1)
bool ArrayLayer::detectBoundary(int row, int col) const
{
    bool found = false;

    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
    {
        cout << "[Boundary] Invalid zone [" << row << "][" << col << "]." << endl;
        return false;
    }

    // Check right neighbour
    if (col + 1 < COLS)
    {
        float diff = fabs(terrain[row][col] - terrain[row][col+1]);
        if (diff > DELTA)
        {
            cout << "[Boundary] DETECTED between Zone ["
                 << row << "][" << col << "]=" << terrain[row][col]
                 << "C and [" << row << "][" << col+1
                 << "]=" << terrain[row][col+1]
                 << "C  |diff=" << diff << "| > DELTA(" << DELTA << ")" << endl;
            found = true;
        }
    }

    // Check bottom neighbour
    if (row + 1 < ROWS)
    {
        float diff = fabs(terrain[row][col] - terrain[row+1][col]);
        if (diff > DELTA)
        {
            cout << "[Boundary] DETECTED between Zone ["
                 << row << "][" << col << "]=" << terrain[row][col]
                 << "C and [" << row+1 << "][" << col
                 << "]=" << terrain[row+1][col]
                 << "C  |diff=" << diff << "| > DELTA(" << DELTA << ")" << endl;
            found = true;
        }
    }

    if (!found)
        cout << "[Boundary] No boundary at [" << row << "][" << col << "]." << endl;

    return found;
}

// ============================================================
// Display Operations
// ============================================================

// Prints all readings in A2 as a formatted time-series table.
// Time Complexity: O(n)
void ArrayLayer::displayTimeSeries() const
{
    cout << "\n╔══════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║            A2 — Dynamic Sensor Stream (Time Series)                 ║" << endl;
    cout << "╠═══╦══════╦════════╦══════════╦═══════╦═══════════╦═══════╦═════════╣" << endl;
    cout << "║ # ║ Zone ║ Temp°C ║ Humidity ║ Smoke ║ Wind km/h ║ Valid ║ Anomaly ║" << endl;
    cout << "╠═══╬══════╬════════╬══════════╬═══════╬═══════════╬═══════╬═════════╣" << endl;

    if (streamCount == 0)
    {
        cout << "║              No readings ingested yet.                              ║" << endl;
    }
    else
    {
        for (int i = 0; i < streamCount; i++)
        {
            const SensorReading &s = sensorStream[i];
            cout << "║ " << setw(1) << i
                 << " ║  " << setw(3) << s.zoneId
                 << " ║  " << setw(5) << fixed << setprecision(1) << s.temperature
                 << " ║    " << setw(5) << s.humidity << "% "
                 << "║ " << setw(5) << s.smokeLevel
                 << " ║    " << setw(5) << s.windSpeed << "   "
                 << "║  " << (s.isValid   ? "YES" : "NO ")
                 << "  ║   " << (s.isAnomaly ? "YES" : "NO ")
                 << "   ║" << endl;
        }
    }

    cout << "╚═══╩══════╩════════╩══════════╩═══════╩═══════════╩═══════╩═════════╝" << endl;
    cout << "  Total readings: " << streamCount << " / " << MAX_READINGS << endl;
}

// Prints A3 static forest grid as a labelled 5x5 matrix.
// Time Complexity: O(n*m)
void ArrayLayer::displayGrid() const
{
    cout << "\n[A3] Static Forest Grid (Baseline Temperature Map)" << endl;
    cout << "     ";
    for (int c = 0; c < COLS; c++)
        cout << "  C" << c << "  ";
    cout << endl;
    cout << "     ";
    for (int c = 0; c < COLS; c++)
        cout << "-------";
    cout << endl;

    for (int r = 0; r < ROWS; r++)
    {
        cout << "  R" << r << " |";
        for (int c = 0; c < COLS; c++)
            cout << " " << fixed << setprecision(1) << setw(5) << grid[r][c] << " ";
        cout << "|" << endl;
    }

    cout << "     ";
    for (int c = 0; c < COLS; c++)
        cout << "-------";
    cout << "\n  (Zone ID = R*" << COLS << "+C,  e.g. R2C3 = Zone " << 2*COLS+3 << ")" << endl;
}

// Prints A4 dynamic terrain as a labelled 5x5 matrix.
// Shows live updated temperatures from sensor readings.
// Time Complexity: O(n*m)
void ArrayLayer::displayTerrain() const
{
    cout << "\n[A4] Dynamic Terrain Matrix (Live Temperature Values)" << endl;
    cout << "     ";
    for (int c = 0; c < COLS; c++)
        cout << "  C" << c << "  ";
    cout << endl;
    cout << "     ";
    for (int c = 0; c < COLS; c++)
        cout << "-------";
    cout << endl;

    for (int r = 0; r < ROWS; r++)
    {
        cout << "  R" << r << " |";
        for (int c = 0; c < COLS; c++)
        {
            // Highlight cells that differ from static grid by more than DELTA
            float diff = fabs(terrain[r][c] - grid[r][c]);
            if (diff > DELTA)
                cout << " [" << fixed << setprecision(1) << setw(4) << terrain[r][c] << "]";
            else
                cout << "  " << fixed << setprecision(1) << setw(4) << terrain[r][c] << " ";
        }
        cout << "|" << endl;
    }

    cout << "     ";
    for (int c = 0; c < COLS; c++)
        cout << "-------";
    cout << "\n  [bracketed] = deviated from baseline by >" << DELTA << "C" << endl;
}

// Prints a summary dashboard of the current A2 stream status.
// Time Complexity: O(n)
void ArrayLayer::displayStreamStatus() const
{
    int validCount   = 0;
    int anomalyCount = 0;

    for (int i = 0; i < streamCount; i++)
    {
        if (sensorStream[i].isValid)   validCount++;
        if (sensorStream[i].isAnomaly) anomalyCount++;
    }

    cout << "\n╔══════════════════════════════════════╗" << endl;
    cout << "║     A2 — Stream Status Dashboard     ║" << endl;
    cout << "╠══════════════════════════════════════╣" << endl;
    cout << "║  Total readings : " << setw(4) << streamCount
         << " / " << MAX_READINGS << "           ║" << endl;
    cout << "║  Valid readings : " << setw(4) << validCount   << "                 ║" << endl;
    cout << "║  Anomalies      : " << setw(4) << anomalyCount << "                 ║" << endl;
    cout << "║  Capacity left  : " << setw(4) << (MAX_READINGS - streamCount)
         << " slots              ║" << endl;

    if (streamCount > 0)
    {
        SensorReading last = sensorStream[streamCount - 1];
        cout << "╠══════════════════════════════════════╣" << endl;
        cout << "║  Latest Reading (Zone " << setw(2) << last.zoneId << "):           ║" << endl;
        cout << "║    Temp     : " << setw(6) << fixed << setprecision(1)
             << last.temperature << " C                ║" << endl;
        cout << "║    Humidity : " << setw(6) << last.humidity
             << " %                ║" << endl;
        cout << "║    Smoke    : " << setw(6) << last.smokeLevel
             << " units            ║" << endl;
    }

    cout << "╚══════════════════════════════════════╝" << endl;
}
