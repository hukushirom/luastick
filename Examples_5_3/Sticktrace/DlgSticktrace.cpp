// DlgSticktrace.cpp : �����t�@�C���B
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

static constexpr int FD_PANEBAR_WIDTH = 6;	// �y�C�������̐����B
static constexpr COLORREF FD_COLREF_DARK_GRAY = RGB(0xa9, 0xa9, 0xa9);
static constexpr COLORREF FD_PANEBAR_COLOR = FD_COLREF_DARK_GRAY;

static constexpr UINT BORDER_TIMER = 5;

// CDlgSticktrace �_�C�A���O�B

IMPLEMENT_DYNAMIC(CDlgSticktrace, CFCDlgModelessBase)

CDlgSticktrace::CDlgSticktrace()
	: BASE_CLASS()
	, m_bIsBtnOnPaneBorder(FALSE)	// �y�C���{�[�_�[��Ń{�^����������Ă��邩�H
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
	ON_COMMAND(ID_SCE_DEBUG_TOGGLE_BREAKPOINT, &CDlgSticktrace::OnSceDebugToggleBreakpoint)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_TOGGLE_BREAKPOINT, &CDlgSticktrace::OnUpdateSceDebugToggleBreakpoint)
//	ON_NOTIFY(TCN_SELCHANGING, IDC_SCE_TAB_SCRIPT, &CDlgSticktrace::OnTcnSelchangingSceTabScript)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_CONTINUE, &CDlgSticktrace::OnBnClickedSceBtnDebugContinue)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_BREAK, &CDlgSticktrace::OnBnClickedSceBtnDebugBreak)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_STOP, &CDlgSticktrace::OnBnClickedSceBtnDebugStop)
	ON_BN_CLICKED(IDC_SCE_BTN_DEBUG_STEP_TO_NEXT, &CDlgSticktrace::OnBnClickedSceBtnDebugStepToNext)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, &CDlgSticktrace::OnIdleUpdateCmdUI)
	ON_COMMAND(ID_SCE_DEBUG_CONTINUE, &CDlgSticktrace::OnSceDebugContinue)
	ON_UPDATE_COMMAND_UI(ID_SCE_DEBUG_CONTINUE, &CDlgSticktrace::OnUpdateSceDebugContinue)
	ON_BN_CLICKED(IDC_SCE_BTN_CHANGE_VARIABLE, &CDlgSticktrace::OnBnClickedSceBtnChangeVariable)
	ON_BN_CLICKED(IDC_SCE_BTN_ADD_WATCH, &CDlgSticktrace::OnBnClickedSceBtnAddWatch)
	ON_BN_CLICKED(IDC_SCE_BTN_DELETE_WATCH, &CDlgSticktrace::OnBnClickedSceBtnDeleteWatch)
	ON_NOTIFY(NM_CLICK, IDC_SCE_LSV_WATCH, &CDlgSticktrace::OnNMClickSceLsvWatch)
	ON_MESSAGE(WM_USER_DDEDIT_DBLCLKED, &CDlgSticktrace::OnUserDdEditDblClked)
	ON_MESSAGE(WM_USER_TEXT_EDIT_CURLINE_CHANGED, &CDlgSticktrace::OnUserTextEditCurlineChanged)
	ON_BN_CLICKED(IDC_SCE_CHK_DEBUG_MODE, &CDlgSticktrace::OnBnClickedSceChkDebugMode)
END_MESSAGE_MAP()

// CDlgSticktrace ���b�Z�[�W �n���h���[�B


