#include "stdafx.h"
#include "Option.h"

using namespace SSINet;
using namespace System::Collections;

ObjectWrapper::ObjectWrapper(ssi::IObject *pObject)
{
	m_pObject = pObject;

	if(NULL != pObject->getOptions())
		m_Options = gcnew OptionCollection(pObject->getOptions());

	m_Name = gcnew String(pObject->getName());
	m_Info = gcnew String(pObject->getInfo());
}
