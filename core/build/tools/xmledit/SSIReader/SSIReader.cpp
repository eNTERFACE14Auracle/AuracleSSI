// Dies ist die Haupt-DLL.

//#include "stdafx.h"

#include "SSIReader.h"
#include <ssi_cons.h>
#include <base/Factory.h>
#include <base/ISensor.h>
#include <base/IConsumer.h>


using namespace XMLReader;

SSIReader::SSIReader()
{
	System::Threading::Thread::CurrentThread->CurrentCulture = gcnew System::Globalization::CultureInfo("en-US");
}

System::Collections::Generic::List<String^>^ SSIReader::ReadDll(String^ path)
{
	Data->Clear();
		
	pin_ptr<const wchar_t> szPath = PtrToStringChars(path);					
	
	HMODULE hDll = LoadLibrary(szPath);
	if(INVALID_HANDLE_VALUE == hDll)
		return Data;

	ssi::Factory::register_fptr_from_dll_t register_fptr = (ssi::Factory::register_fptr_from_dll_t) GetProcAddress (hDll, "Register");
	if (0 == register_fptr)
	{
		FreeLibrary(hDll);
		return Data;
	}	  
	
	register_fptr (ssi::Factory::GetFactory (), 0, 0);
	
	/*for(int i = 0; i < Data->Count; ++i)
		Data[i]->Lib = path;*/

	FreeLibrary(hDll);

	char* szDll = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(path);
	ssi::Factory::RegisterDLL(szDll, 0);
	System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)(void*)szDll);
	return Data;
}

bool Register (const ssi_char_t *name, ssi::IObject::create_fptr_t create_fptr)
{
	SSIReader::Data->Add(gcnew String(name));
	return true;
}

MetaData^ SSIReader::ReadMetaData(String^ name)
{
	char* szName = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
	ssi::IObject* pObject = ssi::Factory::Create(szName, 0, true);
	System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)(void*)szName);

	MetaData^ tmp;

	if((unsigned char)pObject->getType() == SSI_SENSOR)
	{
		tmp = gcnew SensorMetaData();
		ssi::ISensor* pSensor = dynamic_cast<ssi::ISensor*>(pObject);

		ReadChannelData(*pSensor, (SensorMetaData^)tmp);
	}
	else
		tmp = gcnew MetaData();
	
	tmp->Name = gcnew String(name);
	tmp->Info = gcnew String(pObject->getInfo());
	
	tmp->Options = gcnew System::Collections::Generic::List<Option^>();
	tmp->Type = (unsigned char)pObject->getType();
	
	ssi::IOptions* pOptions = pObject->getOptions();

	if(pOptions != 0)
	{
		for(ssi_size_t i = 0; i < pOptions->getSize(); ++i)
		{
			ssi_option_t* pOpt = pOptions->getOption(i);
			Option^ opt = gcnew Option();  				

			opt->Name = gcnew String(pOpt->name);
			opt->Help = gcnew String(pOpt->help);
			opt->Num = pOpt->num;
			opt->Type = (unsigned char)pOpt->type;
			//opt->pValue = pOpt->ptr;
			switch(pOpt->type)
			{
			case SSI_CHAR:
				{
					opt->Value = gcnew String((char*)pOpt->ptr);
				}
				break;
			case SSI_UCHAR:
				{
					if(pOpt->num == 1)
					{
						opt->Value = System::Convert::ToString(*(unsigned char*)pOpt->ptr);
					}
					else
						opt->Value = gcnew String(reinterpret_cast<const char*>(pOpt->ptr));
				}
				break;
			case SSI_SHORT:
				{
					short value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				
				break;
			case SSI_USHORT:
				{
					unsigned short value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				break;
			case SSI_INT:
				{
					int value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				break;
			case SSI_UINT:
				{
					unsigned int value = 0;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				break;
			case SSI_LONG:
				{
					long value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				break;
			case SSI_ULONG:
				{
					unsigned long value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString((unsigned __int64)value);
				}
				break;
			case SSI_FLOAT:
				{
					float value;
					
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				break;
			case SSI_DOUBLE:
				{
					double value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				break;
			case SSI_LDOUBLE:
				{
					long double value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString((double)value);
				}
				break;
			case SSI_STRUCT:
			case SSI_IMAGE:
				break;
			case SSI_BOOL:
				{
					bool value;
					pOptions->getOptionValue(pOpt->name, (void*)&value);
					
					opt->Value = System::Convert::ToString(value);
				}
				break;			
			};
			
			tmp->Options->Add(opt);
		}
	}

	delete pObject;
	
	return tmp;
}

void SSIReader::ReadChannelData(ssi::ISensor& sensor, SensorMetaData^ data)
{
	size_t cChannels = sensor.getChannelSize();
	data->Channels = gcnew System::Collections::Generic::List<Channel^>();

	for(ssi_size_t i = 0; i < cChannels; ++i)
	{
		ssi::IChannel* pChannel = sensor.getChannel(i);
		const char* name = pChannel->getName();
		const char* info = pChannel->getInfo();

		Channel^ channel = gcnew Channel();
		channel->Name = gcnew String(name);
		channel->Info = gcnew String(info);

		data->Channels->Add(channel);
	}
}