BOOL CDlgSticktrace::OnInitDialog()
{
	BASE_CLASS::OnInitDialog();

	//----- �^�u�ؑւ̕����ɃR���g���[����S�Ĉړ������� -----
	// ��t���[���B
	CRect rtTabFrame;
	GetDlgItem(IDC_SCE_STC_TAB_FRAME)->GetWindowRect(rtTabFrame);
	// �o�̓E�B���h�E�t���[���B
	CRect rtOutput;
	GetDlgItem(IDC_SCE_STC_OUTPUT_FRAME)->GetWindowRect(rtOutput);
	// �G���[�o�̓E�B���h�E�t���[���B
	CRect rtError;
	GetDlgItem(IDC_SCE_STC_ERROR_FRAME)->GetWindowRect(rtError);
	// �E�H�b�`�E�B���h�E�t���[���B
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

	// ���C�A�E�g����ݒ�B
	InitLayoutAll();

	// ���W�X�g���ɓo�^���ꂽ�T�C�Y���ɍ��킹�ă��T�C�Y�B
	SizeToRegistered();

	// �{�[�_�[�̈ʒu�����W�X�g������ǂݏo���B
	int borderX;
	const DWORD dwDataSz = FCRegBase::LoadRegBinary(
		GetRegKeyName().c_str(),
		L"BorderX", (LPBYTE)&borderX, sizeof(borderX));
	if (dwDataSz == sizeof(borderX))
		//----- ���W�X�g������{�[�_�[�ʒu���ǂݏo�����ꍇ -----
	{
		// �{�[�_�[�ʒu���ړ��B
		const int dx = borderX - GetBorderRect().left;
		MoveBorder(dx);
	}

	// �t�H���g�ݒ�B
	// �X�N���v�g�G�f�B�^�[�B
	FCRegBase::InitRegFont(m_font, FDFT_SCRIPT_EDITOR);
	GetDlgItem(IDC_SCE_EDT_SCRIPT)->SetFont(&m_font);

	// �X�N���v�gID���X�g���X�V�B
	UpdateScriptIdList();

	// �{�^���ɃA�C�R����ݒ�B
	CButton* pButton;
	pButton = (CButton*)GetDlgItem(IDC_SCE_BTN_FIND_NEXT);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_FIND));
	pButton = (CButton*)GetDlgItem(IDC_SCE_BTN_EXEC_EDITOR);
	pButton->SetIcon(AfxGetApp()->LoadIcon(IDI_START));

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

	// �e�L�X�g�G�f�B�^�[��Undo�\��ݒ�B
	m_textEditor.SetIsUndoable(TRUE);
	m_textEditor.SetLimitText(640000);
	m_textEditor.SetReadOnly(TRUE);
	const auto isDebugMode = (::InterlockedCompareExchange(&m_debugMode.debugMode, 0, 0) != 0);
	m_textEditor.ActivateBreakpoint(isDebugMode);
	m_textEditor.SetTabSize(2);
	//int s[]{ 10, 20, 30, 40, 50 };
	//m_textEditor.SetTabStops(5, s);
	m_textEditor.SetTabStops(10);

	//----- �f�[�^�擾�p���X�g�R���g���[���̐ݒ� -----
	CListCtrl* pLsvEditor = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_EDITOR);
	UtilDlg::InsertColumn(pLsvEditor, 0, L"", LVCFMT_LEFT, 10);
	UtilDlg::InsertColumn(pLsvEditor, 1, L"", LVCFMT_LEFT, 10);
	UtilDlg::InsertColumn(pLsvEditor, 2, L"", LVCFMT_LEFT, 10);
	UtilDlg::InsertColumn(pLsvEditor, 3, L"", LVCFMT_LEFT, 10);
	// �f�[�^�擾�p���X�g�R���g���[���̓��e�����W�X�g������擾�B
	FD_RegLoadListCtrlContents(this, IDC_SCE_LSV_EDITOR);

	//----- �G�f�B�^�[�I���R���{�{�b�N�X�ɐݒ� -----
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_SCE_CMB_SELECT_EDITOR);
	for (int i = 0; i != pLsvEditor->GetItemCount(); i++)
		pCmb->AddString(pLsvEditor->GetItemText(i, 0));
	if (pCmb->GetCount() != 0)
		pCmb->SetCurSel(0);

	//----- �E�H�b�`���X�g�R���g���[���̐ݒ� -----
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// �t�H�[�J�X������Ԃł��I���}�[�N���\�������悤�ɃX�^�C���ύX�B
	pLsvWatch->ModifyStyle(0, LVS_SHOWSELALWAYS);
	// �S�J�����I���̊g���X�^�C����ݒ�B
	pLsvWatch->SetExtendedStyle(pLsvWatch->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//----- 14.10.02 Fukushiro M. �ǉ��n ()-----
	// �e�X�g�B
	m_watchImage.Create(16, 16, ILC_MASK, 4, 1);
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_COLLAPSED));		// ���Ă���(0)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_EXPANDED));		// �J���Ă���(1)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE));		// �ϐ�(2)
	m_watchImage.Add(AfxGetApp()->LoadIcon(IDI_SUBVARIABLE));	// �����o�[�ϐ�(3)
	pLsvWatch->SetImageList(&m_watchImage, LVSIL_SMALL);
	//----- 14.10.02 Fukushiro M. �ǉ��I ()-----

	//----- �J�����̐ݒ� -----
	CRect rect;
	pLsvWatch->GetClientRect(rect);
	// �J�������B�f�t�H���g�l�ŏ������B
	const int aColumnWidthInit[] =
	{
		rect.Width() / 4,
		rect.Width() / 2,
		rect.Width() / 4,
	};
	// ���O�B
	UtilDlg::InsertColumn(pLsvWatch, 0, UtilStr::LoadString(CString(), IDS_NAME), LVCFMT_LEFT, aColumnWidthInit[0]);
	// �l�B
	UtilDlg::InsertColumn(pLsvWatch, 1, UtilStr::LoadString(CString(), IDS_VALUE), LVCFMT_LEFT, aColumnWidthInit[1]);
	// �^�B
	UtilDlg::InsertColumn(pLsvWatch, 2, UtilStr::LoadString(CString(), IDS_TYPE), LVCFMT_LEFT, aColumnWidthInit[2]);
	// ���X�g�R���g���[���̃J�������E��ݒ�B
	FD_RegLoadListCtrlColumnWidth(this, IDC_SCE_LSV_WATCH);

	// �^�u�R���g���[���Ƀ^�u��ݒ�B
	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_OUTPUT);
	// �G���[�o�́B
	tabCtrl->InsertItem(0, UtilStr::LoadString(CString(), IDS_ERROR));
	// �ϐ��B
	tabCtrl->InsertItem(1, UtilStr::LoadString(CString(), IDS_VARIABLE));
	// �o�́B
	tabCtrl->InsertItem(2, UtilStr::LoadString(CString(), IDS_OUTPUT));
	// �^�u�ύX��ʒm�B
	OnTcnSelchangeSceTabOutput(nullptr, nullptr);


	// �^�u�R���g���[���Ƀ^�u��ݒ�B
	CTabCtrl* tabCtrl2 = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);
	// ��B
	tabCtrl2->InsertItem(0, L"");

