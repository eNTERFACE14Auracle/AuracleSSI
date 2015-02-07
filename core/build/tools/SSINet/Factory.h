// SSICPPWrapper.h

#pragma once

#include "Object.h"

using namespace System;

namespace SSINet 
{

	public ref class Factory
	{
		private:
			static String^ m_DllPath;

		private:
			static Factory()
			{
				m_DllPath = gcnew String("");
			}

		public:
			static property String^ DllDirectory
			{
				String^ get() { return m_DllPath; }
				void set(String^ value) { m_DllPath = value; } 
			}

			static bool RegisterDLL(String^ dllpath);
			static void Clear();

			generic<typename T>
			static T Factory::Create(String^ name);
			generic<typename T>
			static T Factory::Create(String^ name, String^ file);
			generic<typename T>
			static T Factory::Create(String^ name, String^ file, bool auto_free);
	};
}
