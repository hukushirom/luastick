// DlgSticktrace.cpp : 実装ファイル。
//

#include "stdafx.h"
#include <thread>
#include "Resource.h"
#include "Astrwstr.h"
#include "UtilString.h"
#include "UtilWin.h"
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
	, m_accelerator(NULL)
	, m_bIsBtnOnPaneBorder(FALSE)	// ペインボーダー上でボタンが押されているか？
	, m_hwndWhenBorderMoving(NULL)
	, m_DGT_debuggerCallbackFunc(nullptr)
	, m_debuggerCallbackData(nullptr)
	, m_isScriptEditable(FALSE)
{
	m_breakpointFast.suspendMode = (LONG)Mode::STOP;
	m_breakpointFast.breakpointCount = 0;

	m_scriptModify.isScriptModified = 0;	// 1:Script is modified/0:Not modified.

	m_breakpointSlow.m_runToCursorLineIndex = -1;

	m_debugMode.debugMode = 1;
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
	ON_NOTIFY(TCN_SELCHANGING, IDC_SCE_TAB_SCRIPT, &CDlgSticktrace::OnTcnSelchangingSceTabScript)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SCE_TAB_SCRIPT, &CDlgSticktrace::OnTcnSelchangeSceTabScript)
	ON_MESSAGE(WM_USER_COMMAND, &CDlgSticktrace::OnUserCommand)
	ON_MESSAGE(WM_USER_BREAKPOINT_UPDATED, &CDlgSticktrace::OnUserBreakpointUpdated)
	ON_MESSAGE(WM_USER_TEXT_EDIT_MARKER_CLICKED, &CDlgSticktrace::OnUserTextEditMarkerClicked)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_CONTINUE, &CDlgSticktrace::OnBnClickedSceBtnDebugContinue)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_BREAK, &CDlgSticktrace::OnBnClickedSceBtnDebugBreak)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_STOP, &CDlgSticktrace::OnBnClickedSceBtnDebugStop)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT, &CDlgSticktrace::OnBnClickedSceBtnDebugStepToNext)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, &CDlgSticktrace::OnIdleUpdateCmdUI)
	ON_COMMAND(ID_FILE_SAVE, &CDlgSticktrace::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CDlgSticktrace::OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_SCRIPT_EDIT, &CDlgSticktrace::OnEditScriptEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SCRIPT_EDIT, &CDlgSticktrace::OnUpdateEditScriptEdit)
	ON_COMMAND(ID_SCE_EDIT_INPUT_LINE, &CDlgSticktrace::OnSceEditInputLine)
	ON_UPDATE_COMMAND_UI(ID_SCE_EDIT_INPUT_LINE, &CDlgSticktrace::OnUpdateEditInputLine)
	ON_COMMAND(ID_SCE_EDIT_GOTO_LINE, &CDlgSticktrace::OnSceEditGotoLine)
	ON_UPDATE_COMMAND_UI(ID_SCE_EDIT_GOTO_LINE, &CDlgSticktrace::OnUpdateSceEditGotoLine)
	ON_COMMAND(ID_SCE_WIN_KEYWORD, &CDlgSticktrace::OnSceWinKeyword)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_KEYWORD, &CDlgSticktrace::OnUpdateSceWinKeyword)
	ON_COMMAND(ID_EDIT_FIND_NEXT_TEXT, &CDlgSticktrace::OnEditFindNextText)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_NEXT_TEXT, &CDlgSticktrace::OnUpdateEditFindNextText)
	ON_COMMAND(ID_EDIT_FIND_PREV_TEXT, &CDlgSticktrace::OnEditFindPrevText)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_PREV_TEXT, &CDlgSticktrace::OnUpdateEditFindPrevText)
	ON_COMMAND(ID_SCE_WIN_CLEAR_ERROR, &CDlgSticktrace::OnSceWinClearError)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_CLEAR_ERROR, &CDlgSticktrace::OnUpdateSceWinClearError)
	ON_COMMAND(ID_SCE_WIN_CLEAR_OUTPUT, &CDlgSticktrace::OnSceWinClearOutput)
	ON_UPDATE_COMMAND_UI(ID_SCE_WIN_CLEAR_OUTPUT, &CDlgSticktrace::OnUpdateSceWinClearOutput)
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
	ON_COMMAND(ID_SCE_DEBUG_RUN_TO_CURSOR, &CDlgSticktrace::OnSceDebugRunToCursor)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_RUN_TO_CURSOR, &CDlgSticktrace::OnUpdateSceDebugRunToCursor)
	ON_COMMAND(ID_SCE_DEBUG_STOP, &CDlgSticktrace::OnSceDebugStop)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_STOP, &CDlgSticktrace::OnUpdateSceDebugStop)
	ON_COMMAND(ID_SCE_DEBUG_SET_VARIABLE, &CDlgSticktrace::OnSceDebugSetVariable)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_SET_VARIABLE, &CDlgSticktrace::OnUpdateSceDebugSetVariable)
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
	ON_COMMAND(ID_OPTION_AUTO_ALTKEY, &CDlgSticktrace::OnOptionAutoAltkey)
	ON_UPDATE_COMMAND_UI(ID_OPTION_AUTO_ALTKEY, &CDlgSticktrace::OnUpdateOptionAutoAltkey)
	ON_BN_CLICKED(IDC_SCE_BTN_SET_VARIABLE, &CDlgSticktrace::OnBnClickedSceBtnSetVariable)
	ON_BN_CLICKED(IDC_SCE_BTN_ADD_WATCH, &CDlgSticktrace::OnBnClickedSceBtnAddWatch)
	ON_BN_CLICKED(IDC_SCE_BTN_DELETE_WATCH, &CDlgSticktrace::OnBnClickedSceBtnDeleteWatch)
	ON_BN_CLICKED(IDC_SCE_CHK_DEBUG_MODE, &CDlgSticktrace::OnBnClickedSceChkDebugMode)
	ON_BN_CLICKED(IDC_SCE_BTN_FIND_NEXT, &CDlgSticktrace::OnBnClickedSceBtnFindNext)
// 21.05.07 Fukushiro M. 1行追加 ()
	ON_BN_CLICKED(IDC_SCE_BTN_GOTO_LINE, &CDlgSticktrace::OnBnClickedSceBtnGotoLine)
	ON_EN_CHANGE(IDC_SCE_EDT_KEYWORD, &CDlgSticktrace::OnChangeSceEdtKeyword)
	ON_WM_INITMENUPOPUP()
	ON_MESSAGE(WM_USER_DDEDIT_DBLCLKED, &CDlgSticktrace::OnUserDdEditDblClked)
	ON_MESSAGE(WM_USER_TEXT_EDIT_CURLINE_CHANGED, &CDlgSticktrace::OnUserTextEditCurlineChanged)
	ON_NOTIFY(NM_CLICK, IDC_SCE_LSV_WATCH, &CDlgSticktrace::OnNMClickSceLsvWatch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SCE_LSV_WATCH, &CDlgSticktrace::OnLvnItemchangedSceLsvWatch)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_NCACTIVATE()
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
	MoveControl(IDC_SCE_BTN_ADD_WATCH, szMove);
	MoveControl(IDC_SCE_BTN_DELETE_WATCH, szMove);
	MoveControl(IDC_SCE_BTN_SET_VARIABLE, szMove);

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
	m_textEditor.SetFont(&m_font);

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
	m_textEditor.SetContextMenu(IDR_POPUP, 0);
	m_textEditor.SetAccelerator(IDR_TEXT_EDITOR_ACCELERATOR);
	m_textEditor.SetReadOnly(!m_isScriptEditable);

	const auto isDebugMode = (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) != 0);
	m_textEditor.ActivateBreakpoint(isDebugMode);
	m_textEditor.SetTabSize(2);
	//tab[]{ 10, 20, 30, 40, 50 ... };
	m_textEditor.SetTabStops(10);
//----- 20.07.25 Fukushiro M. 追加始 ()-----
	m_errorout.SetContextMenu(IDR_POPUP, 1);
	m_output.SetContextMenu(IDR_POPUP, 1);