#if 0
	// �o�̓E�B���h�E�ɃR���e�L�X�g���j���[��ݒ�B
	m_output.SetContextMenu(IDR_RBTN_SCRIPT_EDITOR, 2);

	// ����ALT�L�[���O�ɃG�f�B�^�[�R���g���[����ǉ��B
	m_stAutoAltExcepted.insert(IDC_SCE_EDT_KEYWORD);
	m_stAutoAltExcepted.insert(IDC_SCE_EDT_SCRIPT);
	m_stAutoAltExcepted.insert(IDC_SCE_EDT_VARIABLE_NAME);
	m_stAutoAltExcepted.insert(IDC_SCE_EDT_VARIABLE_VALUE);

	// �_�C�A���O�̒P�ʕ\����ݒ肷��B
	::SetUnitString(m_hWnd);
	// �����r�Ή�������̐ݒ���s���B
	::RightByLeftHand(m_hWnd);
#endif

	// �e�L�X�g�̈���X�V�B
	m_textEditor.UpdateTextRect();

	// Update the display status of script control buttons.
	PostMessage(WM_IDLEUPDATECMDUI);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

/// <summary>
/// Tcs the set source.
/// This function works at the main application's thread.
/// </summary>
/// <param name="name">The name.</param>
/// <param name="src">The source.</param>
/// <returns></returns>
bool CDlgSticktrace::TC_SetSource(const std::string& name, const std::string& src)
{
	AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
	m_incmd.command = InCmd::Command::SET_SOURCE;
	m_incmd.strParam1 = name;
	m_incmd.strParam2 = src;
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

//----- 17.10.20 Fukushiro M. �폜�n ()-----
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
//----- 17.10.20 Fukushiro M. �폜�I ()-----

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
	/// CListCtrl�̍s�����ԂɌ������A�e�[�u���^�ϐ��ŁuEXPANDED�i�W�J�j�v�̃A�C�R�����ݒ肳��Ă���s��
	/// �L�^����B
	/// �s�̕ϐ��́A���}�̂悤�ɃC���f���g���t�����Ă���B�C���f���g�ʒu�ōċA�ďo�������s����B
	/// 
	///  [-] e
	///     [-] 1
	///         �� "abc"
	///      �� "xyz"
	///      a
	/// </summary>
	/// <param name="lvi">���X�g���ڎ擾�p���[�N�B</param>
	/// <param name="pLsvWatch">���X�g�R���g���[��.</param>
	/// <param name="index">���X�g�R���g���[���̍s�C���f�b�N�X�B�����J�n�ʒu�B</param>
	/// <param name="iIndent">�������C���f���g�B</param>
	/// <param name="wcpPiledVarName">�A���ϐ����B�ϐ�������ʂ��珇��'\b'�łȂ������́B��̗��"abc"�ł́ue\b1\b"abc"�v�ƂȂ�B</param>
	/// <param name="vTopLevelName">���x��0�i�g�b�v���x���j�̕ϐ����B���ł́ue�v�ua�v</param>
	/// <param name="stExpandedName">�uEXPANDED�v���ݒ肳�ꂽ�s�̘A���ϐ����B</param>
	/// <returns></returns>
	std::function<int(LVITEM&, CListCtrl*, int, int, const char*, std::vector<std::string>&, std::unordered_set<std::string>&)> GetLevelItem = [&GetLevelItem](LVITEM& lvi, CListCtrl* pLsvWatch, int index, int iIndent, const char* wcpPiledVarName, std::vector<std::string>& vTopLevelName, std::unordered_set<std::string>& stExpandedName)
	{
		// �ȉ��̂悤�ȃE�H�b�`�E�B���h�E�̕\����Ԃ̏ꍇ�A[-]�̕ϐ��iEXPANDED�j��stExpandedName�ɏW�߂�B
		// �܂��A�g�b�v���x���̕ϐ��ie�Aa�j��vTopLevelName�ɏW�߂�B
		// 
		// [-] e
		//    [-] 1
		//        �� "abc"
		//     �� "xyz"
		//     a

		while (index != pLsvWatch->GetItemCount())
		{
			lvi.iItem = index;
			pLsvWatch->GetItem(&lvi);
			if (lvi.iIndent != iIndent) break;

			std::string szText;
			Astrwstr::wstr_to_astr(szText, lvi.pszText);
			// ���x��0�i�܂�g�b�v�̕ϐ��j�̖��O���W�߂�B
			if (lvi.iIndent == 0) vTopLevelName.push_back(szText);
			// �A���ϐ����B
			const std::string wstrNewPiledVarName = (wcpPiledVarName == NULL) ? szText : std::string(wcpPiledVarName) + "\b" + szText;
			if (lvi.iImage == 0)
				//----- �A�C�R�����e�[�u���uCOLLAPSED�i���Ă���j�v��\���ꍇ -----
			{
				// �uCOLLAPSED�i���Ă���j�v�uEXPANDED�i�J���Ă���j�v�͊֌W�Ȃ����ׂ�B�ʏ�͊J���Ă���ꍇ�̂݉��ʂ��\������邪�A
				// �}�E�X�N���b�N�ɂ������Ƃ��ɂ���������������邽�߁B
				index = GetLevelItem(lvi, pLsvWatch, index + 1, iIndent + 1, wstrNewPiledVarName.c_str(), vTopLevelName, stExpandedName);
			}
			else
				if (lvi.iImage == 1)
					//----- �A�C�R�����e�[�u���uEXPANDED�i�J���Ă���j�v��\���ꍇ -----
				{
					// �uCOLLAPSED�i���Ă���j�v�uEXPANDED�i�J���Ă���j�v�͊֌W�Ȃ����ׂ�B�ʏ�͊J���Ă���ꍇ�̂݉��ʂ��\������邪�A
					// �}�E�X�N���b�N�ɂ������Ƃ��ɂ���������������邽�߁B
					// �uEXPANDED�v��stExpandedName�ɋL�^���Ă����B��ŕϐ���\������ۂɁA�����W�J��Ԃ𕜌����邽�߁B
					// stExpandedName �ɂ̓����o�[�ϐ��܂ł�H�閼�́i�A���ϐ����j��}���B��ue\b1\b"abc"�v
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
	// �ϐ������󂯎��o�b�t�@�B
	wchar_t textBuff[1024];
	LVITEM lvi;
	memset(&lvi, 0, sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;	// �e�L�X�g�A�C���[�W�A�C���f���g���擾�B
	lvi.iSubItem = 0;
	lvi.pszText = textBuff;
	lvi.cchTextMax = _countof(textBuff);

	GetLevelItem(lvi, (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH), 0, 0, nullptr, vTopLevelName, stExpandedName);
}

void CDlgSticktrace::UpdateWatchWindow()
{
	// // �X�N���v�g�t�b�N���̂ݓ��삷��B
	// if (!IsScriptPausing()) return;

	// �ϐ������󂯎��o�b�t�@�B
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

	// // �E�H�b�`�E�B���h�E�B
	// CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// �E�H�b�`�E�B���h�E����x�N���A����B
	// pLsvWatch->DeleteAllItems();

	// std::function<int(CListCtrl*, int, int, int, LVITEM&, const char*, const char*, const AnyValue&, const std::unordered_set<std::string>&)> f;


}

void CDlgSticktrace::UpdateScriptIdList()
{
//----- 17.10.05 Fukushiro M. �폜�n ()-----
//	// ���ݑI������Ă���X�N���v�gID�B
//	const std::wstring wstrCurrentScriptId = GetCurrentScriptId();
//	// �X�N���v�gID�ꗗ���X�g�B
//	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SCE_LST_SCRIPT_ID);
//	pListBox->ResetContent();
//	std::wstringSet stScriptId;
//	FFEnv()->GetScriptIdSet(stScriptId);
//	for (std::wstringSet::const_iterator i = stScriptId.begin(); i != stScriptId.end(); i++)
//		pListBox->AddString(*i);
//	// �X�N���v�gID�ꗗ���X�g�̑I����ύX����B
//	if (!SetCurrentScriptId(wstrCurrentScriptId))
//	{
//		// ����ID���I���ł��Ȃ������ꍇ�A�I���͉��������̂ŕύX�������K�v�B
//		OnLbnSelchangeSceLstScriptId();
//	}
//----- 17.10.05 Fukushiro M. �폜�I ()-----
}

void CDlgSticktrace::InitLayoutAll()
{
	ClearLayout();

	//----- ���C�A�E�g�����v�Z -----

	// �E�B���h�E�̍���ƉE��ɒǏ]���ă��T�C�Y�B
	InitLayout(
		FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_TOP | FCDlgLayoutRec::HOOK_RIGHT,
		IDC_SCE_STC_TOOL_BORDER,
		IDC_SCE_TAB_SCRIPT				// �^�u�R���g���[���B
	);		// �c�[���{�[�_�[

//----- 17.10.05 Fukushiro M. �폜�n ()-----
//	// �E�B���h�E�̍���ƍ����ɒǏ]���ă��T�C�Y�B
//	InitLayout(
//		FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_TOP | FCDlgLayoutRec::HOOK_BOTTOM,
//	);
//----- 17.10.05 Fukushiro M. �폜�I ()-----

//----- 17.10.05 Fukushiro M. �폜�n ()-----
//	// �E�B���h�E�̍����ɒǏ]���ă��T�C�Y�B
//	InitLayout(
//		FCDlgLayoutRec::HOOK_LEFT | FCDlgLayoutRec::HOOK_BOTTOM,
//	);
//----- 17.10.05 Fukushiro M. �폜�I ()-----

	// �E�B���h�E�̎l���ɒǏ]���ă��T�C�Y�B
	InitLayout(
		FCDlgLayoutRec::HOOK_ALL,
		IDC_SCE_EDT_SCRIPT	// �X�N���v�g�G�f�B�^�[�B
	);

//----- 17.09.27 Fukushiro M. �폜�n ()-----
//	// �E�B���h�E�̉E��ɒǏ]���ă��T�C�Y�B
//	InitLayout(
//		FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_TOP,
//	);
//----- 17.09.27 Fukushiro M. �폜�I ()-----

	// �E�B���h�E�̉E��ƉE���ɒǏ]���ă��T�C�Y�B
	InitLayout(
		FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_TOP | FCDlgLayoutRec::HOOK_BOTTOM,
		IDC_SCE_STC_BORDER,
		IDC_SCE_EDT_ERROR,
		IDC_SCE_EDT_OUTPUT,
		IDC_SCE_LSV_WATCH
	);

	// �E�B���h�E�̉E��ɒǏ]���ă��T�C�Y�B
	InitLayout(FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_TOP,
		IDC_SCE_TAB_OUTPUT);

	// �E�B���h�E�̉E���ɒǏ]���Ĉړ��B
	InitLayout(FCDlgLayoutRec::HOOK_RIGHT | FCDlgLayoutRec::HOOK_BOTTOM,
		IDC_SCE_EDT_VARIABLE_NAME,
		IDC_SCE_EDT_VARIABLE_VALUE,
		IDC_SCE_BTN_CHANGE_VARIABLE,
		IDC_SCE_BTN_ADD_WATCH,
		IDC_SCE_BTN_DELETE_WATCH,
		IDC_SCE_STC_LINE_NUMBER			// �s�ԍ�
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
	// �_�C�A���O���ړ��B
	GetDlgItem(wControlId)->SetWindowPos(
		NULL, ptTopLeft.x, ptTopLeft.y, 0, 0,
		SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER |
		SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);
} // CDlgSticktrace::MoveControl.

void CDlgSticktrace::MoveBorder(int dx)
{
	const WORD ids[] = {
		IDC_SCE_TAB_SCRIPT,				// �^�u�R���g���[���B
		IDC_SCE_EDT_SCRIPT,				// �X�N���v�g�E�B���h�E�B
		IDC_SCE_TAB_OUTPUT,				// �^�u�R���g���[���B
		IDC_SCE_STC_BORDER,				// �{�[�_�[�E�B���h�E�B
		IDC_SCE_EDT_ERROR,				// �G���[�o�̓E�B���h�E�B
		IDC_SCE_EDT_OUTPUT,				// �o�̓E�B���h�E�B
		IDC_SCE_LSV_WATCH,				// �E�H�b�`�E�B���h�E�B
		IDC_SCE_EDT_VARIABLE_NAME,		// �ϐ����E�B���h�E�B
		IDC_SCE_EDT_VARIABLE_VALUE,		// �ϐ��l�E�B���h�E�B
		IDC_SCE_STC_LINE_NUMBER,		// �s�ԍ��B

										//----- 14.09.16 Fukushiro M. �폜�n ()-----
										//		IDC_SCE_BTN_CHANGE_VARIABLE,	// �ύX�{�^���B
										//		IDC_SCE_BTN_ADD_WATCH,		// �ǉ��{�^���B
										//----- 14.09.16 Fukushiro M. �폜�I ()-----
										//----- 14.09.17 Fukushiro M. �폜�n ()-----
										//		IDC_SCE_CMB_SELECT_EDITOR,		// �G�f�B�^�[�I���R���{�{�b�N�X�B
										//		IDC_SCE_BTN_EXEC_EDITOR,		// �N���{�^���B
										//----- 14.09.17 Fukushiro M. �폜�I ()-----
	};
	for (int i = 0; i != _countof(ids); i++)
	{
		CRect rt;		// �R���g���[���E�B���h�E�̈�B
		GetDlgItem(ids[i])->GetWindowRect(rt);
		ScreenToClient(&rt);
		switch (ids[i])
		{
		case IDC_SCE_TAB_SCRIPT:				// �^�u�R���g���[���B
		case IDC_SCE_EDT_SCRIPT:				// �X�N���v�g�E�B���h�E�B
												// 14.09.17 Fukushiro M. 1�s�폜 ()
												//		case IDC_SCE_CMB_SELECT_EDITOR:			// �G�f�B�^�[�I���R���{�{�b�N�X�B
			rt.right += dx;
			break;
		case IDC_SCE_STC_BORDER:				// �{�[�_�[�E�B���h�E
												//----- 14.09.16 Fukushiro M. �폜�n ()-----
												//		case IDC_SCE_BTN_CHANGE_VARIABLE:		// �ύX�{�^���B
												//		case IDC_SCE_BTN_ADD_WATCH:			// �ǉ��{�^���B
												//----- 14.09.16 Fukushiro M. �폜�I ()-----
												// 14.09.17 Fukushiro M. 1�s�폜 ()
												//		case IDC_SCE_BTN_EXEC_EDITOR:			// �N���{�^���B
		case IDC_SCE_STC_LINE_NUMBER:			// �s�ԍ��B
			rt.left += dx;
			rt.right += dx;
			break;
		default:
			rt.left += dx;
		}
		GetDlgItem(ids[i])->MoveWindow(rt);
	}
	// ���C�A�E�g����ݒ�B
	InitLayoutAll();
} // CDlgSticktrace::MoveBorder.

/// <summary>
/// Sets the source.
/// Usually this function is called from the main application asynchronously.
/// This function works at the Sticktrace Window's thread.
/// </summary>
/// <param name="name">The name.</param>
/// <param name="src">The source.</param>
void CDlgSticktrace::SetSource(const std::string & name, const std::wstring & src)
{
	if (name.empty()) return;
	auto i = m_nameToSource.find(name);
	if (i == m_nameToSource.end())
	{
		m_sourceNameArray.emplace_back(name);
		m_nameToSource[name] = src;
	}
	else
	{
		i->second = src;
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
	// �G���[���b�Z�[�W�͈ȉ��̌`���B
	//
	// -----------------------------------------
	// error 5:�v���O�������s���̃G���[�ł��B
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

	// �I��͈͂��擾�B
	int startCharIndex;
	int endCharIndex;
	m_errorout.GetSel(startCharIndex, endCharIndex);
	// �I��͈͂̍s���擾�B
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
		// match(0)��wcpLine�S�́B
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

	// �E�H�b�`�E�B���h�E�B
	CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
	// �E�H�b�`�E�B���h�E����x�N���A����B
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
		int iImage = 1;	// �A�C�R���C���[�W�̃C���f�b�N�X�B
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
		// �ϐ��l
		pLsvWatch->SetItemText(index, 1, value.c_str());
		// �ϐ��^
		pLsvWatch->SetItemText(index, 2, type.c_str());
		// �e�ϐ��̃C���f�b�N�X��ݒ�B
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
	case 0:	// �G���[�o�́B
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
	case 1:	// �ϐ��B
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
	case 2:	// �o�́B
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
	{	//----- �y�C����������Ƀ}�E�X������ꍇ -----
		// OnLButtonUp�܂ł���Window�ɃC�x���g�B
		::SetCapture(m_hWnd);
		// �y�C���{�[�_�[��Ń{�^����������Ă��邩�H
		m_bIsBtnOnPaneBorder = TRUE;
		// �J�[�\����ύX.
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
		//----- �y�C���������̃N���C�A���g���W��m_rtBar��
		//		�ݒ肵�ACFCMainFrame�̃N���C�A���g���W�ɕϊ����� -----
		CRect rtBorder = GetBorderRect();
		m_rtBar.SetRect((rtBorder.left + rtBorder.right) / 2, rtBorder.top,
			(rtBorder.left + rtBorder.right) / 2, rtBorder.bottom);
		//----- CFCMainFrame��ɋ^���y�C����������`�� -----
		CClientDC dcFrame(this);
		// �C�����`��p�N���X���쐬�B
		FCEditDraw edrawFrame(&dcFrame, FD_PANEBAR_WIDTH, FD_PANEBAR_COLOR ^ RGB(255, 255, 255), -1, 0, 0);
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));

		m_hwndWhenBorderMoving = ::GetFocus();
		SetTimer(BORDER_TIMER, 50, nullptr);
	}
	// Superclass�֐����s�B
	BASE_CLASS::OnLButtonDown(nFlags, point);
}


void CDlgSticktrace::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsBtnOnPaneBorder)
	{	//----- �y�C���{�[�_�[��Ń{�^����������Ă���ꍇ -----
		KillTimer(BORDER_TIMER);
		// �}�E�X�C�x���g�L���v�`���������B
		::ReleaseCapture();
		// �y�C���{�[�_�[��Ń{�^����������Ă��邩�H
		m_bIsBtnOnPaneBorder = FALSE;
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
		//----- �O��`�����^���y�C�������������� -----
		CClientDC dcFrame(this);
		// �C�����`��p�N���X���쐬�B
		FCEditDraw edrawFrame(&dcFrame, FD_PANEBAR_WIDTH, FD_PANEBAR_COLOR ^ RGB(255, 255, 255), -1, 0, 0);
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));

		// �y�C���{�[�_�[�̈ړ������B
		const int pxMove = m_rtBar.CenterPoint().x - GetBorderRect().CenterPoint().x;
		MoveBorder(pxMove);

		// �{�[�_�[�ʒu���擾�B
		const int borderX = GetBorderRect().left;
		// �{�[�_�[�̈ʒu�����W�X�g���ɋL�^�B
		FCRegBase::SaveRegBinary(GetRegKeyName().c_str(), L"BorderX", (LPBYTE)&borderX, sizeof(borderX));
	}
	// Superclass�֐����s�B
	BASE_CLASS::OnLButtonUp(nFlags, point);
}


