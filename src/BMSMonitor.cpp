#include "BMSMonitor.h"

bool BMSMonitor::evaluateNewTestData(Calibration& calibrationInstance, TestDataToSensorMapper& dataMapperInstance){
    m_propagationDetected =  false;
    float absouteRateOfChange = 0.0f;
    auto [sensorId, dataType] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(m_allSensors);
    // evaluate sensor's absoute rate of change and look for propagation if it's beyond threshold
    if (sensorId >= 0){
        Sensor& currentSensor = m_allSensors[sensorId];
        float deltaTime = currentSensor.getCurrentTime() - currentSensor.getLastTime();
        float deltaValue;
        switch(dataType){
            case TEMPERATURE:
                deltaValue = currentSensor.getCurrentTemperature() - currentSensor.getLastTemperature();
                absouteRateOfChange = calculateAbsoluteRateOfChange(deltaValue, deltaTime);
                if (absouteRateOfChange >= calibrationInstance.getTemperatureThreshold()) {
                    handleNewIncident(calibrationInstance, sensorId, dataType, absouteRateOfChange);
                }
                break;
            case VOLTAGE:
                deltaValue = currentSensor.getCurrentVoltage() - currentSensor.getLastVoltage();
                absouteRateOfChange = calculateAbsoluteRateOfChange(deltaValue, deltaTime);
                if (absouteRateOfChange > calibrationInstance.getVoltageThreshold()) {
                    handleNewIncident(calibrationInstance, sensorId, dataType, absouteRateOfChange);
                }
                break;
            case PRESSURE:
                deltaValue = currentSensor.getCurrentPressure() - currentSensor.getLastPressure();
                absouteRateOfChange = calculateAbsoluteRateOfChange(deltaValue, deltaTime);
                if (absouteRateOfChange >= calibrationInstance.getPressureThreshold()) {
                    handleNewIncident(calibrationInstance, sensorId, dataType, absouteRateOfChange);
                }
                break;
        } 
    }
    return m_propagationDetected;
}

float BMSMonitor::calculateAbsoluteRateOfChange(float deltaValue, float deltaTime) {
    float delta = 0.0f;
    if(deltaTime > 0.0f){
        delta = deltaValue / deltaTime;
    }
    return abs(delta);
}

void BMSMonitor::handleNewIncident(Calibration& calibrationInstance, short sensorIndex, dataType type, float absoluteDelta){
    //send warning to terminal
    float logMessageLimiterForTesting = 210.0f;
    if(m_allSensors[sensorIndex].getCurrentTime() < logMessageLimiterForTesting){
        log.logger(LogLevel::WARNING, "Sensor " + std::to_string(sensorIndex) + " detected delta beyond acceptable limit: " + std::to_string(absoluteDelta) + " within " + std::to_string(logMessageLimiterForTesting) + " seconds of read data (at time " + std::to_string(m_allSensors[sensorIndex].getCurrentTime()) + ")");
    }

    // save and evaluate incident in relation to previous incidents
    saveIncident(sensorIndex, absoluteDelta, type);
    setIfPropagationDetected(calibrationInstance, m_allSensors);
}

void BMSMonitor::saveIncident(short sensorIndex, float absoluteDelta, dataType type, float time) {
    incident newIncident;
    newIncident.sensorId = sensorIndex;
    newIncident.deltaAboveLimit = absoluteDelta;
    newIncident.dataType = type;
    // Set time it occured. Default is leaving the variable "time" blank and reading from data file.
    if(time == -1.0f){
        newIncident.timeItOccured = m_allSensors[sensorIndex].getCurrentTime();
    }
    else{
        newIncident.timeItOccured = time;
    }
    m_allIncidents.push_back(newIncident);
}

void BMSMonitor::setIfPropagationDetected(Calibration& calibrationInstance, std::vector<Sensor>& m_allSensors) {
    removeUnrelatedIncidents(calibrationInstance);
    incident& latestIncident = m_allIncidents[m_allIncidents.size() - 1];
    // look for propagation: true if 1 neighbour of another matching sensor type had an incident recently
    if(m_allIncidents.size() > 1){
        std::vector<short> lastestIncidentNeighbours = m_allSensors[latestIncident.sensorId].getNeighbourIds();
        bool foundNeighbour;
        bool differentSensorTypes;
        
        for (incident& currentIncident : m_allIncidents){
            foundNeighbour = find(lastestIncidentNeighbours.begin(), lastestIncidentNeighbours.end(), currentIncident.sensorId) != lastestIncidentNeighbours.end();
            differentSensorTypes = latestIncident.dataType != currentIncident.dataType;
            if(foundNeighbour && differentSensorTypes){
                if(m_pressureSensorExist){
                    if(currentIncident.dataType == PRESSURE || latestIncident.dataType == PRESSURE){
                        m_propagationDetected = true;
                        m_lastTimePropagationWasDetected = m_allSensors[latestIncident.sensorId].getCurrentTime();
                        return;
                    }
                }
                else{
                    m_propagationDetected = true;
                    m_lastTimePropagationWasDetected = m_allSensors[latestIncident.sensorId].getCurrentTime();
                    return;
                }
            }
        }
    }
    m_propagationDetected = false;
}

void BMSMonitor::removeUnrelatedIncidents(Calibration& calibrationInstance) {
    incident& latestIncident = m_allIncidents[m_allIncidents.size() - 1];
    // if there are multiple incidents in the vector: check for old incidents to remove
    if(m_allIncidents.size() > 1){
        // compare the previous incidents with the newest incident
        for (int previousIncidentIndex = 0; previousIncidentIndex < m_allIncidents.size() - 1; previousIncidentIndex++){
            if ((latestIncident.timeItOccured - m_allIncidents[previousIncidentIndex].timeItOccured) < calibrationInstance.getMaxTimeBetweenRelatedIncidents()){
                // found an incident that is not too old to be compared with the newest incident.
                // remove incidents that occured before then, if there were any 
                if(previousIncidentIndex > 0){
                    m_allIncidents.erase(m_allIncidents.begin(), m_allIncidents.begin() + previousIncidentIndex);
                    return;
                }
                else {
                    // the oldest incident in the vector is recent enough to be comparable with the newest.
                    return;
                }
            }
        }
        // all previous incidents were too old to compare with the newest incident. Removing all old incidents. 
        m_allIncidents.erase(m_allIncidents.begin(), m_allIncidents.begin() + m_allIncidents.size() - 1);
    }
}

void BMSMonitor::setIfPressureSensorExist(bool state) {
    m_pressureSensorExist = state;
}

std::vector<Sensor>& BMSMonitor::getAllSensorsVectorAddress() {
    return m_allSensors;
}

float BMSMonitor::getLatestPropagationTimeForTesting(){
    return m_lastTimePropagationWasDetected;
}

std::vector<incident> BMSMonitor::getAllIncidentsForTesting(){
    return m_allIncidents;
}

bool BMSMonitor::getIfPropagationDetectedForTesting(){
    return m_propagationDetected;
}

void BMSMonitor::eraseAllIncidentsForTesting(){
    m_allIncidents.clear();
}