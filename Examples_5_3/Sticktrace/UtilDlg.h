#pragma once

struct FCDiffRecW;

#if 0

/*************************************************************************
 * <列挙>	FTCheckStr
 *
 * <機能>	CheckFloatStr関数等の返値。
 *
 * <履歴>	05.11.11 Fukushiro M. コメント作成
 *************************************************************************/
enum FTCheckStr
{
	FDCS_ERROR = 0,	// 文字列にエラーがある。
	FDCS_EMPTY,		// 文字列が空。
	FDCS_NORMAL,	// 正常。
};

#endif

/*************************************************************************
 * <構造体>	FCDlgLayoutRec
 *
 * <機能>	ダイアログリサイズにおいて、コントロールを再配置するための
 *			レイアウト情報。FFInitDlgLayout関数等で使用。
 *
 * <解説>	各コントロールごとにクラスオブジェクトが作成される。
 *			下図のように、ウィンドウに4つのコントロール(A～D)が
 *			有る場合を考える。
 *			Aはウィンドウの周囲に合わせてサイズを変更。
 *			Bはウィンドウの左下に合わせて移動。
 *			C,Dはウィンドウの右下に合わせて移動。
 *			Aのメンバー変数
 *			m_dwOffsetFlag=HOOK_ALL
 *			m_rtDistance=(Al,At,Ar,Ab)
 *			Bのメンバー変数
 *			m_dwOffsetFlag=HOOK_LEFT|HOOK_BOTTOM
 *			m_rtDistance=(Bl,LONG_MAX,LONG_MAX,Bb)
 *			Cのメンバー変数
 *			m_dwOffsetFlag=HOOK_RIGHT|HOOK_BOTTOM
 *			m_rtDistance=(LONG_MAX,Cr,LONG_MAX,Cb)
 *			Dのメンバー変数
 *			m_dwOffsetFlag=HOOK_RIGHT|HOOK_BOTTOM
 *			m_rtDistance=(LONG_MAX,Dr,LONG_MAX,Db)
 *
 *
 *                              ウィンドウ
 *             ┏━━━━━━━━━━━━━━━━━━━━┓┬
 *             ┠─┤                                ├─┨│At
 *             ┃Al┌────────────────┐Ar┃┴
 *             ┃  │                                │  ┃
 *             ┃  │            IDC_A               │  ┃
 *             ┃  │                                │  ┃
 *             ┃  │                                │  ┃
 *             ┃  └────────────────┘  ┃┬
 *             ┃  ┌───┐    ┌───┐  ┌───┐  ┃│
 *             ┃  │IDC_B │    │IDC_C │  │IDC_D │  ┃│Ab
 *           ┬┃Bl└───┘  ┬└───┘┬└───┘Dr┃│
 *         Bb│┠─┤        Cb│        Db│        ├─┨│
 *           ┴┗━━━━━━━┷━━━━━┷━━━━━━┛┴
 *                                       ├───────┤
 *                                               Cr
 *
 * <履歴>	05.11.11 Fukushiro M. 作成
 *************************************************************************/
struct FCDlgLayoutRec
{
	enum {
		HOOK_LEFT	= (0x0001 << 0),
		HOOK_TOP	= (0x0001 << 1),
		HOOK_RIGHT	= (0x0001 << 2),
		HOOK_BOTTOM	= (0x0001 << 3),
		HOOK_ALL	= HOOK_LEFT | HOOK_TOP | HOOK_RIGHT | HOOK_BOTTOM,
	};
	FCDlgLayoutRec (DWORD dwOffsetFlag = 0,
					const CRect& rtOffset = CRect(LONG_MAX, LONG_MAX, LONG_MAX, LONG_MAX))
		:	m_dwOffsetFlag(dwOffsetFlag),
			m_rtDistance(rtOffset) {}

	DWORD	m_dwOffsetFlag;
	CRect	m_rtDistance;
}; // struct FCDlgLayoutRec.

typedef std::map<long, FCDlgLayoutRec> IdToDlgLayoutRecMap;

class UtilDlg
{
public:

#if 0

