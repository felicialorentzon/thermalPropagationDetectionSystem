#ifndef SENSOR_H
#define SENSOR_H

#include "Log.h"
#include <vector>

class Sensor {
public:
	Sensor(short id = -1, bool isTemperatureSensor = false, bool isVoltageSensor = false, bool isPressureSensor = false);
	static void addSensorToVector(std::vector<Sensor>& allSensors,
        bool isTemperatureSensor, 
        bool isVoltageSensor, 
        bool isPressureSensor);
	void setNeighbourIds(std::vector<short> neighbourIds);
	std::vector<short> getNeighbourIds();
	
	void setCurrentTemperature(float temperature);
	void setCurrentVoltage(float voltage);
	void setCurrentPressure(float pressure);
	void setCurrentTime(float time);

	void setLastTemperature(float temperature);
	void setLastVoltage(float voltage);
	void setLastPressure(float pressure);
	void setLastTime(float time);

	bool getIfTemperatureSensor();
	bool getIfVoltageSensor();
	bool getIfPressureSensor();

	float getCurrentTemperature();
	float getCurrentVoltage();
	float getCurrentPressure();
	float getCurrentTime();

	float getLastTemperature();
	float getLastVoltage();
	float getLastPressure();
	float getLastTime();

	float getNullValue();
	int getSensorIdForTesting();
private:
	Log log;
	float m_nullValue = -999.0f;

	const int m_id;
	const bool m_isTempratureSensor = false;
	const bool m_isVoltageSensor = false;
	const bool m_isPressureSensor = false;

	float m_currentTemperature = m_nullValue;
	float m_currentVoltage = m_nullValue;
	float m_currentPressure = m_nullValue;
	float m_currentTime = m_nullValue;

	float m_lastTemperature;
	float m_lastVoltage;
	float m_lastPressure;
	float m_lastTime;

	std::vector<short> m_neighbourIds;
};

#endif