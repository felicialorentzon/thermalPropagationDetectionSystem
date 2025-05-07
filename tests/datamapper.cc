#include "BMSMonitor.h"
#include "Log.h"
#include <gtest/gtest.h>
#include <TestDataToSensorMapper.h>
#include <Sensor.h>
#include <vector>
#include <string>

class DataToSensorMapperTest : public ::testing::Test {
protected:
  TestDataToSensorMapper dataMapperInstance;
  BMSMonitor bmsMonitorInstance;
  Calibration calibrationInstance;
};

TEST_F(DataToSensorMapperTest, StoredDataFromFileCorrectly){
  // NOTE: assumed sensorSetupBasedOnThesis is called in sensorSetup,
  // and file "../../../../src/propagation_data.csv" is read

// 1.0: data storage
  // 1.1: expected volume of data stored after file read
  calibrationInstance.sensorSetup(bmsMonitorInstance.getAllSensorsVectorAddress());
  std::vector<std::vector<float>>& allReadData = dataMapperInstance.getAllDataForTesting();
  ASSERT_EQ(allReadData.size(), 0);
  dataMapperInstance.readTestDataFile(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(allReadData.size(), 622);
  // consistant row length
  for(std::vector<float> row : allReadData){
    EXPECT_EQ(row.size(), 11);
  }

  // 1.2: expected row stored at expected indexes
  std::vector<float> firstRowOfData = {7.0f,28.2456665f,39.6547318f,33.3080101f,5.75564241f,16.2201233f,4.04426765f,4.08343458f,4.10609436f,4.13001347f,4.13113117f};
  ASSERT_EQ(allReadData[0], firstRowOfData);
  std::vector<float> randomRowOfData = {1198.0f,696.021780362319f,755.184504180416f,738.592667808691f,716.019982043418f,500.723384112426f,0.0f,0.0f,0.0f,0.0f,0.0f};
  ASSERT_EQ(allReadData[333], randomRowOfData);
  std::vector<float> lastRowOfData = {2495.0f,518.0712833683f,567.253701755586f,553.580678326967f,484.645146846188f,327.733255417447f,0.0f,0.0f,0.0f,0.0f,0.0f};
  ASSERT_EQ(allReadData[621], lastRowOfData);
  
  // 1.3: expected value stored at expected indexes
  ASSERT_EQ(allReadData[5][4], 5.75564233865861f);
  ASSERT_EQ(allReadData[200][8], 0.0f);
  ASSERT_EQ(allReadData[600][2], 579.74746582306f);
}

TEST_F(DataToSensorMapperTest, DataMappedToSensorsCorrectly){
  // NOTE: assumed sensorSetupBasedOnThesis is called in sensorSetup,
  // and file "../../../../src/propagation_data.csv" is read

// setup - read data and create sensors (no pressure sensor)
  calibrationInstance.sensorSetup(bmsMonitorInstance.getAllSensorsVectorAddress());
  std::vector<std::vector<float>>& allReadData = dataMapperInstance.getAllDataForTesting();
  ASSERT_EQ(allReadData.size(), 0);
  dataMapperInstance.readTestDataFile(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(allReadData.size(), 622);

// 1.0: type order: TEMPERATURE, VOLTAGE. Sensors created in opposite order.
//      Map right value to right sensor. 

// 1.1: first row mapping
  // 1st temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId0, dataType0] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId0, 5);
  ASSERT_EQ(dataType0, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[5].getCurrentTemperature(), 28.245667f);

  // 2nd temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId1, dataType1] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId1, 6);
  ASSERT_EQ(dataType1, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[6].getCurrentTemperature(), 39.654732f);

  // 3rd temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId2, dataType2] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId2, 7);
  ASSERT_EQ(dataType2, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[7].getCurrentTemperature(), 33.30801f);

  // 4th temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId3, dataType3] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId3, 8);
  ASSERT_EQ(dataType3, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[8].getCurrentTemperature(), 5.7556424f);

  // 5th temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId4, dataType4] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId4, 9);
  ASSERT_EQ(dataType4, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[9].getCurrentTemperature(), 16.220123f);

  // 1st voltage sensor get its data
  auto [sensorId5, dataType5] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId5, 0);
  ASSERT_EQ(dataType5, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[0].getCurrentVoltage(), 4.0442677f);

  // 2nd voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId6, dataType6] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId6, 1);
  ASSERT_EQ(dataType6, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[1].getCurrentVoltage(), 4.0834346f);

  // 3rd voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId7, dataType7] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId7, 2);
  ASSERT_EQ(dataType7, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[2].getCurrentVoltage(), 4.1060944f);

  // 4th voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId8, dataType8] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId8, 3);
  ASSERT_EQ(dataType8, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[3].getCurrentVoltage(), 4.1300133f);

  // 5th voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId9, dataType9] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId9, 4);
  ASSERT_EQ(dataType9, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[4].getCurrentVoltage(), 4.1311309f);


// 1.2: second row mapping
  // 1st temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId00, dataType00] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId00, 5);
  ASSERT_EQ(dataType00, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[5].getCurrentTemperature(), 28.2456656687872f);

  // 2nd temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId11, dataType11] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId11, 6);
  ASSERT_EQ(dataType11, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[6].getCurrentTemperature(), 39.6547324278708f);

  // 3rd temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId22, dataType22] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId22, 7);
  ASSERT_EQ(dataType22, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[7].getCurrentTemperature(), 33.3080097000292f);

  // 4th temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId33, dataType33] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId33, 8);
  ASSERT_EQ(dataType33, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[8].getCurrentTemperature(), 5.75564233865861f);

  // 5th temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId44, dataType44] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId44, 9);
  ASSERT_EQ(dataType44, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[9].getCurrentTemperature(), 16.2508511165831f);

  // 1st voltage sensor get its data
  auto [sensorId55, dataType55] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId55, 0);
  ASSERT_EQ(dataType55, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[0].getCurrentVoltage(), 4.04523122380265f);

  // 2nd voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId66, dataType66] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId66, 1);
  ASSERT_EQ(dataType66, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[1].getCurrentVoltage(), 4.04537252738112f);

  // 3rd voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId77, dataType77] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId77, 2);
  ASSERT_EQ(dataType77, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[2].getCurrentVoltage(), 4.05922027807097f);

  // 4th voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId88, dataType88] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId88, 3);
  ASSERT_EQ(dataType88, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[3].getCurrentVoltage(), 4.12342348581485f);

  // 5th voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId99, dataType99] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId99, 4);
  ASSERT_EQ(dataType99, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[4].getCurrentVoltage(), 4.12831772794178f);
}

