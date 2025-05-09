#include "BMSMonitor.h"
#include "Log.h"
#include "TestDataToSensorMapper.h"
#include "Calibration.h"

Log m_logInstance;
BMSMonitor m_BMSMonitorInstance;
Calibration m_calibrationInstance;
TestDataToSensorMapper m_dataToSensorMapperInstance;
bool m_propagationDetected = false;
// for testing 
bool m_firstTimePropagationWasDetected = true;
bool m_testingEnvironment = true;

void SimulationPreparation(){
    std::vector<Sensor>& allSensorsVectorAddress = m_BMSMonitorInstance.getAllSensorsVectorAddress();
    // set up the sensors and get if a pressure sensor was created
    bool ifPressureSensorExist = m_calibrationInstance.sensorSetup(allSensorsVectorAddress);
    m_BMSMonitorInstance.setIfPressureSensorExist(ifPressureSensorExist);
    // extract and save data from a a file
    m_dataToSensorMapperInstance.readTestDataFile(allSensorsVectorAddress);
}

void PropagationStatusOutput(){
    bool propagationDetected = m_BMSMonitorInstance.evaluateNewTestData(m_calibrationInstance, m_dataToSensorMapperInstance);
    // limited logging during testing for visibility
    if(propagationDetected){
        // Propagation detected. Output: true
        float lastTimePropagationWasDetected = m_BMSMonitorInstance.getLatestPropagationTimeForTesting();
        if(m_testingEnvironment){
            if(m_firstTimePropagationWasDetected){
                m_logInstance.logger(LogLevel::ERROR, "Propagation detected in main() at time " + std::to_string(lastTimePropagationWasDetected));
                m_firstTimePropagationWasDetected = false;
            }
        }
        else{
            m_logInstance.logger(LogLevel::ERROR, "Propagation detected in main() at time " + std::to_string(lastTimePropagationWasDetected));
        }
    }
    else{
        // No propagation detected. Output: false
        if(!m_testingEnvironment){
            m_logInstance.logger(LogLevel::INFO, "Propagation not detected in main()");
        }
    }
}

int main() {
    SimulationPreparation();
    // Evaluate new data points (one by one) mapped to sensor instances, 
    // in search of a propagation pattern
    while(m_dataToSensorMapperInstance.ifMoreDataAvailable()){
        PropagationStatusOutput();
    }
    return 0;
}