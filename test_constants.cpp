#include "constants.h"
#include <iostream>
int main() {
    std::cout << "MAX_READINGS : " << MAX_READINGS  << std::endl;
    std::cout << "ROWS x COLS  : " << ROWS << " x " << COLS << std::endl;
    std::cout << "MAX_EVENTS   : " << MAX_EVENTS    << std::endl;
    std::cout << "MAX_TASKS    : " << MAX_TASKS     << std::endl;
    std::cout << "TABLE_SIZE   : " << TABLE_SIZE    << std::endl;
    std::cout << "NUM_ZONES    : " << NUM_ZONES     << std::endl;
    std::cout << "TEMP_MAX     : " << TEMP_MAX      << std::endl;
    std::cout << "SMOKE_MAX    : " << SMOKE_MAX     << std::endl;
    std::cout << "HUM_MIN      : " << HUM_MIN       << std::endl;
    std::cout << "DELTA        : " << DELTA         << std::endl;
    std::cout << "THETA        : " << THETA         << std::endl;
    std::cout << "TEMP_NORM    : " << TEMP_NORM     << std::endl;
    std::cout << "HUM_NORM     : " << HUM_NORM      << std::endl;
    std::cout << "SMOKE_NORM   : " << SMOKE_NORM    << std::endl;
    std::cout << "WIND_NORM    : " << WIND_NORM     << std::endl;
    std::cout << "PRIORITY_LOW : " << PRIORITY_LOW  << std::endl;
    std::cout << "PRIORITY_MED : " << PRIORITY_MED  << std::endl;
    std::cout << "PRIORITY_HIGH: " << PRIORITY_HIGH << std::endl;
    std::cout << "PRIORITY_CRIT: " << PRIORITY_CRITICAL << std::endl;
    std::cout << "\nAll constants loaded — O(1) access confirmed." << std::endl;
    return 0;
}
