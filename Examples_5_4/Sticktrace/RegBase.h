#pragma once

#define FD_RegSaveListCtrlContents(wnd, list) FCRegBase::SaveListCtrlContents(wnd, L#list L"_C", list)
#define FD_RegLoadListCtrlContents(wnd, list) FCRegBase::LoadListCtrlContents(wnd, L#list L"_C", list)
#define FD_RegSaveListCtrlColumnWidth(wnd, list) FCRegBase::SaveListCtrlColumnWidth(wnd, L#list L"_W", list)
#define FD_RegLoadListCtrlColumnWidth(wnd, list) FCRegBase::LoadListCtrlColumnWidth(wnd, L#list L"_W", list)

enum FDRegFontType
{
	FDFT_SCRIPT_DEBUGGER = 0,	// Script debugger
	FDFT_END,
}; // enum FDRegFontType.

class FCRegBase
{
public:
	static void SetApplicationInfo(const wchar_t* companyName, const wchar_t* packageName, const wchar_t* applicationName);
	static void InitRegFont(CFont& font, FDRegFontType nType, long lEscapement = 0, double dZoom = -1.0);
	static void InitRegFont(LOGFONT& logFont, FDRegFontType nType, long lEscapement = 0, double dZoom = -1.0);

	static void SaveListCtrlContents(CWnd* wnd, const wchar_t* name, int list);
	static void LoadListCtrlContents(CWnd* wnd, const wchar_t* name, int list);
	static void SaveListCtrlColumnWidth(CWnd* wnd, const wchar_t* name, int list);
	static void LoadListCtrlColumnWidth(CWnd* wnd, const wchar_t* name, int list);

	//----- Low level methods -----
	static void SaveRegBinary (const wchar_t* wcpSection, const wchar_t* wcpName, const BYTE* pData, DWORD dwDataSz);
	static DWORD LoadRegBinary (const wchar_t* wcpSection, const wchar_t* wcpName, BYTE* pData, DWORD dwDataSz);

	static void SaveRegString(const wchar_t* wcpSection, const wchar_t* wcpName, const wchar_t* wcpData);
	static BOOL LoadRegString(const wchar_t* wcpSection, const wchar_t* wcpName, std::wstring& strData);

	static void SaveRegDWORD (const wchar_t* wcpSection, const wchar_t* wcpName, DWORD dwValue);
	static bool LoadRegDWORD (const wchar_t* wcpSection, const wchar_t* wcpName, DWORD& dwValu);

	static BOOL GetIsAutoAlt (int isAutoAlt = -1);
	static void SetIsAutoAlt (BOOL isAutoAlt);


protected:
	//----- Low level methods -----
	static void SaveBinary (	const BYTE* pData,
								DWORD dwDataSz,
								HKEY hKey,
								const wchar_t* wcpSubKey,
								const wchar_t* wcpName);

	static DWORD LoadBinary (	BYTE* pData,
								DWORD dwDataSz,
								HKEY hKey,
								const wchar_t* wcpSubKey,
								const wchar_t* wcpName);
	static BOOL LoadBinary(
		std::vector<BYTE>& vData,
		HKEY hKey,
		const wchar_t* wcpSubKey,
		const wchar_t* wcpName
	);

	static void SaveDWORD (DWORD dwValue,
							HKEY hKey,
							const wchar_t* wcpSubKey,
							const wchar_t* wcpName);
	static bool LoadDWORD (DWORD& dwValue,
							HKEY hKey,
							const wchar_t* wcpSubKey,
							const wchar_t* wcpName);

}; // class FCRegBase.

