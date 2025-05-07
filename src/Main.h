#ifndef MAIN_H
#define MAIN_H

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
bool m_testingEnvironment = false;

#endif