#pragma once

#include "UtilDlg.h"	// For FCDiffRecW.

class CFCDdEdit : public CEdit
{
protected:
	using BASE_CLASS = CEdit;

	DECLARE_DYNCREATE(CFCDdEdit)

	CFCDdEdit ();
	virtual ~CFCDdEdit ();
	virtual void SetIsUndoable (BOOL isUndoable);
	virtual int GetCurrentUndoBuffer () const;
	virtual void ResetUndoBuffer ();

// ユーザー関数。
public:
	virtual void SetContextMenu (DWORD menuId, int submenuIndex);
protected:
	virtual void AddUndoBuffer ();

protected:
	virtual BOOL PreTranslateMessage (MSG* pMsg);

protected:
	DWORD	m_dwMenuId;			// コンテキストメニューID。
	int		m_iSubmenuIndex;	// コンテキストメニューのサブメニューインデックス。
	BOOL	m_bIsUndoable;		// 無制限のUndoが可能か？
	std::vector<FCDiffRecW> m_vUndoBuffer;	// Undoバッファー。
	int		m_iCurUndoBuffer;	// Undoバッファー現在位置。
	std::wstring m_wstrPreText;		// Undo前のテキスト。

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnContextMenu (CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
	afx_msg BOOL OnUpdate ();
	afx_msg void OnEditUndo ();
	afx_msg void OnUpdateEditUndo (CCmdUI* pCmdUI);
	afx_msg void OnEditRedo ();
	afx_msg void OnUpdateEditRedo (CCmdUI* pCmdUI);
}; // class CFCDdEdit : public SUPER_CLASS

