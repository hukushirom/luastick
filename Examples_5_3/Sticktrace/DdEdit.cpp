// DdEdit.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "UtilStr.h"	// CompText.
#include "UtilWin.h"	// For FFGetWindowText
#include "UtilDlg.h"	// For FFUndoEdit.
#include "DdEdit.h"		// This header.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCDdEdit プロパティ ページ

IMPLEMENT_DYNCREATE(CFCDdEdit, CEdit)

BEGIN_MESSAGE_MAP(CFCDdEdit, BASE_CLASS)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT_EX(EN_UPDATE, &CFCDdEdit::OnUpdate)
	ON_COMMAND(ID_EDIT_UNDO, &CFCDdEdit::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CFCDdEdit::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CFCDdEdit::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CFCDdEdit::OnUpdateEditRedo)
END_MESSAGE_MAP()

//********************************************************************************************
/*!
 * @brief	コンストラクター。
 * @author	Fukushiro M.
 * @date	2014/09/12(金) 09:55:03
 *
 * @return	なし (none)
 */
//********************************************************************************************
CFCDdEdit::CFCDdEdit ()
		:m_dwMenuId(DWORD(-1))
		,m_iSubmenuIndex(-1)
		,m_bIsUndoable(FALSE)
		,m_iCurUndoBuffer(0)	// Undoバッファー現在位置。
{
} // CFCDdEdit::CFCDdEdit

//********************************************************************************************
/*!
 * @brief	デストラクタ。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 09:24:06
 *
 * @return	なし (none)
 */
//********************************************************************************************
CFCDdEdit::~CFCDdEdit ()
{
} // CFCDdEdit::CFCDdEdit.

//********************************************************************************************
/*!
 * @brief	IsUndoable のセット。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 09:30:26
 *
 * @param[in]	BOOL	isUndoable	。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::SetIsUndoable (BOOL isUndoable)
{
	m_bIsUndoable = isUndoable;
} // CFCDdEdit::SetIsUndoable.

//********************************************************************************************
/*!
 * @brief	CurrentUndoBuffer の取得。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 15:25:00
 *
 * @return	int	
 */
//********************************************************************************************
int CFCDdEdit::GetCurrentUndoBuffer () const
{
	return m_iCurUndoBuffer;
} // CFCDdEdit::GetCurrentUndoBuffer.

//********************************************************************************************
/*!
 * @brief	ResetUndoBuffer 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 16:04:20
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::ResetUndoBuffer ()
{
	// Undoバッファーをクリア。
	m_vUndoBuffer.clear();
	m_iCurUndoBuffer = 0;
	UtilWin::GetWindowText(this, m_wstrPreText);
} // CFCDdEdit::ResetUndoBuffer.

//********************************************************************************************
/*!
 * @brief	ContextMenu のセット。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/12(金) 09:55:00
 *
 * @param[in]	DWORD	menuId			。
 * @param[in]	int		submenuIndex	番号。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::SetContextMenu (DWORD menuId, int submenuIndex)
{
	m_dwMenuId = menuId;
	m_iSubmenuIndex = submenuIndex;
} // CFCDdEdit::SetContextMenu.

//********************************************************************************************
/*!
 * @brief	AddUndoBuffer 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 09:43:30
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::AddUndoBuffer ()
{
	std::wstring wstrText;	// テキスト。
	UtilWin::GetWindowText(this, wstrText);
	if (wstrText != m_wstrPreText)
	{
		// Undoバッファー現在位置にリサイズ。
		m_vUndoBuffer.resize(m_iCurUndoBuffer);
		// Undoバッファー追加。
		UtilStr::CompText(m_vUndoBuffer, m_wstrPreText.c_str(), m_wstrPreText.length(), wstrText.c_str(), wstrText.length());
		m_iCurUndoBuffer = m_vUndoBuffer.size();
		m_wstrPreText = wstrText;
	}
} // CFCDdEdit::AddUndoBuffer.

//********************************************************************************************
/*!
 * @brief	PreTranslateMessage 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 08:23:21
 *
 * @param[in,out]	MSG*	pMsg	。
 *
 * @return	BOOL	TRUE:成 / FALSE:否
 */
//********************************************************************************************
BOOL CFCDdEdit::PreTranslateMessage (MSG* pMsg)
{
//----- 14.09.22 Fukushiro M. 削除始 ()-----
//	TRACE(L"CFCDdEdit::PreTranslateMessage %x %x %x\n", pMsg->message, pMsg->wParam, pMsg->lParam);
//	// 無制限Undo && キーダウンの場合。
//	if (m_bIsUndoable && pMsg->message == WM_KEYDOWN)
//	{
//		// キーダウン && CTRL-KEYの場合。
//		if (GetKeyState(VK_CONTROL) < 0)
//		{
//			// 仮想キーを実際のキャラクタに変換。
//			const UINT dwKey = ::MapVirtualKey(pMsg->wParam, 2);
//			// Ctrl+Z の場合。
//			if (dwKey == 'Z')
//			{
//				// Ctrl+Z によるUndoを無効にする。但し、コンテキストメニューによるUndoは無効にできない。
//				// コンテキストメニューのUndoをPreTranslationMessageでチェックしたがそれらしきメッセージが存在しない。
//				// コンテキストメニューを置き換えるしか方法が無い。
//				// EM_UNDOも渡ってきていない。
//				// FALSEを返すことで親ダイアログで処理できる。
//				return FALSE;
//			}
//		}
//	}
//----- 14.09.22 Fukushiro M. 削除終 ()-----
	// Baseclass関数。
	return BASE_CLASS::PreTranslateMessage(pMsg);
} // CFCDdEdit::PreTranslateMessage

