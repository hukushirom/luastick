// DlgSticktrace.cpp : 実装ファイル。
//

#include "stdafx.h"
#include "Resource.h"
#include "Astrwstr.h"
#include "UtilString.h"
#include "UtilStr.h"
#include "UtilGraphics.h"	// For FCEditDraw.
#include "RegBase.h"		// For FCRegBase.
#include "DlgSticktrace.h"
#include "afxdialogex.h"

static constexpr int FD_PANEBAR_WIDTH = 6;	// ペイン分割の線幅。
static constexpr COLORREF FD_COLREF_DARK_GRAY = RGB(0xa9, 0xa9, 0xa9);
static constexpr COLORREF FD_PANEBAR_COLOR = FD_COLREF_DARK_GRAY;
static constexpr UINT BORDER_TIMER = 5;
static constexpr int TAB_INDEX_ERROR = 0;
static constexpr int TAB_INDEX_WATCH = 1;
static constexpr int TAB_INDEX_OUTPUT = 2;

// CDlgSticktrace ダイアログ。

IMPLEMENT_DYNAMIC(CDlgSticktrace, CFCDlgModelessBase)

CDlgSticktrace::CDlgSticktrace()
	: BASE_CLASS()
	, m_bIsBtnOnPaneBorder(FALSE)	// ペインボーダー上でボタンが押されているか？
	, m_hwndWhenBorderMoving(NULL)
{
	m_breakpointFast.suspendMode = (LONG)Mode::STOP;
	m_breakpointFast.breakpointCount = 0;
	m_debugMode.debugMode = 0;
}

CDlgSticktrace::~CDlgSticktrace()
{
}

void CDlgSticktrace::DoDataExchange(CDataExchange* pDX)
{
	BASE_CLASS::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCE_EDT_SCRIPT, m_textEditor);
	DDX_Control(pDX, IDC_SCE_EDT_OUTPUT, m_output);
	DDX_Control(pDX, IDC_SCE_EDT_ERROR, m_errorout);
}


BEGIN_MESSAGE_MAP(CDlgSticktrace, BASE_CLASS)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SCE_TAB_OUTPUT, &CDlgSticktrace::OnTcnSelchangeSceTabOutput)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_SCE_TAB_SCRIPT, &CDlgSticktrace::OnTcnSelchangeSceTabScript)
	ON_MESSAGE(WM_USER_COMMAND, &CDlgSticktrace::OnUserCommand)
	ON_MESSAGE(WM_USER_BREAKPOINT_UPDATED, &CDlgSticktrace::OnUserBreakpointUpdated)
	ON_MESSAGE(WM_USER_TEXT_EDIT_MARKER_CLICKED, &CDlgSticktrace::OnUserTextEditMarkerClicked)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_CONTINUE, &CDlgSticktrace::OnBnClickedSceBtnDebugContinue)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_BREAK, &CDlgSticktrace::OnBnClickedSceBtnDebugBreak)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_STOP, &CDlgSticktrace::OnBnClickedSceBtnDebugStop)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT, &CDlgSticktrace::OnBnClickedSceBtnDebugStepToNext)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, &CDlgSticktrace::OnIdleUpdateCmdUI)
	ON_COMMAND(ID_SCE_EDIT_GOTO_LINE, &CDlgSticktrace::OnSceEditGotoLine)
	ON_UPDATE_COMMAND_UI(ID_SCE_EDIT_GOTO_LINE, &CDlgSticktrace::OnUpdateSceEditGotoLine)
	ON_COMMAND(ID_SCE_WIN_KEYWORD, &CDlgSticktrace::OnSceWinKeyword)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_KEYWORD, &CDlgSticktrace::OnUpdateSceWinKeyword)
	ON_COMMAND(ID_EDIT_FIND_NEXT_TEXT, &CDlgSticktrace::OnEditFindNextText)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_NEXT_TEXT, &CDlgSticktrace::OnUpdateEditFindNextText)
	ON_COMMAND(ID_EDIT_FIND_PREV_TEXT, &CDlgSticktrace::OnEditFindPrevText)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_PREV_TEXT, &CDlgSticktrace::OnUpdateEditFindPrevText)
	ON_COMMAND(ID_SCE_OUTWIN_GOTO_LOCATION, &CDlgSticktrace::OnSceOutwinGotoLocation)
	ON_UPDATE_COMMAND_UI(ID_SCE_OUTWIN_GOTO_LOCATION, &CDlgSticktrace::OnUpdateSceOutwinGotoLocation)
	ON_COMMAND(ID_SCE_OUTWIN_CLEAR, &CDlgSticktrace::OnSceOutwinClear)
	ON_UPDATE_COMMAND_UI(ID_SCE_OUTWIN_CLEAR, &CDlgSticktrace::OnUpdateSceOutwinClear)
	ON_COMMAND(ID_SCE_DEBUG_TOGGLE_BREAKPOINT, &CDlgSticktrace::OnSceDebugToggleBreakpoint)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_TOGGLE_BREAKPOINT, &CDlgSticktrace::OnUpdateSceDebugToggleBreakpoint)
	ON_COMMAND(ID_SCE_DEBUG_CLEAR_BREAKPOINT, &CDlgSticktrace::OnSceDebugClearBreakpoint)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_CLEAR_BREAKPOINT, &CDlgSticktrace::OnUpdateSceDebugClearBreakpoint)
	ON_COMMAND(ID_SCE_DEBUG_MODE, &CDlgSticktrace::OnSceDebugMode)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_MODE, &CDlgSticktrace::OnUpdateSceDebugMode)
	ON_COMMAND(ID_SCE_DEBUG_CONTINUE, &CDlgSticktrace::OnSceDebugContinue)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_CONTINUE, &CDlgSticktrace::OnUpdateSceDebugContinue)
	ON_COMMAND(ID_SCE_DEBUG_BREAK, &CDlgSticktrace::OnSceDebugBreak)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_BREAK, &CDlgSticktrace::OnUpdateSceDebugBreak)
	ON_COMMAND(ID_SCE_DEBUG_STEP_TO_NEXT, &CDlgSticktrace::OnSceDebugStepToNext)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_STEP_TO_NEXT, &CDlgSticktrace::OnUpdateSceDebugStepToNext)
	ON_COMMAND(ID_SCE_DEBUG_UNTIL_CARET, &CDlgSticktrace::OnSceDebugUntilCaret)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_UNTIL_CARET, &CDlgSticktrace::OnUpdateSceDebugUntilCaret)
	ON_COMMAND(ID_SCE_DEBUG_STOP, &CDlgSticktrace::OnSceDebugStop)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_STOP, &CDlgSticktrace::OnUpdateSceDebugStop)
	ON_COMMAND(ID_SCE_DEBUG_CHANGE_VARIABLE, &CDlgSticktrace::OnSceDebugChangeVariable)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_CHANGE_VARIABLE, &CDlgSticktrace::OnUpdateSceDebugChangeVariable)
	ON_COMMAND(ID_SCE_DEBUG_ADD_WATCH, &CDlgSticktrace::OnSceDebugAddWatch)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_ADD_WATCH, &CDlgSticktrace::OnUpdateSceDebugAddWatch)
	ON_COMMAND(ID_SCE_DEBUG_DELETE_WATCH, &CDlgSticktrace::OnSceDebugDeleteWatch)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_DELETE_WATCH, &CDlgSticktrace::OnUpdateSceDebugDeleteWatch)
	ON_COMMAND(ID_SCE_WIN_ERROR, &CDlgSticktrace::OnSceWinError)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_ERROR, &CDlgSticktrace::OnUpdateSceWinError)
	ON_COMMAND(ID_SCE_WIN_WATCH, &CDlgSticktrace::OnSceWinWatch)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_WATCH, &CDlgSticktrace::OnUpdateSceWinWatch)
	ON_COMMAND(ID_SCE_WIN_OUTPUT, &CDlgSticktrace::OnSceWinOutput)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_OUTPUT, &CDlgSticktrace::OnUpdateSceWinOutput)
	ON_COMMAND(ID_SCE_WIN_VARIABLE_NAME, &CDlgSticktrace::OnSceWinVariableName)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_VARIABLE_NAME, &CDlgSticktrace::OnUpdateSceWinVariableName)
	ON_COMMAND(ID_SCE_WIN_VARIABLE_VALUE, &CDlgSticktrace::OnSceWinVariableValue)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_VARIABLE_VALUE, &CDlgSticktrace::OnUpdateSceWinVariableValue)
	ON_COMMAND(ID_HELP, &CDlgSticktrace::OnHelp)
	ON_UPDATE_COMMAND_UI(ID_HELP, &CDlgSticktrace::OnUpdateHelp)
	ON_BN_CLICKED(IDC_SCE_BTN_CHANGE_VARIABLE, &CDlgSticktrace::OnBnClickedSceBtnChangeVariable)
	ON_BN_CLICKED(IDC_SCE_BTN_ADD_WATCH, &CDlgSticktrace::OnBnClickedSceBtnAddWatch)
	ON_BN_CLICKED(IDC_SCE_BTN_DELETE_WATCH, &CDlgSticktrace::OnBnClickedSceBtnDeleteWatch)
	ON_NOTIFY(NM_CLICK, IDC_SCE_LSV_WATCH, &CDlgSticktrace::OnNMClickSceLsvWatch)
	ON_MESSAGE(WM_USER_DDEDIT_DBLCLKED, &CDlgSticktrace::OnUserDdEditDblClked)
	ON_MESSAGE(WM_USER_TEXT_EDIT_CURLINE_CHANGED, &CDlgSticktrace::OnUserTextEditCurlineChanged)
	ON_BN_CLICKED(IDC_SCE_CHK_DEBUG_MODE, &CDlgSticktrace::OnBnClickedSceChkDebugMode)
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SCE_LSV_WATCH, &CDlgSticktrace::OnLvnItemchangedSceLsvWatch)
END_MESSAGE_MAP()