TEST_F(DataToSensorMapperTest, DataAvailabilityTest){
  // setup - read data and create sensors (no pressure sensor)
  calibrationInstance.sensorSetup(bmsMonitorInstance.getAllSensorsVectorAddress());
  std::vector<std::vector<float>>& allReadData = dataMapperInstance.getAllDataForTesting();
  ASSERT_EQ(allReadData.size(), 0);
  dataMapperInstance.readTestDataFile(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(allReadData.size(), 622);

// 1.1: data available before mapping
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());

// 1.2: data available while reading the last row
  // skip to the last row
  short sensorIdTracker = -1; dataType datatypeTracker;
  while(bmsMonitorInstance.getAllSensorsVectorAddress()[sensorIdTracker].getCurrentTime() != 2495.0f){
    auto [sensorId, dataType] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
    sensorIdTracker = sensorId;
    datatypeTracker = dataType;
  }

  // ensure the last row of data is assigned as expected
  // 1rd temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  ASSERT_EQ(sensorIdTracker, 5);
  ASSERT_EQ(datatypeTracker, 0);
  ASSERT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[sensorIdTracker].getCurrentTemperature(), 518.0712833683f);
  
  // 2rd temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId11, dataType11] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId11, 6);
  ASSERT_EQ(dataType11, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[6].getCurrentTemperature(), 567.253701755586f);

  // 3rd temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId22, dataType22] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId22, 7);
  ASSERT_EQ(dataType22, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[7].getCurrentTemperature(), 553.580678326967f);

  // 4th temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId33, dataType33] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId33, 8);
  ASSERT_EQ(dataType33, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[8].getCurrentTemperature(), 484.645146846188f);

  // 5th temp sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId44, dataType44] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId44, 9);
  ASSERT_EQ(dataType44, 0);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[9].getCurrentTemperature(), 327.733255417447f);

  // 1st voltage sensor get its data
  auto [sensorId55, dataType55] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId55, 0);
  ASSERT_EQ(dataType55, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[0].getCurrentVoltage(), 0.0f);

  // 2nd voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId66, dataType66] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId66, 1);
  ASSERT_EQ(dataType66, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[1].getCurrentVoltage(), 0.0f);

  // 3rd voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId77, dataType77] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId77, 2);
  ASSERT_EQ(dataType77, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[2].getCurrentVoltage(), 0.0f);

  // 4th voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId88, dataType88] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId88, 3);
  ASSERT_EQ(dataType88, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[3].getCurrentVoltage(), 0.0f);

  // 5th voltage sensor get its data
  ASSERT_TRUE(dataMapperInstance.ifMoreDataAvailable());
  auto [sensorId99, dataType99] = dataMapperInstance.setNextTestDataPointForSensorAndGetSensorInfo(bmsMonitorInstance.getAllSensorsVectorAddress());
  ASSERT_EQ(sensorId99, 4);
  ASSERT_EQ(dataType99, 1);
  ASSERT_FLOAT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[4].getCurrentVoltage(), 0.0f);
  
// 1.3: no more data available after mapping the last row of data
  ASSERT_FALSE(dataMapperInstance.ifMoreDataAvailable());
}

// The main entry point for running the tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}