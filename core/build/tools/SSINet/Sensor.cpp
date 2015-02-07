#include "stdafx.h"
#include "Sensor.h"
#include <base/ISensor.h>
#include <frame/Sensor.h>
#include "Util.h"
#include "Provider.h"

using namespace SSINet;

ISensorWrapper::ISensorWrapper(ssi::ISensor* pSensor)
{
	m_pSensor = pSensor;
	m_Object = gcnew ObjectWrapper(pSensor);
}

inline ssi_size_t ISensorWrapper::getChannelSize()
{
	return m_pSensor->getChannelSize();
}

inline bool ISensorWrapper::connect()
{
	return m_pSensor->connect();
}

inline bool ISensorWrapper::disconnect()
{
	return m_pSensor->disconnect();
}

inline bool ISensorWrapper::setProvider(System::String ^name, SSINet::IProvider ^provider)
{
	return m_pSensor->setProvider(Util::SSIString(name), provider);
}

//Sensor
Sensor::Sensor(ssi::IRunnable *pSensor)
{
	m_pSensor = pSensor;
}

inline bool Sensor::start()
{
	return m_pSensor->start();
}

inline bool Sensor::stop()
{
	return m_pSensor->stop();
}