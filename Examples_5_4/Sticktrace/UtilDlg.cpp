#include "stdafx.h"
#include "LeException.h"
#include "UtilStr.h"
#include "UtilDlg.h"			// This header.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int UtilDlg::GetCurSel (const CListCtrl* pListCtrl)
{
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (pos == NULL) return -1;
	return pListCtrl->GetNextSelectedItem(pos);
} // int FfGetCurSel (const CListCtrl* pListCtrl)

int UtilDlg::InsertColumn(CListCtrl* pCtrl, int nCol, const wchar_t* lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	return pCtrl->InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
}

// UNICODEで文字列を取得。
std::wstring& UtilDlg::GetDlgItemText(const CWnd* pWnd, DWORD dwCtrl, std::wstring& wstr)
{
	CString str;
	pWnd->GetDlgItemText(dwCtrl, str);
	wstr = str;
	return wstr;
	}

// UTF-8で文字列を取得。
std::string& UtilDlg::GetDlgItemText(const CWnd* pWnd, DWORD dwCtrl, std::string& astr)
{
	std::wstring wstr;
	UtilDlg::GetDlgItemText(pWnd, dwCtrl, wstr);
	Astrwstr::wstr_to_astr(astr, wstr);
	return astr;
}

//********************************************************************************************
/*!
 * @brief	EditコントロールにUndoを実行する。
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 09:51:25
 *
 * @param[in]	CEdit*							edit			Undo対象のEditコントロール。
 * @param[in]	const std::vector<FCDiffRecW>&	vUndoBuffer		Undoバッファ。
 * @param[in]	int								iCurUndoBuffer	Undoバッファの現在位置。Undo対象の
 *																バッファの1つ後を指す。
 *
 * @return	int	新しいUndoバッファの位置。
 */
//********************************************************************************************
void UtilDlg::UndoEdit (CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer)
{
	for (auto irec = vUndoBuffer.rbegin(); irec != vUndoBuffer.rend(); irec++)
	{
		if (irec->cmd == FCDiffRecW::INS)
		{	//----- 挿入の場合 -----
			edit->SetSel(irec->begin, irec->begin + irec->text.length());
			edit->ReplaceSel(L"");
		}
		else
		{	//----- 削除の場合 -----
			edit->SetSel(irec->begin, irec->begin);
			edit->ReplaceSel(irec->text.c_str());
		}
	}
} // FFUndoEdit

//********************************************************************************************
/*!
 * @brief	EditコントロールにRedoを実行する。
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 09:51:25
 *
 * @param[in]	CEdit*							edit			Redo対象のEditコントロール。
 * @param[in]	const std::vector<FCDiffRecW>&	vUndoBuffer		Undoバッファ。
 * @param[in]	int								iCurUndoBuffer	Undoバッファの現在位置。Redo対象の
 *																バッファの1つ前を指す。
 *
 * @return	int	新しいUndoバッファの位置。
 */
//********************************************************************************************
void UtilDlg::RedoEdit (CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer)
{
	for (auto irec = vUndoBuffer.begin(); irec != vUndoBuffer.end(); irec++)
	{
		if (irec->cmd == FCDiffRecW::INS)
		{	//----- 挿入の場合 -----
			edit->SetSel(irec->begin, irec->begin);
			edit->ReplaceSel(irec->text.c_str());
		}
		else
		{	//----- 削除の場合 -----
			edit->SetSel(irec->begin, irec->begin + irec->text.length());
			edit->ReplaceSel(L"");
		}
	}
} // FFRedoEdit