	//----- 10.02.20 Fukushiro M. 変更前 ()-----
	///*************************************************************************
	// * <構造体>	FCTabFrameRec
	// *
	// * <機能>	タブコントロールとフレームのコントロール制御用のレコード。
	// *
	// * <解説>	フレーム配下のコントロールの集合を管理する。
	// *
	// * <履歴>	08.08.05 Fukushiro M. 作成
	// *************************************************************************/
	//struct FCTabFrameRec
	//{
	//	intVector		vid;		// コントロールID配列。
	//	std::wstringVector	vwstrText;	// vidの各コントロールに対応するウィンドウテキスト。
	//};
	//
	//typedef std::vector<FCTabFrameRec> FTTabFrameVector;
	//----- 10.02.20 Fukushiro M. 変更後 ()-----
	typedef std::map<int, std::wstring>			FTCtrlIdToTextMap;
	typedef std::vector<FTCtrlIdToTextMap>	FTTabFrameData;
	//----- 10.02.20 Fukushiro M. 変更終 ()-----

	//;------ </#DD SDK EXPORT DEF_H> -----;

	extern FTCheckStr CheckFloatStr(std::wstring& str, CEdit* pEdit);
	//----------<#DD VIEWER NOT EXPORT>-----------
	extern FTCheckStr CheckFloatStr(std::wstring& str, CComboBox* pBox);
	extern FTCheckStr CheckDWORDStr(std::wstring& str, CEdit* pEdit, WORD wBase);
	extern void SetStdBtnString(HWND hWndDlg);
	//----------</#DD VIEWER NOT EXPORT>-----------

	extern void SetUnitString(HWND hWndDlg);
	extern void RightByLeftHand(HWND hWndDlg);

	//----------<#DD VIEWER NOT EXPORT>-----------
	// 07.04.03 Fukushiro M. 1行変更 ()
	//extern void RightByLeftHand (HMENU hMenu, BOOL bToLeft);
	extern void ChangeHanded(HMENU hMenu);
	extern wchar_t RightByLeftHand(wchar_t tc);
	extern BOOL ChangeToLeftHanded(std::wstring& wstr);
	extern BOOL ChangeToRightHanded(std::wstring& wstr);
	extern void ShortenMenu(HMENU hMenu);
	extern void LengthenMenu(HMENU hMenu);

	// 06.03.04 Fukushiro M. 1行追加 ()
	extern std::wstring FFMakeCommandText(const wchar_t* wcpTitle, wchar_t wcAccesskey);

	extern void SetHiListByRealData(CComboBox* pCmb, int iDataSz, const mmetol mmaData[]);
	extern void SetXtmListByTmsData(CComboBox* pCmb, int iDataSz, const tmsc tmsaData[]);
	extern void SetAngleListByAngleData(CComboBox* pCmb, int iDataSz, const angle daData[]);

	//----- 09.06.08 Fukushiro M. 追加始 ()-----
	extern void FFGetLBText(const CComboBox* pCmb, int nIndex, std::wstring& rString);
	extern void FFGetLBText(const CFCDdComboBox* pCmb, int nIndex, std::wstring& rString);
	extern BOOL FFSetItemText(CListCtrl* pCtrl, int nItem, int nSubItem, const wchar_t* lpszText);
	extern BOOL FFSetItemText(CTreeCtrl* pCtrl, HTREEITEM hItem, const wchar_t* lpszItem);

#endif

	static int InsertColumn(CListCtrl* pCtrl, int nCol, const wchar_t* lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1)
	{
		return pCtrl->InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
	}

#if 0

	//----- 09.06.08 Fukushiro M. 追加終 ()-----

	//----- 09.06.08 Fukushiro M. 追加始 ()-----
	extern BOOL FFDeleteCurSel(CListBox* pListBox);
	extern BOOL FFBringUpCurSel(CListBox* pListBox);
	extern BOOL FFBringDownCurSel(CListBox* pListBox);
	//----- 09.06.08 Fukushiro M. 追加終 ()-----

	//----- 14.08.28 Fukushiro M. 追加始 ()-----
	extern int FFGetCurSel(const CListCtrl* pListCtrl);
	extern void FFSetCurSel(CListCtrl* pListCtrl, int index);
	extern void FFExchangeItem(CListCtrl* pListCtrl, int indexA, int indexB);
	extern void FFScrollItemVisible(CListCtrl* pListCtrl, int index);
	//----- 14.08.28 Fukushiro M. 追加終 ()-----

	extern CPropertyPage* FindPropPage(const CPropertySheet* pSheet,
		CRuntimeClass* pClass);
	//----------</#DD VIEWER NOT EXPORT>-----------

