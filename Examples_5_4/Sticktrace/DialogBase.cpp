// DialogBase.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "UtilString.h"		// For UtilString::Format.
#include "RegBase.h"		// For FFRegBase.
#include "DialogBase.h"		// This header.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCDialogBase ダイアログ

IMPLEMENT_DYNAMIC(CFCDialogBase, CDialogEx)

CFCDialogBase::CFCDialogBase ()
	: BASE_CLASS()
	, m_isSizable(FALSE)		// サイズ変更可能？
	, m_nIDTemplate(DWORD(-1))	// ダイアログリソースID
	, m_dialogId(DWORD(-1))		// Dialog id. Application identifies this dialog using this ID.
	, m_szMinFrame(0,0)			// フレームサイズの最小値。
{
} // CFCDialogBase::CFCDialogBase.

CFCDialogBase::CFCDialogBase (LPCTSTR lpszTemplateName, CWnd* pParentWnd)
	: BASE_CLASS(lpszTemplateName, pParentWnd)
	, m_isSizable(FALSE)		// サイズ変更可能？
	, m_nIDTemplate(DWORD(-1))	// ダイアログリソースID
	, m_dialogId(DWORD(-1))		// Dialog id. Application identifies this dialog using this ID.
	, m_szMinFrame(0, 0)		// フレームサイズの最小値。
{
} // CFCDialogBase::CFCDialogBase.

CFCDialogBase::CFCDialogBase (
	UINT nIDTemplate,
	CWnd* pParentWnd,
	BOOL isSizable
)
	: BASE_CLASS(nIDTemplate, pParentWnd)
	, m_isSizable(isSizable)		// サイズ変更可能？
	, m_nIDTemplate(nIDTemplate)	// ダイアログリソースID
	, m_dialogId(DWORD(-1))			// Dialog id. Application identifies this dialog using this ID.
	, m_szMinFrame(0, 0)			// フレームサイズの最小値。
{
} // CFCDialogBase::CFCDialogBase.

void CFCDialogBase::SetDialogId (UINT dialogId)
{
	m_dialogId = dialogId;
}

/*************************************************************************
 * <関数>	CFCDialogBase::GetRegKeyName
 *
 * <機能>	このウィンドウ固有のレジストリ登録名を取得する。
 *
 * <返値>	レジストリ登録名。
 *
 * <解説>	ウィンドウに設定されたIDを元にユニークな名前を作成する。
 *
 * <履歴>	06.02.22 Fukushiro M. 作成
 *************************************************************************/
std::wstring CFCDialogBase::GetRegKeyName () const
{
	ASSERT(m_nIDTemplate != DWORD(-1));
	return UtilString::Format(L"CFCDialogBase-%x", (m_dialogId != DWORD(-1)) ? m_dialogId : m_nIDTemplate);
} // CFCDialogBase::GetRegKeyName.