// CDlgSticktrace メッセージ ハンドラー。


BOOL CDlgSticktrace::OnInitDialog()
{
	BASE_CLASS::OnInitDialog();

	//----- タブ切替の部分にコントロールを全て移動させる -----
	// 基準フレーム。
	CRect rtTabFrame;
	GetDlgItem(IDC_SCE_STC_TAB_FRAME)->GetWindowRect(rtTabFrame);
	// 出力ウィンドウフレーム。
	CRect rtOutput;
	GetDlgItem(IDC_SCE_STC_OUTPUT_FRAME)->GetWindowRect(rtOutput);
	// エラー出力ウィンドウフレーム。
	CRect rtError;
	GetDlgItem(IDC_SCE_STC_ERROR_FRAME)->GetWindowRect(rtError);
	// ウォッチウィンドウフレーム。
	CRect rtWatch;
	GetDlgItem(IDC_SCE_STC_WATCH_FRAME)->GetWindowRect(rtWatch);

	CSize szMove = rtTabFrame.TopLeft() - rtOutput.TopLeft();
	MoveControl(IDC_SCE_EDT_OUTPUT, szMove);
	szMove = rtTabFrame.TopLeft() - rtError.TopLeft();
	MoveControl(IDC_SCE_EDT_ERROR, szMove);
	szMove = rtTabFrame.TopLeft() - rtWatch.TopLeft();
	MoveControl(IDC_SCE_LSV_WATCH, szMove);
	MoveControl(IDC_SCE_EDT_VARIABLE_NAME, szMove);
	MoveControl(IDC_SCE_EDT_VARIABLE_VALUE, szMove);
	MoveControl(IDC_SCE_BTN_CHANGE_VARIABLE, szMove);
	MoveControl(IDC_SCE_BTN_ADD_WATCH, szMove);
	MoveControl(IDC_SCE_BTN_DELETE_WATCH, szMove);

	// レイアウト情報を設定。
	InitLayoutAll();

	// レジストリに登録されたサイズ情報に合わせてリサイズ。
	SizeToRegistered();

	// ボーダーの位置（比率）をレジストリから読み出す。
	double borderX;
	const DWORD dwDataSz = FCRegBase::LoadRegBinary(GetRegKeyName().c_str(), L"BorderX", (LPBYTE)&borderX, sizeof(borderX));
	if (dwDataSz == sizeof(borderX))
	{	//----- レジストリからボーダー位置が読み出せた場合 -----
		// ボーダー位置を移動。
		CRect clientRect;
		GetClientRect(clientRect);
		auto newBorderX = (int)((double)clientRect.Width() * borderX);
		const int dx = newBorderX - GetBorderRect().CenterPoint().x;
		MoveBorder(dx);
	}

	// フォント設定。
	// スクリプトエディター。
	FCRegBase::InitRegFont(m_font, FDFT_SCRIPT_DEBUGGER);
	GetDlgItem(IDC_SCE_EDT_SCRIPT)->SetFont(&m_font);

	// スクリプトIDリストを更新。
	UpdateScriptIdList();

	// ボタンにアイコンを設定。
	CButton* pButton;
	pButton = (CButton*)GetDlgItem(IDC_SCE_BTN_FIND_NEXT);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_FIND));
	pButton = (CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_DEBUG_MODE));
	pButton = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_DEBUG_CONTINUE));
	pButton = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_DEBUG_BREAK));
	pButton = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_DEBUG_STOP));
	pButton = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_DEBUG_STEP_TO_NEXT));

	// テキストエディターにUndo可能を設定。
	m_textEditor.SetIsUndoable(TRUE);
	m_textEditor.SetLimitText(640000);
	m_textEditor.SetReadOnly(TRUE);
	const auto isDebugMode = (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) != 0);
	m_textEditor.ActivateBreakpoint(isDebugMode);
	m_textEditor.SetTabSize(2);
	//int s[]{ 10, 20, 30, 40, 50 };
	//m_textEditor.SetTabStops(5, s);
	m_textEditor.SetTabStops(10);

	//----- ウォッチリストコントロールの設定 -----
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// フォーカス無し状態でも選択マークが表示されるようにスタイル変更。
	pLsvWatch->ModifyStyle(0, LVS_SHOWSELALWAYS);
	// 全カラム選択の拡張スタイルを設定。
	pLsvWatch->SetExtendedStyle(pLsvWatch->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//----- 14.10.02 Fukushiro M. 追加始 ()-----
	m_watchImage.Create(16, 16, ILC_MASK, 4, 1);
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_COLLAPSED));		// 閉じている(0)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_EXPANDED));		// 開いている(1)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE));		// 変数(2)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_SUBVARIABLE));	// メンバー変数(3)
	pLsvWatch->SetImageList(&m_watchImage, LVSIL_SMALL);
	//----- 14.10.02 Fukushiro M. 追加終 ()-----

	//----- カラムの設定 -----
	CRect rect;
	pLsvWatch->GetClientRect(rect);
	// カラム幅。デフォルト値で初期化。
	const int aColumnWidthInit[] =
	{
		rect.Width() / 4,
		rect.Width() / 2,
		rect.Width() / 4,
	};
	// 名前。
	UtilDlg::InsertColumn(pLsvWatch, 0, UtilStr::LoadString(CString(), IDS_NAME), LVCFMT_LEFT, aColumnWidthInit[0]);
	// 値。
	UtilDlg::InsertColumn(pLsvWatch, 1, UtilStr::LoadString(CString(), IDS_VALUE), LVCFMT_LEFT, aColumnWidthInit[1]);
	// 型。
	UtilDlg::InsertColumn(pLsvWatch, 2, UtilStr::LoadString(CString(), IDS_TYPE), LVCFMT_LEFT, aColumnWidthInit[2]);
	// リストコントロールのカラム境界を設定。
	FD_RegLoadListCtrlColumnWidth(this, IDC_SCE_LSV_WATCH);

	// タブコントロールにタブを設定。
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	// エラー出力。
	tabCtrl->InsertItem(TAB_INDEX_ERROR, UtilStr::LoadString(CString(), IDS_ERROR));
	// 変数。
	tabCtrl->InsertItem(TAB_INDEX_WATCH, UtilStr::LoadString(CString(), IDS_WATCH));
	// 出力。
	tabCtrl->InsertItem(TAB_INDEX_OUTPUT, UtilStr::LoadString(CString(), IDS_OUTPUT));
	// タブ変更を通知。
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);

	// Add empty tab into the tab control for script.
	CTabCtrl* tabCtrl2 = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);
	// 空。
	tabCtrl2->InsertItem(0, L"");

	// テキスト領域を更新。
	m_textEditor.UpdateTextRect();

	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

/// <summary>
/// Tcs the set source.
/// This function works at the main application's thread.
/// </summary>
/// <param name="sandbox">Name of sandbox.</param>
/// <param name="name">The name.</param>
/// <param name="src">The source.</param>
/// <returns></returns>
bool CDlgSticktrace::TC_SetSource(const std::string& sandbox, const std::string& name, const std::string& src)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::SET_SOURCE;
	m_incmd.strParam1 = sandbox;
	m_incmd.strParam2 = name;
	m_incmd.strParam3 = src;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Returns debug mode or not.
/// This function works at the main application's thread.
/// </summary>
/// <returns></returns>
bool CDlgSticktrace::TC_IsDebugMode()
{
	return (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) != 0);
}

