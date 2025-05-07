#include <gtest/gtest.h>
#include <Sensor.h>
#include <vector>

TEST(SensorTest, ConstructorSetsTypeCorrectly) {
  Sensor sensor(0, true, false, false);
  EXPECT_TRUE(sensor.getIfTemperatureSensor());
  EXPECT_FALSE(sensor.getIfVoltageSensor());
  EXPECT_FALSE(sensor.getIfPressureSensor()); 
}

TEST(SensorTest, CurrentValuesSetAndGetCorrectly) {
  Sensor sensor(1);
  // null value test before setting values
  EXPECT_EQ(sensor.getNullValue(), -999.0f);
  EXPECT_EQ(sensor.getCurrentTemperature(), sensor.getNullValue());

  sensor.setCurrentTemperature(25.5f);
  sensor.setCurrentVoltage(3.7f);
  sensor.setCurrentPressure(101.3f);
  sensor.setCurrentTime(10.0f);

  ASSERT_EQ(sensor.getSensorIdForTesting(), 1);
  EXPECT_FLOAT_EQ(sensor.getCurrentTemperature(), 25.5f);
  EXPECT_FLOAT_EQ(sensor.getCurrentVoltage(), 3.7f);
  EXPECT_FLOAT_EQ(sensor.getCurrentPressure(), 101.3f);
  EXPECT_FLOAT_EQ(sensor.getCurrentTime(), 10.0f);
}

TEST(SensorTest, LastValueSetAndGetCorrectly) { 
  Sensor sensor(2);
  sensor.setLastTemperature(20.0f);
  sensor.setLastVoltage(3.6f);
  sensor.setLastPressure(100.0f);
  sensor.setLastTime(5.0f);

  EXPECT_FLOAT_EQ(sensor.getLastTemperature(), 20.0f);
  EXPECT_FLOAT_EQ(sensor.getLastVoltage(), 3.6f);
  EXPECT_FLOAT_EQ(sensor.getLastPressure(), 100.0f);
  EXPECT_FLOAT_EQ(sensor.getLastTime(), 5.0f);
}

TEST(SensorTest, NeighbourIdsAreStoredCorrectly) {
  Sensor sensor(3);
  std::vector<short> neighbours = {1, 2, 4};
  sensor.setNeighbourIds(neighbours);
  ASSERT_EQ(neighbours, sensor.getNeighbourIds());
}

TEST(SensorTest, addSensorToVectorSavesNewSensor) {
  std::vector<Sensor> sensorList;
  ASSERT_EQ(sensorList.size(), 0);
  Sensor::addSensorToVector(sensorList, true, false, false);

  ASSERT_EQ(sensorList.size(), 1);
  EXPECT_EQ(sensorList[0].getSensorIdForTesting(), 0);
  EXPECT_TRUE(sensorList[0].getIfTemperatureSensor());
  EXPECT_FALSE(sensorList[0].getIfVoltageSensor());
  EXPECT_FALSE(sensorList[0].getIfPressureSensor());
}

// The main entry point for running the tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}