//----- 20.07.25 Fukushiro M. 追加終 ()-----
	//----- ウォッチリストコントロールの設定 -----
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// フォーカス無し状態でも選択マークが表示されるようにスタイル変更。
	pLsvWatch->ModifyStyle(0, LVS_SHOWSELALWAYS);
	// 全カラム選択の拡張スタイルを設定。
	pLsvWatch->SetExtendedStyle(pLsvWatch->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//----- 14.10.02 Fukushiro M. 追加始 ()-----
	m_watchImage.Create(16, 16, ILC_MASK, 6, 1);
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_COLLAPSED));		// 閉じている(0)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_EXPANDED));		// 開いている(1)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE));		// 変数(2)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_SUBVARIABLE));	// メンバー変数(3)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_VAR_UNKNOWN));	// 未定変数(4)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_VAR_WARN));		// 警告変数(5)

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
	UtilDlg::InsertColumn(pLsvWatch, 0, UtilStr::LoadString((CString&)CString(), IDS_NAME), LVCFMT_LEFT, aColumnWidthInit[0]);
	// 値。
	UtilDlg::InsertColumn(pLsvWatch, 1, UtilStr::LoadString((CString&)CString(), IDS_VALUE), LVCFMT_LEFT, aColumnWidthInit[1]);
	// 型。
	UtilDlg::InsertColumn(pLsvWatch, 2, UtilStr::LoadString((CString&)CString(), IDS_TYPE), LVCFMT_LEFT, aColumnWidthInit[2]);
	// リストコントロールのカラム境界を設定。
	FD_RegLoadListCtrlColumnWidth(this, IDC_SCE_LSV_WATCH);

	// タブコントロールにタブを設定。
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	// エラー出力。
	tabCtrl->InsertItem(TAB_INDEX_ERROR, UtilStr::LoadString((CString&)CString(), IDS_ERROR));
	// 変数。
	tabCtrl->InsertItem(TAB_INDEX_WATCH, UtilStr::LoadString((CString&)CString(), IDS_WATCH));
	// 出力。
	tabCtrl->InsertItem(TAB_INDEX_OUTPUT, UtilStr::LoadString((CString&)CString(), IDS_OUTPUT));
	// タブ変更を通知。
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);

	// Add empty tab into the tab control for script.
	CTabCtrl* tabCtrl2 = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);
	// 空。
	tabCtrl2->InsertItem(0, L"");

	// テキスト領域を更新。
	m_textEditor.UpdateTextRect();

	// アクセラレーター
	m_accelerator = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_DEBUGGER_ACCELERATOR));

	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgSticktrace::SetDebuggerCallback(SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc, void * debuggerCallbackData)
{
	m_DGT_debuggerCallbackFunc = DGT_debuggerCallbackFunc;
	m_debuggerCallbackData = debuggerCallbackData;
}

void CDlgSticktrace::ThreadForDebbugerCallback(
	SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
	void* debuggerCallbackData,
	unsigned int dialogId,
	SticktraceDef::DebuggerCommand command,
	std::string strParam1,
	std::string strParam2
)
{
	SticktraceDef::DebuggerCallbackParam param;
	param.command = command;
	param.strParam1 = strParam1.c_str();
	param.strParam2 = strParam2.c_str();
	DGT_debuggerCallbackFunc(dialogId, &param, debuggerCallbackData);
}

/// <summary>
/// Set the visibility of the debugger window.
/// This function works at the main application's thread.
/// </summary>
/// <param name="show">true:Visible/false:Invisible</param>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_Show(bool show)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::SHOW;
	m_incmd.i64Param1 = show ? 1 : 0;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Check the debugger window is visible or not.
/// This function works at the main application's thread.
/// </summary>
/// <param name="isVisible">true:Visible/false:Invisible</param>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_IsVisible(bool & isVisible)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::IS_VISIBLE;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	isVisible = (m_incmd.i64Param1 != 0);
	return true;
}

/// <summary>
/// Check the script is modified or not.
/// This function works at the main application's thread.
/// </summary>
/// <param name="isModified">true:Modified/false:Not modified</param>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_IsScriptModified(bool & isModified)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::IS_SCRIPT_MODIFIED;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	isModified = (m_incmd.i64Param1 != 0);
	return true;
}

/// <summary>
/// Tcs the set source.
/// This function works at the main application's thread.
/// </summary>
/// <param name="sandbox">Name of sandbox.</param>
/// <param name="name">The name.</param>
/// <param name="src">The source.</param>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_SetSource(const char * sandbox, const char * name, const char * src)
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
/// <returns>true:Debug mode/false:Not debug mode</returns>
bool CDlgSticktrace::APT_IsDebugMode()
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
/// <returns>true:hit to the breakpoint/false:do not hit</returns>
bool CDlgSticktrace::APT_IsBreakpoint(const char * name, int lineIndex)
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
	if (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0)
		return false;

	{
		AutoLeaveCS acs(m_breakpointSlow.cs);
		if ((int)m_breakpointSlow.lineIndexToIsBreak.size() <= lineIndex)
			return false;
		if (m_breakpointSlow.lineIndexToIsBreak[lineIndex] == 0)
			return false;
		if (name == nullptr)
			return true;
		if (m_breakpointSlow.lineIndexAndSourceNameSet.find(std::make_pair(lineIndex, name)) != m_breakpointSlow.lineIndexAndSourceNameSet.end())
			return true;
		if (m_breakpointSlow.m_runToCursorSourceName == name && m_breakpointSlow.m_runToCursorLineIndex == lineIndex)
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
//bool CDlgSticktrace::APT_IsSuspended()
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
//int CDlgSticktrace::APT_GetMode()
//{
//	return (int)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
//}
//----- 17.10.20 Fukushiro M. 削除終 ()-----

/// <summary>
/// Tcs the on suspended.
/// This function works at the main application's thread.
/// </summary>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_OnSuspended()
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
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_OnResumed()
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
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_Jump(const char * name, int lineIndex)
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
/// This function notify the beginning of new session.
/// This function works at the main application's thread.
/// </summary>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_NewSession()
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::NEW_SESSION;
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
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_OnStart(SticktraceDef::ExecType execType)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::ON_START;
	m_incmd.i64Param1 = (__int64)execType;
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
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_OnStop(SticktraceDef::ExecType execType)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::ON_STOP;
	m_incmd.i64Param1 = (__int64)execType;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

//----- 21.05.18 Fukushiro M. 削除始 ()-----
////----- 21.05.18 Fukushiro M. 追加始 ()-----
///// <summary>
///// Tcs the on error stop.
///// This function works at the main application's thread.
///// </summary>
///// <param name="message">The message.</param>
///// <returns>true:accepted/false:timeout</returns>
//bool CDlgSticktrace::APT_OnErrorStop(const char * message)
//{
//	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
//	m_incmd.command = InCmd::Command::ON_ERROR_STOP;
//	m_incmd.strParam1 = (message == nullptr) ? "" : message;
//	PostMessage(WM_USER_COMMAND);
//	// Wait for the command to be accepted by the Sticktrace Window.
//	if (!acs.SleepConditionVariable(10000))
//		return false;
//	return true;
//}
////----- 21.05.18 Fukushiro M. 追加終 ()-----
//----- 21.05.18 Fukushiro M. 削除終 ()-----

/// <summary>
/// Tcs the output error.
/// This function works at the main application's thread.
/// </summary>
/// <param name="message">The message.</param>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_OutputError(const char * message)
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
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_OutputDebug(const char * message)
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
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_SetWatch(const char * data)
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
/// Notification of variable setting.
/// </summary>
/// <param name="succeeded">true:variable was set/false:not set</param>
/// <returns>true:accepted/false:timeout</returns>
bool CDlgSticktrace::APT_SetVariableNotify(bool succeeded)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::SET_VARIABLE_NOTIFY;
	m_incmd.i64Param1 = succeeded ? 1 : 0;
	PostMessage(WM_USER_COMMAND);
	// Wait for the command to be accepted by the Sticktrace Window.
	if (!acs.SleepConditionVariable(10000))
		return false;
	return true;
}

/// <summary>
/// Wait any command is set by debugger window.
/// This function works at the main application's thread.
/// </summary>
/// <param name="paramA">The parameter.</param>
/// <param name="waitMilliseconds">The wait milliseconds.</param>
/// <returns>Command</returns>
SticktraceDef::SuspendCommand CDlgSticktrace::APT_WaitCommandIsSet(StickString & paramA, uint32_t waitMilliseconds)
{
	AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
	if (m_outcmd.command == SticktraceDef::SuspendCommand::NONE)
	{
		// Wait for being set the command by the Sticktrace Window.
		if (!acs.SleepConditionVariable(waitMilliseconds))
			return SticktraceDef::SuspendCommand::NONE;
	}
	paramA = m_outcmd.strParamA.c_str();
	auto command = m_outcmd.command;
	m_outcmd.command = SticktraceDef::SuspendCommand::NONE;
	return command;
}