/// <summary>
/// Check the existence of breakpoint where given source and line.
/// If name == nullptr, this function works fast but returns only the possibility of breakpoint existence.
/// This function works at the main application's thread.
/// </summary>
/// <param name="name">The name.</param>
/// <param name="lineIndex">Index of the line.</param>
/// <returns></returns>
bool CDlgSticktrace::TC_IsBreakpoint(const char * name, int lineIndex)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	switch (mode)
	{
	case Mode::SUSPEND:
	case Mode::STOPPING:
	case Mode::STOP:
		return true;
	}
	if (::InterlockedCompareExchange(&m_breakpointFast.breakpointCount, 0, 0) == 0)
		return false;
	{
		AutoLeaveCS acs(m_breakpointSlow.cs);
		if ((int)m_breakpointSlow.lineIndexToIsBreak.size() <= lineIndex)
			return false;
		if (!m_breakpointSlow.lineIndexToIsBreak[lineIndex])
			return false;
		if (name == nullptr)
			return true;
		if (m_breakpointSlow.lineIndexAndSourceNameSet.find(std::make_pair(lineIndex, name)) != m_breakpointSlow.lineIndexAndSourceNameSet.end())
			return true;
	}
	return false;
}

//----- 17.10.20 Fukushiro M. 削除始 ()-----
///// <summary>
///// Tcs the is suspended.
///// This function works at the main application's thread.
///// </summary>
///// <returns></returns>
//bool CDlgSticktrace::TC_IsSuspended()
//{
//	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
//	switch (mode)
//	{
//	case Mode::SUSPEND:
//		return true;
//	default:
//		return false;
//	}
//}
//
///// <summary>
///// Tcs the get mode.
///// This function works at the main application's thread.
///// </summary>
///// <returns></returns>
//int CDlgSticktrace::TC_GetMode()
//{
//	return (int)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
//}
//----- 17.10.20 Fukushiro M. 削除終 ()-----

/// <summary>
/// Tcs the on suspended.
/// This function works at the main application's thread.
/// </summary>
/// <returns></returns>
bool CDlgSticktrace::TC_OnSuspended()
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::ON_SUSPENDED;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Tcs the on suspended.
/// This function works at the main application's thread.
/// </summary>
/// <returns></returns>
bool CDlgSticktrace::TC_OnResumed()
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::ON_RESUMED;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// This function works at the main application's thread.
/// </summary>
/// <param name="name">The name.</param>
/// <param name="lineIndex">Index of the line.</param>
/// <returns></returns>
bool CDlgSticktrace::TC_Jump(const char * name, int lineIndex)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::JUMP;
	m_incmd.strParam1 = (name == nullptr) ? "" : name;
	m_incmd.i64Param1 = lineIndex;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Tcs the on start.
/// This function works at the main application's thread.
/// </summary>
/// <returns></returns>
bool CDlgSticktrace::TC_OnStart()
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::ON_START;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Tcs the on stop.
/// This function works at the main application's thread.
/// </summary>
/// <returns></returns>
bool CDlgSticktrace::TC_OnStop()
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::ON_STOP;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Tcs the output error.
/// This function works at the main application's thread.
/// </summary>
/// <param name="message">The message.</param>
/// <returns></returns>
bool CDlgSticktrace::TC_OutputError(const char * message)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::OUTPUT_ERROR;
	m_incmd.strParam1 = (message == nullptr) ? "" : message;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Tcs the output debug.
/// This function works at the main application's thread.
/// </summary>
/// <param name="message">The message.</param>
/// <returns></returns>
bool CDlgSticktrace::TC_OutputDebug(const char * message)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::OUTPUT_DEBUG;
	m_incmd.strParam1 = (message == nullptr) ? "" : message;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Tcs the set watch.
/// This function works at the main application's thread.
/// </summary>
/// <param name="data">The data.</param>
/// <returns></returns>
bool CDlgSticktrace::TC_SetWatch(const std::string& data)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::SET_WATCH;
	m_incmd.strParam1 = data;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Tcs the get command.
/// This function works at the main application's thread.
/// </summary>
/// <param name="param">The parameter.</param>
/// <param name="waitMilliseconds">The wait milliseconds.</param>
/// <returns></returns>
SticktraceCommand CDlgSticktrace::TC_GetCommand(std::string & param, uint32_t waitMilliseconds)
{
	AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
	if (m_outcmd.command == SticktraceCommand::NONE)
	{
		// Wait for being set the command by the Sticktrace Window.
		if (!acs.SleepConditionVariable(waitMilliseconds))
			return SticktraceCommand::NONE;
	}
	param = m_outcmd.strParam1;
	const auto command = m_outcmd.command;
	m_outcmd.command = SticktraceCommand::NONE;
	return command;
}

void CDlgSticktrace::GetWatchedVariables(std::vector<std::string>& vTopLevelName, std::unordered_set<std::string>& stExpandedName) const
{
	/// <summary>
	/// CListCtrlの行を順番に検査し、テーブル型変数で「EXPANDED（展開）」のアイコンが設定されている行を
	/// 記録する。
	/// 行の変数は、下図のようにインデントが付けられている。インデント位置で再帰呼出しを実行する。
	/// 
	///  [-] e
	///     [-] 1
	///         ├ "abc"
	///      ├ "xyz"
	///      a
	/// </summary>
	/// <param name="lvi">リスト項目取得用ワーク。</param>
	/// <param name="pLsvWatch">リストコントロール.</param>
	/// <param name="index">リストコントロールの行インデックス。検索開始位置。</param>
	/// <param name="iIndent">許されるインデント。</param>
	/// <param name="wcpPiledVarName">連結変数名。変数名を上位から順に'\b'でつないだもの。上の例の"abc"では「e\b1\b"abc"」となる。</param>
	/// <param name="vTopLevelName">レベル0（トップレベル）の変数名。上例では「e」「a」</param>
	/// <param name="stExpandedName">「EXPANDED」が設定された行の連結変数名。</param>
	/// <returns></returns>
	std::function<int(LVITEM&, CListCtrl*, int, int, const char*, std::vector<std::string>&, std::unordered_set<std::string>&)> GetLevelItem = [&GetLevelItem](LVITEM& lvi, CListCtrl* pLsvWatch, int index, int iIndent, const char* wcpPiledVarName, std::vector<std::string>& vTopLevelName, std::unordered_set<std::string>& stExpandedName)
	{
		// 以下のようなウォッチウィンドウの表示状態の場合、[-]の変数（EXPANDED）をstExpandedNameに集める。
		// また、トップレベルの変数（e、a）をvTopLevelNameに集める。
		// 
		// [-] e
		//    [-] 1
		//        ├ "abc"
		//     ├ "xyz"
		//     a

		while (index != pLsvWatch->GetItemCount())
		{
			lvi.iItem = index;
			pLsvWatch->GetItem(&lvi);
			if (lvi.iIndent != iIndent) break;

			std::string szText;
			Astrwstr::wstr_to_astr(szText, lvi.pszText);
			// レベル0（つまりトップの変数）の名前を集める。
			if (lvi.iIndent == 0) vTopLevelName.push_back(szText);
			// 連結変数名。
			const std::string wstrNewPiledVarName = (wcpPiledVarName == NULL) ? szText : std::string(wcpPiledVarName) + "\b" + szText;
			if (lvi.iImage == 0)
				//----- アイコンがテーブル「COLLAPSED（閉じている）」を表す場合 -----
			{
				// 「COLLAPSED（閉じている）」「EXPANDED（開いている）」は関係なく調べる。通常は開いている場合のみ下位が表示されるが、
				// マウスクリックにより閉じたときにもここが処理されるため。
				index = GetLevelItem(lvi, pLsvWatch, index + 1, iIndent + 1, wstrNewPiledVarName.c_str(), vTopLevelName, stExpandedName);
			}
			else
				if (lvi.iImage == 1)
					//----- アイコンがテーブル「EXPANDED（開いている）」を表す場合 -----
				{
					// 「COLLAPSED（閉じている）」「EXPANDED（開いている）」は関係なく調べる。通常は開いている場合のみ下位が表示されるが、
					// マウスクリックにより閉じたときにもここが処理されるため。
					// 「EXPANDED」はstExpandedNameに記録しておく。後で変数を表示する際に、同じ展開状態を復元するため。
					// stExpandedName にはメンバー変数までを辿る名称（連結変数名）を挿入。例「e\b1\b"abc"」
					stExpandedName.insert(wstrNewPiledVarName);
					index = GetLevelItem(lvi, pLsvWatch, index + 1, iIndent + 1, wstrNewPiledVarName.c_str(), vTopLevelName, stExpandedName);
				}
				else
				{
					index++;
				}
		}
		return index;
	};
	// 変数名を受け取るバッファ。
	wchar_t textBuff[1024];
	LVITEM lvi;
	memset(&lvi, 0, sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;	// テキスト、イメージ、インデントを取得。
	lvi.iSubItem = 0;
	lvi.pszText = textBuff;
	lvi.cchTextMax = _countof(textBuff);

	std::vector<std::string> vTopLevelNameTmp;
	std::unordered_set<std::string> stExpandedNameTmp;
	GetLevelItem(lvi, (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH), 0, 0, nullptr, vTopLevelNameTmp, stExpandedNameTmp);
	if (m_sandbox.empty())
	{
		vTopLevelName = vTopLevelNameTmp;
		stExpandedName = stExpandedNameTmp;
	}
	else
	{
		for (const auto & name : vTopLevelNameTmp)
			vTopLevelName.emplace_back(m_sandbox + "." + name);
		for (const auto & name : stExpandedNameTmp)
			stExpandedName.insert(m_sandbox + "." + name);
	}
}

void CDlgSticktrace::UpdateWatchWindow()
{
	// // スクリプトフック中のみ動作する。
	// if (!IsScriptPausing()) return;

	// 変数名を受け取るバッファ。
	std::vector<std::string> vTopLevelName;
	std::unordered_set<std::string> stExpandedName;
	GetWatchedVariables(vTopLevelName, stExpandedName);
	std::string data;
	Stickutil::Serialize(data, vTopLevelName);
	Stickutil::Serialize(data, stExpandedName);

	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceCommand::GET_VARIABLE;
		m_outcmd.strParam1 = data;
		acs.WakeConditionVariable();
	}

	// // ウォッチウィンドウ。
	// CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// ウォッチウィンドウを一度クリアする。
	// pLsvWatch->DeleteAllItems();

	// std::function<int(CListCtrl*, int, int, int, LVITEM&, const char*, const char*, const AnyValue&, const std::unordered_set<std::string>&)> f;


}

