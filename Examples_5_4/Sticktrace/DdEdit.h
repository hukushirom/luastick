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
	virtual BOOL IsReadOnly () const;
	virtual void SetAccelerator (DWORD accelId);
	virtual BOOL IsModified() const;
	virtual void ResetModified ();
	virtual int GetlineHeight ();
	virtual const std::wstring & GetText () const;

	BOOL CanUndo() const;
	BOOL CanRedo() const;

protected:
	virtual void AddUndoBuffer ();

protected:
	HACCEL	m_accelerator;
	DWORD	m_dwMenuId;				// コンテキストメニューID。
	int		m_iSubmenuIndex;		// コンテキストメニューのサブメニューインデックス。
	BOOL	m_bIsUndoable;			// 無制限のUndoが可能か？
	std::vector<std::vector<FCDiffRecW>> m_vUndoBuffer;	// Undoバッファー。
	int		m_iCurUndoBuffer;		// Undoバッファー現在位置。
	std::wstring m_currentText;		// 現在のテキスト。
	int		m_iSavedUndoBuffer;		// Undoバッファーセーブ時位置。
	__int64 m_lastTextEditedMillisec;	// 最後に修正したmsec。

	//
	//            ┌──┬┬────────┐
	//         A─│    ││                │┬
	//            │    ││TEXT TEXT TEXT  ││m_lineHeight
	//            │    ││                │┴
	//            │    ││TEXT TEXT TEXT  │
	//            │ ◇ ││                │
	//            │    ││TEXT TEXT TEXT  │
	//            │ → ││                │
	//            │    ││TEXT TEXT TEXT  │
	//            │    ││                │
	//            │    ││TEXT TEXT TEXT  │
	//            │    ││                │
	//            │    ││TEXT TEXT TEXT  │
	//            └──┴┴────────┘
	//
	int		m_lineHeight;
	BOOL	m_noSetWindowTextNotify;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnContextMenu (CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
	afx_msg BOOL OnUpdate ();
	afx_msg void OnEditUndo ();
	afx_msg void OnUpdateEditUndo (CCmdUI* pCmdUI);
	afx_msg void OnEditRedo ();
	afx_msg void OnUpdateEditRedo (CCmdUI* pCmdUI);
	afx_msg void OnEditCut ();
	afx_msg void OnUpdateEditCut (CCmdUI* pCmdUI);
	afx_msg void OnEditCopy ();
	afx_msg void OnUpdateEditCopy (CCmdUI* pCmdUI);
	afx_msg void OnEditPaste ();
	afx_msg void OnUpdateEditPaste (CCmdUI* pCmdUI);
	afx_msg void OnEditDelete ();
	afx_msg void OnUpdateEditDelete (CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll ();
	afx_msg void OnUpdateEditSelectAll (CCmdUI* pCmdUI);
	afx_msg void OnEditClear ();
	afx_msg void OnUpdateEditClear (CCmdUI* pCmdUI);
	afx_msg LRESULT OnUserSettextNotify (WPARAM, LPARAM);
	afx_msg void OnDestroy ();
	virtual LRESULT WindowProc (UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage (MSG* pMsg);
}; // class CFCDdEdit : public SUPER_CLASS

