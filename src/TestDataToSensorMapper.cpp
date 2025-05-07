#include "TestDataToSensorMapper.h"

void TestDataToSensorMapper::readTestDataFile(std::vector<Sensor>& allSensorsAddress){
    m_dataFileAddress = m_dataFileBasedOnPropagationResearch;
    std::ifstream dataFile(m_dataFileAddress);
    
    if(dataFile.is_open()){
        extractAndSaveTestDataFromFile(dataFile);
        dataFile.close();
        log.logger(LogLevel::INFO, "Data read from file " + m_dataFileAddress + ". Rows, columns = " + std::to_string(m_allData.size()) + ", " + std::to_string(m_allData[0].size())); 
        
        // set in which order the sensor types shall be extracted from the data file,
        // assuming the columns of data are sorted by type. Using default alternatives here.
        setTestDataTypeOrder(allSensorsAddress);
    }
    else{
        log.logger(LogLevel::ERROR, "File could not be opened: " + m_dataFileAddress);
    }
}

void TestDataToSensorMapper::extractAndSaveTestDataFromFile(std::ifstream &dataFile) {
    std::string dataFileType = m_dataFileAddress.substr(m_dataFileAddress.size() - 4, m_dataFileAddress.size() - 1);
    std::vector<float> rowOfSegmentedData;
    std::string currentDataPoint;
    std::string rowOfData;

    // save data points --> rows --> m_allData
    while (std::getline(dataFile, rowOfData)){
        std::istringstream row(rowOfData);
        if(dataFileType == ".txt"){
            float value;
            while (row >> value){
                rowOfSegmentedData.push_back(value);
            }
        }
        else if(dataFileType == ".csv"){
            try{
                while(std::getline(row, currentDataPoint, ';')){
                    rowOfSegmentedData.push_back(stof(currentDataPoint));
                }
            }
            catch (std::invalid_argument){
                log.logger(LogLevel::DEBUG, currentDataPoint + " could not be converted to a float. Row of potential data not saved");
            }
        }
        else{
            log.logger(LogLevel::ERROR, "Unknown file type. (use .csv or .txt)");
        }

        // only save a row if it contains data points
        if(!rowOfSegmentedData.empty()){
            m_allData.push_back(rowOfSegmentedData);
            rowOfSegmentedData.clear();
        }
    }
}

void TestDataToSensorMapper::setTestDataTypeOrder(std::vector<Sensor>& allSensorsAddress){
    Calibration cal;
    if(cal.getIfPressureSensorExist(allSensorsAddress)){
        m_chosenDataTypeOrder = m_dataTypeOrderWithPressureSensor;
    }
    else{
        m_chosenDataTypeOrder = m_dataTypeOrderWithoutPressureSensor;
    }
}