void CDlgSticktrace::UpdateScriptIdList()
{
//----- 17.10.05 Fukushiro M. 削除始 ()-----
//	// 現在選択されているスクリプトID。
//	const std::wstring wstrCurrentScriptId = GetCurrentScriptId();
//	// スクリプトID一覧リスト。
//	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SCE_LST_SCRIPT_ID);
//	pListBox->ResetContent();
//	std::wstringSet stScriptId;
//	FFEnv()->GetScriptIdSet(stScriptId);
//	for (std::wstringSet::const_iterator i = stScriptId.begin(); i != stScriptId.end(); i++)
//		pListBox->AddString(*i);
//	// スクリプトID一覧リストの選択を変更する。
//	if (!SetCurrentScriptId(wstrCurrentScriptId))
//	{
//		// 同じIDが選択できなかった場合、選択は解除されるので変更処理が必要。
//		OnLbnSelchangeSceLstScriptId();
//	}
//----- 17.10.05 Fukushiro M. 削除終 ()-----
}

void CDlgSticktrace::InitLayoutAll()
{
	ClearLayout();

	//----- レイアウト情報を計算 -----

	// ウィンドウの左上と右上に追従してリサイズ。
	InitLayout(
		FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_TOP | FCDlgLayoutRec::HOOK_RIGHT,
		IDC_SCE_STC_TOOL_BORDER,
		IDC_SCE_TAB_SCRIPT				// Tab control for script.
	);		// ツールボーダー

//----- 17.10.05 Fukushiro M. 削除始 ()-----
//	// ウィンドウの左上と左下に追従してリサイズ。
//	InitLayout(
//		FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_TOP | FCDlgLayoutRec::HOOK_BOTTOM,
//	);
//----- 17.10.05 Fukushiro M. 削除終 ()-----

//----- 17.10.05 Fukushiro M. 削除始 ()-----
//	// ウィンドウの左下に追従してリサイズ。
//	InitLayout(
//		FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_BOTTOM,
//	);
//----- 17.10.05 Fukushiro M. 削除終 ()-----

	// ウィンドウの四方に追従してリサイズ。
	InitLayout(
		FCDlgLayoutRec::HOOK_ALL,
		IDC_SCE_EDT_SCRIPT	// スクリプトエディター。
	);

//----- 17.09.27 Fukushiro M. 削除始 ()-----
//	// ウィンドウの右上に追従してリサイズ。
//	InitLayout(
//		FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_TOP,
//	);
//----- 17.09.27 Fukushiro M. 削除終 ()-----

	// ウィンドウの右上と右下に追従してリサイズ。
	InitLayout(
		FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_TOP | FCDlgLayoutRec::HOOK_BOTTOM,
		IDC_SCE_STC_BORDER,
		IDC_SCE_EDT_ERROR,
		IDC_SCE_EDT_OUTPUT,
		IDC_SCE_LSV_WATCH
	);

	// ウィンドウの右上に追従してリサイズ。
	InitLayout(FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_TOP,
		IDC_SCE_TAB_OUTPUT);

	// ウィンドウの右下に追従して移動。
	InitLayout(FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_BOTTOM,
		IDC_SCE_EDT_VARIABLE_NAME,
		IDC_SCE_EDT_VARIABLE_VALUE,
		IDC_SCE_BTN_CHANGE_VARIABLE,
		IDC_SCE_BTN_ADD_WATCH,
		IDC_SCE_BTN_DELETE_WATCH,
		IDC_SCE_STC_LINE_NUMBER			// 行番号
	);
} // void CDlgSticktrace::InitLayoutAll ()

CRect CDlgSticktrace::GetBorderRect() const
{
	CRect rtBorder;
	GetDlgItem(IDC_SCE_STC_BORDER)->GetWindowRect(rtBorder);
	ScreenToClient(&rtBorder);
	return rtBorder;
} // CDlgSticktrace::GetBorderRect.

void CDlgSticktrace::MoveControl(WORD wControlId, const CSize& szMove)
{
	CRect rt;
	GetDlgItem(wControlId)->GetWindowRect(rt);
	ScreenToClient(rt);
	const CPoint ptTopLeft = rt.TopLeft() + szMove;
	// ダイアログを移動。
	GetDlgItem(wControlId)->SetWindowPos(
		NULL, ptTopLeft.x, ptTopLeft.y, 0, 0,
		SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER |
		SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);
} // CDlgSticktrace::MoveControl.

void CDlgSticktrace::MoveBorder(int dx)
{
	const WORD ids[] = {
		IDC_SCE_TAB_SCRIPT,				// Tab control for script.
		IDC_SCE_EDT_SCRIPT,				// スクリプトウィンドウ。
		IDC_SCE_TAB_OUTPUT,				// タブコントロール。
		IDC_SCE_STC_BORDER,				// ボーダーウィンドウ。
		IDC_SCE_EDT_ERROR,				// エラー出力ウィンドウ。
		IDC_SCE_EDT_OUTPUT,				// 出力ウィンドウ。
		IDC_SCE_LSV_WATCH,				// ウォッチウィンドウ。
		IDC_SCE_EDT_VARIABLE_NAME,		// 変数名ウィンドウ。
		IDC_SCE_EDT_VARIABLE_VALUE,		// 変数値ウィンドウ。
		IDC_SCE_STC_LINE_NUMBER,		// 行番号。
	};
	for (int i = 0; i != _countof(ids); i++)
	{
		CRect rt;		// コントロールウィンドウ領域。
		GetDlgItem(ids[i])->GetWindowRect(rt);
		ScreenToClient(&rt);
		switch (ids[i])
		{
		case IDC_SCE_TAB_SCRIPT:				// Tab control for script.
		case IDC_SCE_EDT_SCRIPT:				// スクリプトウィンドウ。
			rt.right += dx;
			break;
		case IDC_SCE_STC_BORDER:				// ボーダーウィンドウ
		case IDC_SCE_STC_LINE_NUMBER:			// 行番号。
			rt.left += dx;
			rt.right += dx;
			break;
		default:
			rt.left += dx;
		}
		GetDlgItem(ids[i])->MoveWindow(rt);
	}
	// レイアウト情報を設定。
	InitLayoutAll();
} // CDlgSticktrace::MoveBorder.

/// <summary>
/// Sets the source.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
/// <param name="sandbox">Name of sandbox.</param>
/// <param name="name">The name.</param>
/// <param name="src">The source.</param>
void CDlgSticktrace::SetSource(const std::string & sandbox, const std::string & name, const std::wstring & src)
{
	auto i = m_nameToSandSource.find(name);
	if (i == m_nameToSandSource.end())
	{
		m_sourceNameArray.emplace_back(name);
		m_nameToSandSource[name] = std::make_pair(sandbox, src);
	}
	else
	{
		i->second.first = sandbox;
		i->second.second = src;
	}

	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);
	const auto text = UtilDlg::GetItemText(tabCtrl, tabCtrl->GetCurSel());
	tabCtrl->DeleteAllItems();
	int curIndex = 0;
	std::wstring wname;
	for (int i = 0; i != m_sourceNameArray.size(); i++)
	{
		Astrwstr::astr_to_wstr(wname, m_sourceNameArray[i]);
		tabCtrl->InsertItem(i, wname.c_str());
		if (text == wname)
			curIndex = i;
	}
	if (m_sourceNameArray.empty())
		tabCtrl->InsertItem(0, L"");
	tabCtrl->SetCurSel(curIndex);

	OnTcnSelchangeSceTabScript(nullptr, nullptr);

}

