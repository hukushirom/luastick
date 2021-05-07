#pragma once

class UtilWin
{
public:
	static void GetWindowText(const CWnd* pWnd, std::wstring& rString)
	{
		CString str;
		pWnd->GetWindowText(str);
		rString = (const wchar_t*)str;
	}

	static int GetWindowText(HWND hWnd, wchar_t* wszStringBuf, int nMaxCount)
	{
		return ::GetWindowText(hWnd, wszStringBuf, nMaxCount);
	} // FFGetWindowText.
};

