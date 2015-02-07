#pragma once

#include <base/IObject.h>
#include <ioput/option/Optionlist.h>

using namespace System;
using namespace System::Collections::Generic;

namespace SSINet
{
	ref class Option;
	ref class OptionCollection;

	public interface class IObject
	{
		public:
			property String^ Name { String^ get(); }
			property String^ Info { String^ get(); }
			property OptionCollection^ Options { OptionCollection^ get(); }
			
			String^ getName();
			String^ getInfo();
	};

	ref class ObjectWrapper : IObject
	{
		private:
			ssi::IObject* m_pObject;
			OptionCollection^ m_Options;

			String^ m_Name;
			String^ m_Info;

		public:
			virtual property String^ Name { String^ get() { return m_Name; } }
			virtual property String^ Info { String^ get() { return m_Info; } }
			virtual property OptionCollection^ Options { OptionCollection^ get() { return m_Options; } }

		public:
			ObjectWrapper(ssi::IObject* pObject);			
			
			virtual String^ getName() { return m_Name; }
			virtual String^ getInfo() { return m_Info; }
			
	};
}