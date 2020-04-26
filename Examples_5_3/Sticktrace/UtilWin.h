#pragma once

class UtilWin
{

#if 0

/*************************************************************************
 * <定義>	FTDirMacro
 *
 * <機能>	ディレクトリマクロの定義。
 *
 * <履歴>	08.05.07 Fukushiro M. 作成
 *************************************************************************/
enum FTDirMacro
{
	FD_DIR_BEGIN = 1,				// ディレクトリマクロ定義の開始。
	FD_DIR_INSTALL = FD_DIR_BEGIN,	// アプリのインストールフォルダー。例「C:\Program Files\Dynamic Draw Project\DD Thought Tickler 5.4」
	FD_DIR_NEUTRAL_TEMPLATES,		// 言語非依存のテンプレートフォルダー。例「C:\Users\Public\Documents\Dynamic Draw\Templates」
	FD_DIR_TEMPLATES,				// 言語別のテンプレートフォルダー。例「C:\Users\Public\Documents\Dynamic Draw\Templates\1041」
//----- 10.02.12 Fukushiro M. 追加始 ()-----
	FD_DIR_DESKTOP,					// デスクトップフォルダー。例「C:\Users\Masayuki\Desktop」
	FD_DIR_COMMON_DOCUMENTS,		// コモンドキュメントフォルダー。例「C:\Users\Public\Documents」
//----- 10.03.02 Fukushiro M. 追加始 ()-----
	FD_DIR_COMMON_NEUTRAL_TEMPLATES,// 言語非依存のテンプレートフォルダー。例「C:\Users\Public\Documents\Dynamic Draw\Templates」
	FD_DIR_COMMON_TEMPLATES,		// 言語別のテンプレートフォルダー。例「C:\Users\Public\Documents\Dynamic Draw\Templates\1041」
//----- 10.03.02 Fukushiro M. 追加終 ()-----
	FD_DIR_MY_DOCUMENTS,			// マイドキュメントフォルダー。例「C:\Users\Fukushiro\Documents」
	FD_DIR_MY_NEUTRAL_TEMPLATES,	// 言語非依存のマイテンプレートフォルダー。例「C:\Users\Fukushiro\Documents\Dynamic Draw\Templates」
	FD_DIR_MY_TEMPLATES,			// 言語別のマイテンプレートフォルダー。例「C:\Users\Fukushiro\Documents\Dynamic Draw\Templates\1041」
//----- 10.02.12 Fukushiro M. 追加終 ()-----
	FD_DIR_END,						// ディレクトリマクロ定義の終了。
}; // enum FTDirMacro.

////////////////////////////////////////////////////////////////////////
// クラス、構造体。

class WString;

struct FCWinTitleRec
{
//;----- <#DD SDK NOT EXPORT> -----;
	FCWinTitleRec ();
	FCWinTitleRec (HWND hwnd, const wchar_t* tcp);
// 08.01.20 Fukushiro M. 1行変更 ()
//	operator = (const FCWinTitleRec& rec);
	FCWinTitleRec& operator = (const FCWinTitleRec& rec);
//;----- </#DD SDK NOT EXPORT> -----;
	HWND		hwndApp;
	WCharAuto	atpTitle;
}; // struct FCWinTitleRec.

typedef std::vector<FCWinTitleRec> FCWinTitleRecVector;

struct FCWndAndId
{
	CWnd*	pWnd;
	DWORD	dwId;
}; // struct FCWndAndId.

//;----- </#DD SDK EXPORT DEF_H> -----;

////////////////////////////////////////////////////////////////////////
// グローバル関数。

//----- 03.09.15 Fukushiro M. 変更前 ()-----
//extern void EnumWinTitle (FCWinTitleRecVector& vWinTtl, const wchar_t* tcpAppTitle = NULL);
//extern HWND FFFindDdDocumentWindow (const wchar_t* tcpDocTitle, const wchar_t* tcpAppTitle = NULL,
//							HWND hwndFrom = NULL);
//----- 03.09.15 Fukushiro M. 変更後 ()-----
extern void EnumWinTitle (FCWinTitleRecVector& vWinTtl);
extern HWND FFFindDdDocumentWindow (const wchar_t* tcpDocTitle, HWND hwndFrom = NULL);
//----- 03.09.15 Fukushiro M. 変更終 ()-----
extern HWND FFFindHidocumentWindow (const wchar_t* wcpProjectPath,
									long lDocId,
									HWND hwndFrom = NULL,
									const wchar_t* wcpAppName = NULL);
//----- 08.04.17 Fukushiro M. 変更前 ()-----
//extern BOOL ExecMolipDraw (	const wchar_t* szParam = NULL,
//----- 08.04.17 Fukushiro M. 変更後 ()-----
extern BOOL ExecMolipDraw (	WORD wAppMode,
							const wchar_t* szParam = NULL,
//----- 08.04.17 Fukushiro M. 変更終 ()-----
							HWND hWndMsg = NULL,
							DWORD dwQuitMsg = 0,
							DWORD* pdwThreadId = NULL,
							DWORD* pdwProcId = NULL,
							HANDLE* phThread = NULL,
							HANDLE* phProcess = NULL);

//----- 14.08.29 Fukushiro M. 追加始 ()-----
extern BOOL FFExecApp (	const wchar_t* wcpAppPath,
						const wchar_t* wcpParam = NULL,
						HWND hWndMsg = NULL,
						DWORD dwQuitMsg = NULL,
						DWORD* pdwThreadId = NULL,
						DWORD* pdwProcId = NULL,
						HANDLE* phThread = NULL,
						HANDLE* phProcess = NULL);
//----- 14.08.29 Fukushiro M. 追加終 ()-----

extern BOOL ExecAndWaitApp (const wchar_t* szAppName, const wchar_t* szParam = NULL,
							DWORD dwMillisec = INFINITE);
extern HWND GetThreadTopWnd (DWORD pdwThreadId);
//----------</#DD VIEWER NOT EXPORT>-----------

// 09.10.07  1行変更 ()
//extern void CloseImeStatus (HWND hWnd);
extern BOOL CloseImeStatus (HWND hWnd);

//----------<#DD VIEWER NOT EXPORT>-----------
// 08.04.13 Fukushiro M. 1行変更 ()
//extern const wchar_t* FFGetMolipExePath ();
extern const wchar_t* FFGetMolipExePath (WORD wAppMode);

//----- 07.11.26 Fukushiro M. 変更前 ()-----
//extern WString FFGetTemplateDirPath ();
//----- 07.11.26 Fukushiro M. 変更後 ()-----
extern WString FFGetMolipExeDirPath (BOOL bChopLast = FALSE);

// 08.05.07 Fukushiro M. 1行削除 ()
//extern WString FFGetTemplateDirPath (BOOL bChopLast = FALSE);

//----- 08.05.07 Fukushiro M. 追加始 ()-----
extern WString FFGetDirMacroPath (FTDirMacro dirMacro, BOOL bChopLast = FALSE);
extern const wchar_t* FFGetDirMacroName (FTDirMacro dirMacro);
//----- 08.05.07 Fukushiro M. 追加終 ()-----
// 10.02.12 Fukushiro M. 1行追加 ()
extern WString FFGetDirDisplayName (FTDirMacro dirMacro);

extern WString FFFullPathToMacroPath (const wchar_t* wcpPath);
extern WString FFMacroPathToFullPath (const wchar_t* wcpPath);
//----- 07.11.26 Fukushiro M. 変更終 ()-----
// 10.02.12 Fukushiro M. 1行追加 ()
extern WString FFMacroPathToDisplayPath (const wchar_t* wcpPath);

// 08.04.22 Fukushiro M. 1行追加 ()
extern WString FFGetLangDirPath (BOOL bChopLast = FALSE);

// 08.05.23 Fukushiro M. 1行追加 ()
extern WORD FFGetValidLangId ();

extern void FFHorznizeToolBar (CToolBar* pToolBar, FCWndAndId wid[], long lWidSz);
//----------</#DD VIEWER NOT EXPORT>-----------

extern int FFGetWindowText (const CWnd* pWnd, wchar_t* wszStringBuf, int nMaxCount,
							WORD wCodePage = CP_ACP);
extern int FFGetWindowText (HWND hWnd, wchar_t* wszStringBuf, int nMaxCount,
							WORD wCodePage = CP_ACP);
extern void FFGetWindowText (const CWnd* pWnd, WString& rString, WORD wCodePage = CP_ACP);
extern void FFGetWindowText (HWND hWnd, WString& rString, WORD wCodePage = CP_ACP);
extern void FFSetWindowText (CWnd* pWnd, const wchar_t* lpszString, WORD wCodePage = CP_ACP);
extern void FFSetWindowText (HWND hWnd, const wchar_t* lpszString, WORD wCodePage = CP_ACP);
// 10.02.21 Fukushiro M. 1行追加 ()
extern CRect FFGetWindowRect (const CWnd* pWnd);

//----- 09.06.08 Fukushiro M. 削除始 ()-----
//extern void FFGetLBText (const CComboBox* pCmb, int nIndex, WString& rString);
//// 09.05.09 Fukushiro M. 1行追加 ()
//extern void FFGetLBText (const CFCDdComboBox* pCmb, int nIndex, WString& rString);
//----- 09.06.08 Fukushiro M. 削除終 ()-----

//----------<#DD VIEWER NOT EXPORT>-----------
extern int FFGetMenuString (const CMenu* pMenu, UINT nIDItem, WString& rString, UINT nFlags);
//----------</#DD VIEWER NOT EXPORT>-----------

//----- 09.06.08 Fukushiro M. 削除始 ()-----
//extern BOOL FFSetItemText (CListCtrl* pCtrl, int nItem, int nSubItem, const wchar_t* lpszText);
//extern BOOL FFSetItemText (CTreeCtrl* pCtrl, HTREEITEM hItem, const wchar_t* lpszItem);
//extern int FFInsertColumn (CListCtrl* pCtrl, int nCol, const wchar_t* lpszColumnHeading,
//							int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
//----- 09.06.08 Fukushiro M. 削除終 ()-----

extern void FFGetMonitorRect (std::vector<CRect>& vMonitorRect);

#endif