void CDlgSticktrace::GetWatchedVariables(
	std::vector<std::string>& vTopLevelName,
	std::unordered_set<std::string>& stExpandedName
) const
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
	std::function<int(LVITEM&, CListCtrl*, int, int, const char*, std::vector<std::string>&, std::unordered_set<std::string>&)>
		GetLevelItem = [&GetLevelItem](
			LVITEM& lvi,
			CListCtrl* pLsvWatch,
			int index,
			int iIndent,
			const char* wcpPiledVarName,
			std::vector<std::string>& vTopLevelName,
			std::unordered_set<std::string>& stExpandedName
		)
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
			{	//----- アイコンがテーブル「COLLAPSED（閉じている）」を表す場合 -----
				// 「COLLAPSED（閉じている）」「EXPANDED（開いている）」は関係なく調べる。通常は開いている場合のみ下位が表示されるが、
				// マウスクリックにより閉じたときにもここが処理されるため。
				index = GetLevelItem(lvi, pLsvWatch, index + 1, iIndent + 1, wstrNewPiledVarName.c_str(), vTopLevelName, stExpandedName);
			}
			else if (lvi.iImage == 1)
			{	//----- アイコンがテーブル「EXPANDED（開いている）」を表す場合 -----
				// 「COLLAPSED（閉じている）」「EXPANDED（開いている）」は関係なく調べる。通常は開いている場合のみ下位が表示されるが、
				// マウスクリックにより閉じたときにもここが処理されるため。
				// 「EXPANDED」はstExpandedNameに記録しておく。後で変数を表示する際に、同じ展開状態を復元するため。
				// stExpandedName にはメンバー変数までを辿る名称（連結変数名）を挿入。
				// 但し、先頭にはvTopLevelNameのindexを挿入しておく。同じ変数名が２回以上使われた場合に区別するため。
				// 例
				// A
				// B
				// D
				// E
				// |-1
				// |-2
				// | |-"abc"  <---「3\bE\b2\b"abc"」
				//
				stExpandedName.insert(std::to_string(vTopLevelName.size() - 1) + "\b" + wstrNewPiledVarName);
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

	GetLevelItem(lvi, (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH), 0, 0, nullptr, vTopLevelName, stExpandedName);
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
	Stickutil::Serialize(data, m_sandbox);
	Stickutil::Serialize(data, vTopLevelName);
	Stickutil::Serialize(data, stExpandedName);

	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::GET_VARIABLE;
		m_outcmd.strParamA = data;
		acs.WakeConditionVariable();
	}

	// // ウォッチウィンドウ。
	// CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// ウォッチウィンドウを一度クリアする。
	// pLsvWatch->DeleteAllItems();

	// std::function<int(CListCtrl*, int, int, int, LVITEM&, const char*, const char*, const TrAnyValue&, const std::unordered_set<std::string>&)> f;


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
		IDC_SCE_BTN_ADD_WATCH,
		IDC_SCE_BTN_DELETE_WATCH,
		IDC_SCE_BTN_SET_VARIABLE,
		IDC_SCE_STC_LINE_NUMBER			// 行番号
	);

	// ウィンドウの右下と左下に追従して移動。
	InitLayout(FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_BOTTOM,
		IDC_SCE_STC_MESSAGE
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
		IDC_SCE_STC_MESSAGE,			// メッセージ
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
		case IDC_SCE_STC_MESSAGE:				// メッセージ。
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

	m_textEditor.ClearMarker(CFCTextEdit::MarkerType::MARKER_NOTIFY);

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
/// This function notify the beginning of new session.
/// </summary>
/// <returns></returns>
void CDlgSticktrace::NewSession()
{
	m_textEditor.SetWindowText(L"");
	m_textEditor.SetContentName("");
	m_textEditor.ClearAllMarker();
	m_sandbox.clear();
	// Clear the error message window.
	OnSceWinClearError();
	OnSceWinClearOutput();
	m_isScriptEditable = FALSE;
	OnSceDebugClearBreakpoint();
	m_sourceNameArray.clear();
	m_nameToSandSource.clear();
}

/// <summary>
/// Called when script starts.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
void CDlgSticktrace::OnStart(SticktraceDef::ExecType execType)
{
	::InterlockedExchange(&m_breakpointFast.suspendMode, (LONG)Mode::RUN);

	m_textEditor.ClearMarker(CFCTextEdit::MarkerType::MARKER_NOTIFY);

	// If this function is called via function calling, the editor should not recover the editable mode at OnStop function.
	// Because the function calling might be done frequently.
	if (execType == SticktraceDef::ExecType::ON_CALL)
		m_isScriptEditable = FALSE;
	if (!m_textEditor.IsReadOnly())
	{
		// You cannot edit script while script running.
		m_textEditor.SetReadOnly(TRUE);
		// Update the display status of script control buttons.
		PostMessage(WM_IDLEUPDATECMDUI);
	}
}

/// <summary>
/// Called when script stop.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
void CDlgSticktrace::OnStop(SticktraceDef::ExecType execType)
{
	::InterlockedExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOP);
	if (m_isScriptEditable != !m_textEditor.IsReadOnly())
	{
		m_textEditor.SetReadOnly(!m_isScriptEditable);
		// Update the display status of script control buttons.
		PostMessage(WM_IDLEUPDATECMDUI);
	}
	// Erase the trace line marker on the text editor.
	m_textEditor.ClearMarker(CFCTextEdit::MARKER_TRACELINE);
	if (m_textEditor.IsThereMarkerRegion())
		m_textEditor.RedrawMarker();
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

	{
		AutoLeaveCS acs(m_breakpointSlow.cs);
		if (m_breakpointSlow.m_runToCursorLineIndex != -1)
		{
			m_breakpointSlow.lineIndexToIsBreak[m_breakpointSlow.m_runToCursorLineIndex]--;
			m_breakpointSlow.m_runToCursorSourceName.clear();
			m_breakpointSlow.m_runToCursorLineIndex = -1;
			::InterlockedExchange(&m_breakpointFast.breakpointCount, (LONG)(m_breakpointSlow.lineIndexAndSourceNameSet.size()));
		}
	}

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

//----- 21.05.18 Fukushiro M. 削除始 ()-----
////----- 21.05.18 Fukushiro M. 追加始 ()-----
///// <summary>
///// Notify the script runtime error to the main application and output the error message to the error message window.
///// This function clears the last message and set the new message.
///// Usually this function is called from the main application asynchronously.
///// This function works at the Sticktrace Window's thread.
///// </summary>
///// <param name="message">The message.</param>
//void CDlgSticktrace::OnErrorStop(const std::string & message)
//{
//	if (m_DGT_debuggerCallbackFunc != nullptr)
//	{
//		SticktraceDef::DebuggerCallbackParam param;
//		param.strParam1 = message.c_str();
//		m_DGT_debuggerCallbackFunc(m_dialogId, SticktraceDef::DebuggerCommand::ON_ERROR_STOP, &param, m_debuggerCallbackData);
//	}
//	std::wstring wmessage;
//	Astrwstr::astr_to_wstr(wmessage, message);
//	if (wmessage.back() != L'\n')
//		wmessage += L"\r\n";
//	auto len = m_errorout.GetWindowTextLength();
//	m_errorout.SetSel(len, len);
//	m_errorout.ReplaceSel(wmessage.c_str());
//}
////----- 21.05.18 Fukushiro M. 追加終 ()-----
//----- 21.05.18 Fukushiro M. 削除終 ()-----

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
// 20.07.16 Fukushiro M. 1行変更 ()
//	m_errorout.SetWindowText(wmessage.c_str());
	if (wmessage.back() != L'\n')
		wmessage += L"\r\n";
	auto len = m_errorout.GetWindowTextLength();
	m_errorout.SetSel(len, len);
	m_errorout.ReplaceSel(wmessage.c_str());

//----- 21.05.18 Fukushiro M. 追加始 ()-----
	if (m_DGT_debuggerCallbackFunc != nullptr)
	{
//----- 21.05.19 Fukushiro M. 変更前 ()-----
//		SticktraceDef::DebuggerCallbackParam param;
//		param.command = SticktraceDef::DebuggerCommand::ON_ERROR_OUTPUT;
//		param.strParam1 = message.c_str();
//		m_DGT_debuggerCallbackFunc(m_dialogId, &param, m_debuggerCallbackData);
//----- 21.05.19 Fukushiro M. 変更後 ()-----
		std::thread{ ThreadForDebbugerCallback, m_DGT_debuggerCallbackFunc, m_debuggerCallbackData, m_dialogId, SticktraceDef::DebuggerCommand::ON_ERROR_OUTPUT, message, std::string() }.detach();
//----- 21.05.19 Fukushiro M. 変更終 ()-----

	}
//----- 21.05.18 Fukushiro M. 追加終 ()-----
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

//----- 21.05.18 Fukushiro M. 追加始 ()-----
	if (m_DGT_debuggerCallbackFunc != nullptr)
	{
//----- 21.05.19 Fukushiro M. 変更前 ()-----
//		SticktraceDef::DebuggerCallbackParam param;
//		param.command = SticktraceDef::DebuggerCommand::ON_DEBUG_OUTPUT;
//		param.strParam1 = message.c_str();
//		m_DGT_debuggerCallbackFunc(m_dialogId, &param, m_debuggerCallbackData);
//----- 21.05.19 Fukushiro M. 変更後 ()-----
		std::thread{ ThreadForDebbugerCallback, m_DGT_debuggerCallbackFunc, m_debuggerCallbackData, m_dialogId, SticktraceDef::DebuggerCommand::ON_DEBUG_OUTPUT, message, std::string() }.detach();
//----- 21.05.19 Fukushiro M. 変更終 ()-----
	}
//----- 21.05.18 Fukushiro M. 追加終 ()-----
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
	std::vector<SticktraceDef::WatchInfo> varIconIndentNameTypeValueArray;
	Stickutil::Unserialize(varIconIndentNameTypeValueArray, receiveddata);
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
		int iImage = 4;	// アイコンイメージのインデックス。
		if (elem.icon == "CLOSED")
			iImage = 0;
		else if (elem.icon == "OPENED")
			iImage = 1;
		else if (elem.icon == "VARIABLE")
			iImage = 2;
		else if (elem.icon == "MEMBER")
			iImage = 3;
		else if (elem.icon == "UNKNOWN")
			iImage = 4;
		else if (elem.icon == "WARN")
			iImage = 5;
		const auto iIndent = std::strtol(elem.indent.c_str(), nullptr, 10);
		std::wstring name;
		std::wstring type;
		std::wstring value;
		Astrwstr::astr_to_wstr(name, elem.name);
		Astrwstr::astr_to_wstr(type, elem.type);
		Astrwstr::astr_to_wstr(value, elem.value);

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

/// <summary>
/// Notification of variable setting.
/// </summary>
/// <param name="succeeded">true:variable was set/false:not set</param>
void CDlgSticktrace::SetVariableNotify(bool succeeded)
{
	UpdateWatchWindow();
}

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
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_SCE_EDT_ERROR)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->EnableWindow(FALSE);
		break;
	case 1:	// 変数。
		GetDlgItem(IDC_SCE_EDT_ERROR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_SCE_EDT_ERROR)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->EnableWindow(TRUE);
		break;
	case 2:	// 出力。
		GetDlgItem(IDC_SCE_EDT_ERROR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SCE_LSV_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_SCE_EDT_ERROR)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_OUTPUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCE_LSV_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_EDT_VARIABLE_VALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->EnableWindow(FALSE);
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
		GetDlgItem(IDC_SCE_EDT_SCRIPT)->GetWindowRect(rtScript);		// スクリプトエディター
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->GetWindowRect(rtChange);		// 追加ボタン
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
/// Called when the script tab control selection will be changed.
/// </summary>
/// <param name="pNMHDR">The p NMHDR.</param>
/// <param name="pResult">The p result.</param>
void CDlgSticktrace::OnTcnSelchangingSceTabScript(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_textEditor.IsModified())
	{
		MessageBox(L"Script was changed. Save it before changing the selection of the script tab.", nullptr, MB_OK);
		// Inhibit to change the tab selection.
		*pResult = 1;
	}
	else
	{
		// Allow to change the tab selection.
		*pResult = 0;
	}
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

	std::wstring wtext;
	UtilWin::GetWindowText(&m_textEditor, wtext);
	if (
		m_sandbox != sand_src.first ||
		m_textEditor.GetContentName() != scriptName ||
		wtext != sand_src.second
		)
	{
		// Do not reset the source code to the editor, if the source code is not changed.
		// Because this function is called when the source code is saved, not only when the tab selection changing.
		m_sandbox = sand_src.first;
		m_textEditor.SetContentName(scriptName);
		m_textEditor.SetWindowText(sand_src.second.c_str());
	}
	else
	{
		m_textEditor.ResetModified();
	}

	if (pResult != nullptr) *pResult = 0;
}

