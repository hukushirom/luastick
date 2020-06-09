#pragma once

#include "Astrwstr.h"

struct FCDiffRecW;

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
	static int GetCurSel (const CListCtrl* pListCtrl)
	{
		POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
		if (pos == NULL) return -1;
		return pListCtrl->GetNextSelectedItem(pos);
	} // int FfGetCurSel (const CListCtrl* pListCtrl)

	static int InsertColumn(CListCtrl* pCtrl, int nCol, const wchar_t* lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1)
	{
		return pCtrl->InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
	}

	// UNICODEで文字列を取得。
	static std::wstring& GetDlgItemText(const CWnd* pWnd, DWORD dwCtrl, std::wstring& wstr)
	{
		CString str;
		pWnd->GetDlgItemText(dwCtrl, str);
		wstr = str;
		return wstr;
	}

	// UTF-8で文字列を取得。
	static std::string& GetDlgItemText(const CWnd* pWnd, DWORD dwCtrl, std::string& astr)
	{
		std::wstring wstr;
		UtilDlg::GetDlgItemText(pWnd, dwCtrl, wstr);
		Astrwstr::wstr_to_astr(astr, wstr);
		return astr;
	}

	static int UndoEdit(CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer, int iCurUndoBuffer);
	static int RedoEdit(CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer, int iCurUndoBuffer);

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