/*************************************************************************
 * <関数>	FFInitDlgLayout
 *
 * <機能>	ダイアログのコントロールの初期位置によって、レイアウト情報
 *			mpLayoutInfo を設定する。
 *
 * <引数>	mpLayoutInfo	:レイアウト情報。設定して返す。
 *			pWnd			:ダイアログウィンドウを指定。
 *			lControlSize	:コントロールの数を指定。
 *			aControlId		:コントロールのID配列を指定。
 *			dwOffsetFlag	:コントロールがダイアログのどの辺に追従するか
 *							 指定。
 *
 * <解説>	FFInitDlgLayout関数、FFInitDlgLayout関数は、リサイズ可能な
 *			ダイアログにおいて、リサイズ時のコントロールの再配置を
 *			自動化する。
 *			FFInitDlgLayout関数は、コントロールの再配置に必要な
 *			レイアウト情報をIdToDlgLayoutRecMap型変数に蓄積する。
 *			FFInitDlg関数は、コントロールの再配置を実行する。
 *			下図の場合を考える。
 *
 *             ┏━━━━━━━━━━━━━━━━━┓
 *             ┃┌───────────────┐┃
 *             ┃│                              │┃
 *             ┃│            IDC_A             │┃
 *             ┃│                              │┃
 *             ┃└───────────────┘┃
 *             ┃┌───┐    ┌───┐┌───┐┃
 *             ┃│IDC_B │    │IDC_C ││IDC_D │┃
 *             ┃└───┘    └───┘└───┘┃
 *             ┗━━━━━━━━━━━━━━━━━┛ 
 *                             ┌─┐                 
 *                           ┌┘  └┐               
 *                            ＼    ／                
 *                              ＼／                  
 *          ┏━━━━━━━━━━━━━━━━━━━━━┓
 *          ┃┌───────────────────┐┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃│                 IDC_A                │┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃└───────────────────┘┃
 *          ┃┌───┐            ┌───┐┌───┐┃
 *          ┃│IDC_B │            │IDC_C ││IDC_D │┃
 *          ┃└───┘            └───┘└───┘┃
 *          ┗━━━━━━━━━━━━━━━━━━━━━┛
 *
 *			IDC_A：ダイアログの四方に合わせてリサイズ
 *			IDC_B：ダイアログの左下に合わせて移動
 *			IDC_C：ダイアログの右下に合わせて移動
 *			IDC_D：ダイアログの右下に合わせて移動
 *
 *			この場合は、以下のように、FFInitDlgLayout関数を３回実行する。
 *			--------------------------------------------------------
 *			OnInitDialog ()
 *			{
 *				const long aId1[] = { IDC_A }
 *				const long aId2[] = { IDC_B }
 *				const long aId3[] = { IDC_C, IDC_D }
 *				FFInitDlgLayout(mpInfo, pWnd, lSize1, aId1,
 *								FCDlgLayoutRec::HOOK_LEFT |
 *								FCDlgLayoutRec::HOOK_TOP |
 *								FCDlgLayoutRec::HOOK_RIGHT |
 *								FCDlgLayoutRec::HOOK_BOTTOM);
 *				FFInitDlgLayout(mpInfo, pWnd, lSize2, aId2,
 *								FCDlgLayoutRec::HOOK_LEFT |
 *								FCDlgLayoutRec::HOOK_BOTTOM);
 *				FFInitDlgLayout(mpInfo, pWnd, lSize3, aId3,
 *								FCDlgLayoutRec::HOOK_RIGHT |
 *								FCDlgLayoutRec::HOOK_BOTTOM);
 *			}
 *			--------------------------------------------------------
 *
 *			コントロールの再配置は以下のようにFFDlgLayout関数を１回実行する。
 *			--------------------------------------------------------
 *			OnSize (int cx, int cy)
 *			{
 *				FFDlgLayout(mpInfo, pWnd, CSize(cx, cy));
 *			}
 *			--------------------------------------------------------
 *
 * <履歴>	05.11.11 Fukushiro M. 作成
 *************************************************************************/
