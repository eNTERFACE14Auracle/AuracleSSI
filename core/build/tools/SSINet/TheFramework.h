#pragma once

using namespace System;

namespace ssi
{
	class IFilter;
}

namespace SSINet
{
	ref class Provider;
	ref class Sensor;
	ref class Consumer;

	interface class ISensor;
	interface class IConsumer;
	interface class ITransformer;
	interface class ITransformable;
	
	public ref class TheFramework
	{
		public:
			static Provider^ AddProvider();
			static Provider^ AddProvider(ssi::IFilter* pFilter);
			static Provider^ AddProvider(ssi::IFilter* pFilter, ssi_time_t buffer_capacity_in_seconds);
			static Provider^ AddProvider(ssi::IFilter* pFilter, ssi_time_t buffer_capacity_in_seconds, 
										 ssi_time_t check_interval_in_seconds);
			static Provider^ AddProvider(ssi::IFilter* pFilter, ssi_time_t buffer_capacity_in_seconds, 
										 ssi_time_t check_interval_in_seconds, ssi_time_t sync_interval_in_seconds);

			static Consumer^ AddConsumer(ITransformable^ source, IConsumer^ consumer,
										 String^ frame_size);
			

			static Sensor^ AddSensor(ISensor^ sensor);


			static void Start();
			static void Stop();
			static void Shutdown();
	};
}