LRESULT CDlgSticktrace::OnUserCommand(WPARAM, LPARAM)
{
	CDlgSticktrace::InCmd::Command command;
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

		// Some commands that require the return value must be described before acs.WakeConditionVariable() is called.
		switch (command)
		{
		case CDlgSticktrace::InCmd::Command::IS_VISIBLE:
			m_incmd.i64Param1 = IsWindowVisible() ? 1 : 0;
			break;
		case CDlgSticktrace::InCmd::Command::IS_SCRIPT_MODIFIED:
			m_incmd.i64Param1 = m_textEditor.IsModified() ? 1 : 0;
			break;
		}
		acs.WakeConditionVariable();
	}

	switch (command)
	{
	case CDlgSticktrace::InCmd::Command::NONE:
		break;
	case CDlgSticktrace::InCmd::Command::SHOW:
		ShowWindow((i64Param1 != 0) ? SW_SHOW : SW_HIDE);
		EnableWindow((i64Param1 != 0));
		if (i64Param1 != 0)
			::SetForegroundWindow(m_hWnd);
		break;
	case CDlgSticktrace::InCmd::Command::SET_SOURCE:
	{
		std::wstring wsrc;
		Astrwstr::astr_to_wstr(wsrc, strParam3);
		SetSource(strParam1, strParam2, wsrc);
		break;
	}
	case CDlgSticktrace::InCmd::Command::NEW_SESSION:
		NewSession();
		break;
	case CDlgSticktrace::InCmd::Command::ON_START:
		OnStart((SticktraceDef::ExecType)i64Param1);
		break;
	case CDlgSticktrace::InCmd::Command::ON_STOP:
		OnStop((SticktraceDef::ExecType)i64Param1);
		break;
	case CDlgSticktrace::InCmd::Command::ON_SUSPENDED:
		OnSuspended();
		break;
	case CDlgSticktrace::InCmd::Command::ON_RESUMED:
		OnResumed();
		break;
//----- 21.05.18 Fukushiro M. 削除始 ()-----
////----- 21.05.18 Fukushiro M. 追加始 ()-----
//	case CDlgSticktrace::InCmd::Command::ON_ERROR_STOP:
//		OnErrorStop(strParam1);
//		break;
////----- 21.05.18 Fukushiro M. 追加終 ()-----
//----- 21.05.18 Fukushiro M. 削除終 ()-----
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
	case CDlgSticktrace::InCmd::Command::SET_VARIABLE_NOTIFY:
		SetVariableNotify(i64Param1 != 0);
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
		m_breakpointSlow.lineIndexAndSourceNameSet.clear();
		for (const auto & bp : breakpoint)
		{
			if ((int)m_breakpointSlow.lineIndexToIsBreak.size() <= bp.first.second)
				m_breakpointSlow.lineIndexToIsBreak.resize(bp.first.second + 1, 0);
			m_breakpointSlow.lineIndexToIsBreak[bp.first.second]++;
			m_breakpointSlow.lineIndexAndSourceNameSet.insert(std::make_pair(bp.first.second, bp.first.first));
		}
		if (m_breakpointSlow.m_runToCursorLineIndex != -1)
		{
			m_breakpointSlow.lineIndexToIsBreak[m_breakpointSlow.m_runToCursorLineIndex]++;
			::InterlockedExchange(&m_breakpointFast.breakpointCount, (LONG)(m_breakpointSlow.lineIndexAndSourceNameSet.size() + 1));
		}
		else
		{
			::InterlockedExchange(&m_breakpointFast.breakpointCount, (LONG)m_breakpointSlow.lineIndexAndSourceNameSet.size());
		}
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
	// TRACE(L"OnBnClickedSceBtnDebugContinue\n");
	// ::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::RUN, (LONG)Mode::SUSPEND);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::RESUME;
		acs.WakeConditionVariable();
	}
