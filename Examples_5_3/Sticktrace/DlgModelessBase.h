#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgModelessBase.h : ヘッダー ファイル
//

#include "DialogBase.h"	// For Baseclass.

/////////////////////////////////////////////////////////////////////////////
// CFCDlgModelessBase ダイアログ

/*************************************************************************
 * <クラス>	CFCDlgModelessBase
 *
 * <機能>	モードレスダイアログのベースクラス。ウィンドウ位置をレジストリに
 *			保存する。
 *
 * <履歴>	05.11.13 Fukushiro M. 作成
 *************************************************************************/
class CFCDlgModelessBase : public CFCDialogBase
{
protected:
	using BASE_CLASS = CFCDialogBase;

	DECLARE_DYNAMIC(CFCDlgModelessBase)

// コンストラクション
public:
	CFCDlgModelessBase ();

// ユーザ関数。
public:
	virtual void	SetWindowVisible ();
	virtual void	SetWindowVisible (BOOL bIsVisible);

public:
	virtual BOOL	Create (UINT nIDTemplate, CWnd* pParentWnd = nullptr, BOOL isSizable = TRUE);

// オーバーライド関数。
protected:
	virtual INT_PTR DoModal () override;

// ユーザ変数。
protected:

// ダイアログ データ
public:
	//{{AFX_DATA(CFCDlgModelessBase)
		// メモ: ClassWizard はこの位置にデータ メンバを追加します。
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CFCDlgModelessBase)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CFCDlgModelessBase)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
 }; // class CFCDlgModelessBase.

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。
