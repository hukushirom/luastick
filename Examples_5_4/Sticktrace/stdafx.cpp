#include "stdafx.h"

double GetDisplayPPM()
{
	static double DISPLAY_PPM = -1.0;
	if (DISPLAY_PPM < 0)
	{
		HDC hAttribDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		if (hAttribDC == NULL) return -1.0;
		DISPLAY_PPM = (double)GetDeviceCaps(hAttribDC, LOGPIXELSX) / 25.4;
		// HDC‚ðíœB
		::DeleteDC(hAttribDC);
	}
	return DISPLAY_PPM;
}
