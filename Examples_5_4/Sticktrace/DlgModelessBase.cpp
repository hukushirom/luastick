// DlgModelessBase.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "RegBase.h"
#include "LeException.h"
#include "DlgModelessBase.h"	// This header.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCDlgModelessBase ダイアログ

IMPLEMENT_DYNAMIC(CFCDlgModelessBase, CFCDialogBase)


/*************************************************************************
 * <関数>	CFCDlgModelessBase::CFCDlgModelessBase
 *
 * <機能>	デフォルトコンストラクタ。
 *
 * <履歴>	05.11.13 Fukushiro M. 作成
 *************************************************************************/
CFCDlgModelessBase::CFCDlgModelessBase ()
					:BASE_CLASS()
{
	//{{AFX_DATA_INIT(CFCDlgModelessBase)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
} // CFCDlgModelessBase::CFCDlgModelessBase.

void CFCDlgModelessBase::DoDataExchange (CDataExchange* pDX)
{
	BASE_CLASS::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFCDlgModelessBase)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFCDlgModelessBase, BASE_CLASS)
	//{{AFX_MSG_MAP(CFCDlgModelessBase)
//	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
// 05.11.13 Fukushiro M. 1行削除 ()
//	ON_WM_CREATE()

/////////////////////////////////////////////////////////////////////////////
// CFCDlgModelessBase ユーザ関数。

/*************************************************************************
 * <関数>	CFCDlgModelessBase::SetWindowVisible
 *
 * <機能>	レジストリの値に従ってウィンドウの表示・非表示を切り替える。
 *
 * <解説>	SetWindowVisible(BOOL)は呼び出し時に表示状態を設定できるが、
 *			この関数はレジストリの値で表示・非表示を設定するだけ。
 *			通常はCreate関数の後に１回だけ実行する。
 *
 * <履歴>	05.11.13 Fukushiro M. 作成
 *************************************************************************/
void CFCDlgModelessBase::SetWindowVisible ()
{
	ASSERT(m_nIDTemplate != DWORD(-1));
	// ウィンドウの表示・非表示をレジストリから読み出す。
	DWORD dwIsVisible;
	if (!FCRegBase::LoadRegDWORD(GetRegKeyName().c_str(), L"Visible", dwIsVisible))
		dwIsVisible = (DWORD)FALSE;
	ShowWindow(dwIsVisible ? SW_SHOW : SW_HIDE);
} // CFCDlgModelessBase::SetWindowVisible.

/*************************************************************************
 * <関数>	CFCDlgModelessBase::SetWindowVisible
 *
 * <機能>	ウィンドウの表示・非表示を切り替える。
 *
 * <引数>	bIsVisible	:表示状態にする場合はTRUE、非表示状態にする場合は
 *						 FALSE を指定。
 *
 * <解説>	表示状態を変更した後、レジストリに表示・非表示状態を記録する。
 *
 * <履歴>	05.11.13 Fukushiro M. 作成
 *************************************************************************/
void CFCDlgModelessBase::SetWindowVisible (BOOL bIsVisible)
{
	ASSERT(m_nIDTemplate != DWORD(-1));
	if (::IsWindow(m_hWnd))
		ShowWindow(bIsVisible ? SW_SHOW : SW_HIDE);
	//----- レジストリに値を設定 -----
	// ウィンドウの表示・非表示をレジストリに記録。
	FCRegBase::SaveRegDWORD(GetRegKeyName().c_str(), L"Visible", DWORD(bIsVisible));
} // CFCDlgModelessBase::SetWindowVisible.

/////////////////////////////////////////////////////////////////////////////
// CFCDlgModelessBase オーバーライド関数。

/*************************************************************************
 * <関数>	CFCDlgModelessBase::Create
 *
 * <機能>	ウィンドウをモードレスで作成する。
 *
 * <引数>	nIDTemplate	:リソーステンプレートを指定。
 *			pParentWnd	:親ウィンドウを指定。
 *
 * <返値>	成功した場合はTRUE。
 *
 * <履歴>	05.11.13 Fukushiro M. 作成
 *************************************************************************/
BOOL CFCDlgModelessBase::Create (UINT nIDTemplate, CWnd* pParentWnd, BOOL isSizable)
{
	// ダイアログのID。レジストリへの保存に使う。
	m_nIDTemplate = nIDTemplate;
	m_isSizable = isSizable;
	// Baseclass関数実行。
	return BASE_CLASS::Create(nIDTemplate, pParentWnd);
} // CFCDlgModelessBase::Create.

/*************************************************************************
 * <関数>	CFCDlgModelessBase::DoModal
 *
 * <機能>	モーダルダイアログを表示。
 *
 * <解説>	この関数を実行してはいけない。
 *
 * <履歴>	05.11.13 Fukushiro M. 作成
 *************************************************************************/
INT_PTR CFCDlgModelessBase::DoModal ()
{
	ThrowLeSystemError();
	return FALSE;
} // CFCDlgModelessBase::DoModal.

/////////////////////////////////////////////////////////////////////////////
// CFCDlgModelessBase メッセージ ハンドラ。

/*************************************************************************
 * <関数>	CFCDlgModelessBase::OnCancel
 *
 * <機能>	「キャンセル」ボタンから実行。
 *
 * <履歴>	08.08.03 Fukushiro M. 作成
 *************************************************************************/
void CFCDlgModelessBase::OnCancel ()
{
	// デフォルト処理でも非表示になるが、OnShowWindowを実行するため以下。
	SetWindowVisible(FALSE);
} // CFCDlgModelessBase::OnCancel.


void CFCDlgModelessBase::OnOK()
{
	// To avoid to close this window with Enter key, comment out the following call.
	// CFCDialogBase::OnOK();
}