	static CSize FFJustifyToMonitor (const CRect& rect);

#if 0

extern CSize FFJustifyToMonitor (const CPoint& pt);
extern DWORD FFPointToMenuId (HWND hwnd, const CPoint& pt);
extern DWORD FFPointToMenuId (const CPoint& pt);
extern HWND FFWindowFromPoint (HWND hwndParent, const CPoint& pt, UINT uFlags);
// 06.03.03 Fukushiro M. 1行追加 ()
extern HWND FFWindowFromPoint (HWND hwndParent, const std::set<HWND>& stHwnd, const CPoint& pt, UINT uFlags);

// 06.02.27 Fukushiro M. 1行追加 ()
extern void FFSendDataMessage (HWND hWnd, DWORD dwMessage, DWORD dwDataSz, void* pData);

//----- 06.03.16 Fukushiro M. 追加始 ()-----
extern BOOL FFScreenToClient (HWND hwnd, LPRECT lpRect);
extern BOOL FFClientToScreen (HWND hwnd, LPRECT lpRect);
//----- 06.03.16 Fukushiro M. 追加終 ()-----

//----- 06.12.05 Fukushiro M. 削除始 ()-----
//// 06.12.01 Fukushiro M. 1行追加 ()
//extern void FFAddRgn (HWND hwnd);
//----- 06.12.05 Fukushiro M. 削除終 ()-----

//----- 08.03.13 Fukushiro M. 追加始 ()-----
extern WString FFGetLongDateTime (const COleDateTime& otm);
//----- 08.03.13 Fukushiro M. 追加終 ()-----

//;----- <#DD SDK EXPORT DEF_H> -----;

///////////////////////////////////////////////////////////////////////////
// PendingMessage関連。

#define FD_PMSG_MASK_INSTANCE	0xff000000	// インスタンスのマスク。
#define FD_PMSG_MASK_STATUS		0x00ffffff	// インスタンス状態のマスク。

//----- FCPendingMessageのメッセージ送付先ウィンドウ・条件ウィンドウ ------
#define FD_PMSG_NONE		(0L << 24)	// 送付先なし。
#define FD_PMSG_MAIN_WND	(1L << 24)	// FFMainWnd()->GetWnd()。
//----- 09.04.15 Fukushiro M. 削除始 ()-----
//#define FD_PMSG_FRAME		(2L << 24)	// FFFrame()。
//#define FD_PMSG_IN_PLACE	(3L << 24)	// FFInplace()。
//----- 09.04.15 Fukushiro M. 削除終 ()-----
#define FD_PMSG_VIEW		(2L << 24)	// FFView().
#define FD_PMSG_DOCUMENT	(3L << 24)	// FFDoc().
#define FD_PMSG_MAIN_HRULER	(4L << 24)	// FFMainWnd()->HRuler().
#define FD_PMSG_MAIN_VRULER	(5L << 24)	// FFMainWnd()->VRuler().
#define FD_PMSG_TEXT_EDITOR	(6L << 24)	// FFView()->GetRTFEditor().

//----- FCPendingMessageのメッセージ送付条件 ------
//----- 10.04.25 Fukushiro M. 変更前 ()-----
//#define FD_PMSG_IF_ENABLE		(1L << 0)	// ウィンドウ/ドキュメントが有効。
//#define FD_PMSG_IF_DISABLE		(1L << 1)	// ウィンドウ/ドキュメントが無効。
//----- 10.04.25 Fukushiro M. 変更後 ()-----
#define FD_PMSG_IF_VALID		(1L << 0)	// ウィンドウ/ドキュメントが有効。
#define FD_PMSG_IF_INVALID		(1L << 1)	// ウィンドウ/ドキュメントが無効。
//----- 10.04.25 Fukushiro M. 変更終 ()-----
#define FD_PMSG_IF_VISIBLE		(1L << 2)	// ウィンドウが表示状態。
#define FD_PMSG_IF_INVISIBLE	(1L << 3)	// ウィンドウが非表示状態。
//----- 10.04.25 Fukushiro M. 追加始 ()-----
#define FD_PMSG_IF_ENABLE		(1L << 4)	// ドキュメントが操作可能状態。
#define FD_PMSG_IF_DISABLE		(1L << 5)	// ドキュメントが操作不可能状態。
//----- 10.04.25 Fukushiro M. 追加終 ()-----

/*************************************************************************
 * <構造体>	FCPendingMessage
 *
 * <機能>	送付保留メッセージの内容を記載する構造体。
 *
 * <履歴>	03.07.14 Fukushiro M. 作成
 *************************************************************************/
struct FCPendingMessage
{
//;----- <#DD SDK NOT EXPORT> -----;
	FCPendingMessage ();
	FCPendingMessage (	DWORD dwDestinationConditons,
						DWORD dwMessage,
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//						DWORD wParam = NULL,
//						DWORD lParam = NULL,
//----- 08.03.15 Fukushiro M. 変更後 ()-----
						WPARAM wParam = NULL,
						LPARAM lParam = NULL,
//----- 08.03.15 Fukushiro M. 変更終 ()-----
						BOOL bImmediate = TRUE);
	FCPendingMessage (	DWORD dwDestination,
						DWORD dwMessage,
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//						DWORD wParam,
//						DWORD lParam,
//----- 08.03.15 Fukushiro M. 変更後 ()-----
						WPARAM wParam,
						LPARAM lParam,
//----- 08.03.15 Fukushiro M. 変更終 ()-----
						BOOL bImmediate,
						DWORD dwCondition0);
	FCPendingMessage (	DWORD dwDestination,
						DWORD dwMessage,
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//						DWORD wParam,
//						DWORD lParam,
//----- 08.03.15 Fukushiro M. 変更後 ()-----
						WPARAM wParam,
						LPARAM lParam,
//----- 08.03.15 Fukushiro M. 変更終 ()-----
						BOOL bImmediate,
						DWORD dwCondition0,
						DWORD dwCondition1);
	FCPendingMessage (	DWORD dwDestination,
						DWORD dwMessage,
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//						DWORD wParam,
//						DWORD lParam,
//----- 08.03.15 Fukushiro M. 変更後 ()-----
						WPARAM wParam,
						LPARAM lParam,
//----- 08.03.15 Fukushiro M. 変更終 ()-----
						BOOL bImmediate,
						DWORD dwCondition0,
						DWORD dwCondition1,
						DWORD dwCondition2);

//;----- </#DD SDK NOT EXPORT> -----;

