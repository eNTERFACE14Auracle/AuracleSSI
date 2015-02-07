#include "stdafx.h"
#include "Extensions.h"
#include "Option.h"

using namespace SSINet;

void OptionCollectionExtensions::PrintOptions(OptionCollection^ collection)
{
	for each(String^ opt in collection->Keys)
	{
		Option^ value = collection[opt];
		Console::WriteLine(String::Format("{0} : {1}", opt, collection[opt]->Value));
	}
}