	extern void FFJustifyWindow(HWND hWnd, HWND hWndOffset);
	extern void FFJustifyWindow(HWND hWnd, CPoint* pOffset = NULL);

	//----------<#DD VIEWER NOT EXPORT>-----------
	extern void FFUpdateTextComboList(CComboBox* pBox);

#endif
	// UNICODEで文字列を取得。
	static std::wstring& GetDlgItemText(const CWnd* pWnd, DWORD dwCtrl, std::wstring& wstr)
	{
		CString str;
		pWnd->GetDlgItemText(dwCtrl, str);
		wstr = str;
		return wstr;
	}

#if 0

	static std::wstring& GetDlgItemText(HWND hwnd, DWORD dwCtrl, std::wstring& wstr)
	{

	}
	//----------</#DD VIEWER NOT EXPORT>-----------

	// UNICODEで文字列を設定。
	extern void FFSetDlgItemText(CWnd* pWnd, DWORD dwCtrl, const wchar_t* wcpText);
	extern void FFSetDlgItemText(HWND hwnd, DWORD dwCtrl, const wchar_t* wcpText);
	//----- 07.10.30 Fukushiro M. 追加始 ()-----
	extern void FFSetDlgItemTextEx(HWND hwnd, DWORD dwCtrl, const wchar_t* wcpText);
	extern void FFSetDlgItemComboTextEx(HWND hwnd, DWORD dwCtrl, const wchar_t* wcpText);
	//----- 07.10.30 Fukushiro M. 追加終 ()-----

#endif

	//----- 14.09.22 Fukushiro M. 追加始 ()-----
	static int UndoEdit(CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer, int iCurUndoBuffer);
	static int RedoEdit(CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer, int iCurUndoBuffer);
	//----- 14.09.22 Fukushiro M. 追加終 ()-----

#if 0
	extern void FFPopupSubmenu(CPoint scpt, DWORD dwMenuId, int iSubmenuIdx, CWnd* pParent, DWORD dwTopCommand = DWORD(-1));
	extern void FFPopupSubmenu(const DBLPoint& mmpt, DWORD dwMenuId, int iSubmenuIdx, CWnd* pParent, DWORD dwTopCommand = DWORD(-1));

	//----------<#DD VIEWER NOT EXPORT>-----------
	extern void FFInitKeyboardTable(const wchar_t* tcpPath, HMENU hMenu, HMENU hScMenu);
	// 14.09.12 Fukushiro M. 1行削除 ()
	//extern void FFInitHandsTable (astring& astrBuff, const wchar_t* tcpPath, long& lCurLine);

	//----- コマンド文字列変換 -----
	extern DWORD FFCommandStringToId(const char* cpCommandString);
	extern const char* FFCommandIdToString(DWORD dwCommandId);
	extern std::wstring FFCommandIdToTitle(DWORD dwCommandId, BOOL bSecond);
	extern void FFRegisterCommand(const char* cpCommandString,
		DWORD dwCommandId,
		const wchar_t* wcpCommandTitle);
	extern void FFUnregisterCommand(const char* cpCommandString);
	extern std::wstring FFCommandStringToErrorMessage(const char* cpCommandString);

	//----- リソース文字列変換 -----
	extern DWORD FFIdsStringToId(const char* cpIdsString);
	extern const char* FFIdsIdToString(DWORD dwIdsId);

	//----- 14.09.12 Fukushiro M. 削除始 ()-----
	//extern void FFInitUserAccelerator (astring& strBuff, BOOL bLeft, const wchar_t* tcpPath,
	//									long& lCurLine, const DWORDSet& stCommandId);
	//----- 14.09.12 Fukushiro M. 削除終 ()-----
	extern long FFGetUserAccelerator(const ACCEL*& pAccel, long& lAccel);
	// 14.09.12 Fukushiro M. 1行追加 ()
	extern long FFGetScUserAccelerator(const ACCEL*& pAccel, long& lAccel);

	// ショートカットキー情報を書き込む。
	extern void FFAddShortcutText(HMENU hMenu, const ACCEL* pAccel, long lAccel);

