#pragma once

#include <SSI_Cons.h>

namespace SSINet
{
	public enum class SSIType
	{
		SSI_UNDEF = 0,
		SSI_CHAR,
		SSI_UCHAR,
		SSI_SHORT,
		SSI_USHORT,
		SSI_INT,
		SSI_LONG,
		SSI_ULONG,
		SSI_FLOAT,
		SSI_DOUBLE,
		SSI_LDOUBLE,
		SSI_STRUCT,
		SSI_IMAGE,
		SSI_BOOL
	};
}