#pragma once

#include "Transformer.h"

using namespace System;
using namespace System::Collections;

namespace ssi
{
	class IConsumer;
	class Consumer;
}

namespace SSINet
{
	public interface class IConsumer : public IObject
	{
		public:
			operator ssi::IConsumer*();
	};

	ref class IConsumerWrapper : public IConsumer
	{
		private:
			ssi::IConsumer* m_pConsumer;
			ObjectWrapper^ m_Object;

		public:
			IConsumerWrapper(ssi::IConsumer* pConsumer) 
			{ 
				m_pConsumer = pConsumer; 
				m_Object = gcnew ObjectWrapper((ssi::IObject*)pConsumer);
			}

			virtual String^ getName() { return m_Object->getName(); }
			virtual String^ getInfo() { return m_Object->getInfo(); }

			virtual property OptionCollection^ Options { OptionCollection^ get() { return m_Object->Options; } }
			virtual property String^ Name { String^ get() { return getName(); } }
			virtual property String^ Info { String^ get() { return getInfo(); } }			
			


			virtual operator ssi::IConsumer*() { return m_pConsumer; }
	};

	public ref class Consumer
	{
		private:
			ssi::Consumer* m_pConsumer;

		public:
			Consumer(ssi::Consumer* pConsumer) : m_pConsumer(pConsumer) {}

			operator ssi::Consumer*() { return m_pConsumer; }
	};
}