#include <gtest/gtest.h>
#include <BMSMonitor.h>
#include <Calibration.h>
#include <Sensor.h>
#include <TestDataToSensorMapper.h>
#include <utility>
#include <vector>

class MockDataToSensorMapper : public TestDataToSensorMapper{
public:
  std::pair<short, dataType> setNextTestDataPointForSensorAndGetSensorInfo(std::vector<Sensor>& allSensors) override {
    return forcedSensorFocus;
  }
  void setNextSensorToEvaluate(short id, dataType type) {
    forcedSensorFocus = {id, type};
  }
private:
  std::pair<short, dataType> forcedSensorFocus = {-1, TEMPERATURE};
};

class BMSMonitorTest : public ::testing::Test {
protected:
  void SetUp() {
    // thresholds set and 2 sensors created (1 temp, 1 voltage)
    calibration.setThresholdsForTesting(5.0f, 0.5f, 0.2f, 10.0f);
    
    Sensor sensor1(0, true, false, false);
    sensor1.setNeighbourIds({1});
    sensor1.setLastTime(0.0f);
    sensor1.setLastTemperature(20.0f);
    sensor1.setCurrentTime(1.0f);
    sensor1.setCurrentTemperature(30.0f);
  
    Sensor sensor2(1, false, true, false);
    sensor2.setNeighbourIds({0});
    sensor2.setLastTime(0.0f);
    sensor2.setLastVoltage(3.0f);
    sensor2.setCurrentTime(1.0f);
    sensor2.setCurrentVoltage(3.1f);
  
    bmsMonitorInstance.getAllSensorsVectorAddress().push_back(sensor1);
    bmsMonitorInstance.getAllSensorsVectorAddress().push_back(sensor2);
  }
  BMSMonitor bmsMonitorInstance;
  Calibration calibration;
  MockDataToSensorMapper mapper;
};

TEST_F(BMSMonitorTest, CheckSetupValues) {
  // ensure right amount of sensors in vector before starting the test
  ASSERT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress().size(), 2);
  
// 1.0: default values in sensors checked for propagation
  // 1.1: temperature sensor default values checked for propagation
  ASSERT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[0].getCurrentTemperature(), 30.0f);
  ASSERT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[0].getLastTemperature(), 20.0f);
  mapper.setNextSensorToEvaluate(0, TEMPERATURE);
  ASSERT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 0);
  auto [propagationDetected11, timePropagationWasDetected11] = bmsMonitorInstance.evaluateNewTestData(calibration, mapper);
  // one incident != propagation
  ASSERT_FALSE(propagationDetected11);
  ASSERT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 1);
  
  // 1.2: voltage sensor default values checked for propagation
  ASSERT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[1].getCurrentVoltage(), 3.1f);
  ASSERT_EQ(bmsMonitorInstance.getAllSensorsVectorAddress()[1].getLastVoltage(), 3.0f);
  mapper.setNextSensorToEvaluate(1, VOLTAGE);
  auto [propagationDetected12, timePropagationWasDetected12] = bmsMonitorInstance.evaluateNewTestData(calibration, mapper);
  ASSERT_FALSE(propagationDetected12);
  ASSERT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 1);
}

