#include <gtest/gtest.h>
#include <Calibration.h>
#include <Sensor.h>
#include <vector>

TEST(CalibrationTest, ThresholdSetAndGetCorrectly) {
  Calibration calibration;

  calibration.setThresholdsForTesting(10.5f, 4.2f, 8.1f, 60.0f);

  EXPECT_EQ(calibration.getTemperatureThreshold(), 10.5f);
  EXPECT_EQ(calibration.getVoltageThreshold(), 4.2f);
  EXPECT_EQ(calibration.getPressureThreshold(), 8.1f);
  EXPECT_EQ(calibration.getMaxTimeBetweenRelatedIncidents(), 60.0f);
}

TEST(CalibrationTest, SensorSetupDetectsPressureSensor) {
  Calibration calibration;
  std::vector<Sensor> sensors;

  // 1.1: pressure sensor does not exist.
  // test based on function "sensorSetupBasedOnThesis(allSensors)"  
  bool pressureSensorExists = calibration.sensorSetup(sensors);
  EXPECT_FALSE(pressureSensorExists);
  ASSERT_EQ(sensors.size(), 10);
  for (size_t i = 0; i < sensors.size(); i++) {
    EXPECT_FALSE(sensors[i].getIfPressureSensor());
  }

  // 1.2: pressure sensor does exist
  sensors.clear();
  ASSERT_EQ(sensors.size(), 0);
  // modified sensor setup (in order): temp, voltage, pressure, voltage, temp.
  Sensor sensorInstance;
  sensorInstance.addSensorToVector(sensors, true,false, false); // temp
  sensorInstance.addSensorToVector(sensors, false, true, false); // voltage
  sensorInstance.addSensorToVector(sensors, false, false, true); // pressure
  sensorInstance.addSensorToVector(sensors, false, true, false); // voltage
  sensorInstance.addSensorToVector(sensors, true,false, false); // temp
  ASSERT_EQ(sensors.size(), 5);
  pressureSensorExists = calibration.getIfPressureSensorExist(sensors);
  EXPECT_TRUE(pressureSensorExists);
}

TEST(CalibrationTest, SensorSetupCorrectNeighbourMapping) {
  Calibration calibration;
  std::vector<Sensor> sensors;

  // test based on function "sensorSetupBasedOnThesis(allSensors)"
  calibration.sensorSetup(sensors);
  ASSERT_EQ(sensors.size(), 10);
  std::vector<short> expectedNeighboursSensor1 = {0, 2, 5, 6};
  EXPECT_EQ(sensors[1].getNeighbourIds(), expectedNeighboursSensor1);
  std::vector<short> expectedNeighboursSensor9 = {4, 8};
  EXPECT_EQ(sensors[9].getNeighbourIds(), expectedNeighboursSensor9);
}

// The main entry point for running the tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}