//----- 20.07.16 Fukushiro M. 削除始 ()-----
//	// // Update the display status of script control buttons.
//	// PostMessage(WM_IDLEUPDATECMDUI);
//	::SetFocus(NULL);
//----- 20.07.16 Fukushiro M. 削除終 ()-----
	m_textEditor.SetFocus();
	// (CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)
}


void CDlgSticktrace::OnBnClickedSceBtnDebugBreak()
{
	// TRACE(L"OnBnClickedSceBtnDebugBreak\n");

	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::SUSPEND, (LONG)Mode::RUN);
//----- 20.07.16 Fukushiro M. 削除始 ()-----
//	// Update the display status of script control buttons.
//	PostMessage(WM_IDLEUPDATECMDUI);
//----- 20.07.16 Fukushiro M. 削除終 ()-----
	m_textEditor.SetFocus();
}


void CDlgSticktrace::OnBnClickedSceBtnDebugStop()
{
	// TRACE(L"OnBnClickedSceBtnDebugStop\n");
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOPPING, (LONG)Mode::RUN);
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOPPING, (LONG)Mode::SUSPEND);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::STOP;
		acs.WakeConditionVariable();
	}
	m_textEditor.SetFocus();
}

void CDlgSticktrace::OnBnClickedSceBtnDebugStepToNext()
{
	// TRACE(L"OnBnClickedSceBtnDebugStepToNext\n");
	//::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::PROCEED_TO_NEXT, (LONG)Mode::SUSPEND);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::PROCEED_NEXT;
		acs.WakeConditionVariable();
	}
	// // Update the display status of script control buttons.
	// PostMessage(WM_IDLEUPDATECMDUI);
	m_textEditor.SetFocus();
}

LRESULT CDlgSticktrace::OnIdleUpdateCmdUI(WPARAM, LPARAM)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const auto isDebugMode = (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) != 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	switch (mode)
	{
	case CDlgSticktrace::Mode::STOP:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除始 ()-----
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_UNCHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_UNCHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除終 ()-----

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->EnableWindow(!isScriptModified);
		break;
	case CDlgSticktrace::Mode::STOPPING:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除始 ()-----
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_UNCHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_UNCHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除終 ()-----

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->EnableWindow(FALSE);
		break;
	case CDlgSticktrace::Mode::RUN:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除始 ()-----
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_CHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_UNCHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除終 ()-----

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->EnableWindow(FALSE);
		break;
	case CDlgSticktrace::Mode::SUSPEND:
		((CButton*)GetDlgItem(IDC_SCE_CHK_DEBUG_MODE))->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除始 ()-----
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_UNCHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_CHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 削除終 ()-----

		GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(!isScriptModified);
		GetDlgItem(IDC_SCE_BTN_SET_VARIABLE)->EnableWindow(!isScriptModified);
		break;
	default:
		break;
	}

	if (isScriptModified != m_textEditor.IsModified())
	{
		::InterlockedExchange(&m_scriptModify.isScriptModified, (LONG)(m_textEditor.IsModified() ? 1 : 0));

		CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);

		TCITEM tcitem;
		memset(&tcitem, 0, sizeof(tcitem));
		tcitem.mask = TCIF_TEXT;
//----- 20.06.11  削除始 ()-----
//		wchar_t textbuff[Stickrun::SCRIPT_NAME_MAX + 5];
//		tcitem.pszText = textbuff;
//		tcitem.cchTextMax = _countof(textbuff);
//		tabCtrl->GetItem(tabCtrl->GetCurSel(), &tcitem);
//----- 20.06.11  削除終 ()-----

		std::wstring wstrName;
		Astrwstr::astr_to_wstr(wstrName, m_textEditor.GetContentName());
		if (m_textEditor.IsModified())
			wstrName += L" *";
		tcitem.pszText = (wchar_t *)wstrName.c_str();

//----- 20.06.11  削除始 ()-----
//		if (wstrName != textbuff)
//		{
//			::wcscpy_s(textbuff, wstrName.c_str());
//			tabCtrl->SetItem(tabCtrl->GetCurSel(), &tcitem);
//		}
//----- 20.06.11  削除終 ()-----
		tabCtrl->SetItem(tabCtrl->GetCurSel(), &tcitem);

		if (m_textEditor.IsModified())
		{
			m_textEditor.ActivateBreakpoint(FALSE);
			m_textEditor.RedrawMarker();
			SetDlgItemText(IDC_SCE_STC_MESSAGE, L"Breakpoints are deactivated because script is modified.");
		}
		else
		{
			m_textEditor.ActivateBreakpoint(isDebugMode);
			m_textEditor.RedrawMarker();
			SetDlgItemText(IDC_SCE_STC_MESSAGE, L"");
		}
	}
	return 1;
}

void CDlgSticktrace::OnFileSave()
{
	if (m_DGT_debuggerCallbackFunc != nullptr)
	{
		std::wstring wtext;
		UtilWin::GetWindowText(&m_textEditor, wtext);
		std::wstring wtextTrim = wtext;
		UtilString::Trim(wtextTrim);
		if (wtext != wtextTrim)
		{	//----- If there are some spaces at the front of text or at the end of text -----
			// Front new line must be erased from source code. Because Lua load function trim the input source code automatically.
			// So if source code was not trimed previously, source code inside of Lua and inside of editor did not correspond.
			wtext = wtextTrim;
			int startChar, endChar;
			m_textEditor.GetSel(startChar, endChar);
			m_textEditor.SetSel(0, -1);
			m_textEditor.ReplaceSel(wtext.c_str(), TRUE);
			m_textEditor.SetSel(startChar, endChar);
		}
		
		std::string atext;
		Astrwstr::wstr_to_astr(atext, wtext);

//----- 21.05.19 Fukushiro M. 変更前 ()-----
//		SticktraceDef::DebuggerCallbackParam param;
//		param.command = SticktraceDef::DebuggerCommand::SAVE_SCRIPT;
//		param.strParam1 = m_textEditor.GetContentName().c_str();
//		param.strParam2 = atext.c_str();
//		m_DGT_debuggerCallbackFunc(m_dialogId, &param, m_debuggerCallbackData);
//----- 21.05.19 Fukushiro M. 変更後 ()-----
		std::thread{ ThreadForDebbugerCallback, m_DGT_debuggerCallbackFunc, m_debuggerCallbackData, m_dialogId, SticktraceDef::DebuggerCommand::SAVE_SCRIPT, m_textEditor.GetContentName(), atext }.detach();
//----- 21.05.19 Fukushiro M. 変更終 ()-----
	}
}

void CDlgSticktrace::OnUpdateFileSave(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(m_textEditor.IsModified());
}

void CDlgSticktrace::OnEditScriptEdit()
{
	m_isScriptEditable = !m_isScriptEditable;
	m_textEditor.SetReadOnly(!m_isScriptEditable);
}

void CDlgSticktrace::OnUpdateEditScriptEdit(CCmdUI * pCmdUI)
{
	// m_DGT_debuggerCallbackFunc is called when debugger save the script code.
	pCmdUI->Enable(!m_sourceNameArray.empty() && m_DGT_debuggerCallbackFunc != nullptr);
	pCmdUI->SetCheck(m_isScriptEditable ? 1 : 0);
}

void CDlgSticktrace::OnSceEditInputLine()
{
	GetDlgItem(IDC_SCE_EDT_LINE_NUMBER)->SetFocus();
} // CDlgSticktrace::OnSceEditInputLine.

void CDlgSticktrace::OnUpdateEditInputLine(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateEditInputLine.

void CDlgSticktrace::OnSceEditGotoLine()
{
	std::wstring wstrLine;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_LINE_NUMBER, wstrLine);
	auto lineNumber = std::wcstoul(wstrLine.c_str(), nullptr, 10);
	auto charIndex = m_textEditor.LineIndex(lineNumber - 1);
	m_textEditor.SetSel(charIndex, charIndex);
	m_textEditor.SetFocus();
} // CDlgSticktrace::OnSceEditGotoLine.

