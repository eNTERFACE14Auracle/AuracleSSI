#pragma once

using namespace System;
using namespace System::Runtime::CompilerServices;

namespace SSINet
{
	ref class OptionCollection;
	ref class Option;

	[Extension]
    public ref class OptionCollectionExtensions abstract sealed
    {
		public: 
			[Extension]
			static void PrintOptions(OptionCollection^ collection);
    };

	[Extension]
	public ref class IAudioSensorOptionExtensions abstract sealed
	{
		public:
			/*[Extension]
			static void setMove(OptionCollection^ option, int top, int left, int width, int height);

			[Extension]
			static void setMove(OptionCollection^ option, int top, int left, int width, int height, bool accurate);
			*/
		
	};
}