/// <summary>
/// Jumps the specified name.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
/// <param name="name">The name.</param>
/// <param name="lineIndex">Index of the line.</param>
/// <param name="markerType">Type of the marker.</param>
/// <param name="selectLine">if set to <c>true</c> [select line].</param>
void CDlgSticktrace::Jump(const std::string & name, int lineIndex, CFCTextEdit::MarkerType markerType, bool selectLine)
{
	// constexpr auto markerType = CFCTextEdit::MarkerType::MARKER_TRACELINE;
	m_textEditor.ClearMarker(markerType);
	const auto i = std::find(m_sourceNameArray.begin(), m_sourceNameArray.end(), name);
	if (i != m_sourceNameArray.end())
	{
		const auto tabIndex = (int)(i - m_sourceNameArray.begin());
		CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);
		if (tabCtrl->GetCurSel() != tabIndex)
		{
			m_textEditor.AddMarker(markerType, name, lineIndex);
			tabCtrl->SetCurSel(tabIndex);
			OnTcnSelchangeSceTabScript(nullptr, nullptr);
			m_textEditor.SetCurLineIndex(lineIndex, selectLine);
		}
		else
		{
			m_textEditor.AddMarker(markerType, name, lineIndex);
			m_textEditor.SetCurLineIndex(lineIndex, selectLine);
		}
	}
	if (m_textEditor.IsThereMarkerRegion())
		m_textEditor.RedrawMarker();
	//m_textEditor.SetFocus();
}

/// <summary>
/// Jumps the error location, where the cursor is staying in the error message window.
/// </summary>
/// <returns></returns>
bool CDlgSticktrace::JumpErrorLocation()
{
	// エラーメッセージは以下の形式。
	//
	// -----------------------------------------
	// error 5:プログラム実行中のエラーです。
	// [xyz]:33: attempt to call method 'ABC' (a nil value)
	// stack traceback:
	//   [xyz]:33: in function 'CalcLPoint'
	//   [xyz]:58: in function <[xyz]:48>
	// -----------------------------------------
	// Ex. when loading.
	// [string "script1"]:8: unexpected symbol near '>'
	// -----------------------------------------
	// Ex. when running.
	// [string "script1"]:22 : attempt to call a nil value(global 'Stick')
	// 	stack traceback :
	// [string "script1"] : 22 : in function 'abc2'

	// 選択範囲を取得。
	int startCharIndex;
	int endCharIndex;
	m_errorout.GetSel(startCharIndex, endCharIndex);
	// 選択範囲の行を取得。
	const int startLineIndex = m_errorout.LineFromChar(startCharIndex);
	const auto len = m_errorout.LineLength(m_errorout.LineIndex(startLineIndex));
	std::vector<wchar_t> vBuff(len + 1, L'\0');
	m_errorout.GetLine(startLineIndex, vBuff.data(), vBuff.size());
	std::string strline;
	Astrwstr::wstr_to_astr(strline, vBuff.data());
	//const wchar_t* wcpLine = vBuff.data();

	// Hits the following.
	// [string "script1"]:8: unexpected symbol near '>'

	const std::regex re(R"--([^\[]*\[string\s+"([^"]+)"\]\:(\d+)\:.*)--");
	std::smatch match;
	if (std::regex_match(strline, match, re))
	{
		// match(0)はwcpLine全体。
		ASSERT(match.size() == 3);
		auto wstrScriptId = strline.substr(match.position(1), match.length(1));
		auto wstrLineNumber = strline.substr(match.position(2), match.length(2));
		const int iLineNumber = std::strtol(wstrLineNumber.c_str(), nullptr, 10);
		Jump(wstrScriptId, iLineNumber - 1, CFCTextEdit::MARKER_NOTIFY, true);
		return true;
	}
	return false;
}

/// <summary>
/// Called when script starts.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
void CDlgSticktrace::OnStart()
{
	::InterlockedExchange(&m_breakpointFast.suspendMode, (LONG)Mode::RUN);
	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);
	// Clear the error message window.
	OutputError("");
	// Clear the error position mark.
	Jump("", -1, CFCTextEdit::MARKER_NOTIFY, false);
}

/// <summary>
/// Called when script stop.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
void CDlgSticktrace::OnStop()
{
	::InterlockedExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOP);
	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);
}

/// <summary>
/// Called when the script is suspended.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
void CDlgSticktrace::OnSuspended()
{
	// Set the running state to the suspendMode. Surface of the script buttons depend on the suspendMode variable.
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::SUSPEND, (LONG)Mode::RUN);
	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);
	// Update the watch window.
	UpdateWatchWindow();
}

/// <summary>
/// Called when the script is resumed.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
void CDlgSticktrace::OnResumed()
{
	// Set the running state to the runningMode. Surface of the script buttons depend on the suspendMode variable.
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::RUN, (LONG)Mode::SUSPEND);
	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);
}

/// <summary>
/// Outputs the error message to the error message window.
/// This function clears the last message and set the new message.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
/// <param name="message">The message.</param>
void CDlgSticktrace::OutputError(const std::string & message)
{
	std::wstring wmessage;
	Astrwstr::astr_to_wstr(wmessage, message);
	m_errorout.SetWindowText(wmessage.c_str());
}

/// <summary>
/// Outputs the message to the debug message window.
/// This function does not clear the last message but adds the new message.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
/// <param name="message">The message.</param>
void CDlgSticktrace::OutputDebug(const std::string & message)
{
	std::wstring wmessage;
	Astrwstr::astr_to_wstr(wmessage, message);
	m_output.ReplaceSel(wmessage.c_str());
}

/// <summary>
/// Sets the variable values to the watch window.
/// The data is serialized.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
/// <param name="data">The data.</param>
void CDlgSticktrace::SetWatch(const std::string & data)
{
	auto receiveddata = data.c_str();
	std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>> varIconIndentNameTypeValueArray;
	Stickutil::Unserialize(varIconIndentNameTypeValueArray, receiveddata);
	if (!m_sandbox.empty())
	{
		const auto prefix = m_sandbox + ".";
		for (auto & item : varIconIndentNameTypeValueArray)
		{
			if (strncmp(std::get<2>(item).c_str(), prefix.c_str(), prefix.length()) == 0)
				std::get<2>(item) = std::get<2>(item).substr(prefix.length());
		}
	}

	// ウォッチウィンドウ。
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// ウォッチウィンドウを一度クリアする。
	pLsvWatch->DeleteAllItems();

	LVITEM lvi;
	memset(&lvi, 0, sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.iIndent = 0;

	int index = 0;
	for (const auto & elem : varIconIndentNameTypeValueArray)
	{
		int iImage = 1;	// アイコンイメージのインデックス。
		if (std::get<0>(elem) == "CLOSED")
			iImage = 0;
		else if (std::get<0>(elem) == "OPENED")
			iImage = 1;
		else if (std::get<0>(elem) == "VARIABLE")
			iImage = 2;
		else if (std::get<0>(elem) == "MEMBER")
			iImage = 3;
		const auto iIndent = std::strtol(std::get<1>(elem).c_str(), nullptr, 10);
		std::wstring name;
		std::wstring type;
		std::wstring value;
		Astrwstr::astr_to_wstr(name, std::get<2>(elem));
		Astrwstr::astr_to_wstr(type, std::get<3>(elem));
		Astrwstr::astr_to_wstr(value, std::get<4>(elem));

		lvi.iItem = index;
		lvi.iIndent = iIndent;
		lvi.pszText = (wchar_t*)name.c_str();
		lvi.iImage = iImage;
		pLsvWatch->InsertItem(&lvi);
		// 変数値
		pLsvWatch->SetItemText(index, 1, value.c_str());
		// 変数型
		pLsvWatch->SetItemText(index, 2, type.c_str());
		// 親変数のインデックスを設定。
		pLsvWatch->SetItemData(index, -1);
		index++;
	}
}

/*
void CDlgSticktrace::OnStart()
{
	auto button = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE);
	button->SetCheck(BST_CHECKED);
}

void CDlgSticktrace::OnStop()
{
	auto button = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE);
	button->SetCheck(BST_UNCHECKED);
	button = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK);
	button->SetCheck(BST_UNCHECKED);
	button = (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP);
	button->SetCheck(BST_UNCHECKED);
}
*/

void CDlgSticktrace::OnTcnSelchangeSceTabOutput(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	const int index = tabCtrl->GetCurSel();
	switch (index)
	{
	case 0:	// エラー出力。
		GetDlgItem(IDC_SCE_EDT_ERROR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_SCE_EDT_ERROR)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(FALSE);
		break;
	case 1:	// 変数。
		GetDlgItem(IDC_SCE_EDT_ERROR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_SCE_EDT_ERROR)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(TRUE);
		break;
	case 2:	// 出力。
		GetDlgItem(IDC_SCE_EDT_ERROR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_LSV_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_SCE_EDT_ERROR)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(FALSE);
		break;
	}
	OnIdleUpdateCmdUI(0, 0);

	if (pResult != NULL)
		*pResult = 0;
}

void CDlgSticktrace::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetBorderRect().PtInRect(point))
	{	//----- ペイン分割線上にマウスがある場合 -----
		// OnLButtonUpまでこのWindowにイベント。
		::SetCapture(m_hWnd);
		// ペインボーダー上でボタンが押されているか？
		m_bIsBtnOnPaneBorder = TRUE;
		// カーソルを変更.
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
		//----- ペイン分割線のクライアント座標をm_rtBarに
		//		設定し、CFCMainFrameのクライアント座標に変換する -----
		CRect rtBorder = GetBorderRect();
		m_rtBar.SetRect((rtBorder.left + rtBorder.right) / 2, rtBorder.top,
			(rtBorder.left + rtBorder.right) / 2, rtBorder.bottom);
		//----- CFCMainFrame上に疑似ペイン分割線を描画 -----
		CClientDC dcFrame(this);
		// 修正線描画用クラスを作成。
		FCEditDraw edrawFrame(&dcFrame, FD_PANEBAR_WIDTH, FD_PANEBAR_COLOR ^ RGB(255, 255, 255), -1, 0, 0);
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));

		m_hwndWhenBorderMoving = ::GetFocus();
		SetTimer(BORDER_TIMER, 50, nullptr);
	}
	// Superclass関数実行。
	BASE_CLASS::OnLButtonDown(nFlags, point);
}