TEST_F(BMSMonitorTest, readAndEvaluateNewSensorValues){

  // 1.1: temperature sensor gets a new value - no new incident nor propagation
  int incidentCounter = static_cast<int>(bmsMonitorInstance.getAllIncidentsForTesting().size());
  int idForSensorWithNewSavedData = 0;
  Sensor& tempSensor = bmsMonitorInstance.getAllSensorsVectorAddress()[idForSensorWithNewSavedData];
  mapper.setNextSensorToEvaluate(idForSensorWithNewSavedData, TEMPERATURE);
  tempSensor.setLastTemperature(tempSensor.getCurrentTemperature()); // 30.0f
  tempSensor.setCurrentTemperature(31.2f);
  // no new incidents --> no propagation
  auto [propagationDetected11, timePropagationWasDetected11] = bmsMonitorInstance.evaluateNewTestData(calibration, mapper);
  ASSERT_FALSE(propagationDetected11);
  ASSERT_EQ(incidentCounter, bmsMonitorInstance.getAllIncidentsForTesting().size());

  // 1.2: temperature sensor gets a new value - new incident, no propagation
  incidentCounter = static_cast<int>(bmsMonitorInstance.getAllIncidentsForTesting().size());
  mapper.setNextSensorToEvaluate(idForSensorWithNewSavedData, TEMPERATURE);
  tempSensor.setLastTemperature(tempSensor.getCurrentTemperature()); // 31.2f
  tempSensor.setCurrentTemperature(45.7f);
  // 1 new incident --> no propagation
  auto [propagationDetected12, timePropagationWasDetected12] = bmsMonitorInstance.evaluateNewTestData(calibration, mapper);
  ASSERT_FALSE(propagationDetected12);
  ASSERT_EQ(incidentCounter + 1, bmsMonitorInstance.getAllIncidentsForTesting().size());

  // 1.3: voltage sensor gets a new value - no incident nor propagation
  idForSensorWithNewSavedData = 1;
  Sensor& voltageSensor = bmsMonitorInstance.getAllSensorsVectorAddress()[idForSensorWithNewSavedData];
  incidentCounter = static_cast<int>(bmsMonitorInstance.getAllIncidentsForTesting().size());
  mapper.setNextSensorToEvaluate(idForSensorWithNewSavedData, VOLTAGE);
  voltageSensor.setLastVoltage(voltageSensor.getCurrentVoltage()); // 3.1f
  voltageSensor.setCurrentVoltage(3.0f);
  // no new incident --> no propagation
  auto [propagationDetected13, timePropagationWasDetected13] = bmsMonitorInstance.evaluateNewTestData(calibration, mapper);
  ASSERT_FALSE(propagationDetected13);
  ASSERT_EQ(incidentCounter, bmsMonitorInstance.getAllIncidentsForTesting().size());

  // 1.4: voltage sensor gets a new value - new incident and propagation
  incidentCounter = static_cast<int>(bmsMonitorInstance.getAllIncidentsForTesting().size());
  mapper.setNextSensorToEvaluate(idForSensorWithNewSavedData, VOLTAGE);
  voltageSensor.setLastVoltage(voltageSensor.getCurrentVoltage()); // 3.0f
  voltageSensor.setCurrentVoltage(2.0f);
  // 1 new incident --> propagation
  auto [propagationDetected14, timePropagationWasDetected14] = bmsMonitorInstance.evaluateNewTestData(calibration, mapper);
  ASSERT_TRUE(propagationDetected14);
  ASSERT_EQ(incidentCounter + 1, bmsMonitorInstance.getAllIncidentsForTesting().size());
}

TEST_F(BMSMonitorTest, RemoveUnrelatedIncidentsTest){
  float maxDeltaTime = 60.0f;
  calibration.setThresholdsForTesting(5.0f, 0.5f, 0.2f, maxDeltaTime);
  ASSERT_EQ(calibration.getMaxTimeBetweenRelatedIncidents(), maxDeltaTime);
  
// 1.0: No incident removal
  // 1.1: 1 incident in total --> 0 old incidents removed 
  bmsMonitorInstance.saveIncident(0, 50, dataType::TEMPERATURE, 5.6f);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 1);  
  bmsMonitorInstance.removeUnrelatedIncidents(calibration);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 1);  

  // 1.2: 2 incidents, both within acceptable timeframe --> 0 old incidents removed
  bmsMonitorInstance.saveIncident(1, 62.0f, dataType::TEMPERATURE, 9.0f);  
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 2);  
  bmsMonitorInstance.removeUnrelatedIncidents(calibration);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 2);  
  
// 2.0: Incident removal
  // 2.1: 3 incidents, were 1 just got too old (exactly maxDeltaTime difference) --> 1 old incident removed
  bmsMonitorInstance.saveIncident(2, 54.0f, dataType::PRESSURE, (5.6f + maxDeltaTime));
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 3);
  bmsMonitorInstance.removeUnrelatedIncidents(calibration);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 2);
  
  // 2.2: 3 incidents, were all previous incidents are too old  --> 2 old incidents removed
  bmsMonitorInstance.saveIncident(3, 73.0f, dataType::VOLTAGE, 5.6f + maxDeltaTime*3);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 3);  
  bmsMonitorInstance.removeUnrelatedIncidents(calibration);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 1);  
  
  // 2.3: 2 incidents, were 1 is too old  --> 1 old incident removed
  bmsMonitorInstance.saveIncident(0, 47.0f, dataType::TEMPERATURE, 5.6f + maxDeltaTime*4);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 2);  
  bmsMonitorInstance.removeUnrelatedIncidents(calibration);
  EXPECT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 1);  
}