void CDlgSticktrace::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bIsBtnOnPaneBorder)
	{	//----- �y�C���{�[�_�[��Ń{�^����������Ă���ꍇ -----
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
		//----- �O��`�����^���y�C�������������� -----
		CClientDC dcFrame(this);
		// �C�����`��p�N���X���쐬�B
		FCEditDraw edrawFrame(&dcFrame, FD_PANEBAR_WIDTH, FD_PANEBAR_COLOR ^ RGB(255, 255, 255), -1, 0, 0);
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));
		//----- �V�����y�C���������̍��W��ݒ� -----
		// �y�C���o�[�̍��W���v�Z�Bm_rtBar.right �͕s�g�p�B
		m_rtBar.SetRect(point.x, m_rtBar.top, point.x, m_rtBar.bottom);

		//----- �y�C���o�[�̉ғ��͈͂��v�Z -----
		CRect rtScript;		// �X�N���v�g�E�B���h�E�̈�
		CRect rtChange;		// �ύX�{�^���̈�
		GetDlgItem(IDC_SCE_EDT_SCRIPT)->GetWindowRect(rtScript);	// �X�N���v�g�G�f�B�^�[
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->GetWindowRect(rtChange);		// �ύX�{�^��
		ScreenToClient(&rtScript);
		ScreenToClient(&rtChange);
		// �X�N���v�g�E�B���h�E�̕���100�ȉ��ɂȂ�Ȃ��悤��������B�E�͕ύX�{�^�����B���Ȃ��悤�B
		if (m_rtBar.left < rtScript.left + 100)
			m_rtBar.left = rtScript.left + 100;
		if (rtChange.left - 10 < m_rtBar.left)
			m_rtBar.left = rtChange.left - 10;
		m_rtBar.right = m_rtBar.left;
		//----- CFCMainFrame��ɋ^���y�C����������`�� -----
		edrawFrame.Line(&CPoint(m_rtBar.left, m_rtBar.top),
			&CPoint(m_rtBar.left, m_rtBar.bottom));
	}
	else
	{	//----- �y�C���{�[�_�[��Ń{�^����������Ă���ȊO�̏ꍇ -----
		if (GetBorderRect().PtInRect(point))
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SPLIT));
	}
	// Superclass�֐����s�B
	BASE_CLASS::OnMouseMove(nFlags, point);
}


