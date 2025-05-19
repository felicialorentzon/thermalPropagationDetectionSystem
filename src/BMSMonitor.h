#ifndef BMSMONITOR_H
#define BMSMONITOR_H

#include <vector>
#include "Log.h"
#include "Sensor.h"
#include "TestDataToSensorMapper.h"
#include "Calibration.h"

struct incident {
    short sensorId;
    dataType dataType;
    float deltaAboveLimit;
    float timeItOccured;
};

class BMSMonitor {
public:
    bool evaluateNewTestData(Calibration& calibration, TestDataToSensorMapper& dataMapper);
    void setIfPressureSensorExist(bool state);
    std::vector<Sensor>& getAllSensorsVectorAddress();
    void removeUnrelatedIncidents(Calibration& calibrationInstance);
    void saveIncident(short sensorIndex, float absoluteDelta, dataType type, float time=-1.0f);
    void setIfPropagationDetected(Calibration& calibrationInstance, std::vector<Sensor>& m_allSensors);
    
    std::vector<incident> getAllIncidentsForTesting();
    bool getIfPropagationDetectedForTesting();
    void eraseAllIncidentsForTesting();
    float getLatestPropagationTimeForTesting();
private:
    Log log;
    float calculateAbsoluteRateOfChange(float deltaValue, float deltaTime);
    void handleNewIncident(Calibration& calibrationInstance, short sensorIndex, dataType type, float absoluteDelta);
    
    std::vector<Sensor> m_allSensors;
    std::vector<incident> m_allIncidents;
    bool m_pressureSensorExist = false;
    bool m_propagationDetected = false;
    float m_lastTimePropagationWasDetected = -1;
};

#endif