//----- 14.08.27 Fukushiro M. 追加始 ()-----
//********************************************************************************************
/*!
 * @brief	レジストリに登録されたサイズ情報に合わせてリサイズ。
 *
 * @author	Fukushiro M.
 * @date	2014/08/27(水) 23:29:40
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDialogBase::SizeToRegistered ()
{
	if (m_isSizable)
	//----- サイズ変更可能なウィンドウの場合 -----
	{
		// ウィンドウの位置をレジストリから読み出す。
		CRect rtWindow;
		const DWORD dwDataSz = FCRegBase::LoadRegBinary(
												GetRegKeyName().c_str(),
												L"Rect", (LPBYTE)&rtWindow, sizeof(rtWindow));
		if (dwDataSz == sizeof(rtWindow))
		//----- レジストリからウィンドウ座標が読み出せた場合 -----
		{
			// ウィンドウ座標をモニター位置に補正。
			// マルチモニター環境を考慮。
			rtWindow += UtilDlg::JustifyToMonitor(rtWindow);
			// ウィンドウを移動。
			MoveWindow(rtWindow);
		}
	}
} // CFCDialogBase::SizeToRegistered.

//********************************************************************************************
/*!
 * @brief	Layout情報のクリア。
 * @author	Fukushiro M.
 * @date	2014/09/03(水) 18:41:38
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDialogBase::ClearLayout ()
{
	m_mpLayoutInfo.clear();
} // CFCDialogBase::ClearLayout.

//********************************************************************************************
/*!
 * @brief	サイズ変更ダイアログの場合、どのコントロールをどの角にフックするか設定する。
 * @author	Fukushiro M.
 * @date	2014/08/28(木) 16:34:08
 *
 * @param[in]	DWORD	dwOffsetFlag	フック先を指定。例）左下の場合
 *										FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_BOTTOM
 * @param[in]	long	lControlId01～	フック対象のコントロールID。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDialogBase::InitLayout (DWORD dwOffsetFlag,
								long lControlId01,
								long lControlId02,
								long lControlId03,
								long lControlId04,
								long lControlId05,
								long lControlId06,
								long lControlId07,
								long lControlId08,
								long lControlId09,
								long lControlId10,
								long lControlId11,
								long lControlId12,
								long lControlId13,
								long lControlId14,
								long lControlId15,
								long lControlId16,
								long lControlId17,
								long lControlId18,
								long lControlId19,
								long lControlId20)
{
	const long ary[] = { lControlId01, lControlId02, lControlId03, lControlId04, lControlId05, lControlId06, lControlId07, lControlId08, lControlId09, lControlId10, lControlId11, lControlId12, lControlId13, lControlId14, lControlId15, lControlId16, lControlId17, lControlId18, lControlId19, lControlId20, };
	int arySize = 0;
	for (arySize = 0; arySize != _countof(ary); arySize++)
		if (ary[arySize] == -1) break;
	UtilDlg::InitDlgLayout(m_mpLayoutInfo, GetSafeHwnd(), arySize, ary, dwOffsetFlag);
} // CFCDialogBase::InitLayout.

void CFCDialogBase::DoDataExchange (CDataExchange* pDX)
{
	BASE_CLASS::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFCDialogBase)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_MAP
} // CFCDialogBase::DoDataExchange.


BEGIN_MESSAGE_MAP(CFCDialogBase, BASE_CLASS)
	//{{AFX_MSG_MAP(CFCDialogBase)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCDialogBase メッセージ ハンドラ

BOOL CFCDialogBase::OnInitDialog ()
{
	// Superclass関数実行。
	if (!BASE_CLASS::OnInitDialog())
		return FALSE;

	//----- フレームサイズの最小値を計算 -----
	CRect rtStone;
	GetWindowRect(rtStone);
	m_szMinFrame.cx = rtStone.Width();
	m_szMinFrame.cy = rtStone.Height();

	return TRUE;
} // CFCDialogBase::OnInitDialog

int CFCDialogBase::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (BASE_CLASS::OnCreate(lpCreateStruct) == -1)
		return -1;
//----- 17.09.21 Fukushiro M. 削除始 ()-----
//	// ホバーヒントのターゲットに登録。
//	FFHoverHint()->RegisterHoverHintTarget(m_hWnd);
//----- 17.09.21 Fukushiro M. 削除終 ()-----
	return 0;
} // CFCDialogBase::OnCreate.

void CFCDialogBase::OnDestroy ()
{
//----- 17.09.21 Fukushiro M. 削除始 ()-----
//	// ホバーヒントのターゲットから登録解除。
//	FFHoverHint()->UnregisterHoverHintTarget(m_hWnd);
//----- 17.09.21 Fukushiro M. 削除終 ()-----

	if (m_isSizable)
	//----- サイズ変更可能なウィンドウの場合 -----
	{
		// ウィンドウの位置をレジストリに記録。
		CRect rtWindow;
		GetWindowRect(rtWindow);
		FCRegBase::SaveRegBinary(	GetRegKeyName().c_str(),
									L"Rect", (LPBYTE)&rtWindow, sizeof(rtWindow));
	}

	// Baseclass関数。
	BASE_CLASS::OnDestroy();
} // CFCDialogBase::OnDestroy.

//********************************************************************************************
/*!
 * @brief	サイズ変更時に実行される。
 * @author	Fukushiro M.
 * @date	2014/08/17(日) 17:40:40
 *
 * @param[in]	UINT	nType	タイプ。
 * @param[in]	int		cx		。
 * @param[in]	int		cy		。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDialogBase::OnSize (UINT nType, int cx, int cy)
{
	// Superclass関数実行。
	BASE_CLASS::OnSize(nType, cx, cy);
	// OnInitDialog以前のOnSizeの場合は復帰。
	if (!::IsWindow(m_hWnd)) return;
	// コントロール再配置。
	UtilDlg::DlgLayout(m_mpLayoutInfo, GetSafeHwnd());
} // CFCDialogBase::OnSize.

//********************************************************************************************
/*!
 * @brief	WM_GETMINMAXINFOメッセージで実行。ウィンドウの最大値、最小値の情報を返す。リサイズ時等に利用。
 * @author	Fukushiro M.
 * @date	2014/08/17(日) 17:39:58
 *
 * @param[in,out]	MINMAXINFO FAR*	lpMMI	最大、最小値等を返す変数を指定。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDialogBase::OnGetMinMaxInfo (MINMAXINFO FAR* lpMMI)
{
	// Superclass関数実行。
	BASE_CLASS::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = m_szMinFrame.cx;
	lpMMI->ptMinTrackSize.y = m_szMinFrame.cy;
} // CFCDialogBase::OnGetMinMaxInfo.


// LRESULT CFCDialogBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
// {
//	 TRACE(L"CFCDialogBase::WindowProc %x %x %x\r\n", message, wParam, lParam);
//
//	 auto status = GetQueueStatus(QS_ALLINPUT);
//	 if (HIWORD(status) == 0)
//		 PostMessage(WM_IDLEUPDATECMDUI);
//
//	 return BASE_CLASS::WindowProc(message, wParam, lParam);
// }


 BOOL CFCDialogBase::PreTranslateMessage(MSG* pMsg)
 {
//----- 20.07.30 Fukushiro M. 変更前 ()-----
//	 // Any message is in the queue.
//	 auto status = GetQueueStatus(QS_ALLINPUT);
//	 // The high-order word of the return value indicates the types of messages currently in the queue.
//	if (HIWORD(status) == 0)
//		PostMessage(WM_IDLEUPDATECMDUI);
//----- 20.07.30 Fukushiro M. 変更後 ()-----
	if (pMsg->message != WM_IDLEUPDATECMDUI)
	{
		 // Any message is in the queue.
		 auto status = GetQueueStatus(QS_ALLINPUT);
		 // The high-order word of the return value indicates the types of messages currently in the queue.
		if (HIWORD(status) == 0)
			PostMessage(WM_IDLEUPDATECMDUI);
	}
//----- 20.07.30 Fukushiro M. 変更終 ()-----

	 return BASE_CLASS::PreTranslateMessage(pMsg);
 }