BOOL CDlgSticktrace::PreTranslateMessage(MSG* pMsg)
{
	// TRACE(L"MSG=%x LPARAM=%x WPARAM=%x\n", pMsg->message, pMsg->lParam, pMsg->wParam);

	return BASE_CLASS::PreTranslateMessage(pMsg);
}


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
	const auto scriptName = m_sourceNameArray.at(tabCtrl->GetCurSel());
	const auto src = m_nameToSource.at(scriptName);
	m_textEditor.SetContentName(scriptName);
	m_textEditor.SetWindowText(src.c_str());
	// SetDlgItemText(IDC_SCE_EDT_SCRIPT, src.c_str());
	if (pResult != nullptr) *pResult = 0;
}

LRESULT CDlgSticktrace::OnUserCommand(WPARAM, LPARAM)
{
	InCmd::Command command;
	std::string strParam1;
	std::string strParam2;
	__int64 i64Param1;
	{
		AutoLeaveCS acs(m_incmd.cs, m_incmd.cv);
		command = m_incmd.command;
		strParam1 = m_incmd.strParam1;
		strParam2 = m_incmd.strParam2;
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
		Astrwstr::astr_to_wstr(wsrc, strParam2);
		SetSource(strParam1, wsrc);
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

		/*
	case CDlgSticktrace::InCmd::LineHookFuncCommand::ON_START:
		OnStart();
		break;
	case CDlgSticktrace::InCmd::LineHookFuncCommand::ON_STOP:
		OnStop();
		break;
		*/

	default:
		break;
	}

	/*
	{
		AutoLeaveCS acs(m_breakpointSlow.cs);
		if (m_breakpointSlow.m_name.empty()) return 1;
		auto i = m_nameToSource.find(m_breakpointSlow.m_name);
		if (i == m_nameToSource.end())
		{
			m_sourceNameArray.emplace_back(m_breakpointSlow.m_name);
			m_nameToSource[m_breakpointSlow.m_name] = m_breakpointSlow.m_src;
		}
		else
		{
			i->second = m_breakpointSlow.m_src;
		}
		m_breakpointSlow.m_name.clear();
		m_breakpointSlow.m_src.clear();
	}

	CTabCtrl* tabCtrl = (CTabCtrl*)GetDlgItem(IDC_SCE_TAB_SCRIPT);
	const auto text = UtilDlg::GetItemText(tabCtrl, tabCtrl->GetCurSel());
	tabCtrl->DeleteAllItems();
	int curIndex = 0;
	for (int i = 0; i != m_sourceNameArray.size(); i++)
	{
		tabCtrl->InsertItem(i, m_sourceNameArray[i].c_str());
		if (text == m_sourceNameArray[i])
			curIndex = i;
	}
	if (m_sourceNameArray.empty())
		tabCtrl->InsertItem(0, L"");
	tabCtrl->SetCurSel(curIndex);

	OnTcnSelchangeSceTabScript(nullptr, nullptr);
	*/

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
* @brief	CFCTextEdit�̃}�[�J�[�̈�N���b�N�B
* @author	Fukushiro M.
* @date	2014/09/16(��) 21:07:54
*
* @param[in]	WPARAM	wParam	ControlId�B
* @param[in]	LPARAM	lParam	lineIndex�B
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

//********************************************************************************************
/*!
* @brief	[�u���[�N�|�C���g�̐ݒ�/����]�R�}���h������s�����B
* @author	Fukushiro M.
* @date	2014/09/14(��) 07:43:52
*
* @return	�Ȃ� (none)
*/
//********************************************************************************************
void CDlgSticktrace::OnSceDebugToggleBreakpoint()
{
	const int iCurLineIndex = m_textEditor.GetCurLineIndex();
	SendMessage(WM_USER_TEXT_EDIT_MARKER_CLICKED, IDC_SCE_EDT_SCRIPT, iCurLineIndex);
} // CDlgSticktrace::OnSceDebugToggleBreakpoint.

  //********************************************************************************************
  /*!
  * @brief	[�u���[�N�|�C���g�̐ݒ�/����]�R�}���h�̗L���E������ݒ肷��B
  * @author	Fukushiro M.
  * @date	2014/09/14(��) 07:43:52
  *
  * @param[out]	CCmdUI*	pCmdUI	�R�}���h�̏�Ԃ�ݒ�B
  *
  * @return	�Ȃ� (none)
  */
  //********************************************************************************************
void CDlgSticktrace::OnUpdateSceDebugToggleBreakpoint(CCmdUI* pCmdUI)
{
} // CDlgSticktrace::OnUpdateSceDebugToggleBreakpoint.


//  void CDlgSticktrace::OnTcnSelchangingSceTabScript(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//	  // TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
//	  *pResult = 0;
//  }

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
		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(FALSE);
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
//----- 17.10.07 Fukushiro M. �폜�n ()-----
//	case CDlgSticktrace::Mode::PROCEED_TO_NEXT:
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE))->SetCheck(BST_UNCHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK))->SetCheck(BST_CHECKED);
//		((CButton*)GetDlgItem(IDC_SCE_BTN_DEBUG_STOP))->SetCheck(BST_UNCHECKED);
//
//		GetDlgItem(IDC_SCE_BTN_DEBUG_CONTINUE)->EnableWindow(TRUE);
//		GetDlgItem(IDC_SCE_BTN_DEBUG_BREAK)->EnableWindow(TRUE);
//		GetDlgItem(IDC_SCE_BTN_DEBUG_STOP)->EnableWindow(TRUE);
//		GetDlgItem(IDC_SCE_BTN_CHANGE_VARIABLE)->EnableWindow(TRUE);
//		GetDlgItem(IDC_SCE_BTN_ADD_WATCH)->EnableWindow(TRUE);
//		GetDlgItem(IDC_SCE_BTN_DELETE_WATCH)->EnableWindow(TRUE);
//		break;
//----- 17.10.07 Fukushiro M. �폜�I ()-----
	default:
		break;
	}
	return 1;
}


