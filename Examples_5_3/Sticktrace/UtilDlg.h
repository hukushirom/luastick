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
	static int GetCurSel (const CListCtrl* pListCtrl);

	static int InsertColumn(CListCtrl* pCtrl, int nCol, const wchar_t* lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);

	// UNICODEで文字列を取得。
	static std::wstring& GetDlgItemText(const CWnd* pWnd, DWORD dwCtrl, std::wstring& wstr);

	// UTF-8で文字列を取得。
	static std::string& GetDlgItemText(const CWnd* pWnd, DWORD dwCtrl, std::string& astr);

	/// <summary>
	/// EditコントロールにUndoを実行する。
	/// </summary>
	/// <param name="edit">Undo対象のEditコントロール。</param>
	/// <param name="vUndoBuffer">Undoバッファ。</param>
	static void UndoEdit (CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer);

	/// <summary>
	/// EditコントロールにRedoを実行する。
	/// </summary>
	/// <param name="edit">Redo対象のEditコントロール。</param>
	/// <param name="vUndoBuffer">Undoバッファ。</param>
	static void RedoEdit (CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer);

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

	static void DlgLayout(const IdToDlgLayoutRecMap& mpLayoutInfo, HWND hWnd);

	static std::wstring GetItemText(CTabCtrl* tabCtrl, int index);

	/// <summary>
	/// Get the array of every monitor's rect.
	/// </summary>
	/// <param name="vMonitorRect">Array of every monitor's rect</param>
	static void GetMonitorRect (std::vector<CRect>& vMonitorRect);

	/// <summary>
	/// If top left of the specified rect is out of monitor, justify the rect to fit the monitor.
	/// </summary>
	/// <param name="winRect">Window's rect</param>
	/// <returns>Moving distance to justify the rect</returns>
	static CSize JustifyToMonitor (const CRect & winRect);
};
