#include "StdAfx.h"
#include "TheFramework.h"
#include "frame/TheFramework.h"
#include "Provider.h"
#include "Consumer.h"
#include "Transformer.h"
#include "Sensor.h"
#include <frame/Sensor.h>
#include "Util.h"

using namespace SSINet;

Provider^ TheFramework::AddProvider()
{
	return gcnew Provider(ssi::TheFramework::Instance()->AddProvider());
}

Provider^ TheFramework::AddProvider(ssi::IFilter* pFilter)
{
	return gcnew Provider(ssi::TheFramework::Instance()->AddProvider(pFilter));
}

Provider^ TheFramework::AddProvider(ssi::IFilter* pFilter, ssi_time_t buffer_capacity_in_seconds)
{
	return gcnew Provider(ssi::TheFramework::Instance()->AddProvider(pFilter, buffer_capacity_in_seconds));
}

Provider^ TheFramework::AddProvider(ssi::IFilter* pFilter, ssi_time_t buffer_capacity_in_seconds, 
							 ssi_time_t check_interval_in_seconds)
{
	return gcnew Provider(ssi::TheFramework::Instance()->AddProvider(pFilter, buffer_capacity_in_seconds, check_interval_in_seconds));
}

Provider^ TheFramework::AddProvider(ssi::IFilter* pFilter, ssi_time_t buffer_capacity_in_seconds, 
							 ssi_time_t check_interval_in_seconds, ssi_time_t sync_interval_in_seconds)
{
	return gcnew Provider(ssi::TheFramework::Instance()->AddProvider(pFilter, buffer_capacity_in_seconds, check_interval_in_seconds,
		sync_interval_in_seconds));
}

Consumer^ TheFramework::AddConsumer(ITransformable^ source, IConsumer^ consumer, String^ frame_size)
{
	Util::SSIString frame(frame_size);
	
	ssi::Consumer* pConsumer = ssi::TheFramework::Instance()->AddConsumer(source, consumer, frame);

	if(NULL == pConsumer)
		throw gcnew System::Exception("Error creating consumer");

	return gcnew Consumer(pConsumer);
}																					   

Sensor^ TheFramework::AddSensor(ISensor^ pSensor)
{
	return gcnew Sensor(ssi::TheFramework::Instance()->AddSensor(pSensor));
}

inline void TheFramework::Start()
{
	ssi::TheFramework::Instance()->Start();
}

inline void TheFramework::Stop()
{
	ssi::TheFramework::Instance()->Stop();
}

inline void TheFramework::Shutdown()
{
	ssi::TheFramework::Instance()->Shutdown();
}
