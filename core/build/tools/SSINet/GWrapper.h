#pragma once

namespace SSINet
{
	template<typename T>
	interface class GWrapper
	{
		public:
			operator T*();
	};
};