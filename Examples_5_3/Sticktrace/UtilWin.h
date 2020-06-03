#pragma once

class UtilWin
{
	static CSize FFJustifyToMonitor (const CRect& rect);

public:
	static void GetWindowText(const CWnd* pWnd, std::wstring& rString)
	{
		// WindowText長を取得。最後の空文字は含まれないことに注意。
		auto iTextLen = ::GetWindowTextLength(pWnd->m_hWnd);
		// 最大の有りえるサイズを確保。
		rString.assign(iTextLen + 1, L'\0');
		iTextLen = UtilWin::GetWindowText(pWnd->m_hWnd, (wchar_t*)rString.data(), (int)rString.size());
		rString.resize(iTextLen);
	}

	static int GetWindowText(HWND hWnd, wchar_t* wszStringBuf, int nMaxCount)
	{
		return ::GetWindowText(hWnd, wszStringBuf, nMaxCount);
	} // FFGetWindowText.
};

