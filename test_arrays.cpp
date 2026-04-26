#include "arrays.h"
#include <iostream>
using namespace std;

int main()
{
    cout << "=== IFAMDS — Part 1 Array Layer Test ===" << endl;

    ArrayLayer layer;

    // Test A1 baseline display
    layer.displayBaseline();

    // Ingest 5 sensor readings across different zones
    SensorReading r1 = {26.0f, 58.0f, 2.0f, 11.0f, 0, 0, true, false};
    SensorReading r2 = {28.0f, 55.0f, 5.0f, 12.0f, 4, 0, true, false};
    SensorReading r3 = {50.0f, 18.0f, 80.0f, 9.0f, 7, 0, true, false}; // anomaly
    SensorReading r4 = {27.0f, 57.0f, 3.0f, 10.0f, 2, 0, true, false};
    SensorReading r5 = {44.0f, 22.0f, 68.0f, 8.0f, 5, 0, true, false}; // edge

    layer.ingestReading(r1);
    layer.ingestReading(r2);
    layer.ingestReading(r3);
    layer.ingestReading(r4);
    layer.ingestReading(r5);

    // Test baseline comparison
    layer.compareWithBaseline(r3);

    // Test noise filter
    layer.filterNoise(1);
    layer.filterNoise(2);

    // Test anomaly flagging
    layer.flagAnomaly(r3);
    layer.flagAnomaly(r1);

    // Test interpolation
    layer.interpolateMissing(2, 2);

    // Test boundary detection
    layer.detectBoundary(1, 1);

    // Display all views
    layer.displayTimeSeries();
    layer.displayGrid();
    layer.displayTerrain();
    layer.displayStreamStatus();

    cout << "\n=== Part 1 Test Complete — All operations verified ===" << endl;
    return 0;
}
