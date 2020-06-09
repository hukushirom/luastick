﻿// DdEdit.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "resource.h"
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
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT_EX(EN_UPDATE, &CFCDdEdit::OnUpdate)
	ON_COMMAND(ID_EDIT_UNDO, &CFCDdEdit::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CFCDdEdit::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CFCDdEdit::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CFCDdEdit::OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_CUT, &CFCDdEdit::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CFCDdEdit::OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CFCDdEdit::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CFCDdEdit::OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CFCDdEdit::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CFCDdEdit::OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_DELETE, &CFCDdEdit::OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CFCDdEdit::OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CFCDdEdit::OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &CFCDdEdit::OnUpdateEditSelectAll)
	ON_MESSAGE(WM_USER_DDEDIT_SETTEXT_NOTIFY, &CFCDdEdit::OnUserSettextNotify)
	ON_WM_CONTEXTMENU()
//	ON_WM_CREATE()
	ON_WM_DESTROY()
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
	: m_accelerator(NULL)
	, m_dwMenuId(DWORD(-1))
	, m_iSubmenuIndex(-1)
	, m_bIsUndoable(FALSE)
	, m_iCurUndoBuffer(0)	// Undoバッファー現在位置。
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

void CFCDdEdit::SetAccelerator (DWORD accelId)
{
	if (m_accelerator != NULL)
		::DestroyAcceleratorTable(m_accelerator);
	m_accelerator = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(accelId));
}

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

//----- 20.06.03  削除始 ()-----
////********************************************************************************************
///*!
// * @brief	PreTranslateMessage 関数。
// *
// *
// * @author	Fukushiro M.
// * @date	2014/09/22(月) 08:23:21
// *
// * @param[in,out]	MSG*	pMsg	。
// *
// * @return	BOOL	TRUE:成 / FALSE:否
// */
////********************************************************************************************
//BOOL CFCDdEdit::PreTranslateMessage (MSG* pMsg)
//{
//	// Baseclass関数。
//	return BASE_CLASS::PreTranslateMessage(pMsg);
//} // CFCDdEdit::PreTranslateMessage
//
//********************************************************************************************
///*!
// * @brief	右ボタンクリックで呼び出される。
// * @author	Fukushiro M.
// * @date	2014/09/12(金) 09:43:39
// *
// * @param[in]	CWnd*	pWnd	ユーザーによってマウスの右ボタンがクリックされた
// *								ウィンドウのハンドルです。
// * @param[in]	CPoint	point	ユーザーによってマウスがクリックされたときの、
// *								カーソルのスクリーン座標位置です。
// *
// * @return	なし (none)
// */
//********************************************************************************************
//void CFCDdEdit::OnContextMenu (CWnd* pWnd, CPoint point)
//{
//		BASE_CLASS::OnContextMenu(pWnd, point);
//} // CFCDdEdit::OnContextMenu.
//----- 20.06.03  削除終 ()-----

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
	{	//----- 無制限Undoの場合 -----
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
	if (m_bIsUndoable)
	{	//----- 無制限Undoの場合 -----
		pCmdUI->Enable(!IsFlagged(GetStyle(), ES_READONLY) && m_iCurUndoBuffer != 0);
	}
	else
	{
		pCmdUI->Enable(CanUndo());
	}
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
	if (m_bIsUndoable)
	{	//----- 無制限Undoの場合 -----
		pCmdUI->Enable(!IsFlagged(GetStyle(), ES_READONLY) && m_iCurUndoBuffer < (int)m_vUndoBuffer.size());
	}
	else
	{
		pCmdUI->Enable(CanUndo());
	}
} // CFCDdEdit::OnUpdateEditRedo

void CFCDdEdit::OnEditCut()
{
	BASE_CLASS::Cut();
} // CFCDdEdit::OnEditCut.