	// メッセージパネルを表示。
	// 10.02.02 Fukushiro M. 1行変更 ()
	//extern void FFShowMsgPanel (DWORD dwId, const wchar_t* tcpMsg = NULL);
	// 10.02.13 Fukushiro M. 1行変更 ()
	//extern void FFShowMsgPanel (DWORD dwId, const wchar_t* tcpMsg = NULL, long lSurvivalMsec = 1000);
	extern void FFShowMsgPanel(DWORD dwId,
		const wchar_t* tcpMsg = NULL,
		HWND hWnd = NULL,
		long lSurvivalMsec = 1000,
		// 10.02.28 Fukushiro M. 1行追加 ()
		const CPoint ptShow = CPoint(LONG_MAX, LONG_MAX));

	// ファイルダイアログのフィルタ文字列（m_ofn.lpstrFilter）を取得。
	extern void FFGetFileDialogFilterString(CString& strFilter, BOOL bIsSave);

	//----- 06.02.26 Fukushiro M. 削除始 ()-----
	//// OnUpdatePopupMenu 中のマクロ。
	//#define ON_UPDATE_POPUP_MENU(id, val) \
	//	pMenu->EnableMenuItem(id, MF_BYCOMMAND | (val) ? MF_ENABLED : MF_GRAYED);
	//----- 06.02.26 Fukushiro M. 削除終 ()-----

	//----------</#DD VIEWER NOT EXPORT>-----------

#endif//0

/*************************************************************************
 * <関数>	FFInitDlgLayout
 *
 * <機能>	リサイズ可能なダイアログにおいて、各コントロールの位置や
 *			サイズを適切な値に保つための関数。
 *
 *             A                            B
 *          ├──┤                      ├─┤
 *        ┬┌────────────────┐┬
 *       C││                                ││D
 *        ┴│    ┌──┐  ┌──────┐  │┴
 *          │    │    │  │            │  │
 *          │    │    │  │            │  │
 *          │    └──┘  │            │  │
 *          │              │            │  │
 *          │              │            │  │
 *          │              └──────┘  │
 *          │                                │
 *          └────────────────┘
 *
 * <履歴>	08.08.05 Fukushiro M. 作成
 *************************************************************************/
	static void InitDlgLayout(IdToDlgLayoutRecMap& mpLayoutInfo,
		HWND hWnd,
		long lControlSize,
		const long aControlId[],
		DWORD dwOffsetFlag);
	static void DlgLayout(const IdToDlgLayoutRecMap& mpLayoutInfo,
		HWND hWnd);


#if 0

	//----- 07.11.01 Fukushiro M. 追加始 ()-----
	extern xtmc FFGetRevisedDlgItemXtmc(xtmc xtmCurValue,
		xtmc xtmMinValue,
		BOOL bIsIncludeMinEq,
		xtmc xtmMaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);

	//----- 14.02.17 Fukushiro M. 追加始 ()-----
	extern xtmc8 FFGetRevisedDlgItemXtmc8(xtmc8 xtm8CurValue,
		xtmc8 xtm8MinValue,
		BOOL bIsIncludeMinEq,
		xtmc8 xtm8MaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);

	extern double FFGetRevisedDlgItemDouble(double dCurValue,
		double dMinValue,
		BOOL bIsIncludeMinEq,
		double dMaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);
	//----- 14.02.17 Fukushiro M. 追加終 ()-----

	extern hixc FFGetRevisedDlgItemHixc(hixc hixCurValue,
		hixc hixMinValue,
		BOOL bIsIncludeMinEq,
		hixc hixMaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);
	extern dblpnt FFGetRevisedDlgItemDblpnt(dblpnt dbptCurValue,
		dblpnt dbptMinValue,
		BOOL bIsIncludeMinEq,
		dblpnt dbptMaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);
	extern __int64 FFGetRevisedDlgItem__int64(__int64 llCurValue,
		__int64 llMinValue,
		BOOL bIsIncludeMinEq,
		__int64 llMaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);
	extern DWORD FFGetRevisedDlgItemDWORD(DWORD dwCurValue,
		DWORD dwMinValue,
		BOOL bIsIncludeMinEq,
		DWORD dwMaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);
	extern long FFGetRevisedDlgItemLong(long lCurValue,
		long lMinValue,
		BOOL bIsIncludeMinEq,
		long lMaxValue,
		BOOL bIsIncludeMaxEq,
		BOOL bIsSimpleRevise);

	//----- 07.11.01 Fukushiro M. 追加終 ()-----

