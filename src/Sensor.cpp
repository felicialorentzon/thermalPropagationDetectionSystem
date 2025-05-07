#include "Sensor.h"

Sensor::Sensor(short id, bool isTemperatureSensor, bool isVoltageSensor, bool isPressureSensor) 
				: m_id(id), m_isTempratureSensor(isTemperatureSensor), m_isVoltageSensor(isVoltageSensor), m_isPressureSensor(isPressureSensor){
	if (id >= 0){ // if considered sensor and not simply an instance
		log.logger(LogLevel::INFO, "Sensor " + std::to_string(id) + " meassure: Temperature: " + std::to_string(m_isTempratureSensor) + ", Voltage: " + std::to_string(m_isVoltageSensor) + ", Pressure: " + std::to_string(m_isPressureSensor));
	}
}

void Sensor::addSensorToVector(std::vector<Sensor>& allSensors, bool isTemperatureSensor, bool isVoltageSensor, bool isPressureSensor) {
    short nextAvailableId = static_cast<short>(allSensors.size());
    Sensor newSensor(nextAvailableId, isTemperatureSensor, isVoltageSensor, isPressureSensor);
    allSensors.push_back(newSensor);
}

void Sensor::setNeighbourIds(std::vector<short> neighbourIds) {
	for (int id : neighbourIds) {
		m_neighbourIds.push_back(id);
	}
}

void Sensor::setCurrentTemperature(float temperature){
	m_currentTemperature = temperature;
}

void Sensor::setCurrentVoltage(float voltage){
	m_currentVoltage = voltage;
}

void Sensor::setCurrentPressure(float pressure){
	m_currentPressure = pressure;
}

void Sensor::setCurrentTime(float time){
	m_currentTime = time;
}

void Sensor::setLastTemperature(float temperature) {
	m_lastTemperature = temperature;
}

void Sensor::setLastVoltage(float voltage) {
	m_lastVoltage = voltage;
}

void Sensor::setLastPressure(float pressure) {
	m_lastPressure = pressure;
}

void Sensor::setLastTime(float time) {
	m_lastTime = time;
}

std::vector<short> Sensor::getNeighbourIds() {
	return m_neighbourIds;
}

float Sensor::getCurrentTemperature(){
	return m_currentTemperature;
}

float Sensor::getCurrentVoltage(){
	return m_currentVoltage;
}

float Sensor::getCurrentPressure(){
	return m_currentPressure;
}
float Sensor::getCurrentTime(){
	return m_currentTime;
}

float Sensor::getLastTemperature() {
	return m_lastTemperature;
}

float Sensor::getLastVoltage() {
	return m_lastVoltage;
}

float Sensor::getLastPressure() {
	return m_lastPressure;
}

float Sensor::getLastTime() {
	return m_lastTime;
}

bool Sensor::getIfTemperatureSensor() {
	return m_isTempratureSensor;
}

bool Sensor::getIfVoltageSensor() {
	return m_isVoltageSensor;
}

bool Sensor::getIfPressureSensor() {
	return m_isPressureSensor;
}

float Sensor::getNullValue(){
	return m_nullValue;
}

int Sensor::getSensorIdForTesting(){
	return m_id;
}