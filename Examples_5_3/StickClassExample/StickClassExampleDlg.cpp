
// StickClassExampleDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include <string>
#include "StickClassExample.h"
#include "StickClassExampleDlg.h"
#include "TestClass.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const TestClass0 MyTestClass0;

const TestClass0 & TestClass2::TESTCLASS0 = MyTestClass0;


// CStickClassExampleDlg ダイアログ


constexpr const char * SCRIPT_1 = R"(
Func1 = function()
	local tc1 = TestClass1.New();
	local tc2 = TestClass2.New(1);
	tc1:Get();
	tc2:Get2(tc1);

	local p = TestClass0.CreateNull0();
	local b = TestSpace.IsNull(STICK.ObjectToUserdata(p));
	App():Output("Result:" .. tostring(b));

	local c = STICK.IsNullObject(p);
	App():Output("Result:" .. tostring(c));

	App():Output("Func1 OK");
end

Func2 = function()
	local tc1 = TestClass1.Create1();
	local tc2 = TestClass2.Create2();
	tc1:Get();
	tc2:Get2(tc1);
	App():Output("Func2 OK");
end
)";

constexpr const char * SCRIPT_2 = R"(
Func3 = function()
	local tc1 = TestClass1.New();
	tc1:Get();
	local tc2 = TestClass2.New(1);
	tc2:Get2(tc1);
	TestClass2.TESTCLASS0:GetX();
	App():Output("Func3 OK");
end

Func4 = function()
	local tc0_0 = TestClass0.New();
	tc0_0:SetTitle("test1.");
	local tc0_1 = TestClass0.New();
	tc0_1:SetTitle("test2.");
	local tc0_2 = TestClass0.New();
	tc0_2:SetTitle("test3.");

	local tc1 = TestClass1.New();
	tc1:SetClass0Array({tc0_0, tc0_1, tc0_2});
	local tc0ary = tc1:GetClass0Array();
	App():Output(tc0ary[1]:GetTitle() .. tc0ary[2]:GetTitle() .. tc0ary[3]:GetTitle());
end
)";

CStickClassExampleDlg::CStickClassExampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STICKCLASSEXAMPLE_DIALOG, pParent)
	, m_stickrun(luastick_init)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStickClassExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CStickClassExampleDlg::Output (const wchar_t * message)
{
	auto edit = (CEdit *)GetDlgItem(IDC_EDT_OUTPUT);
	edit->ReplaceSel(message);
	edit->ReplaceSel(L"\r\n");
}

BEGIN_MESSAGE_MAP(CStickClassExampleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOAD_1, &CStickClassExampleDlg::OnBnClickedBtnLoad1)
	ON_BN_CLICKED(IDC_BTN_LOAD_2, &CStickClassExampleDlg::OnBnClickedBtnLoad2)
	ON_BN_CLICKED(IDC_BTN_RESET, &CStickClassExampleDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_FUNC_1, &CStickClassExampleDlg::OnBnClickedBtnFunc1)
	ON_BN_CLICKED(IDC_BTN_FUNC_2, &CStickClassExampleDlg::OnBnClickedBtnFunc2)
	ON_BN_CLICKED(IDC_BTN_FUNC_3, &CStickClassExampleDlg::OnBnClickedBtnFunc3)
	ON_BN_CLICKED(IDC_BTN_FUNC_4, &CStickClassExampleDlg::OnBnClickedBtnFunc4)
END_MESSAGE_MAP()


// CStickClassExampleDlg メッセージ ハンドラー

BOOL CStickClassExampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	// Make a sandbox in Lua.
	m_stickrun.MakeSandboxEnv("SNDBX", true);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CStickClassExampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CStickClassExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CStickClassExampleDlg::OnBnClickedBtnLoad1()
{
	std::string error_message;
	if (m_stickrun.DoSandboxString(&error_message, "SNDBX", SCRIPT_1, "script1"))
	{
		Output(L"scrip1 was loaded");
	}
	else
	{
		std::wstring wstr_error;
		Sticklib::T_to_U<std::wstring, std::string>(wstr_error, error_message);
		Output(wstr_error.c_str());
	}
}


void CStickClassExampleDlg::OnBnClickedBtnLoad2()
{
	std::string error_message;
	if (m_stickrun.DoSandboxString(&error_message, "SNDBX", SCRIPT_2, "script2"))
	{
		Output(L"scrip2 was loaded");
	}
	else
	{
		std::wstring wstr_error;
		Sticklib::T_to_U<std::wstring, std::string>(wstr_error, error_message);
		Output(wstr_error.c_str());
	}
}


void CStickClassExampleDlg::OnBnClickedBtnReset()
{
	// Delete the sandbox.
	m_stickrun.RemoveSandboxEnv("SNDBX");
	// Make a sandbox in Lua.
	m_stickrun.MakeSandboxEnv("SNDBX", true);
}


void CStickClassExampleDlg::OnBnClickedBtnFunc1()
{
	// Call Lua function 'Func1'
	std::string error_message;
	if (!m_stickrun.CallFunction(&error_message, "SNDBX.Func1"))
	{
		std::wstring wstr_error;
		Sticklib::T_to_U<std::wstring, std::string>(wstr_error, error_message);
		Output(wstr_error.c_str());
	}
}


void CStickClassExampleDlg::OnBnClickedBtnFunc2()
{
	// Call Lua function 'Func2'
	std::string error_message;
	if (!m_stickrun.CallFunction(&error_message, "SNDBX.Func2"))
	{
		std::wstring wstr_error;
		Sticklib::T_to_U<std::wstring, std::string>(wstr_error, error_message);
		Output(wstr_error.c_str());
	}
}


void CStickClassExampleDlg::OnBnClickedBtnFunc3()
{
	// Call Lua function 'Func3'
	std::string error_message;
	if (!m_stickrun.CallFunction(&error_message, "SNDBX.Func3"))
	{
		std::wstring wstr_error;
		Sticklib::T_to_U<std::wstring, std::string>(wstr_error, error_message);
		Output(wstr_error.c_str());
	}
}


void CStickClassExampleDlg::OnBnClickedBtnFunc4()
{
	// Call Lua function 'Func4'
	std::string error_message;
	if (!m_stickrun.CallFunction(&error_message, "SNDBX.Func4"))
	{
		std::wstring wstr_error;
		Sticklib::T_to_U<std::wstring, std::string>(wstr_error, error_message);
		Output(wstr_error.c_str());
	}
}