void CDlgSticktrace::OnSceDebugContinue()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgSticktrace::OnUpdateSceDebugContinue(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/// <summary>
/// Called when [the change variable button clicked].
/// </summary>
void CDlgSticktrace::OnBnClickedSceBtnChangeVariable()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}

/// <summary>
/// Called when [the add variable button clicked].
/// </summary>
void CDlgSticktrace::OnBnClickedSceBtnAddWatch()
{
	// �ϐ������擾�B
	std::wstring wstr;
	UtilDlg::GetDlgItemText(this, IDC_SCE_EDT_VARIABLE_NAME, wstr);
	UtilString::Trim(wstr);
	if (!wstr.empty())
	{
		// �s��ǉ��B
		CListCtrl* pLsvWatch = (CListCtrl*)GetDlgItem(IDC_SCE_LSV_WATCH);
		const int itemCount = pLsvWatch->GetItemCount();

		// �e�X�g�B
		LVITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = itemCount;
		lvi.iSubItem = 0;
		lvi.pszText = (wchar_t*)wstr.c_str();
		lvi.iImage = 0;
		pLsvWatch->InsertItem(&lvi);

		/*
		std::string varName;
		Astrwstr::wstr_to_astr(varName, wstr);
		{
			AutoLeaveCS acs(m_outcmd.cs, m_outcmd.cv);
			m_outcmd.command = SticktraceCommand::GET_VARIABLE;
			m_outcmd.strParam1 = varName;
			acs.WakeConditionVariable();
		}
		*/
		// �E�H�b�`�E�B���h�E���X�V�B
		UpdateWatchWindow();
	}
}

/// <summary>
/// Called when [the delete variable button clicked].
/// </summary>
void CDlgSticktrace::OnBnClickedSceBtnDeleteWatch()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
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

			// �E�H�b�`�E�B���h�E���X�V�B
			UpdateWatchWindow();
		}
	}
	if (pResult != nullptr) *pResult = 0;
}

//********************************************************************************************
/*!
* @brief	CFCDdEdit�R���g���[�����_�u���N���b�N���ꂽ�Ƃ��Ɏ��s�����B
* @author	Fukushiro M.
* @date	2014/09/12(��) 17:53:35
*
* @param[in]	WPARAM	wParam	���M���̃R���g���[��ID�B
* @param[in]	LPARAM	���g�p
*
* @return	LRESULT	0:CFCDdEdit��DBLCLK����������/1:�������Ȃ�
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
  * @brief	CFCTextEdit�̌��ݍs�̕ύX�̒ʒm�B
  * @author	Fukushiro M.
  * @date	2014/09/17(��) 21:50:33
  *
  * @param[in]	WPARAM	wParam	ControlId�B
  * @param[in]	LPARAM	lParam	lineIndex�B
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