void CDlgSticktrace::OnUpdateSceEditGotoLine(CCmdUI *pCmdUI)
{
	std::wstring wstrLine;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_LINE_NUMBER, wstrLine);
	wchar_t * wcpNext;
	auto lineNumber = std::wcstoul(wstrLine.c_str(), &wcpNext, 10);
	if (lineNumber == ULONG_MAX || lineNumber == 0 || *wcpNext != L'\0')
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceEditGotoLine.

void CDlgSticktrace::OnSceWinKeyword()
{
	GetDlgItem(IDC_SCE_EDT_KEYWORD)->SetFocus();
} // CDlgSticktrace::OnSceWinKeyword.

void CDlgSticktrace::OnUpdateSceWinKeyword(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinKeyword.

void CDlgSticktrace::OnEditFindNextText()
{
	std::wstring keyword;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_KEYWORD, keyword);
	UtilString::Trim(keyword);
	if (!m_textEditor.SearchForward(keyword))
		SetDlgItemText(IDC_SCE_STC_MESSAGE, L"The search word does not exist.");
} // CDlgSticktrace::OnEditFindNextText.

void CDlgSticktrace::OnUpdateEditFindNextText(CCmdUI *pCmdUI)
{
	std::wstring keyword;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_KEYWORD, keyword);
	UtilString::Trim(keyword);
	pCmdUI->Enable(!keyword.empty());
} // CDlgSticktrace::OnUpdateEditFindNextText.

void CDlgSticktrace::OnEditFindPrevText()
{
	std::wstring keyword;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_KEYWORD, keyword);
	UtilString::Trim(keyword);
	if (!m_textEditor.SearchBackward(keyword))
		SetDlgItemText(IDC_SCE_STC_MESSAGE, L"The search word does not exist.");
} // CDlgSticktrace::OnEditFindPrevText.

void CDlgSticktrace::OnUpdateEditFindPrevText(CCmdUI *pCmdUI)
{
	OnUpdateEditFindNextText(pCmdUI);
} // CDlgSticktrace::OnUpdateEditFindPrevText.

void CDlgSticktrace::OnSceWinClearError()
{
	m_errorout.SetWindowText(L"");
} // CDlgSticktrace::OnSceWinClearError.

void CDlgSticktrace::OnUpdateSceWinClearError(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinClearError.

void CDlgSticktrace::OnSceWinClearOutput()
{
	m_output.SetWindowText(L"");
} // CDlgSticktrace::OnSceWinClearOutput.

void CDlgSticktrace::OnUpdateSceWinClearOutput(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
} // CDlgSticktrace::OnUpdateSceWinClearOutput.

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
	AutoLeaveCS acs(m_breakpointSlow.cs);
	m_breakpointSlow.lineIndexToIsBreak.clear();
	m_breakpointSlow.lineIndexAndSourceNameSet.clear();
	m_breakpointSlow.m_runToCursorSourceName.clear();
	m_breakpointSlow.m_runToCursorLineIndex = -1;
	::InterlockedExchange(&m_breakpointFast.breakpointCount, 0);
} // CDlgSticktrace::OnSceDebugClearBreakpoint.

void CDlgSticktrace::OnUpdateSceDebugClearBreakpoint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(::InterlockedCompareExchange(&m_breakpointFast.breakpointCount, 0, 0) != 0);
} // CDlgSticktrace::OnUpdateSceDebugClearBreakpoint.

void CDlgSticktrace::OnSceDebugMode()
{
//----- 20.07.16 Fukushiro M. 変更前 ()-----
//	const auto isDebugMode = (IsDlgButtonChecked(IDC_SCE_CHK_DEBUG_MODE) == BST_CHECKED);
//	CheckDlgButton(IDC_SCE_CHK_DEBUG_MODE, isDebugMode ? BST_UNCHECKED : BST_CHECKED);
//	PostMessage(WM_COMMAND, MAKELPARAM(IDC_SCE_CHK_DEBUG_MODE, BN_CLICKED), 0);
//----- 20.07.16 Fukushiro M. 変更後 ()-----
	const auto isDebugModeNew = (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) == 0);
	::InterlockedExchange(&m_debugMode.debugMode, isDebugModeNew ? 1 : 0);
	m_textEditor.ActivateBreakpoint(isDebugModeNew);
	m_textEditor.RedrawMarker();
//----- 20.07.16 Fukushiro M. 変更終 ()-----
} // CDlgSticktrace::OnSceDebugMode.

void CDlgSticktrace::OnUpdateSceDebugMode(CCmdUI *pCmdUI)
{
//----- 20.07.16 Fukushiro M. 変更前 ()-----
//	pCmdUI->Enable(GetDlgItem(IDC_SCE_CHK_DEBUG_MODE)->IsWindowEnabled());
//	const auto isDebugMode = (IsDlgButtonChecked(IDC_SCE_CHK_DEBUG_MODE) == BST_CHECKED);
//	pCmdUI->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 変更後 ()-----
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const auto isDebugMode = (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) != 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	if (mode == CDlgSticktrace::Mode::STOP)
		pCmdUI->Enable(!isScriptModified);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(isDebugMode ? BST_CHECKED : BST_UNCHECKED);
//----- 20.07.16 Fukushiro M. 変更終 ()-----
} // CDlgSticktrace::OnUpdateSceDebugMode.

void CDlgSticktrace::OnSceDebugContinue()
{
	AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
	m_outcmd.command = SticktraceDef::SuspendCommand::RESUME;
	acs.WakeConditionVariable();
} // CDlgSticktrace::OnSceDebugContinue.

void CDlgSticktrace::OnUpdateSceDebugContinue(CCmdUI *pCmdUI)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	if (mode == CDlgSticktrace::Mode::SUSPEND)
		pCmdUI->Enable(!isScriptModified);
	else
		pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugContinue.

void CDlgSticktrace::OnSceDebugBreak()
{
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::SUSPEND, (LONG)Mode::RUN);
} // CDlgSticktrace::OnSceDebugBreak.

void CDlgSticktrace::OnUpdateSceDebugBreak(CCmdUI *pCmdUI)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	if (mode == CDlgSticktrace::Mode::RUN)
		pCmdUI->Enable(!isScriptModified);
	else
		pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugBreak.

void CDlgSticktrace::OnSceDebugStepToNext()
{
	AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
	m_outcmd.command = SticktraceDef::SuspendCommand::PROCEED_NEXT;
	acs.WakeConditionVariable();
} // CDlgSticktrace::OnSceDebugStepToNext.

void CDlgSticktrace::OnUpdateSceDebugStepToNext(CCmdUI *pCmdUI)
{
	OnUpdateSceDebugContinue(pCmdUI);
} // CDlgSticktrace::OnUpdateSceDebugStepToNext.

void CDlgSticktrace::OnSceDebugRunToCursor()
{
	auto sourceName = m_textEditor.GetContentName();
	auto lineIndex = m_textEditor.GetCurLineIndex();
	{
		AutoLeaveCS acs(m_breakpointSlow.cs);
		if (m_breakpointSlow.m_runToCursorLineIndex == -1)
		{
			m_breakpointSlow.m_runToCursorSourceName = sourceName;
			m_breakpointSlow.m_runToCursorLineIndex = lineIndex;
			if ((int)m_breakpointSlow.lineIndexToIsBreak.size() <= m_breakpointSlow.m_runToCursorLineIndex)
				m_breakpointSlow.lineIndexToIsBreak.resize(m_breakpointSlow.m_runToCursorLineIndex + 1, 0);
			m_breakpointSlow.lineIndexToIsBreak[m_breakpointSlow.m_runToCursorLineIndex]++;
			::InterlockedExchange(&m_breakpointFast.breakpointCount, (LONG)(m_breakpointSlow.lineIndexAndSourceNameSet.size() + 1));
		}
	}
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::RESUME;
		acs.WakeConditionVariable();
	}
} // CDlgSticktrace::OnSceDebugRunToCursor.

void CDlgSticktrace::OnUpdateSceDebugRunToCursor(CCmdUI *pCmdUI)
{
	OnUpdateSceDebugContinue(pCmdUI);
} // CDlgSticktrace::OnUpdateSceDebugRunToCursor.

void CDlgSticktrace::OnSceDebugStop()
{
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOPPING, (LONG)Mode::RUN);
	::InterlockedCompareExchange(&m_breakpointFast.suspendMode, (LONG)Mode::STOPPING, (LONG)Mode::SUSPEND);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::STOP;
		acs.WakeConditionVariable();
	}
} // CDlgSticktrace::OnSceDebugStop.

