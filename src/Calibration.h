#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "Sensor.h"

class Calibration {
public:
    bool sensorSetup(std::vector<Sensor>& allSensors);
    float getTemperatureThreshold();
    float getVoltageThreshold();
    float getPressureThreshold();
    float getMaxTimeBetweenRelatedIncidents();
    bool getIfPressureSensorExist(std::vector<Sensor>& allSensors);

    void setThresholdsForTesting(float temperature, float voltage, float pressure, float timeDelta);
private:
    void sensorSetupBasedOnResearch(std::vector<Sensor>& allSensors);
    Log log;
    
    // Average rate of change thresholds
    float m_temperatureARoCThreshold = 3.0f;
    float m_voltageARoCThreshold = 0.01f;
    float m_pressureARoCThreshold = 3.0f;
    float m_maxARoCTimeBetweenRelatedIncidents = 60.0f;
};

#endif
