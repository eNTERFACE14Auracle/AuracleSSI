// Dies ist die Haupt-DLL.

#include "stdafx.h"

#include <SSI_Cons.h>
#include "Factory.h"
#include "base/Factory.h"
#include "Util.h"
#include "Sensor.h"
#include "Consumer.h"

using namespace SSINet;

inline bool Factory::RegisterDLL(String^ dllpath)
{
	dllpath = System::IO::Path::Combine(DllDirectory, dllpath);
	
	Util::SSIString path(dllpath); 

	return ssi::Factory::RegisterDLL(path);
}

inline void Factory::Clear()
{
	ssi::Factory::Clear();
}

generic<typename T>
inline T Factory::Create(String^ name)
{
	Util::SSIString szName(name);

	return Create<T>(name, String::Empty, true);
}

generic<typename T>
inline T Factory::Create(String^ name, String^ file)
{
	return Create<T>(name, file, true);
}

generic<typename T>
inline T Factory::Create(String^ name, String^ file, bool auto_free)
{
	Util::SSIString szName(name);
	Util::SSIString szFile(file);

	ssi::IObject* pObject = NULL;

	if(System::String::IsNullOrEmpty(file))
		pObject = ssi::Factory::Create(szName, NULL, auto_free);
	else
		pObject = ssi::Factory::Create(szName, szFile, auto_free);

	if(NULL == pObject)
		throw gcnew System::Exception("Error creating " + name);

	switch(pObject->getType())
	{
		case SSI_SENSOR:
			return (T)gcnew ISensorWrapper((ssi::ISensor*)pObject);
			break;
		case SSI_CONSUMER:
			return (T)gcnew IConsumerWrapper((ssi::IConsumer*)pObject);
			break;
		default:
			return (T)gcnew ObjectWrapper(pObject);
	}
}