
// StickTestDlg.cpp : 実装ファイル
//

// Do not include "Stickrun.h". It must be included via "Stick.h".

#include "stdafx.h"
#if defined(_CPLUS_LUA_)
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#else
#include "lua.hpp"
#endif
#include "lauxlib.h"
#include "lualib.h"
#include "StickTest.h"
#include "StickTestDlg.h"
#include "Stick.h"
#include "Sticktrace.h"
#include "afxdialogex.h"
#pragma comment(lib, "Sticktrace.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string script1 = u8R"(
xyz = "hello"
Test1 = function()
	-- os.execute("sleep 2")
	STICKTRACE.DebugBreak()
	STICKTRACE.OutputDebugMessage("hello")
	ShowMessage(debug.traceback())
	c1 = {"a", "b", "c", "d"}
	c1["x"] = { [1] = "p", [2] = "q", [3] = "r" }
	-- local d = {"A"->"B"}
	b = X.B.New()
	local ii, a = b:Add1()
	ShowMessage("A::Get=" .. a:Get() .. "\r\n")
	ShowMessage(X.A.ABC .. "\r\n")
	local c = X.B.CreateA(15)
	ShowMessage(c:Get() .. "\r\n")
	X.B.DeleteA(a)
	X.B.DeleteA(c)

	local rf1, rf2 = RawFunc("hello");
	STICKTRACE.OutputDebugMessage(rf2)

	return 5, "abc"
end

Test2 = function(x)
	x:DebugOutput("hello\r\n")
	STICKTRACE.OutputDebugMessage("ABC")
end

Test3 = function()
  r1, r2 = MyFunc2(5.5, 8)
  ShowMessage(r1 .. r2 .. "\r\n")
end

Test4 = function()
  r1, r2 = NM1.MMM1.MyFunc1("abc", "あいう")
  ShowMessage(r1 .. r2 .. "\r\n")
  r1, r2 = NM1.MMM1.MyFunc2y(5.5, 8)
  ShowMessage(r1 .. r2 .. "\r\n")
  r1, r2 = NM1.MMM1.MyFunc2y(5.5)
  ShowMessage(r1 .. r2 .. "\r\n")
  r1, r2 = NM1.MMM1.MyFunc2x("ABC")
  ShowMessage(r1 .. r2 .. "\r\n")
end
)";

std::string script2 = u8R"(
Test5 = function()

  obj, rx = NM1.NM1_NM2.New(5)
  ShowMessage(rx .. "\r\n")
  r1, r2 = obj:MyFunc2(5.5, 8)
  ShowMessage(r1 .. r2 .. "\r\n")
end

Test6 = function()
  obj = NM1.NM1_NM3.New()
  r1, r2 = obj:MyFunc3(7, 15)
  ShowMessage(r1 .. r2 .. "\r\n")
  r1, r2 = obj:MyFunc2(5.5, 8)
  ShowMessage(r1 .. r2 .. "\r\n")
end

)";

std::string script3 = u8R"(
Test7 = function()
	-------- test 1 ------
	function locals()
	  local idx = 1
	  while true do
		local ln, lv = debug.getlocal(2, idx)
		if ln ~= nil then
		  ShowMessage("Local:" .. ln .. "\r\n")
		else
		  break
		end
		idx = 1 + idx
	  end
	end

	local a1 = {}
	a1["xx"] = {}
	a1["xx"][5] = "HELLO"
	local b = "XXX"
	local i
	for i = 1, 1 do
		local c = "ABC"
		for j = 1, 1 do
			local d = "ABC"
			ShowMessage("--- A ---\r\n")
			locals()
		end
		ShowMessage("--- B ---\r\n")
		locals()
	end
	ShowMessage("--- C ---\r\n")
	locals()

	ShowMessage(b .. "\r\n")
	-- locals()

	-------- test 2 ------
	_ENV["1"] = "abx"
	_ENV[1] = "ab1"
	-- _ENV[2] = "ab2"
	for orig_key, orig_value in pairs(_ENV) do
		ShowMessage("Key=" .. orig_key .. "\r\n")
	end
	-- ShowMessage("Key11=" .. _ENV[1] .. "  " .. _ENV["1"] .. "\r\n")


	-------- test 3 ------
	aaa = "xxx"
	__C_F = {}
	for orig_key, orig_value in pairs(_ENV) do
		__C_F[orig_key]=orig_value
	end
	bbb = "xxx"
	local f1 = function(t)
		local _ENV=t
		r1, r2 = NM2.MyFunc2(5.5, 8)
		ShowMessage(_G.aaa .. "\r\n")
	end
	f1(__C_F)
	f1 = nil
	local f2 = function(t)
		local _ENV=t
		r1, r2 = NM2.MyFunc2(5.5, 8)
		-- ShowMessage(aaa .. "\r\n")		-- Encounters an error.
	end
	f2(__C_F)
	f2 = nil

	local f3 = function(t)
		local _ENV=t
		r1, r2 = NM2.MyFunc2(5.5, 8)
		-- ShowMessage(aaa .. "\r\n")		-- Encounters an error.
	end
	f3("HELLO")
	f3 = nil
