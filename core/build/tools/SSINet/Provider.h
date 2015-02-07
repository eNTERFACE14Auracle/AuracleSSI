#pragma once

#include "Object.h"
#include "Transformer.h"
#include <frame/Provider.h>

namespace SSINet
{
	public interface class IProvider : IObject
	{
		public:
			property ssi_time_t SampleRate { ssi_time_t get(); }

		public:
			int getBufferId();
			ssi_time_t getSampleRate();
			ssi_size_t getSampleDimension();
			ssi_size_t getSampleBytes();
			ssi_type_t getSampleType();

			operator ssi::IProvider*();
	};

	public ref class Provider : public IProvider, ITransformable
	{
		private:
			ObjectWrapper^ m_Object;
			ssi::Provider* m_pProvider;

		public:
			virtual property ssi_time_t SampleRate
			{
				ssi_time_t get() { return 0; /* getSampleRate();*/ }
			}

		public:
			Provider(ssi::Provider* provider);

			//IProvider
			virtual int getBufferId()				{ return m_pProvider->getBufferId(); }
			virtual ssi_time_t getSampleRate()		{ return m_pProvider->getSampleRate(); }
			virtual ssi_size_t getSampleDimension() { return m_pProvider->getSampleDimension(); }
			virtual ssi_size_t getSampleBytes()		{ return m_pProvider->getSampleBytes(); }
			virtual ssi_type_t getSampleType()		{ return m_pProvider->getSampleType(); }
			virtual operator ssi::IProvider*()		{ return m_pProvider; }
			//~IProvider

			//IObject
			virtual String^ getName() { return m_Object->getName(); }
			virtual String^ getInfo() { return m_Object->getInfo(); }

			virtual property OptionCollection^ Options { OptionCollection^ get() { return nullptr; } }
			virtual property String^ Name { String^ get() { return getName(); } }
			virtual property String^ Info { String^ get() { return getInfo(); } }			
			//~IObject		

			virtual operator ssi::ITransformable*() { return m_pProvider; }
	};
}
