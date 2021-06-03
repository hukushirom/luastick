
// StickClassExampleDlg.h : ヘッダー ファイル
//

#pragma once

#include "Stick.h"

// CStickClassExampleDlg ダイアログ

class CStickClassExampleDlg : public CDialogEx
{
// コンストラクション
public:
	CStickClassExampleDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STICKCLASSEXAMPLE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

public:
	/// <summary>
	/// Output the message to the message window.
	/// </summary>
	/// <param name="message">message</param>
	virtual void Output (const wchar_t * message);

protected:
	Stickrun m_stickrun;

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoad1();
	afx_msg void OnBnClickedBtnLoad2();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnFunc1();
	afx_msg void OnBnClickedBtnFunc2();
	afx_msg void OnBnClickedBtnFunc3();
	afx_msg void OnBnClickedBtnFunc4();
};