TEST_F(BMSMonitorTest, SetIfPropagationDetectedTest){
  // Setup for tests
  bmsMonitorInstance.setIfPressureSensorExist(false);
  // vector with sensor(s) of the same type
  std::vector<Sensor> sensorSameTypeVector;
  Sensor sensorSame0(0, true, false, false);
  sensorSameTypeVector.push_back(sensorSame0);
  
  // vector with sensors of different types. No neighbour matching.
  std::vector<Sensor> sensorDifferentTypeVector;
  Sensor sensorDiff0(0, true, false, false);
  Sensor sensorDiff1(1, false, true, false);
  sensorDiff0.setNeighbourIds({3});
  sensorDiff1.setNeighbourIds({3});
  sensorDifferentTypeVector.push_back(sensorDiff0);
  sensorDifferentTypeVector.push_back(sensorDiff1);
  
  // ensure there are no unexpected incidents in vector before test begins
  ASSERT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 0);
  bmsMonitorInstance.saveIncident(0, 40.0f, TEMPERATURE, 1.0f);
  ASSERT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 1);
  
// 1.0: General tests
  // 1.1: less than 2 incident in vector
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorSameTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), false);
  
  // 1.2: sensors detect neighbours but incidents are of the same type
  bmsMonitorInstance.saveIncident(1, 40.0f, TEMPERATURE, 1.1f);
  ASSERT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 2);
  Sensor sensorSame1(1, true, false, false);
  Sensor sensorSame2(2, true, false, false);
  sensorSameTypeVector.push_back(sensorSame1);
  sensorSameTypeVector.push_back(sensorSame2);
  sensorSameTypeVector[0].setNeighbourIds({1,2});
  sensorSameTypeVector[1].setNeighbourIds({0,2});
  sensorSameTypeVector[2].setNeighbourIds({0,1});
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorSameTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), false);
  
  // 1.3: different types, no neighbour matches
  bmsMonitorInstance.saveIncident(1, 41.0f, VOLTAGE, 1.1f);
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorDifferentTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), false);
  
  // 2.1: no pressure sensor exist && neighbour detected of different type
  sensorDifferentTypeVector[0].setNeighbourIds({1});
  sensorDifferentTypeVector[1].setNeighbourIds({0});
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorDifferentTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), true);

  // 3.0: pressure sensor exist
  bmsMonitorInstance.setIfPressureSensorExist(true);
  // 3.1: 2 pressure incidents, matching neighbours
  bmsMonitorInstance.eraseAllIncidentsForTesting();
  ASSERT_EQ(bmsMonitorInstance.getAllIncidentsForTesting().size(), 0);
  Sensor sensorDiff2(2, false,false,true);
  Sensor sensorDiff3(3, false, false, true);
  sensorDiff2.setNeighbourIds({0, 1, 3});
  sensorDiff3.setNeighbourIds({2});
  sensorDifferentTypeVector.push_back(sensorDiff2);
  sensorDifferentTypeVector.push_back(sensorDiff3);
  bmsMonitorInstance.saveIncident(2, 40.0f, PRESSURE, 1.0f);
  bmsMonitorInstance.saveIncident(3, 41.0f, PRESSURE, 1.4f);
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorDifferentTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), false);

  // 3.2: 1 temperature and 1 voltage incident, neighbour match
  bmsMonitorInstance.eraseAllIncidentsForTesting();
  bmsMonitorInstance.saveIncident(0, 40.0f, TEMPERATURE, 1.0f);
  bmsMonitorInstance.saveIncident(1, 41.0f, VOLTAGE, 1.2f);
  sensorDifferentTypeVector[0].setNeighbourIds({1, 2});
  sensorDifferentTypeVector[1].setNeighbourIds({0, 2});
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorDifferentTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), false);

  // 3.3: 1 pressure and 1 temperature incident, neighbour match
  bmsMonitorInstance.eraseAllIncidentsForTesting();
  bmsMonitorInstance.saveIncident(0, 40.0f, TEMPERATURE, 1.0f);
  bmsMonitorInstance.saveIncident(2, 45.0f, PRESSURE, 1.1f);
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorDifferentTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), true);

  // 3.4: 1 pressure and 1 voltage incident, neighbour match
  bmsMonitorInstance.eraseAllIncidentsForTesting();
  bmsMonitorInstance.saveIncident(1, 40.0f, VOLTAGE, 1.0f);
  bmsMonitorInstance.saveIncident(2, 45.0f, PRESSURE, 1.1f);
  bmsMonitorInstance.setIfPropagationDetected(calibration, sensorDifferentTypeVector);
  EXPECT_EQ(bmsMonitorInstance.getIfPropagationDetectedForTesting(), true);
}

// The main entry point for running the tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}