void CDlgSticktrace::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsBtnOnPaneBorder)
	{	//----- ペインボーダー上でボタンが押されている場合 -----
		KillTimer(BORDER_TIMER);
		// マウスイベントキャプチャを解除。
		::ReleaseCapture();
		// ペインボーダー上でボタンが押されているか？
		m_bIsBtnOnPaneBorder = FALSE;
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
		//----- 前回描いた疑似ペイン分割線を消去 -----
		CClientDC dcFrame(this);
		// 修正線描画用クラスを作成。
		FCEditDraw edrawFrame(&dcFrame, FD_PANEBAR_WIDTH, FD_PANEBAR_COLOR ^ RGB(255, 255, 255), -1, 0, 0);
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));

		// ペインボーダーの移動距離。
		const int pxMove = m_rtBar.CenterPoint().x - GetBorderRect().CenterPoint().x;
		MoveBorder(pxMove);

		// ボーダーの位置（比率）を取得。
		CRect clientRect;
		GetClientRect(clientRect);
		const double borderX = (double)GetBorderRect().CenterPoint().x / (double)clientRect.Width();

		// ボーダーの位置をレジストリに記録。
		FCRegBase::SaveRegBinary(GetRegKeyName().c_str(), L"BorderX", (LPBYTE)&borderX, sizeof(borderX));
	}
	// Superclass関数実行。
	BASE_CLASS::OnLButtonUp(nFlags, point);
}


void CDlgSticktrace::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bIsBtnOnPaneBorder)
	{	//----- ペインボーダー上でボタンが押されている場合 -----
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
		//----- 前回描いた疑似ペイン分割線を消去 -----
		CClientDC dcFrame(this);
		// 修正線描画用クラスを作成。
		FCEditDraw edrawFrame(&dcFrame, FD_PANEBAR_WIDTH, FD_PANEBAR_COLOR ^ RGB(255, 255, 255), -1, 0, 0);
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));
		//----- 新しいペイン分割線の座標を設定 -----
		// ペインバーの座標を計算。m_rtBar.right は不使用。
		m_rtBar.SetRect(point.x, m_rtBar.top, point.x, m_rtBar.bottom);

		//----- ペインバーの稼働範囲を計算 -----
		CRect rtScript;		// スクリプトウィンドウ領域
		CRect rtChange;		// 変更ボタン領域
		GetDlgItem(IDC_SCE_EDT_SCRIPT)->GetWindowRect(rtScript);	// スクリプトエディター
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->GetWindowRect(rtChange);		// 変更ボタン
		ScreenToClient(&rtScript);
		ScreenToClient(&rtChange);
		// スクリプトウィンドウの幅が100以下にならないよう調整する。右は変更ボタンを隠さないよう。
		if (m_rtBar.left < rtScript.left + 100)
			m_rtBar.left = rtScript.left + 100;
		if (rtChange.left - 10 < m_rtBar.left)
			m_rtBar.left = rtChange.left - 10;
		m_rtBar.right = m_rtBar.left;
		//----- CFCMainFrame上に疑似ペイン分割線を描画 -----
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));
	}
	else
	{	//----- ペインボーダー上でボタンが押されている以外の場合 -----
		if (GetBorderRect().PtInRect(point))
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
	}
	// Superclass関数実行。
	BASE_CLASS::OnMouseMove(nFlags, point);
}


//BOOL CDlgSticktrace::PreTranslateMessage(MSG* pMsg)
//{
//	// TRACE(L"MSG=%x LPARAM=%x WPARAM=%x\n", pMsg->message, pMsg->lParam, pMsg->wParam);
//
//	return BASE_CLASS::PreTranslateMessage(pMsg);
//}


void CDlgSticktrace::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == BORDER_TIMER)
	{
		if (m_hwndWhenBorderMoving != ::GetFocus())
			SendMessage(WM_LBUTTONUP, 0, MAKELPARAM(-1, -1));
	}

	BASE_CLASS::OnTimer(nIDEvent);
}

/// <summary>
/// Called when the script tab control selection was changed.
/// </summary>
/// <param name="pNMHDR">The p NMHDR.</param>
/// <param name="pResult">The p result.</param>
void CDlgSticktrace::OnTcnSelchangeSceTabScript(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);
	const auto & scriptName = m_sourceNameArray.at(tabCtrl->GetCurSel());
	const auto & sand_src = m_nameToSandSource.at(scriptName);
	m_sandbox = sand_src.first;
	m_textEditor.SetContentName(scriptName);
	m_textEditor.SetWindowText(sand_src.second.c_str());
	// SetDlgItemText(IDC_SCE_EDT_SCRIPT, src.c_str());
	if (pResult != nullptr) *pResult = 0;
}

LRESULT CDlgSticktrace::OnUserCommand(WPARAM, LPARAM)
{
	InCmd::Command command;
	std::string strParam1;
	std::string strParam2;
	std::string strParam3;
	__int64 i64Param1;
	{
		AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
		command = m_incmd.command;
		strParam1 = m_incmd.strParam1;
		strParam2 = m_incmd.strParam2;
		strParam3 = m_incmd.strParam3;
		i64Param1 = m_incmd.i64Param1;
		m_incmd.command = CDlgSticktrace::InCmd::Command::NONE;
		acs.WakeConditionVariable();
	}
	switch (command)
	{
	case CDlgSticktrace::InCmd::Command::NONE:
		break;
	case CDlgSticktrace::InCmd::Command::SET_SOURCE:
	{
		std::wstring wsrc;
		Astrwstr::astr_to_wstr(wsrc, strParam3);
		SetSource(strParam1, strParam2, wsrc);
		break;
	}
	case CDlgSticktrace::InCmd::Command::ON_START:
		OnStart();
		break;
	case CDlgSticktrace::InCmd::Command::ON_STOP:
		OnStop();
		break;
	case CDlgSticktrace::InCmd::Command::ON_SUSPENDED:
		OnSuspended();
		break;
	case CDlgSticktrace::InCmd::Command::ON_RESUMED:
		OnResumed();
		break;
	case CDlgSticktrace::InCmd::Command::JUMP:
		Jump(strParam1, (int)i64Param1, CFCTextEdit::MarkerType::MARKER_TRACELINE, false);
		break;
	case CDlgSticktrace::InCmd::Command::OUTPUT_ERROR:
		OutputError(strParam1);
		break;
	case CDlgSticktrace::InCmd::Command::OUTPUT_DEBUG:
		OutputDebug(strParam1);
		break;
	case CDlgSticktrace::InCmd::Command::SET_WATCH:
		SetWatch(strParam1);
		break;
	default:
		break;
	}

	return 1;
}

LRESULT CDlgSticktrace::OnUserBreakpointUpdated(WPARAM, LPARAM)
{
	const auto & breakpoint = m_textEditor.GetBreakpoint();
	{
		AutoLeaveCS acs(m_breakpointSlow.cs);
		m_breakpointSlow.lineIndexToIsBreak.clear();
		for (const auto & bp : breakpoint)
		{
			if ((int)m_breakpointSlow.lineIndexToIsBreak.size() <= bp.first.second)
				m_breakpointSlow.lineIndexToIsBreak.resize(bp.first.second + 1);
			m_breakpointSlow.lineIndexToIsBreak[bp.first.second] = 1;
			m_breakpointSlow.lineIndexAndSourceNameSet.insert(std::make_pair(bp.first.second, bp.first.first));
		}
		::InterlockedExchange(&m_breakpointFast.breakpointCount, (LONG)m_breakpointSlow.lineIndexAndSourceNameSet.size());
	}
	return 1;
}

