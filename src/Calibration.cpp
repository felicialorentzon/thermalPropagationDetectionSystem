#include "Calibration.h"

bool Calibration::sensorSetup(std::vector<Sensor>& allSensors) {
    sensorSetupBasedOnResearch(allSensors);
    return getIfPressureSensorExist(allSensors);
}

void Calibration::sensorSetupBasedOnResearch(std::vector<Sensor>& allSensors){
    Sensor sensorInstance;
    bool pressureSetupTest = true;

    // voltage sensors setup
    sensorInstance.addSensorToVector(allSensors, false, true, false); // id = 0. U_Cell 1
    sensorInstance.addSensorToVector(allSensors, false, true, false); // id = 1. U_Cell 2
    sensorInstance.addSensorToVector(allSensors, false, true, false); // id = 2. U_Cell 3
    sensorInstance.addSensorToVector(allSensors, false, true, false); // id = 3. U_Cell 4
    sensorInstance.addSensorToVector(allSensors, false, true, false); // id = 4. U_Cell 5

    if(pressureSetupTest == true){
        // pressure sensor setup
        sensorInstance.addSensorToVector(allSensors, false, false, true); // id = 5. T_Cell 1_2
        // temperature sensors setup
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 6. T_Cell 2_3
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 7. T_Cell 3_4
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 8. T_Cell 4_5
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 9. T_Cell 5_Insulation

        // neighbour mapping - ajacent sensors + pressure sensor
        // map neighbours for voltage sensors 
        allSensors[0].setNeighbourIds({ 1, 5 });
        allSensors[1].setNeighbourIds({ 0, 2, 5, 6 });
        allSensors[2].setNeighbourIds({ 1, 3, 5, 6, 7 });
        allSensors[3].setNeighbourIds({ 2, 4, 5, 7, 8 });
        allSensors[4].setNeighbourIds({ 3, 5, 8, 9 });
        // map neighbours for temperature sensors
        allSensors[5].setNeighbourIds({ 0, 1, 5, 6 });
        allSensors[6].setNeighbourIds({ 1, 2, 5, 7 });
        allSensors[7].setNeighbourIds({ 2, 3, 5, 6, 8 });
        allSensors[8].setNeighbourIds({ 3, 4, 5, 7, 9 });
        allSensors[9].setNeighbourIds({ 4, 5, 8 });
    }
    else{
        // temperature sensors setup
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 5. T_Cell 1_2
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 6. T_Cell 2_3
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 7. T_Cell 3_4
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 8. T_Cell 4_5
        sensorInstance.addSensorToVector(allSensors, true, false, false); // id = 9. T_Cell 5_Insulation

        // neighbour mapping - ajacent sensors
        // map neighbours for voltage sensors 
        allSensors[0].setNeighbourIds({ 1, 5 });
        allSensors[1].setNeighbourIds({ 0, 2, 5, 6 });
        allSensors[2].setNeighbourIds({ 1, 3, 6, 7 });
        allSensors[3].setNeighbourIds({ 2, 4, 7, 8 });
        allSensors[4].setNeighbourIds({ 3, 8, 9 });
        // map neighbours for temperature sensors
        allSensors[5].setNeighbourIds({ 0, 1, 6 });
        allSensors[6].setNeighbourIds({ 1, 2, 5, 7 });
        allSensors[7].setNeighbourIds({ 2, 3, 6, 8 });
        allSensors[8].setNeighbourIds({ 3, 4, 7, 9 });
        allSensors[9].setNeighbourIds({ 4, 8 });
    }
    
}

bool Calibration::getIfPressureSensorExist(std::vector<Sensor>& allSensors){
    for (Sensor& currentSensor : allSensors){
        if(currentSensor.getIfPressureSensor()){
            return true;
        }
    }
    return false;
}

float Calibration::getTemperatureThreshold() {
    return m_temperatureARoCThreshold;
}

float Calibration::getVoltageThreshold() {
    return m_voltageARoCThreshold;
}

float Calibration::getPressureThreshold() {
    return m_pressureARoCThreshold;
}

float Calibration::getMaxTimeBetweenRelatedIncidents(){
    return m_maxARoCTimeBetweenRelatedIncidents;
}

void Calibration::setThresholdsForTesting(float temperature, float voltage, float pressure, float timeDelta) {
    m_temperatureARoCThreshold = temperature;
    m_voltageARoCThreshold = voltage;
    m_pressureARoCThreshold = pressure;
    m_maxARoCTimeBetweenRelatedIncidents = timeDelta;
}