	//----- 10.02.20 Fukushiro M. 変更前 ()-----
	//extern void FFTabFrameInit (FTTabFrameVector& vtc,
	//							HWND hWndDlg,
	//							long lTabControlId,
	//							long iFrameCount,
	//							const int aFrameId[]);
	//
	//extern void FFTabFrameSelChanged (	const FTTabFrameVector& vtc,
	//									HWND hWndDlg,
	//									long lTabControlId);
	//
	//extern BOOL FFTabFrameGetItemText (	const FTTabFrameVector& vtc,
	//									long lTabControlId,
	//									std::wstring& wstrText);
	//
	//extern BOOL FFTabFrameSetItemText (	FTTabFrameVector& vtc,
	//									HWND hWndDlg,
	//									long lTabControlId,
	//									const wchar_t* wcpText);
	//----- 10.02.20 Fukushiro M. 変更後 ()-----
	extern void FFTabFrameInit(FTTabFrameData& vtc,
		HWND hWndDlg,
		long lTabControlId,
		int iFrameCount,
		const int aFrameId[]);

	extern void FFTabFrameSelChanged(const FTTabFrameData& vtc,
		HWND hWndDlg,
		long lTabControlId);

	extern BOOL FFTabFrameSetItemText(FTTabFrameData& vtc,
		HWND hWndDlg,
		long lTabControlId,
		const wchar_t* wcpText);
	//----- 10.02.20 Fukushiro M. 変更終 ()-----

	//----- 10.02.21 Fukushiro M. 追加始 ()-----
	extern HBRUSH FFTabFrameCtlColor(const FTTabFrameData& vtc,
		HWND hWndDlg,
		long lTabControlId,
		long lChildControlId,
		UINT nCtlColor);
	//----- 10.02.21 Fukushiro M. 追加終 ()-----

	//----- 14.09.09 Fukushiro M. 追加始 ()-----
	// コントロールを適切な大きさにリサイズする。
	extern void FFResizeToIdeal(CWnd* control);
	extern CSize FFGetIdealSize(CWnd* control);
	//----- 14.09.09 Fukushiro M. 追加終 ()-----

	////////////////////////////////////////////////////////////////////////
	// template関数。

	// UNICODEで文字列を追加。
	template<class T> int FFAddString(T* pCtrl, const wchar_t* wstr)
	{
		return pCtrl->AddString(WStrToTStr(wstr));
	} // template<class T> int FFAddString.

	// UNICODEで文字列を追加。
	template<class T> int FFAddStringData(T* pCtrl, const wchar_t* wstr, DWORD_PTR itemData = 0)
	{
		const int index = pCtrl->AddString(WStrToTStr(wstr));
		pCtrl->SetItemData(index, itemData);
		return index;
	} // template<class T> int FFAddStringData.

	// UNICODEで文字列を追加。
	template<class T> void FFReplaceSel(T* pCtrl, const wchar_t* wstr, BOOL bCanUndo = FALSE)
	{
		pCtrl->ReplaceSel(WStrToTStr(wstr), bCanUndo);
	} // template<class T> void FFReplaceSel.

	template<class T> long FFInsertItem(T* pCtrl, long lItem, const wchar_t* wstr)
	{
		return pCtrl->InsertItem(lItem, WStrToTStr(wstr));
	} // template<class T> void FFInsertItem.

	//----- 06.10.24 Fukushiro M. 追加始 ()-----
	template<class T> std::wstring FFGetText(T* pCtrl, int iIndex)
	{
		CString str;
		pCtrl->GetText(iIndex, str);
		return (const wchar_t*)str;
	} // template<class T> void FFGetText.

	template<class T> int FFInsertString(T* pCtrl, int iIndex, const wchar_t* wstr)
	{
		return pCtrl->InsertString(iIndex, WStrToTStr(wstr));
	} // template<class T> int FFInsertString.
	//----- 06.10.24 Fukushiro M. 追加終 ()-----


#endif
	static std::wstring GetItemText(CTabCtrl* tabCtrl, int index)
	{
		wchar_t textBuff[200];
		TCITEM item;
		item.mask = TCIF_TEXT;
		item.pszText = textBuff;
		item.cchTextMax = _countof(textBuff);
		tabCtrl->GetItem(index, &item);
		return textBuff;
	}

};
