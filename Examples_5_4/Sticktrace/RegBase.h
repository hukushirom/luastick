#pragma once

#define FD_RegSaveListCtrlContents(wnd, list) FCRegBase::SaveListCtrlContents(wnd, L#list L"_C", list)
#define FD_RegLoadListCtrlContents(wnd, list) FCRegBase::LoadListCtrlContents(wnd, L#list L"_C", list)
#define FD_RegSaveListCtrlColumnWidth(wnd, list) FCRegBase::SaveListCtrlColumnWidth(wnd, L#list L"_W", list)
#define FD_RegLoadListCtrlColumnWidth(wnd, list) FCRegBase::LoadListCtrlColumnWidth(wnd, L#list L"_W", list)

enum FDRegFontType
{
//----- 17.09.26 Fukushiro M. 削除始 ()-----
//	FDFT_HRULER_DIGITS = 0,	// 水平ルーラーの数字。
//	FDFT_VRULER_DIGITS,			// 垂直ルーラーの数字。
//	FDFT_TOOLBAR,				// ツールバーの表示用。
//	FDFT_OBJ_INFO_BAR,			// 部品情報バーの表示用。
//	FDFT_FUNCTION_BAR,			// ファンクションバー表示用。
//	FDFT_MATRIX_TITLE,			// テーブルの表示用。
//	FDFT_OBJECT_INIT,			// 部品の初期化用。
//	FDFT_POP_HINT,				// ホバーヒント用。
//	FDFT_POP_HINT_MONOSPACE,	// ホバーヒント固定幅用。
//	FDFT_FOOTER_INIT,			// フッタの初期化用。
//	FDFT_HTREE_TEXT,			// 履歴コンソール中のテキスト表示用。
//	FDFT_HTREE_TIME,			// 履歴コンソール中の時間表示用。
//	FDFT_TIME_LABEL_TEXT,		// 時間ラベルテキスト用。
//
//								//----- 項目を追加する場合はここへ -----
//	FDFT_TOOLBAR_LABEL,			// ツールバーのラベル。
//	FDFT_TOOLBAR_GROUP_NAME,	// ツールバーのグループ名。
//----- 17.09.26 Fukushiro M. 削除終 ()-----
	FDFT_SCRIPT_EDITOR = 0,		// スクリプトエディター。
	FDFT_END,
}; // enum FDRegFontType.

class FCRegBase
{
public:
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

