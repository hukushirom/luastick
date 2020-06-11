#pragma once

#include "UtilDlg.h"	// For IdToDlgLayoutRecMap.

// DialogBase.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CFCDialogBase ダイアログ

class CFCDialogBase : public CDialogEx
{
protected:
using BASE_CLASS = CDialogEx;

DECLARE_DYNAMIC(CFCDialogBase)

// コンストラクション
public:
	CFCDialogBase ();
	CFCDialogBase (	LPCTSTR lpszTemplateName,
					CWnd* pParentWnd = nullptr);
	CFCDialogBase (	UINT nIDTemplate,
					CWnd* pParentWnd = nullptr,
					BOOL isSizable = FALSE);
public:
	virtual void SetDialogId (UINT dialogId);

protected:
	virtual std::wstring	GetRegKeyName () const;
	virtual void	SizeToRegistered ();
	virtual void	ClearLayout ();
	virtual void	InitLayout (DWORD dwOffsetFlag,
								long lControlId01,
								long lControlId02 = -1,
								long lControlId03 = -1,
								long lControlId04 = -1,
								long lControlId05 = -1,
								long lControlId06 = -1,
								long lControlId07 = -1,
								long lControlId08 = -1,
								long lControlId09 = -1,
								long lControlId10 = -1,
								long lControlId11 = -1,
								long lControlId12 = -1,
								long lControlId13 = -1,
								long lControlId14 = -1,
								long lControlId15 = -1,
								long lControlId16 = -1,
								long lControlId17 = -1,
								long lControlId18 = -1,
								long lControlId19 = -1,
								long lControlId20 = -1);

protected:
	BOOL	m_isSizable;	// サイズ変更可能？
	DWORD	m_nIDTemplate;	// ダイアログリソースID
	DWORD	m_dialogId;		// ダイアログID

	IdToDlgLayoutRecMap	m_mpLayoutInfo;	// レイアウト情報。
	CSize				m_szMinFrame;	// フレームサイズの最小値。

// ダイアログ データ

// オーバーライド
protected:
	virtual void DoDataExchange (CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

// インプリメンテーション
public:
	virtual BOOL OnInitDialog ();
	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy ();
	afx_msg void OnSize (UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo (MINMAXINFO FAR* lpMMI);
//	virtual void OnOK();
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
}; // class CFCDialogBase.