//********************************************************************************************
/*!
* @brief	CFCTextEditのマーカー領域クリック。
* @author	Fukushiro M.
* @date	2014/09/16(火) 21:07:54
*
* @param[in]	WPARAM	wParam	ControlId。
* @param[in]	LPARAM	lParam	lineIndex。
*
* @return	LRESULT	1
*/
//********************************************************************************************
LRESULT CDlgSticktrace::OnUserTextEditMarkerClicked(WPARAM wParam, LPARAM lParam)
{
	ASSERT(wParam == IDC_SCE_EDT_SCRIPT);
	const auto name = m_sourceNameArray.at(((CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT))->GetCurSel());

	const BOOL isThereMarker = m_textEditor.IsThereMarker(CFCTextEdit::MARKER_BREAKPOINT, name, lParam);
	if (isThereMarker)
		m_textEditor.RemoveMarker(CFCTextEdit::MARKER_BREAKPOINT, name, lParam);
	else
		m_textEditor.AddMarker(CFCTextEdit::MARKER_BREAKPOINT, name, lParam);
	if (m_textEditor.IsThereMarkerRegion())
		m_textEditor.RedrawMarker();
	return 1;
} // CDlgSticktrace::OnUserTextEditMarkerClicked

void CDlgSticktrace::OnBnClickedSceBtnDebugContinue()
{
	TRACE(L"OnBnClickedSceBtnDebugContinue\n");
	// ::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::RUN, (LONG)Mode::SUSPEND);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceCommand::RESUME;
		acs.WakeConditionVariable();
	}
	// // Update the display status of script control buttons.
	// PostMessage(WM_IDLEUPDATECMDUI);
	::SetFocus(NULL);
	// (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)
}


void CDlgSticktrace::OnBnClickedSceBtnDebugBreak()
{
	TRACE(L"OnBnClickedSceBtnDebugBreak\n");

	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::SUSPEND, (LONG)Mode::RUN);
	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);
	::SetFocus(NULL);
}


void CDlgSticktrace::OnBnClickedSceBtnDebugStop()
{
	TRACE(L"OnBnClickedSceBtnDebugStop\n");
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOPPING, (LONG)Mode::RUN);
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOPPING, (LONG)Mode::SUSPEND);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceCommand::STOP;
		acs.WakeConditionVariable();
	}
	::SetFocus(NULL);
}

void CDlgSticktrace::OnBnClickedSceBtnDebugStepToNext()
{
	TRACE(L"OnBnClickedSceBtnDebugStepToNext\n");
	//::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::PROCEED_TO_NEXT, (LONG)Mode::SUSPEND);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceCommand::PROCEED_NEXT;
		acs.WakeConditionVariable();
	}
	// // Update the display status of script control buttons.
	// PostMessage(WM_IDLEUPDATECMDUI);
	::SetFocus(NULL);
}

LRESULT CDlgSticktrace::OnIdleUpdateCmdUI(WPARAM, LPARAM)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const auto isDebugMode = (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) != 0);
	switch (mode)
	{
	case CDlgSticktrace::Mode::STOP:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(TRUE);
		break;
	case CDlgSticktrace::Mode::STOPPING:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(TRUE);
		break;
	case CDlgSticktrace::Mode::RUN:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(TRUE);
		break;
	case CDlgSticktrace::Mode::SUSPEND:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
	return 1;
}

void CDlgSticktrace::OnSceEditGotoLine()
{
} // CDlgSticktrace::OnSceEditGotoLine.

void CDlgSticktrace::OnUpdateSceEditGotoLine(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceEditGotoLine.

void CDlgSticktrace::OnSceWinKeyword()
{
} // CDlgSticktrace::OnSceWinKeyword.

void CDlgSticktrace::OnUpdateSceWinKeyword(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceWinKeyword.

void CDlgSticktrace::OnEditFindNextText()
{
} // CDlgSticktrace::OnEditFindNextText.

void CDlgSticktrace::OnUpdateEditFindNextText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateEditFindNextText.

void CDlgSticktrace::OnEditFindPrevText()
{
} // CDlgSticktrace::OnEditFindPrevText.

void CDlgSticktrace::OnUpdateEditFindPrevText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateEditFindPrevText.

void CDlgSticktrace::OnSceOutwinGotoLocation()
{
} // CDlgSticktrace::OnSceOutwinGotoLocation.

void CDlgSticktrace::OnUpdateSceOutwinGotoLocation(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceOutwinGotoLocation.

void CDlgSticktrace::OnSceOutwinClear()
{
} // CDlgSticktrace::OnSceOutwinClear.

void CDlgSticktrace::OnUpdateSceOutwinClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceOutwinClear.

//********************************************************************************************
/*!
* @brief	[ブレークポイントの設定/解除]コマンドから実行される。
* @author	Fukushiro M.
* @date	2014/09/14(日) 07:43:52
*
* @return	なし (none)
*/
//********************************************************************************************
void CDlgSticktrace::OnSceDebugToggleBreakpoint()
{
	const int iCurLineIndex = m_textEditor.GetCurLineIndex();
	SendMessage(WM_USER_TEXT_EDIT_MARKER_CLICKED, IDC_SCE_EDT_SCRIPT, iCurLineIndex);
} // CDlgSticktrace::OnSceDebugToggleBreakpoint.

  //********************************************************************************************
  /*!
  * @brief	[ブレークポイントの設定/解除]コマンドの有効・無効を設定する。
  * @author	Fukushiro M.
  * @date	2014/09/14(日) 07:43:52
  *
  * @param[out]	CCmdUI*	pCmdUI	コマンドの状態を設定。
  *
  * @return	なし (none)
  */
  //********************************************************************************************
void CDlgSticktrace::OnUpdateSceDebugToggleBreakpoint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceDebugToggleBreakpoint.

void CDlgSticktrace::OnSceDebugClearBreakpoint()
{
} // CDlgSticktrace::OnSceDebugClearBreakpoint.

void CDlgSticktrace::OnUpdateSceDebugClearBreakpoint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugClearBreakpoint.

void CDlgSticktrace::OnSceDebugMode()
{
	const auto isDebugMode = (IsDlgButtonChecked(IDC_SCE_CHK_DEBUG_MODE) == BST_CHECKED);
	CheckDlgButton(IDC_SCE_CHK_DEBUG_MODE, isDebugMode ? BST_UNCHECKED : BST_CHECKED);
	PostMessage(WM_COMMAND, MAKELPARAM(IDC_SCE_CHK_DEBUG_MODE, BN_CLICKED), 0);
} // CDlgSticktrace::OnSceDebugMode.

void CDlgSticktrace::OnUpdateSceDebugMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->IsWindowEnabled());
	const auto isDebugMode = (IsDlgButtonChecked(IDC_SCE_CHK_DEBUG_MODE) == BST_CHECKED);
	pCmdUI->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);

} // CDlgSticktrace::OnUpdateSceDebugMode.

void CDlgSticktrace::OnSceDebugContinue()
{
	PostMessage(WM_COMMAND, MAKELPARAM(IDC_SCE_BTN_DEBUG_CONTINUE, BN_CLICKED), 0);
} // CDlgSticktrace::OnSceDebugContinue.

void CDlgSticktrace::OnUpdateSceDebugContinue(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->IsWindowEnabled());
} // CDlgSticktrace::OnUpdateSceDebugContinue.

void CDlgSticktrace::OnSceDebugBreak()
{
	PostMessage(WM_COMMAND, MAKELPARAM(IDC_SCE_BTN_DEBUG_BREAK, BN_CLICKED), 0);
} // CDlgSticktrace::OnSceDebugBreak.

void CDlgSticktrace::OnUpdateSceDebugBreak(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->IsWindowEnabled());
} // CDlgSticktrace::OnUpdateSceDebugBreak.

void CDlgSticktrace::OnSceDebugStepToNext()
{
	PostMessage(WM_COMMAND, MAKELPARAM(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT, BN_CLICKED), 0);
} // CDlgSticktrace::OnSceDebugStepToNext.

void CDlgSticktrace::OnUpdateSceDebugStepToNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->IsWindowEnabled());
} // CDlgSticktrace::OnUpdateSceDebugStepToNext.

void CDlgSticktrace::OnSceDebugUntilCaret()
{
} // CDlgSticktrace::OnSceDebugUntilCaret.