void CFCDdEdit::OnUpdateEditCut (CCmdUI* pCmdUI)
{
	int startChar, endChar;
	GetSel(startChar, endChar);
	pCmdUI->Enable(!IsFlagged(GetStyle(), ES_READONLY) && startChar < endChar);
} // CFCDdEdit::OnUpdateEditCut.

void CFCDdEdit::OnEditCopy()
{
	BASE_CLASS::Copy();
} // CFCDdEdit::OnEditCopy.

void CFCDdEdit::OnUpdateEditCopy (CCmdUI* pCmdUI)
{
	int startChar, endChar;
	GetSel(startChar, endChar);
	pCmdUI->Enable(startChar < endChar);
} // CFCDdEdit::OnUpdateEditCopy.

void CFCDdEdit::OnEditPaste()
{
	BASE_CLASS::Paste();
} // CFCDdEdit::OnEditPaste.

void CFCDdEdit::OnUpdateEditPaste (CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!IsFlagged(GetStyle(), ES_READONLY) && IsClipboardFormatAvailable(CF_TEXT));
} // CFCDdEdit::OnUpdateEditPaste.

void CFCDdEdit::OnEditDelete()
{
	SendMessage(WM_KEYDOWN, VK_DELETE, 0);
} // CFCDdEdit::OnEditDelete.

void CFCDdEdit::OnUpdateEditDelete (CCmdUI* pCmdUI)
{
	int startChar, endChar;
	GetSel(startChar, endChar);
	pCmdUI->Enable(!IsFlagged(GetStyle(), ES_READONLY) && startChar != GetWindowTextLength());
} // CFCDdEdit::OnUpdateEditDelete.

void CFCDdEdit::OnEditSelectAll()
{
	SetSel(0, -1);
} // CFCDdEdit::OnEditSelectAll.

void CFCDdEdit::OnUpdateEditSelectAll (CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetWindowTextLength() != 0);
} // CFCDdEdit::OnUpdateEditSelectAll.

LRESULT CFCDdEdit::OnUserSettextNotify(WPARAM, LPARAM)
{
	// 標準のUndoバッファをクリア。
	EmptyUndoBuffer();
	if (m_bIsUndoable)
	{
		// 無制限Undoバッファをクリア。
		ResetUndoBuffer();
	}

	return 1;
} // CFCDdEdit::OnUserSettextNotify.

void CFCDdEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (m_dwMenuId != DWORD(-1) && m_iSubmenuIndex != -1)
	{
		CMenu menu;
		menu.LoadMenu(m_dwMenuId);
		auto subMenu = menu.GetSubMenu(m_iSubmenuIndex);

		for (int i = 0; i != subMenu->GetMenuItemCount(); i++)
		{
			auto id = subMenu->GetMenuItemID(i);
			CCmdUI state;
			state.m_pMenu = subMenu;
			state.m_pOther = this;
			state.m_nIndexMax = subMenu->GetMenuItemCount();
			state.m_nIndex = (UINT)i;
			state.m_nID = id;
			OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, nullptr);
		}

		// ClientToScreen(&point);
		subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	else
	{
		BASE_CLASS::OnContextMenu(pWnd, point);
	}
}


//int CFCDdEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CEdit::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//
//	return 0;
//}


void CFCDdEdit::OnDestroy()
{
	BASE_CLASS::OnDestroy();

	if (m_accelerator != NULL)
		::DestroyAcceleratorTable(m_accelerator);
	m_accelerator = NULL;
}


BOOL CFCDdEdit::PreTranslateMessage(MSG* pMsg)
{
	// コントロールにCtrl+C,Ctrl+V,Ctrl+X,Ctrl+Zを配信する。
	if (m_accelerator != NULL && ::TranslateAccelerator(m_hWnd, m_accelerator, pMsg))
		return TRUE;
	else
		return BASE_CLASS::PreTranslateMessage(pMsg);
}


LRESULT CFCDdEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_SETTEXT)
		PostMessage(WM_USER_DDEDIT_SETTEXT_NOTIFY);

	return BASE_CLASS::WindowProc(message, wParam, lParam);
}