end

Test8 = function()
	local objA = X.A.New(202);
	local t = os.time()
	local a = 0
	while true do
		a = a + 1
		objA:Get()
		if a == 100000000 then
			break
		end
	end
	STICKTRACE.OutputDebugMessage("time=" .. os.time() - t)
end

Test9 = function()
	local a = {"abc", "efg", "hij", "あいう" }
	a = MyArrayFunc0(a)
	for k, v in pairs(a) do
		STICKTRACE.OutputDebugMessage(v)
	end
end

)";


// 17.09.19 Fukushiro M. 1行変更 ()
//const int* NM1::NM1_NM1::ARRAY = new int[3]{ 1, 2, 3 };
const int* NM1::NM1_NM1::ARRAY = nullptr;


void MyArrayFunc0(std::vector<std::string> & v1)
{
	for (auto & v : v1)
	{
		v = v + "-hello";
	}
}

void MyArrayFunc1(std::vector<int> & v1)
{
	for (auto & v : v1)
	{
		v = v + 10;
	}
}

void MyArrayFunc2(std::vector<std::wstring> & v1)
{
	for (auto & v : v1)
	{
		v = v + L"-hello";
	}
}

void RawFunc(lua_State * L)
{
	// Generated at "c:\src\lua\luastick\luastick\src\luastick.cpp"(4361)
	std::string param1;
	Sticklib::check_lvalue<std::string>(param1, L, 1);
	param1 += "-OK";
	Sticklib::push_lvalue<std::string>(L, param1, false);
	param1 += "-OK";
	Sticklib::push_lvalue<std::string>(L, param1, false);
}

CStickTestDlg* ToStickTestDlg(void* data)
{
	return (CStickTestDlg*)data;
}

// CStickTestDlg ダイアログ

