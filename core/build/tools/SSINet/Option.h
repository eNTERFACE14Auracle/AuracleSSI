#pragma once

#include "Util.h"

using namespace System;
using namespace System::Collections;

namespace SSINet
{
	interface class PlaceHolder
	{
		public:
			Object^ Get();
			void Set(Object^ value);
	};

	template<typename TManaged, typename TNative>
	ref class OptionHolder : public PlaceHolder
	{
		private:
			ssi::IOptions*	m_pOptions;
			ssi_option_t*	m_pOption;
		public:
			TManaged^ Hold;

			OptionHolder(ssi::IOptions* pOptions, ssi_option_t* pOpt) 
			{ 
				m_pOptions = pOptions;
				m_pOption = pOpt;
				
				Hold = Util::OptionHelper::GetValue<TManaged, TNative>(pOptions, pOpt);
			}

			virtual Object^ Get() { return Hold; }
			virtual void Set(Object^ value)
			{
				Util::OptionHelper::SetValue<TManaged, TNative>(m_pOptions, m_pOption, value);
				Hold = Util::OptionHelper::GetValue<TManaged, TNative>(m_pOptions, m_pOption);
			}
	};

	ref class OptionValue
	{
		private:
			PlaceHolder^ m_Holder;
		
		public:
			OptionValue(PlaceHolder^ holder)
			{
				m_Holder = holder;
			}

			Object^ Get()
			{
				return m_Holder->Get();
			}

			void Set(Object^ value)
			{
				m_Holder->Set(value);
			}

			virtual String^ ToString() override { return m_Holder->Get()->ToString(); }
	};

	public ref class Option
	{
		private:
			String^			m_Name;
			String^			m_Help;
			
			OptionValue^	m_Value;

		public:
			Option(ssi::IOptions* pOptions, ssi_option_t* pOption) 
			{ 
				m_Name = gcnew String(pOption->name);
				m_Help = gcnew String(pOption->help);
				

				if(pOption->num == 1)
				{
					switch(pOption->type)
					{
						case 1:
							m_Value = gcnew OptionValue(gcnew OptionHolder<SByte, char>(pOptions, pOption));
							break;
						case 2:
							m_Value = gcnew OptionValue(gcnew OptionHolder<Byte, unsigned char>(pOptions, pOption));
							break;
						case 3:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::Int16, short>(pOptions, pOption));
							break;
						case 4:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::UInt16, unsigned short>(pOptions, pOption));
							break;
						case 5:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::Int32, int>(pOptions, pOption));
							break;
						case 6:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::UInt32, unsigned int>(pOptions, pOption));
							break;
						case 7:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::Int32, long>(pOptions, pOption));
							break;
						case 8:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::UInt32, unsigned long>(pOptions, pOption));
							break;
						case 9:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::Single, float>(pOptions, pOption));						
							break;
						case 10:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::Double, double>(pOptions, pOption));
							break;
						case 11:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::Double, long double>(pOptions, pOption));
							break;
						case 14:
							m_Value = gcnew OptionValue(gcnew OptionHolder<System::Boolean, bool>(pOptions, pOption));
							break;
					}
				}
				else
				{
					switch(pOption->type)
					{
						case 1:
						case 2:
							m_Value = gcnew OptionValue(gcnew OptionHolder<String, char*>(pOptions, pOption));
							break;
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
						case 9:
						case 10:
						case 11:
						case 14:
							m_Value = gcnew OptionValue(gcnew OptionHolder<ArrayList, void*>(pOptions, pOption));
							break;
					}	
				}

			}

			property String^ Name { String^ get() { return m_Name; } }
			property String^ Help { String^ get() { return m_Help; } }
			property OptionValue^ Value { OptionValue^ get() { return m_Value; } }

			void setValue(Object^ value) { m_Value->Set(value); }
	};

	public ref class OptionCollection : public System::Collections::Generic::SortedList<String^, Option^>
	{	
		public:
			OptionCollection(ssi::IOptions* pOptions)
			{
				int size = pOptions->getSize();

				for(int i = 0; i < size; ++i)
				{
					ssi_option_t* p = pOptions->getOption(i);

					Option^ opt = gcnew Option(pOptions, p);
					Add(opt->Name, opt);
				}
			}
	};
}