	DWORD		m_dwDestination;	// メッセージ送付先ウィンドウ。
	DWORD		m_dwMessage;		// メッセージ内容。
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//	DWORD		m_wParam;			// メッセージのWPARAM。
//	DWORD		m_lParam;			// メッセージのLPARAM。
//----- 08.03.15 Fukushiro M. 変更後 ()-----
	WPARAM		m_wParam;			// メッセージのWPARAM。
	LPARAM		m_lParam;			// メッセージのLPARAM。
//----- 08.03.15 Fukushiro M. 変更終 ()-----
	BOOL		m_bImmediate;		// SendMessageで送付するか？
	DWORDVector	m_vConditions;		// メッセージ送付条件。AND。
}; // struct FCPendingMessage.

//;----- </#DD SDK EXPORT DEF_H> -----;

extern void FFPushPendingMessage (const FCPendingMessage& pmsg, BOOL bCheckNow = TRUE);
extern void FFNotifyPendingMessage (CWnd* pWnd, BOOL bWndShow);

extern void FFSendPendingMessage (	DWORD dwDestination,
									DWORD dwMessage,
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//									DWORD wParam = NULL,
//									DWORD lParam = NULL);
//----- 08.03.15 Fukushiro M. 変更後 ()-----
									WPARAM wParam = NULL,
									LPARAM lParam = NULL);
//----- 08.03.15 Fukushiro M. 変更終 ()-----
extern void FFPostPendingMessage (	DWORD dwDestination,
									DWORD dwMessage,
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//									DWORD wParam = NULL,
//									DWORD lParam = NULL);
//----- 08.03.15 Fukushiro M. 変更後 ()-----
									WPARAM wParam = NULL,
									LPARAM lParam = NULL);
//----- 08.03.15 Fukushiro M. 変更終 ()-----

extern void FFPostSendMessage (	DWORD dwDestination,
								DWORD dwMessage,
//----- 08.03.15 Fukushiro M. 変更前 ()-----
//								DWORD wParam = NULL,
//								DWORD lParam = NULL);
//----- 08.03.15 Fukushiro M. 変更後 ()-----
								WPARAM wParam = NULL,
								LPARAM lParam = NULL);
//----- 08.03.15 Fukushiro M. 変更終 ()-----

//----- 10.04.25 Fukushiro M. 追加始 ()-----
// レベルバージョンアップで追加のため、最後に追加する。
extern WString FFDisplayPathToMacroPath (const wchar_t* wcpPath);
extern WString FFDisplayPathToFullPath (const wchar_t* wcpPath);
extern WString FFFullPathToDisplayPath (const wchar_t* wcpPath);
//----- 10.04.25 Fukushiro M. 追加終 ()-----

#endif
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


