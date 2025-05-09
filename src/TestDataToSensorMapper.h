#ifndef DATAREADER_H
#define DATAREADER_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Sensor.h"
#include "Log.h"
#include "Calibration.h"

enum dataType {
    TEMPERATURE,
    VOLTAGE,
    PRESSURE,
};

class TestDataToSensorMapper{
public:
    bool ifMoreDataAvailable();
    void readTestDataFile(std::vector<Sensor>& allSensorsAddress);
    // specifically virtual for testing only
    virtual std::pair<short, dataType> setNextTestDataPointForSensorAndGetSensorInfo(std::vector<Sensor>& m_allSensors);

    void setAllDataForTesting(std::vector<std::vector<float>> data);
    std::vector<std::vector<float>>& getAllDataForTesting();
private:
    void updateIndexes(short currentSensorIndex, int totalSensorCount);
    void extractAndSaveTestDataFromFile(std::ifstream &dataFile);
    void updateTimeData(Sensor& currentSensor);
    void setTestDataTypeOrder(std::vector<Sensor>& allSensorsAddress);
    Log log;
    std::vector<std::vector<float>> m_allData;
    // data type order used to read the data file as intended
    std::vector<dataType> m_chosenDataTypeOrder;
    std::vector<dataType> m_dataTypeOrderWithoutPressureSensor = { TEMPERATURE, VOLTAGE };
    std::vector<dataType> m_dataTypeOrderWithPressureSensor = { PRESSURE, TEMPERATURE, VOLTAGE};

    // data file paths containing simulated sensor data (for testing)
    std::string m_dataFileAddress; 
    std::string m_dataFileNoPropagationNoPressureSensor = "../../../../src/no_propagation_data.csv";
    std::string m_dataFileNoPropagationWithPressureSensor = "../../../../src/no_propagation_with_pressure.csv";
    std::string m_dataFileBasedOnPropagationResearch = "../../../../src/propagation_data.csv";
    std::string m_dataFilePropagationWithPressureSensor = "../../../../src/propagation_with_pressure.csv";

    // indexes and checks used to update values for sensor instances,
    // one data point at a time. Be careful when tampering.
    unsigned int m_rowIndex = 0;
    unsigned int m_columnIndex = 1; // column index 0 is intended for time data
    unsigned int m_nextSensorIndex = 0;
    unsigned short m_dataTypeIndex = 0;
    bool m_moreDataAvailable = true;
    bool m_sensorAndDataMatch = false;
};

#endif