void CDlgSticktrace::OnUpdateSceDebugStop(CCmdUI *pCmdUI)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	if (mode == CDlgSticktrace::Mode::RUN || mode == CDlgSticktrace::Mode::SUSPEND)
		pCmdUI->Enable(!isScriptModified);
	else
		pCmdUI->Enable(FALSE);
} // CDlgSticktrace::OnUpdateSceDebugStop.

void CDlgSticktrace::OnSceDebugSetVariable ()
{
	// Get the variable name.
	std::string varName;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_NAME, varName);
	UtilString::Trim(varName);
	// Get the value of variable.
	std::string varValue;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_VALUE, varValue);
	UtilString::Trim(varValue);

	if (varName.empty())
	{
		MessageBox(L"Variable name is not filled.", nullptr, MB_OK);
		return;
	}
	if (varValue.empty())
	{
		MessageBox(L"Variable value is not filled.", nullptr, MB_OK);
		return;
	}

	__int32 varType;
	std::string newValue;
	if (_stricmp(varValue.c_str(), "nil") == 0)
	{
		varType = LUA_TNIL;
		newValue = "";
	}
	else if (_stricmp(varValue.c_str(), "true") == 0)
	{
		varType = LUA_TBOOLEAN;
		newValue = "true";
	}
	else if (_stricmp(varValue.c_str(), "false") == 0)
	{
		varType = LUA_TBOOLEAN;
		newValue = "false";
	}
	else if (1 < varValue.length() && varValue.front() == '\"' && varValue.back() == '\"')
	{
		varType = LUA_TSTRING;
		newValue = varValue.substr(1, varValue.length() - 2);
	}
	else
	{
		// Assume it is a numeric.
		char* endPtr;
		(void)::strtod(varValue.c_str(), &endPtr);
		if (endPtr[0] != '\0')
		{
			MessageBox(L"Form of the variable value is wrong.", nullptr, MB_OK);
			return;
		}
		varType = LUA_TNUMBER;
		newValue = varValue;
	}
	std::string data;
	Stickutil::Serialize(data, m_sandbox);
	Stickutil::Serialize(data, varName);
	Stickutil::Serialize(data, varType);
	Stickutil::Serialize(data, newValue);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::SET_VARIABLE;
		m_outcmd.strParamA = data;
		acs.WakeConditionVariable();
	}
} // CDlgSticktrace::OnSceDebugSetVariable.

void CDlgSticktrace::OnUpdateSceDebugSetVariable(CCmdUI *pCmdUI)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	if (mode == CDlgSticktrace::Mode::SUSPEND)
	{
		// Get the variable name.
		std::string varName;
		UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_NAME, varName);
		UtilString::Trim(varName);
		// Get the value of variable.
		std::string varValue;
		UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_VALUE, varValue);
		UtilString::Trim(varValue);
		pCmdUI->Enable(!isScriptModified && !varName.empty() && !varValue.empty());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // CDlgSticktrace::OnUpdateSceDebugSetVariable.

void CDlgSticktrace::OnSceDebugAddWatch()
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
		lvi.iImage = 4;
		pLsvWatch->InsertItem(&lvi);

		// ウォッチウィンドウを更新。
		UpdateWatchWindow();
	}
} // CDlgSticktrace::OnSceDebugAddWatch.

void CDlgSticktrace::OnUpdateSceDebugAddWatch(CCmdUI *pCmdUI)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	if (mode == CDlgSticktrace::Mode::STOP || mode == CDlgSticktrace::Mode::SUSPEND)
	{
		// Get the variable name.
		std::string varName;
		UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_NAME, varName);
		UtilString::Trim(varName);
		pCmdUI->Enable(!isScriptModified && !varName.empty());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
} // CDlgSticktrace::OnUpdateSceDebugAddWatch.

void CDlgSticktrace::OnSceDebugDeleteWatch ()
{
	// e.g.
	//
	// X      indent=0
	// A      indent=0
	// |-B    indent=1
	// |-C    indent=1  <-- curSel
	// |-D    indent=1
	// Z      indent=0
	//
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	auto curSel = UtilDlg::GetCurSel(pLsvWatch);
	if (curSel != -1)
	{
		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_INDENT;

		// Move curSel upward until indent=0
		//
		// X      indent=0
		// A      indent=0  <-- curSel
		// |-B    indent=1
		// |-C    indent=1
		// |-D    indent=1
		// Z      indent=0
		//
		for (;;)
		{
			item.iItem = curSel;
			pLsvWatch->GetItem(&item);
			if (item.iIndent == 0) break;
			curSel--;
		}

		// Delete curSel item until indent=0
		//
		// X      indent=0
		// Z      indent=0  <-- curSel
		//
		for (;;)
		{
			pLsvWatch->DeleteItem(curSel);
			if (curSel == pLsvWatch->GetItemCount()) break;
			item.iItem = curSel;
			pLsvWatch->GetItem(&item);
			if (item.iIndent == 0) break;
		}
	}
} // CDlgSticktrace::OnSceDebugDeleteWatch.