void CDlgSticktrace::OnUpdateSceDebugUntilCaret(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugUntilCaret.

void CDlgSticktrace::OnSceDebugStop()
{
	PostMessage(WM_COMMAND, MAKELPARAM(IDC_SCE_BTN_DEBUG_STOP, BN_CLICKED), 0);
} // CDlgSticktrace::OnSceDebugStop.

void CDlgSticktrace::OnUpdateSceDebugStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->IsWindowEnabled());
} // CDlgSticktrace::OnUpdateSceDebugStop.

void CDlgSticktrace::OnSceDebugChangeVariable()
{
} // CDlgSticktrace::OnSceDebugChangeVariable.

void CDlgSticktrace::OnUpdateSceDebugChangeVariable(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugChangeVariable.

void CDlgSticktrace::OnSceDebugAddWatch()
{
} // CDlgSticktrace::OnSceDebugAddWatch.

void CDlgSticktrace::OnUpdateSceDebugAddWatch(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugAddWatch.

void CDlgSticktrace::OnSceDebugDeleteWatch()
{
} // CDlgSticktrace::OnSceDebugDeleteWatch.

void CDlgSticktrace::OnUpdateSceDebugDeleteWatch(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugDeleteWatch.

void CDlgSticktrace::OnSceWinError()
{
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	tabCtrl->SetCurSel(TAB_INDEX_ERROR);
	// Notify the tab selection has been changed.
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);
} // CDlgSticktrace::OnSceWinError.

void CDlgSticktrace::OnUpdateSceWinError(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinError.

void CDlgSticktrace::OnSceWinWatch()
{
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	tabCtrl->SetCurSel(TAB_INDEX_WATCH);
	// Notify the tab selection has been changed.
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);
} // CDlgSticktrace::OnSceWinWatch.

void CDlgSticktrace::OnUpdateSceWinWatch(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinWatch.

void CDlgSticktrace::OnSceWinOutput()
{
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	tabCtrl->SetCurSel(TAB_INDEX_OUTPUT);
	// Notify the tab selection has been changed.
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);
} // CDlgSticktrace::OnSceWinOutput.

void CDlgSticktrace::OnUpdateSceWinOutput(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinOutput.

void CDlgSticktrace::OnSceWinVariableName()
{
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	tabCtrl->SetCurSel(TAB_INDEX_WATCH);
	// Notify the tab selection has been changed.
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);
	GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->SetFocus();
} // CDlgSticktrace::OnSceWinVariableName.

void CDlgSticktrace::OnUpdateSceWinVariableName(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinVariableName.

void CDlgSticktrace::OnSceWinVariableValue()
{
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	tabCtrl->SetCurSel(TAB_INDEX_WATCH);
	// Notify the tab selection has been changed.
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);
	GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->SetFocus();
} // CDlgSticktrace::OnSceWinVariableValue.

void CDlgSticktrace::OnUpdateSceWinVariableValue(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinVariableValue.

void CDlgSticktrace::OnHelp()
{
} // CDlgSticktrace::OnHelp.

void CDlgSticktrace::OnUpdateHelp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateHelp.

/// <summary>
/// Called when [the change variable button clicked].
/// </summary>
void CDlgSticktrace::OnBnClickedSceBtnChangeVariable()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}

/// <summary>
/// Called when [the add variable button clicked].
/// </summary>
void CDlgSticktrace::OnBnClickedSceBtnAddWatch()
{
	// 変数名を取得。
	std::wstring wstr;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_NAME, wstr);
	UtilString::Trim(wstr);
	if (!wstr.empty())
	{
		// 行を追加。
		CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
		const int itemCount = pLsvWatch->GetItemCount();

		LVITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = itemCount;
		lvi.iSubItem = 0;
		lvi.pszText = (wchar_t*)wstr.c_str();
		lvi.iImage = 0;
		pLsvWatch->InsertItem(&lvi);

		// ウォッチウィンドウを更新。
		UpdateWatchWindow();
	}
}

/// <summary>
/// Called when [the delete variable button clicked].
/// </summary>
void CDlgSticktrace::OnBnClickedSceBtnDeleteWatch()
{
	// 行を追加。
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	auto curSel = UtilDlg::GetCurSel(pLsvWatch);
	if (curSel != -1)
	{
		pLsvWatch->DeleteItem(curSel);
	}
}

void CDlgSticktrace::OnNMClickSceLsvWatch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMITE = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMITE->iSubItem == 0)
	{
		CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
		LVITEM lvi;
		memset(&lvi, 0, sizeof(lvi));
		lvi.mask = LVIF_IMAGE | LVIF_INDENT;
		lvi.iSubItem = 0;
		lvi.iItem = pNMITE->iItem;
		pLsvWatch->GetItem(&lvi);
		int left = 2 + 16 * lvi.iIndent;
		int right = left + 16;
		if (left <= pNMITE->ptAction.x && pNMITE->ptAction.x <= right)
		{
			lvi.iImage = (lvi.iImage == 1) ? 0 : 1;
			pLsvWatch->SetItem(&lvi);

			// ウォッチウィンドウを更新。
			UpdateWatchWindow();
		}
	}
	if (pResult != nullptr) *pResult = 0;
}

//********************************************************************************************
/*!
* @brief	CFCDdEditコントロールがダブルクリックされたときに実行される。
* @author	Fukushiro M.
* @date	2014/09/12(金) 17:53:35
*
* @param[in]	WPARAM	wParam	送信元のコントロールID。
* @param[in]	LPARAM	未使用
*
* @return	LRESULT	0:CFCDdEditでDBLCLKを処理する/1:処理しない
*/
//********************************************************************************************
LRESULT CDlgSticktrace::OnUserDdEditDblClked(WPARAM wParam, LPARAM)
{
	if (wParam == IDC_SCE_EDT_ERROR)
	{
		if (JumpErrorLocation())
			return 1;
	}
	return 0;
} // CDlgSticktrace::OnUserDdEditDblClked

  //********************************************************************************************
  /*!
  * @brief	CFCTextEditの現在行の変更の通知。
  * @author	Fukushiro M.
  * @date	2014/09/17(水) 21:50:33
  *
  * @param[in]	WPARAM	wParam	ControlId。
  * @param[in]	LPARAM	lParam	lineIndex。
  *
  * @return	LRESULT	1
  */
  //********************************************************************************************
LRESULT CDlgSticktrace::OnUserTextEditCurlineChanged(WPARAM wParam, LPARAM lParam)
{
	static CString FORM;
	if (FORM.IsEmpty()) FORM.LoadString(IDS_LINE_FORM);
	if (wParam == IDC_SCE_EDT_SCRIPT)
	{
		CString str;
		str.Format((const wchar_t*)FORM, lParam + 1);
		SetDlgItemText(IDC_SCE_STC_LINE_NUMBER, str);
	}
	return 1;
} // CDlgSticktrace::OnUserTextEditCurlineChanged

void CDlgSticktrace::OnBnClickedSceChkDebugMode()
{
	const auto isDebugMode = (IsDlgButtonChecked(IDC_SCE_CHK_DEBUG_MODE) == BST_CHECKED);
	::InterlockedExchange(&m_debugMode.debugMode, isDebugMode ? 1 : 0);
	m_textEditor.ActivateBreakpoint(isDebugMode);
	m_textEditor.RedrawMarker();
}


//BOOL CDlgSticktrace::PreTranslateMessage(MSG* pMsg)
//{
//	TRACE(L"CDlgSticktrace %x %x %x \r\n", pMsg->message, pMsg->wParam, pMsg->lParam);
//
//	return CFCDlgModelessBase::PreTranslateMessage(pMsg);
//}


//LRESULT CDlgSticktrace::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	TRACE(L"CDlgSticktrace::WindowProc %x %x %x \r\n", message, wParam, lParam);
//	if (message == 0x0112)
//		AfxDebugBreak();
//
//	return CFCDlgModelessBase::WindowProc(message, wParam, lParam);
//}


void CDlgSticktrace::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFCDlgModelessBase::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	CString str;
	for (int i = 0; i != pPopupMenu->GetMenuItemCount(); i++)
	{
		auto id = pPopupMenu->GetMenuItemID(i);
		CCmdUI state;
		state.m_pMenu = pPopupMenu;
		state.m_pOther = this;
		state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
		state.m_nIndex = (UINT)i;
		state.m_nID = id;
		OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, nullptr);


		pPopupMenu->GetMenuString(i, str, MF_BYPOSITION);
	}
	// TODO: ここにメッセージ ハンドラー コードを追加します。
}

void CDlgSticktrace::OnLvnItemchangedSceLsvWatch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged & LVIF_STATE && pNMLV->uNewState & LVIS_SELECTED)
	{
		CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
		SetDlgItemText(IDC_SCE_EDT_VARIABLE_NAME, pLsvWatch->GetItemText(pNMLV->iItem, 0));
		SetDlgItemText(IDC_SCE_EDT_VARIABLE_VALUE, pLsvWatch->GetItemText(pNMLV->iItem, 1));
	}

	*pResult = 0;
}
