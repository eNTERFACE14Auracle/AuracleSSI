#include "StdAfx.h"
#include "Provider.h"

using SSINet::Provider;

Provider::Provider(ssi::Provider* provider)
{
	m_pProvider = provider;
	m_Object = gcnew ObjectWrapper(provider);
}