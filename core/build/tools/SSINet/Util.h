#pragma once

#include <vcclr.h>
#include <stdio.h>
#include "Object.h"
#include "SSINet.h"

using namespace System;
using namespace System::Collections;
using namespace System::Runtime::InteropServices;



namespace SSINet { namespace Util
{
	struct SSIString
	{
		char* szValue;

		SSIString(String^ value) : szValue(NULL)
		{
			pin_ptr<const wchar_t> pValue = PtrToStringChars(value);

			size_t convertedChars = 0;
			size_t sizeInBytes = ((value->Length + 1) << 1);
			szValue = new char[sizeInBytes];
			wcstombs_s(&convertedChars, szValue, sizeInBytes, pValue, sizeInBytes);
		}

		~SSIString()
		{
			if(NULL != szValue)
			{
				delete[] szValue;
				szValue = NULL;
			}
		}

		operator LPCSTR() const { return szValue; }
	};

	ref class OptionHelper
	{
		public:
			template <typename TManaged, typename TNative>
			static TManaged^ GetValue(ssi::IOptions* pOpts, ssi_option_t* pOpt)
			{
				TNative data = *(TNative*)pOpt->ptr;
				
				TManaged^ tmp = gcnew TManaged(data);
				return tmp;
			}

			delegate Object^ TypeToObjectDelegate(void* pData);

			template<>
			static ArrayList^ GetValue<ArrayList, void*>(ssi::IOptions* pOpts, ssi_option_t* pOpt)
			{
				size_t cTypeSize = ssi_type2bytes(pOpt->type);
				unsigned char* pData = new unsigned char[pOpt->num * ssi_type2bytes(pOpt->type)];

				Object^ value;
				pOpts->getOptionValue(pOpt->name, (void*)pData);
				
				TypeToObjectDelegate^ del = nullptr;

				switch(pOpt->type)
					{
						case 1:
						case 2:
							break;
						case 3:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::Int16, short>);
							break;
						case 4:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::UInt16, unsigned short>);
							break;
						case 5:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::Int32, int>);
							break;
						case 6:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::UInt32, unsigned int>);
							break;
						case 7:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::Int32, long>);
							break;
						case 8:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::UInt32, unsigned long>);					
							break;
						case 9:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::Single, float>);
							break;
						case 10:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::Double, double>);
							break;
						case 11:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::Double, long double>);
							break;
						case 14:
							del = gcnew TypeToObjectDelegate(TypeToObject<System::Boolean, bool>);
							break;
					}

				if(del != nullptr)
				{
					ArrayList^ list = gcnew ArrayList();
					for(int i = 0; i < pOpt->num; ++i)
						list->Add(del(pData + (i * cTypeSize)));

					delete[] pData;
					return list;
				}

				delete[] pData;
				return nullptr;
			}

			template<typename TManaged, typename TNative>
			static Object^ TypeToObject(void* pData)
			{
				TNative* pNtData = (TNative*)pData;
				return gcnew TManaged(*pNtData);
			}

			template<>
			static String^ GetValue<String, char*>(ssi::IOptions* pOpts, ssi_option_t* pOpt)
			{
				char* data = new char[pOpt->num];
				pOpts->getOptionValue(pOpt->name, (void*)data);

				String^ result = gcnew String(data);
				delete[] data;

				return result;
			}

			template<typename TManaged, typename TNative>
			static void SetValue(ssi::IOptions* pOptions, ssi_option_t* pOpt, Object^ value)
			{
				TNative data = *safe_cast<TManaged^>(Convert::ChangeType(value, TNative::typeid)); 

				pOptions->setOptionValue(pOpt->name, &data);
			}

			template<>
			static void SetValue<ArrayList, void*>(ssi::IOptions* pOptions, ssi_option_t* pOpt, Object^ value)
			{
				/*SSIString data((String^)value);
				const char* szData = data;

				pOptions->setOptionValue(pOpt->name,(void*)szData);*/
			}

			template<>
			static void SetValue<String, char*>(ssi::IOptions* pOptions, ssi_option_t* pOpt, Object^ value)
			{
				SSIString data((String^)value);
				const char* szData = data;

				pOptions->setOptionValue(pOpt->name,(void*)szData);
			}
	};
}}									  