std::pair<short, dataType> TestDataToSensorMapper::setNextTestDataPointForSensorAndGetSensorInfo(std::vector<Sensor>& allSensors) {
    if (!m_moreDataAvailable) {
        return {-1, TEMPERATURE};
    }

    dataType currentDataType = m_chosenDataTypeOrder[m_dataTypeIndex];
    float value = m_allData[m_rowIndex][m_columnIndex];

    // data columns are ordered by type, with time in the first column. For every type, for every row, 
    // attempt to match data points with sensors (in order). First sensor to match the data point type is assigned it.
    // return update identifiers: data type updated (in case of multipurpose sensors) in what sensor. 
    for (short i = m_nextSensorIndex; i < allSensors.size(); i++) {
        Sensor& currentSensor = allSensors[i];

        m_sensorAndDataMatch = false;

        // check for sensors of the current type. If there is a sensor match:
        // update the sensor's data. Then move on to the next column and sensor. 
        switch (currentDataType) {
        case TEMPERATURE:
            if (currentSensor.getIfTemperatureSensor()) {
                // if data stored from last time, update data
                if (currentSensor.getCurrentTemperature() != currentSensor.getNullValue()) {
                    currentSensor.setLastTemperature(currentSensor.getCurrentTemperature());
                }
                else{
                    currentSensor.setLastTemperature(value);
                }
                currentSensor.setCurrentTemperature(value);
                m_sensorAndDataMatch = true;
            }
            break;

        case VOLTAGE:
            if (currentSensor.getIfVoltageSensor()) {
                // if data stored from last time, update data
                if (currentSensor.getCurrentVoltage() != currentSensor.getNullValue()) {
                    currentSensor.setLastVoltage(currentSensor.getCurrentVoltage());
                }
                else{
                    currentSensor.setLastVoltage(value);
                }
                currentSensor.setCurrentVoltage(value);
                m_sensorAndDataMatch = true;
            }
            break;

        case PRESSURE:
            if (currentSensor.getIfPressureSensor()) {
                // if data stored from last time, update data
                if (currentSensor.getCurrentPressure() != currentSensor.getNullValue()) {
                    currentSensor.setLastPressure(currentSensor.getCurrentPressure());
                }
                else{
                    currentSensor.setLastPressure(value);
                }
                currentSensor.setCurrentPressure(value);
                m_sensorAndDataMatch = true;
            }
            break;
        }

        if (m_sensorAndDataMatch) {
            updateTimeData(allSensors[i]);
            updateIndexes(i, static_cast<int>(allSensors.size()));
            return {i, currentDataType};
        }
    }
    updateIndexes(-1, static_cast<int>(allSensors.size())); // pass -1 to indicate no sensor was used
    return {-1, currentDataType};
}

void TestDataToSensorMapper::updateTimeData(Sensor& currentSensor){
    float currentTime = m_allData[m_rowIndex][0];

    if (currentSensor.getCurrentTime() != currentSensor.getNullValue()){
        currentSensor.setLastTime(currentSensor.getCurrentTime());
    }
    else{
        currentSensor.setLastTime(currentTime);
    }
    currentSensor.setCurrentTime(currentTime);
}

void TestDataToSensorMapper::updateIndexes(short currentSensorIndex, int totalSensorCount){
    unsigned short lastColumnIndex = static_cast<short>(m_allData[m_rowIndex].size() - 1);
    
    // if exit with -1, move on
    if(currentSensorIndex == -1){
        m_nextSensorIndex = 0;
        m_dataTypeIndex++;
    }
    else{
        // set sensor index so that sensors that had already gotten data mapped to them this row
        // gets ignored next time data is supposed to be mapped
        if ((currentSensorIndex + 1) < totalSensorCount){
            m_nextSensorIndex = currentSensorIndex + 1;
        }
        else {
            // restart looping of sensors and look for the next value type
            m_nextSensorIndex = 0;
            m_dataTypeIndex++;
        }
    }

    // update column (and potentially row) index if there was a sensor and data match
    if (m_sensorAndDataMatch){
        if (m_columnIndex < lastColumnIndex){
            m_columnIndex++;
        }
        else{
            // start new row (the first column is reserved for time data)
            m_columnIndex = 1;
            m_rowIndex++;
            m_dataTypeIndex = 0;
        }
    }

    // if last value checked, start from the beginning
    if(m_dataTypeIndex >= m_chosenDataTypeOrder.size()){
        m_dataTypeIndex = 0;
    }

    // if last row read
    if(m_rowIndex >= m_allData.size()){
        log.logger(LogLevel::INFO, "All data read.");
        m_moreDataAvailable = false;
    }
}

bool TestDataToSensorMapper::ifMoreDataAvailable(){
    return m_moreDataAvailable;
}

void TestDataToSensorMapper::setAllDataForTesting(std::vector<std::vector<float>> data) {
    m_allData = data;
    m_moreDataAvailable = true;
    m_rowIndex = 0;
    m_columnIndex = 1;
    m_nextSensorIndex = 0;
    m_dataTypeIndex = 0;
}

std::vector<std::vector<float>>& TestDataToSensorMapper::getAllDataForTesting(){
    return m_allData;
}