void CDlgSticktrace::OnUpdateSceDebugDeleteWatch(CCmdUI *pCmdUI)
{
	const auto mode = (CDlgSticktrace::Mode)::InterlockedCompareExchange(&m_breakpointFast.suspendMode, 0, 0);
	const BOOL isScriptModified = (::InterlockedCompareExchange(&m_scriptModify.isScriptModified, 0, 0) != 0);
	if (mode == CDlgSticktrace::Mode::STOP || mode == CDlgSticktrace::Mode::SUSPEND)
	{
		CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
		auto curSel = UtilDlg::GetCurSel(pLsvWatch);
		pCmdUI->Enable(!isScriptModified && curSel != -1);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
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
	((CEdit *)GetDlgItem(IDC_SCE_EDT_VARIABLE_NAME))->SetSel(0, -1);
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

void CDlgSticktrace::OnOptionAutoAltkey()
{
	FCRegBase::SetIsAutoAlt(!FCRegBase::GetIsAutoAlt());
}

void CDlgSticktrace::OnUpdateOptionAutoAltkey(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(FCRegBase::GetIsAutoAlt());
}

/// <summary>
/// Called when [the change variable button clicked].
/// </summary>
void CDlgSticktrace::OnBnClickedSceBtnSetVariable()
{
	// Get the variable name.
	std::string varName;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_NAME, varName);
	UtilString::Trim(varName);
	// Get the value of variable.
	std::string varValue;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_VALUE, varValue);
	UtilString::Trim(varValue);

	if (varName.empty())
	{
		MessageBox(L"Variable name is not filled.", nullptr, MB_OK);
		return;
	}
	if (varValue.empty())
	{
		MessageBox(L"Variable value is not filled.", nullptr, MB_OK);
		return;
	}

	__int32 varType;
	std::string newValue;
	if (_stricmp(varValue.c_str(), "nil") == 0)
	{
		varType = LUA_TNIL;
		newValue = "";
	}
	else if (_stricmp(varValue.c_str(), "true") == 0)
	{
		varType = LUA_TBOOLEAN;
		newValue = "true";
	}
	else if (_stricmp(varValue.c_str(), "false") == 0)
	{
		varType = LUA_TBOOLEAN;
		newValue = "false";
	}
	else if (1 < varValue.length() && varValue.front() == '\"' && varValue.back() == '\"')
	{
		varType = LUA_TSTRING;
		newValue = varValue.substr(1, varValue.length() - 2);
	}
	else
	{
		// Assume it is a numeric.
		char* endPtr;
		(void)::strtod(varValue.c_str(), &endPtr);
		if (endPtr[0] != '\0')
		{
			MessageBox(L"Form of the variable value is wrong.", nullptr, MB_OK);
			return;
		}
		varType = LUA_TNUMBER;
		newValue = varValue;
	}
	std::string data;
	Stickutil::Serialize(data, m_sandbox);
	Stickutil::Serialize(data, varName);
	Stickutil::Serialize(data, varType);
	Stickutil::Serialize(data, newValue);
	{
		AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
		m_outcmd.command = SticktraceDef::SuspendCommand::SET_VARIABLE;
		m_outcmd.strParamA = data;
		acs.WakeConditionVariable();
	}
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
		lvi.iImage = 4;
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
	// e.g.
	//
	// X      indent=0
	// A      indent=0
	// |-B    indent=1
	// |-C    indent=1  <-- curSel
	// |-D    indent=1
	// Z      indent=0
	//
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	auto curSel = UtilDlg::GetCurSel(pLsvWatch);
	if (curSel != -1)
	{
		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_INDENT;

		// Move curSel upward until indent=0
		//
		// X      indent=0
		// A      indent=0  <-- curSel
		// |-B    indent=1
		// |-C    indent=1
		// |-D    indent=1
		// Z      indent=0
		//
		for (;;)
		{
			item.iItem = curSel;
			pLsvWatch->GetItem(&item);
			if (item.iIndent == 0) break;
			curSel--;
		}

		// Delete curSel item until indent=0
		//
		// X      indent=0
		// Z      indent=0  <-- curSel
		//
		for (;;)
		{
			pLsvWatch->DeleteItem(curSel);
			if (curSel == pLsvWatch->GetItemCount()) break;
			item.iItem = curSel;
			pLsvWatch->GetItem(&item);
			if (item.iIndent == 0) break;
		}
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

void CDlgSticktrace::OnBnClickedSceBtnFindNext()
{
	PostMessage(WM_COMMAND, ID_EDIT_FIND_NEXT_TEXT);
}

//----- 21.05.07 Fukushiro M. 追加始 ()-----
void CDlgSticktrace::OnBnClickedSceBtnGotoLine()
{
	PostMessage(WM_COMMAND, ID_SCE_EDIT_GOTO_LINE);
}
//----- 21.05.07 Fukushiro M. 追加終 ()-----

void CDlgSticktrace::OnChangeSceEdtKeyword()
{
	// FIND_NEXT_TEXT command start the search from the end of selection.
	// So change the end of selection to the first character of the selection.
	int startChar, endChar;
	m_textEditor.GetSel(startChar, endChar);
	m_textEditor.SetSel(startChar, startChar);
	PostMessage(WM_COMMAND, ID_EDIT_FIND_NEXT_TEXT);
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
	BASE_CLASS::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
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
	}
	// TODO: ここにメッセージ ハンドラー コードを追加します。
}

/// <summary>
/// Called when any item of watch-window has been changed. Catch the selection of watch-window too.
/// </summary>
/// <param name="pNMHDR"></param>
/// <param name="pResult"></param>
void CDlgSticktrace::OnLvnItemchangedSceLsvWatch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged & LVIF_STATE && pNMLV->uNewState & LVIS_SELECTED)
	{	//----- If selection was changed -----
		CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
		// e.g.
		//
		// X       indent=0
		// A       indent=0
		// |-"B"   indent=1
		// |-"C"   indent=1
		// |  |-1  indent=2
		// |  |-2  indent=2  <-- curSel
		// |  |-3  indent=2
		// Z       indent=0
		//
		// text="2", indent=2
		//
		// The text of the selection is '2', but variable name must be 'A["C"][2]'.
		// The steps below makes a variable name "A["C"][2]"
		//
		auto curSel = pNMLV->iItem;
		std::wstring varName;
		std::wstring varValue;
		if (curSel != -1)
		{
			LVITEM item;
			wchar_t buff[1000];
			memset(&item, 0, sizeof(item));
			// Get indent and text.
			item.mask = LVIF_TEXT|LVIF_INDENT;
			item.pszText = buff;
			item.cchTextMax = _countof(buff);
			int indent = INT_MIN;
			std::vector<std::wstring> memberStack;
			// Move curSel upward until indent=0
			//
			// X       indent=0
			// A       indent=0  <-- curSel
			// |-"B"   indent=1        A
			// |-"C"   indent=1        |
			// |  |-1  indent=2        |
			// |  |-2  indent=2        +
			// |  |-3  indent=2
			// Z       indent=0
			//
			// memberStack={"2",""C"","A"}
			//
			for (;;)
			{
				item.iItem = curSel;
				pLsvWatch->GetItem(&item);
				if (indent != item.iIndent)
					memberStack.emplace_back(item.pszText);
				indent = item.iIndent;
				if (item.iIndent == 0) break;
				curSel--;
			}
			varName = memberStack.back();
			memberStack.pop_back();
			while (!memberStack.empty())
			{
				varName += L'[';
				varName += memberStack.back();
				memberStack.pop_back();
				varName += L']';
			}
			varValue = (const wchar_t*)pLsvWatch->GetItemText(pNMLV->iItem, 1);
		}
		SetDlgItemText(IDC_SCE_EDT_VARIABLE_NAME, varName.c_str());
		SetDlgItemText(IDC_SCE_EDT_VARIABLE_VALUE, varValue.c_str());
	}

	*pResult = 0;
}

BOOL CDlgSticktrace::PreTranslateMessage(MSG* pMsg)
{
	// TRACE(L"CDlgSticktrace::PreTranslateMessage %x %x %x\n", pMsg->message, pMsg->wParam, pMsg->lParam);
	// キーダウン && 自動ALT && not CTRL-KEY not SHIFT-KEY && not MENU-KEY && NotCapture の場合。
	if (
		pMsg->message == WM_KEYDOWN &&
		FCRegBase::GetIsAutoAlt() &&
		0 <= GetKeyState(VK_CONTROL) &&
		0 <= GetKeyState(VK_SHIFT) &&
		0 <= GetKeyState(VK_MENU)
		)
	{
		bool isAlt = true;
		switch (::GetDlgCtrlID(pMsg->hwnd))
		{
			case IDC_SCE_EDT_KEYWORD:
			case IDC_SCE_EDT_LINE_NUMBER:
			case IDC_SCE_EDT_VARIABLE_NAME:
			case IDC_SCE_EDT_VARIABLE_VALUE:
				isAlt = false;
				break;
			case IDC_SCE_EDT_SCRIPT:
				if (m_isScriptEditable)
					isAlt = false;
				break;
		}
		if (isAlt)
		{
			// 仮想キーを実際のキャラクタに変換。
			UINT dwAft = ::MapVirtualKey(pMsg->wParam, 2);
			// 通常のキー（スペースキーから~の次）の場合。
			if (0x20 <= dwAft && dwAft <= 0x7f)
			{
				// ALTキーを押しながらのキーとしてメッセージを投げる。
				SendMessage(WM_SYSCOMMAND, SC_KEYMENU, dwAft);
				return TRUE;
			}
		}
	}

	// コントロールにCtrl+C,Ctrl+V,Ctrl+X,Ctrl+Zを配信する。
	if (m_accelerator != NULL && ::TranslateAccelerator(m_hWnd, m_accelerator, pMsg))
		return TRUE;
	else
		return BASE_CLASS::PreTranslateMessage(pMsg);
}

void CDlgSticktrace::OnDestroy()
{
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	pLsvWatch->SetImageList(nullptr, LVSIL_SMALL);


	BASE_CLASS::OnDestroy();

	::DestroyAcceleratorTable(m_accelerator);
	m_accelerator = NULL;
}

HBRUSH CDlgSticktrace::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = BASE_CLASS::OnCtlColor(pDC, pWnd, nCtlColor);

	// Change the color of the message text to red.
	int ctrlId;
	auto hwndParent = ::GetParent(pWnd->m_hWnd);
	if (hwndParent == m_hWnd)
		ctrlId = ::GetDlgCtrlID(pWnd->m_hWnd);
	else
		ctrlId = ::GetDlgCtrlID(hwndParent);
	if (ctrlId == IDC_SCE_STC_MESSAGE)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	// TODO: 既定値を使用したくない場合は別のブラシを返します。
	return hbr;
}

/// <summary>
/// Called when taskbar will be activated.
/// </summary>
/// <param name="bActive"></param>
/// <returns></returns>
BOOL CDlgSticktrace::OnNcActivate(BOOL bActive)
{
	// TRACE(L"CDlgSticktrace::OnNcActivate\n");

	// Usually the dialog is focused when the taskbar is activated.
	// If the dialog has the focus, keyboard shortcut does not work. So, move the focus to the script editor.
	if (::GetFocus() == m_hWnd)
		GetDlgItem(IDC_SCE_EDT_SCRIPT)->SetFocus();

	return BASE_CLASS::OnNcActivate(bActive);
}

//BOOL CDlgSticktrace::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//
//	return CFCDlgModelessBase::OnWndMsg(message, wParam, lParam, pResult);
//}
