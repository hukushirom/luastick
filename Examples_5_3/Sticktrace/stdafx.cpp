#include "stdafx.h"

double GetDisplayPPM()
{
	static double DISPLAY_PPM = -1.0;
	if (DISPLAY_PPM < 0)
	{
		HDC hAttribDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		if (hAttribDC == NULL) return -1.0;
		DISPLAY_PPM = (double)GetDeviceCaps(hAttribDC, LOGPIXELSX) / 25.4;
		// HDCを削除。
		::DeleteDC(hAttribDC);
	}
	return DISPLAY_PPM;
}

__int64 GetCurrentMillisecTime()
{
	// 100-nanosecond since January 1, 1601.
	FILETIME ftSystemTime;
	::GetSystemTimeAsFileTime(&ftSystemTime);
	const ULARGE_INTEGER li = { ftSystemTime.dwLowDateTime, ftSystemTime.dwHighDateTime };
	return li.QuadPart / 10000;
}
