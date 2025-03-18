#ifndef _PROTO_TEXT_UTILS_
#define _PROTO_TEXT_UTILS_

#include <windows.h>

namespace ProtoText
{
	//For class independent functions
	namespace Utils
	{
		/*
		[IN] dip : the DIP value to be converted to physical pixels
		[IN] screenDpi : The DPI of the screen
		
		[RETURN] Pixels
			
		*/
		inline FLOAT DIPsToPixels(FLOAT dip, FLOAT screenDpi)
		{
			return dip * (screenDpi / USER_DEFAULT_SCREEN_DPI);
		}
	}
}

#endif //!_PROTO_TEXT_UTILS_