//********************************************************************************************
/*!
 * @brief	右ボタンクリックで呼び出される。
 * @author	Fukushiro M.
 * @date	2014/09/12(金) 09:43:39
 *
 * @param[in]	CWnd*	pWnd	ユーザーによってマウスの右ボタンがクリックされた
 *								ウィンドウのハンドルです。
 * @param[in]	CPoint	point	ユーザーによってマウスがクリックされたときの、
 *								カーソルのスクリーン座標位置です。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::OnContextMenu (CWnd* pWnd, CPoint point)
{
//----- 17.09.24 Fukushiro M. 削除始 ()-----
//	// サブメニューをポップアップ。
//	if (m_dwMenuId != DWORD(-1))
//		FFPopupSubmenu(point, m_dwMenuId, m_iSubmenuIndex, GetParent());
//	else
//----- 17.09.24 Fukushiro M. 削除終 ()-----

		BASE_CLASS::OnContextMenu(pWnd, point);
} // CFCDdEdit::OnContextMenu.

//********************************************************************************************
/*!
 * @brief	OnLButtonDblClk 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/12(金) 17:41:19
 *
 * @param[in]	UINT	nFlags	。
 * @param[in]	CPoint	point	。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::OnLButtonDblClk (UINT nFlags, CPoint point)
{
	// CFCDdEditのダブルクリック。
	LRESULT lResult = GetParent()->SendMessage(WM_USER_DDEDIT_DBLCLKED, (WPARAM)GetDlgCtrlID(), (LPARAM)0);

	// Baseclass関数実行。
	if (lResult == 0)
		BASE_CLASS::OnLButtonDblClk(nFlags, point);
} // void CFCDdEdit::OnLButtonDblClk (UINT nFlags, CPoint point)

//********************************************************************************************
/*!
 * @brief	OnUpdate 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 10:39:54
 *
 * @return	BOOL	TRUE:成 / FALSE:否
 */
//********************************************************************************************
BOOL CFCDdEdit::OnUpdate ()
{
	// 無制限Undoの場合はUndoバッファを設定。
	if (m_bIsUndoable)
	{
		// 標準のUndoバッファをクリア。
		EmptyUndoBuffer();
		// 無制限Undoバッファを設定。
		AddUndoBuffer();
	}
	return FALSE;	 // 親にも通知する。
} // CFCTextEdit::OnUpdate.

//********************************************************************************************
/*!
 * @brief	OnEditUndo 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/21(日) 23:53:25
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::OnEditUndo ()
{
	if (m_bIsUndoable)
	//----- 無制限Undoの場合 -----
	{
		// 一時的に無制限Undoを不可にする。FFUndoEdit中でReplaceSelを実行するが、これがEN_UPDATEを発生させOnUpdateを実行するため。
		m_bIsUndoable = FALSE;
		m_iCurUndoBuffer = UtilDlg::UndoEdit(this, m_vUndoBuffer, m_iCurUndoBuffer);
		m_bIsUndoable = TRUE;
		UtilWin::GetWindowText(this, m_wstrPreText);
	} else
	//----- 標準Undoの場合 -----
	{
		Undo();
	}
} // CFCDdEdit::OnEditUndo

//********************************************************************************************
/*!
 * @brief	OnUpdateEditUndo 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/21(日) 23:53:27
 *
 * @param[in,out]	CCmdUI*	pCmdUI	。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::OnUpdateEditUndo (CCmdUI* pCmdUI)
{
} // CFCDdEdit::OnUpdateEditUndo

//********************************************************************************************
/*!
 * @brief	OnEditRedo 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/21(日) 23:53:28
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::OnEditRedo ()
{
	if (m_bIsUndoable)
	//----- 無制限Undoの場合 -----
	{
		// 一時的に無制限Undoを不可にする。FFRedoEdit中でReplaceSelを実行するが、これがEN_UPDATEを発生させOnUpdateを実行するため。
		m_bIsUndoable = FALSE;
		m_iCurUndoBuffer = UtilDlg::RedoEdit(this, m_vUndoBuffer, m_iCurUndoBuffer);
		m_bIsUndoable = TRUE;
		UtilWin::GetWindowText(this, m_wstrPreText);
	} else
	//----- 標準Undoの場合 -----
	{
		Undo();
	}
} // CFCDdEdit::OnEditRedo

//********************************************************************************************
/*!
 * @brief	OnUpdateEditRedo 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/21(日) 23:53:29
 *
 * @param[in,out]	CCmdUI*	pCmdUI	。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCDdEdit::OnUpdateEditRedo (CCmdUI* pCmdUI)
{
} // CFCDdEdit::OnUpdateEditRedo
