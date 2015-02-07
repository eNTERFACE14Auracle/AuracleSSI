#pragma once

#include "Object.h"

using namespace System::Collections;

namespace ssi
{
	class ISensor;
	class IRunnable;
}

namespace SSINet
{
	interface class IProvider;

	public interface class ISensor : public IObject
	{
		public:
			ssi_size_t getChannelSize() ;
			//IChannel getCHannel(ssi_size_t index) =0;
			bool setProvider(String^ name, IProvider^ provider);
			bool connect();
			bool disconnect();

			operator ssi::ISensor*();
	};

	public ref class ISensorWrapper : public ISensor
	{
		private:
			ssi::ISensor*	m_pSensor;
			ObjectWrapper^	m_Object;
	
		public:
			virtual property OptionCollection^ Options { OptionCollection^ get() { return m_Object->Options; } }
			virtual property String^ Name { String^ get() { return m_Object->Name; } }
			virtual property String^ Info { String^ get() { return m_Object->Info; } }

		public:
			ISensorWrapper(ssi::ISensor* pSensor);

			virtual ssi_size_t getChannelSize();
			virtual bool connect();
			virtual bool disconnect();
			virtual bool setProvider(String^ name, IProvider^ provider);

			virtual String^ getName() { return Name; }
			virtual String^ getInfo() { return Info; }

			virtual operator ssi::ISensor*() { return m_pSensor; }
	};

	public ref class Sensor
	{
		private:
			ssi::IRunnable* m_pSensor;

		public:
			Sensor(ssi::IRunnable* pSensor);

		protected:
			bool start();
			bool stop();
	};
}