void UtilDlg::InitDlgLayout (
	IdToDlgLayoutRecMap& mpLayoutInfo,
	HWND hWnd,
	long lControlSize,
	const long aControlId[],
	DWORD dwOffsetFlag
)
{
	const CWnd* pWnd = CWnd::FromHandle(hWnd);
	// ダイアログのウィンドウ領域を取得。
	CRect rtParent;
	pWnd->GetClientRect(rtParent);
	pWnd->ClientToScreen(rtParent);
	long lC;
	for (lC = 0; lC != lControlSize; lC++)
	{
		FCDlgLayoutRec rec(dwOffsetFlag, CRect(LONG_MAX, LONG_MAX, LONG_MAX, LONG_MAX));
		// コントロールのウィンドウ領域をスクリーン座標系で取得。
		CRect rtControl;
		pWnd->GetDlgItem(aControlId[lC])->GetWindowRect(rtControl);
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_LEFT)
			rec.m_rtDistance.left = rtControl.left - rtParent.left;
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_TOP)
			rec.m_rtDistance.top = rtControl.top - rtParent.top;
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_RIGHT)
			rec.m_rtDistance.right = rtParent.right - rtControl.right;
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_BOTTOM)
			rec.m_rtDistance.bottom = rtParent.bottom - rtControl.bottom;
		mpLayoutInfo[aControlId[lC]] = rec;
	}
} // InitDlgLayout.

/*************************************************************************
 * <関数>	FFDlgLayout
 *
 * <機能>	ダイアログのコントロールを再配置する。
 *
 * <引数>	mpLayoutInfo	:レイアウト情報を指定。
 *			pWnd			:ダイアログウィンドウを指定。
 *			szWnd			:新しいダイアログウィンドウサイズを指定
 *
 * <解説>	FFInitDlgLayout関数、FFInitDlgLayout関数は、リサイズ可能な
 *			ダイアログにおいて、リサイズ時のコントロールの再配置を
 *			自動化する。
 *			詳しくはFFInitDlgLayout関数の解説を参照。
 *
 * <履歴>	05.11.11 Fukushiro M. 作成
 *************************************************************************/
void UtilDlg::DlgLayout (const IdToDlgLayoutRecMap& mpLayoutInfo, HWND hWnd)
{
	CWnd* pWnd = CWnd::FromHandle(hWnd);
	// ダイアログのウィンドウ領域を取得。
	CRect rtParent;
	pWnd->GetClientRect(rtParent);

	IdToDlgLayoutRecMap::const_iterator iInfo;
	for (iInfo = mpLayoutInfo.begin(); iInfo != mpLayoutInfo.end(); iInfo++)
	{
		// コントロールの矩形サイズを取得。
		CRect rtControl;
		pWnd->GetDlgItem(iInfo->first)->GetWindowRect(rtControl);
		pWnd->ScreenToClient(rtControl);

		const CSize szControl(rtControl.Width(), rtControl.Height());
		// コントロールの矩形を設定。
		rtControl.SetRect(LONG_MAX, LONG_MAX, LONG_MAX, LONG_MAX);
		if (iInfo->second.m_rtDistance.left != LONG_MAX)
			rtControl.left = rtParent.left + iInfo->second.m_rtDistance.left;
		if (iInfo->second.m_rtDistance.top != LONG_MAX)
			rtControl.top = rtParent.top + iInfo->second.m_rtDistance.top;
		if (iInfo->second.m_rtDistance.right != LONG_MAX)
			rtControl.right = rtParent.right - iInfo->second.m_rtDistance.right;
		if (iInfo->second.m_rtDistance.bottom != LONG_MAX)
			rtControl.bottom = rtParent.bottom - iInfo->second.m_rtDistance.bottom;
		// デフォルトの座標を補正。
		if (rtControl.left == LONG_MAX)
			rtControl.left = rtControl.right - szControl.cx;
		else
		if (rtControl.right == LONG_MAX)
			rtControl.right = rtControl.left + szControl.cx;
		if (rtControl.top == LONG_MAX)
			rtControl.top = rtControl.bottom - szControl.cy;
		else
		if (rtControl.bottom == LONG_MAX)
			rtControl.bottom = rtControl.top + szControl.cy;

		// コントロールをダイアログサイズ変更に合わせて移動。
		pWnd->GetDlgItem(iInfo->first)->MoveWindow(rtControl);
		// コントロールを再描画。MoveWindow では、サイズが変わらない場合、
		// 中身の再描画をしないよう(移動前の場所の再描画はする)なので、
		// ここで強制的に再描画する。
		pWnd->GetDlgItem(iInfo->first)->RedrawWindow();
	}
} // DlgLayout.