CStickTestDlg::CStickTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_STICKTEST_DIALOG, pParent)
	, m_stickrun1(nullptr)
	, m_stickrun2(nullptr)
	, m_sticktrace1(nullptr)
	, m_sticktrace2(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStickTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CStickTestDlg::DebugOutput(const char* message)
{
	m_sticktrace1->OutputDebug(message);
}

void CStickTestDlg::OnSaveScript(const std::string & name, const std::string & code)
{
	std::string error_message;
	if (name == "script1")
	{
		script1 = code;
		m_sticktrace1->DetachStickrun();
		delete m_stickrun1;
		m_stickrun1 = new Stickrun(luastick_init);
		m_sticktrace1->AttachStickrun(
			m_stickrun1,
			OnScriptCallback,
			this,
			100
		);
		// Do not call Stickrun::NewSession when saving.
		m_stickrun1->DoString(&error_message, script1, "script1");

	}
	else if (name == "script2")
	{
		script2 = code;

		m_sticktrace1->DetachStickrun();
		delete m_stickrun1;
		m_stickrun1 = new Stickrun(luastick_init);
		m_sticktrace1->AttachStickrun(
			m_stickrun1,
			OnScriptCallback,
			this,
			100
		);
		// Do not call Stickrun::NewSession when saving.
		m_stickrun1->DoSandboxString(&error_message, "SCR2", script2, "script2");
	}
	else if (name == "script3")
	{
		script3 = code;

		m_sticktrace2->DetachStickrun();
		delete m_stickrun2;
		m_stickrun2 = new Stickrun(luastick_init);
		m_sticktrace2->AttachStickrun(
			m_stickrun2,
			OnScriptCallback,
			this,
			100
		);
		// Do not call Stickrun::NewSession when saving.
		m_stickrun2->DoSandboxString(&error_message, "SCR3", script3, "script3");
	}
}

BEGIN_MESSAGE_MAP(CStickTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_IDLEUPDATECMDUI, &CStickTestDlg::OnIdleUpdateCmdUI)
	ON_BN_CLICKED(IDC_BTN_TEST1, &CStickTestDlg::OnBnClickedBtnTest1)
	ON_BN_CLICKED(IDC_BTN_TEST2, &CStickTestDlg::OnBnClickedBtnTest2)
	ON_BN_CLICKED(IDC_BTN_TEST3, &CStickTestDlg::OnBnClickedBtnTest3)
	ON_BN_CLICKED(IDC_BTN_TEST4, &CStickTestDlg::OnBnClickedBtnTest4)
	ON_BN_CLICKED(IDC_BTN_TEST5, &CStickTestDlg::OnBnClickedBtnTest5)
	ON_BN_CLICKED(IDC_BTN_TEST6, &CStickTestDlg::OnBnClickedBtnTest6)
	ON_BN_CLICKED(IDC_BTN_TEST7, &CStickTestDlg::OnBnClickedBtnTest7)
	ON_BN_CLICKED(IDC_BTN_SHOW_EDITOR, &CStickTestDlg::OnBnClickedBtnShowEditor)
	ON_BN_CLICKED(IDC_BTN_HIDE_EDITOR, &CStickTestDlg::OnBnClickedBtnHideEditor)
	ON_BN_CLICKED(IDC_BTN_STOP_SCRIPT, &CStickTestDlg::OnBnClickedBtnStopScript)
	ON_BN_CLICKED(IDC_BTN_RESUME_SCRIPT, &CStickTestDlg::OnBnClickedBtnResumeScript)
	ON_BN_CLICKED(IDC_BTN_SUSPEND_SCRIPT, &CStickTestDlg::OnBnClickedBtnSuspendScript)
	ON_BN_CLICKED(IDC_BTN_NEXT_SCRIPT, &CStickTestDlg::OnBnClickedBtnNextScript)
	ON_BN_CLICKED(IDC_BTN_TEST8, &CStickTestDlg::OnBnClickedBtnTest8)
	ON_BN_CLICKED(IDC_BTN_TEST9, &CStickTestDlg::OnBnClickedBtnTest9)
	ON_BN_CLICKED(IDC_BTN_INIT_STICKRUN, &CStickTestDlg::OnBnClickedBtnInitStickrun)
	ON_MESSAGE(WM_USER_COMMAND, &CStickTestDlg::OnUserCommand)

END_MESSAGE_MAP()

// CStickTestDlg メッセージ ハンドラー

bool CStickTestDlg::DGT_DebuggerCallback(unsigned int dialogId, SticktraceDef::DebuggerCallbackParam * param)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.Clear();
	m_incmd.command = param->command;
	m_incmd.strParam1 = (param->strParam1 != nullptr) ? param->strParam1 : "";
	m_incmd.strParam2 = (param->strParam2 != nullptr) ? param->strParam2 : "";
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

bool CStickTestDlg::DGT_DebuggerCallback(
	unsigned int dialogId, 
	SticktraceDef::DebuggerCallbackParam* param,
	void* userData
)
{
	return ((CStickTestDlg*)userData)->DGT_DebuggerCallback(dialogId, param);
}

void CStickTestDlg::OnScriptCallback(
	lua_State * luaState,
	lua_Debug * luaDebug,
	void * userData,
	SticktraceDef::Mode mode,
	Sticktrace* sticktrace
)
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

BOOL CStickTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定
	OnBnClickedBtnInitStickrun();
	
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CStickTestDlg::OnPaint()
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
HCURSOR CStickTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CStickTestDlg::OnBnClickedBtnTest1()
{
	std::string error_message;
	int a;
	std::string b;
	m_stickrun1->CallFunction(&error_message, "Test1", Stickrun::Out(a), Stickrun::Out(b));
}

// SticktraceWindow stickTrace;

void CStickTestDlg::OnDestroy()
{
	m_sticktrace1->Terminate();
	delete m_sticktrace1;
	m_sticktrace1 = nullptr;
	delete m_stickrun1;
	m_stickrun1 = nullptr;


	m_sticktrace2->Terminate();
	delete m_sticktrace2;
	m_sticktrace2 = nullptr;
	delete m_stickrun2;
	m_stickrun2 = nullptr;

	CDialogEx::OnDestroy();
}

void CStickTestDlg::OnBnClickedBtnTest2()
{
	std::string error_message;
	int ii = 10;
	m_stickrun1->CallFunction(&error_message, "Test2", *this);
}

void CStickTestDlg::OnBnClickedBtnTest3()
{
	std::string error_message;
	m_stickrun1->CallFunction(&error_message, "Test3");
}

void CStickTestDlg::OnBnClickedBtnTest4()
{
	std::string error_message;
	m_stickrun1->CallFunction(&error_message, "Test4");
}


void CStickTestDlg::OnBnClickedBtnTest5()
{
	std::string error_message;
	m_stickrun1->CallFunction(&error_message, "SCR2.Test5");
}


void CStickTestDlg::OnBnClickedBtnTest6()
{
	std::string error_message;
	m_stickrun1->CallFunction(&error_message, "SCR2.Test6");
}


void CStickTestDlg::OnBnClickedBtnTest7()
{
	std::string error_message;
	m_stickrun2->CallFunction(&error_message, "SCR3.Test7");
}


void CStickTestDlg::OnBnClickedBtnTest8()
{
	std::string error_message;
	m_stickrun2->CallFunction(&error_message, "SCR3.Test8");
}

void CStickTestDlg::OnBnClickedBtnTest9()
{
	std::string error_message;
	m_stickrun2->CallFunction(&error_message, "SCR3.Test9");
}

LRESULT CStickTestDlg::OnUserCommand(WPARAM, LPARAM)
{
	SticktraceDef::DebuggerCommand command;
	std::string strParam1;
	std::string strParam2;
	{
		AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
		command = m_incmd.command;
		strParam1 = m_incmd.strParam1;
		strParam2 = m_incmd.strParam2;
// 21.05.18 Fukushiro M. 1行変更 ()
//		m_incmd.command = SticktraceDef::DebuggerCommand::NONE;
		m_incmd.Clear();
		acs.WakeConditionVariable();
	}
	switch (command)
	{
	case SticktraceDef::DebuggerCommand::SAVE_SCRIPT:
		OnSaveScript(strParam1, strParam2);
		break;
	case SticktraceDef::DebuggerCommand::ON_ERROR_OUTPUT:
	case SticktraceDef::DebuggerCommand::ON_DEBUG_OUTPUT:
	{
		std::wstring wstrText;
		Sticklib::T_to_U<std::wstring, std::string>(wstrText, strParam1);
		ShowMessage(wstrText.c_str());
		break;
	}
	default:
		break;
	}
	return 1;
}

void ShowMessage(const wchar_t* text)
{
	auto edit = (CEdit*)AfxGetMainWnd()->GetDlgItem(IDC_EDT_OUTPUT);
	edit->ReplaceSel(text);
}

std::wstring MyFunc1(std::wstring & wstr1, const wchar_t * wcp2)
{
	wstr1 += L"あいうえお";
	return wstr1 + wcp2;
}

int MyFunc2(double & v1, __int64 v2)
{
	v1 *= 2;
	return (int)(v1 + v2);
}


int NM1::MyFunc2(double & v1, __int64 v2)
{
	v1 *= 2;
	return (int)(v1 + v2);
}

std::wstring NM1::NM1_NM1::MyFunc1(std::wstring & wstr1, const wchar_t * wcp2)
{
	wstr1 += L"あいうえお";
	return wstr1 + wcp2;
}

int NM1::NM1_NM1::MyFunc2(double & v1, __int64 v2)
{
	v1 *= 2;
	return (int)(v1 + v2);
}

int NM1::NM1_NM1::MyFunc2(double & v1)
{
	return 0;
}

int NM1::NM1_NM1::MyFunc2(std::string & v1)
{
	return 0;
}

// この関数は以下のようなLuaのhookになるべき。
// 
// static int __lstickfunc_NM1_NM1_NM1_MyFunc2(lua_State* L)
// {
// 	if (lua_gettop(L) != 2)
// 		luaL_error(L, "incorrect argument");
// 
// 	double v1;
// 	Sticklib::lnumber_to_floatX<double>(v1, luaL_checknumber(L, 2));
// 	__int64 v2;
// 	Sticklib::linteger_to_intX<__int64>(v2, luaL_checkinteger(L, 3));
// 	int __lstickvar_ret;
// 	try
// 	{
// 		__lstickvar_ret = NM1::NM1_NM1::MyFunc2(v1, v2);
// 	}
// 	catch (...)
// 	{
// 		std::string __lstickvar_message = "C function error:MyFunc2:";
// 		luaL_error(L, __lstickvar_message.c_str());
// 	}
// 	Sticklib::l_number __lstickvar_r1;
// 	Sticklib::floatX_to_lnumber<double>(__lstickvar_r1, v1);
// 	lua_pushnumber(L, __lstickvar_r1);
// 	Sticklib::l_integer __lstickvar_r2;
// 	Sticklib::intX_to_linteger<int>(__lstickvar_r2, __lstickvar_ret);
// 	lua_pushinteger(L, __lstickvar_r2);
// 	return 2;
// }



int NM1::NM1_NM2::MyFunc2(double & v1, __int64 v2)
{
	v1 *= 2;
	return (int)(v1 + v2);
}

int NM1::NM1_NM3::MyFunc3(double & v1, __int64 v2)
{
	v1 *= 2;
	return (int)(v1 + v2);
}

int NM1::NM1_NM3::MyFunc3(double & v1)
{
	return 0;
}

// この関数は以下のようなLuaのhookになるべき。
// 
// static int __lstickfunc_NM1_NM1_NM2_MyFunc2(lua_State* L)
// {
// 	if (lua_gettop(L) != 3)
// 		luaL_error(L, "incorrect argument");
// 
// 	NM1::NM1_NM2* p = (NM1::NM1_NM2*)luaL_checkudata(L, 1, NM1_NM1_NM2);
// 	if (!p) { return 0; }
// 
// 	double v1;
// 	Sticklib::lnumber_to_floatX<double>(v1, luaL_checknumber(L, 2));
// 	__int64 v2;
// 	Sticklib::linteger_to_intX<__int64>(v2, luaL_checkinteger(L, 3));
// 	int __lstickvar_ret;
// 	try
// 	{
// 		__lstickvar_ret = p->MyFunc2(v1, v2);
// 	}
// 	catch (...)
// 	{
// 		std::string __lstickvar_message = "C function error:MyFunc2:";
// 		luaL_error(L, __lstickvar_message.c_str());
// 	}
// 	Sticklib::l_number __lstickvar_r1;
// 	Sticklib::floatX_to_lnumber<double>(__lstickvar_r1, v1);
// 	lua_pushnumber(L, __lstickvar_r1);
// 	Sticklib::l_integer __lstickvar_r2;
// 	Sticklib::intX_to_linteger<int>(__lstickvar_r2, __lstickvar_ret);
// 	lua_pushinteger(L, __lstickvar_r2);
// 	return 2;
// }

int NM2::MyFunc2(double & v1, __int64 v2)
{
	v1 *= 2;
	return (int)(v1 + v2);
}


const char * X::A::ABC = "abc";

void CStickTestDlg::OnBnClickedBtnShowEditor()
{
	 m_sticktrace1->ShowWindow(true);
	 m_sticktrace2->ShowWindow(true);
}


void CStickTestDlg::OnBnClickedBtnHideEditor()
{
	 m_sticktrace1->ShowWindow(false);
	 m_sticktrace2->ShowWindow(false);
}


LRESULT CStickTestDlg::OnIdleUpdateCmdUI(WPARAM, LPARAM)
{
	 return 1;
}


void CStickTestDlg::OnBnClickedBtnStopScript()
{
	 m_sticktrace1->SetScriptMode(SticktraceDef::Mode::STOP);
	 m_sticktrace2->SetScriptMode(SticktraceDef::Mode::STOP);
}


void CStickTestDlg::OnBnClickedBtnResumeScript()
{
	 m_sticktrace1->SetScriptMode(SticktraceDef::Mode::RUN);
	 m_sticktrace2->SetScriptMode(SticktraceDef::Mode::RUN);
}


void CStickTestDlg::OnBnClickedBtnSuspendScript()
{
	 m_sticktrace1->SetScriptMode(SticktraceDef::Mode::SUSPEND);
	 m_sticktrace2->SetScriptMode(SticktraceDef::Mode::SUSPEND);
}


void CStickTestDlg::OnBnClickedBtnNextScript()
{
	 m_sticktrace1->SetScriptMode(SticktraceDef::Mode::PROCEED_NEXT);
	 m_sticktrace2->SetScriptMode(SticktraceDef::Mode::PROCEED_NEXT);
}

void CStickTestDlg::OnBnClickedBtnInitStickrun()
{
	if (m_sticktrace1 != nullptr)
	{
		m_sticktrace1->Terminate();
		delete m_sticktrace1;
		m_sticktrace1 = nullptr;
		delete m_stickrun1;
		m_stickrun1 = nullptr;
	}
	m_stickrun1 = new Stickrun(luastick_init);
	m_sticktrace1 = new Sticktrace();
	m_sticktrace1->Initialize(
		m_hWnd,
		L"Dynamic Draw Project",
		L"",
		L"StickTest",
		(DWORD)-1,
		DGT_DebuggerCallback,
		this
	);
	m_sticktrace1->AttachStickrun(
		m_stickrun1,
		OnScriptCallback,
		this,
		100
	);

	 m_stickrun1->NewSession();
	 std::string error_message;
	 m_stickrun1->DoString(&error_message, script1, "script1");
	 m_stickrun1->MakeSandboxEnv("SCR2", true);
	 m_stickrun1->DoSandboxString(&error_message, "SCR2", script2, "script2");





	if (m_sticktrace2 != nullptr)
	{
		m_sticktrace2->Terminate();
		delete m_sticktrace2;
		m_sticktrace2 = nullptr;
		delete m_stickrun2;
		m_stickrun2 = nullptr;
	}
	m_stickrun2 = new Stickrun(luastick_init);
	m_sticktrace2 = new Sticktrace();
	m_sticktrace2->Initialize(
		m_hWnd,
		L"Dynamic Draw Project",
		L"",
		L"StickTest",
		(DWORD)-2,
		DGT_DebuggerCallback,
		this
	);
	m_sticktrace2->AttachStickrun(
		m_stickrun2,
		OnScriptCallback,
		this,
		100
	);

	 m_stickrun2->NewSession();
	 m_stickrun2->MakeSandboxEnv("SCR3", true);
	 m_stickrun2->DoSandboxString(&error_message, "SCR3", script3, "script3");
}


void CStickTestDlg::OnOK()
{
	 if (m_sticktrace1->IsScriptModified())
	 {
		 MessageBox(L"Save the script before to close the window.");
		 return;
	 }
	 if (!m_sticktrace1->Terminate())
	 {
		 MessageBox(L"You have to stop the script running before you quit the application.");
		 return;
	 }

	 if (m_sticktrace2->IsScriptModified())
	 {
		 MessageBox(L"Save the script before to close the window.");
		 return;
	 }
	 if (!m_sticktrace2->Terminate())
	 {
		 MessageBox(L"You have to stop the script running before you quit the application.");
		 return;
	 }
	 CDialogEx::OnOK();
}


void CStickTestDlg::OnCancel()
{
	 if (!m_sticktrace1->Terminate())
	 {
		 MessageBox(L"You have to stop the script running before you quit the application.");
		 return;
	 }

	 if (!m_sticktrace2->Terminate())
	 {
		 MessageBox(L"You have to stop the script running before you quit the application.");
		 return;
	 }

	 CDialogEx::OnCancel();
}