std::wstring UtilDlg::GetItemText(CTabCtrl* tabCtrl, int index)
{
	wchar_t textBuff[200];
	TCITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = textBuff;
	item.cchTextMax = _countof(textBuff);
	tabCtrl->GetItem(index, &item);
	return textBuff;
}

void UtilDlg::GetMonitorRect (std::vector<CRect> & vMonitorRect)
{
	vMonitorRect.clear();
	struct My
	{
		static BOOL CALLBACK MonitorEnumProc (
			HMONITOR hMonitor,	// ディスプレイモニタのハンドル。
			HDC hdcMonitor,		// モニタに適したデバイスコンテキストのハンドル。
			LPRECT lprcMonitor,	// モニタ上の交差部分を表す長方形領域へのポインタ。
			LPARAM dwData		// EnumDisplayMonitors から渡されたデータ。
		)
		{
			auto vMonitorRect = (std::vector<CRect> *)dwData;
			MONITORINFO monitorInfo;
			memset(&monitorInfo, 0, sizeof(monitorInfo));
			monitorInfo.cbSize = sizeof(monitorInfo);
			if (::GetMonitorInfo(hMonitor, &monitorInfo))
			{	//----- モニターのワーク領域（タスクバーを除いた領域）を取得 -----
				vMonitorRect->emplace_back(monitorInfo.rcWork);
			} else
			{	//----- モニターのワーク領域（タスクバーを除いた領域）を取得できない場合 -----
				vMonitorRect->emplace_back(*lprcMonitor);
			}
			return TRUE;	// 列挙を続行。
		}; // MonitorEnumProc.
	};
	::EnumDisplayMonitors(NULL, nullptr, My::MonitorEnumProc, LPARAM(&vMonitorRect));
} // UtilDlg::GetMonitorRect.

CSize UtilDlg::JustifyToMonitor (const CRect & winRect)
{
	auto POW2 = [](__int64 x) { return x * x; };
	const CPoint winPoint = winRect.TopLeft() + CSize(50, 50);

//----- 20.07.19 Fukushiro M. 削除始 ()-----
//	/// <summary>
//	/// CRect::PtInRect regards the point as outside if point is on rect.right or rect.bottom.
//	/// This function regards it as inside.
//	/// </summary>
//	/// <param name="rect"></param>
//	/// <param name="point"></param>
//	/// <returns></returns>
//	auto PtInRect = [](const CRect & rect, const CPoint & point)
//	{
//		return (
//			rect.left <= point.x &&
//			point.x <= rect.right &&
//			rect.top <= point.y &&
//			point.y <= rect.bottom
//			);
//	};
//----- 20.07.19 Fukushiro M. 削除終 ()-----

	// モニター領域の配列を取得。
	std::vector<CRect> vMonitorRect;
	UtilDlg::GetMonitorRect(vMonitorRect);

	BOOL isOK = FALSE;
	CRect oneMonRect;
	__int64 powLen = _I64_MAX;
	for (const auto & monRect : vMonitorRect)
	{
		if (monRect.PtInRect(winPoint))
		{
			isOK = TRUE;
			break;
		}
		else
		{
			auto sz = monRect.TopLeft() - winRect.TopLeft();
			auto tmp = POW2(sz.cx) + POW2(sz.cy);
			if (tmp < powLen)
			{
				powLen = tmp;
				oneMonRect = monRect;
			}
		}
	}
	return isOK ? CSize(0, 0) : (oneMonRect.TopLeft() - winRect.TopLeft());
} // UtilDlg::JustifyToMonitor.
