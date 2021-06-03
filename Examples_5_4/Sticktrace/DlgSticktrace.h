#pragma once

#include "DlgModelessBase.h"	// Superclass.
#include "UtilSync.h"			// For AutoCS,AutoLeaveCS.
#include "TextEdit.h"
#include "DdEdit.h"
#include "Sticktrace.h"

// CDlgSticktrace ダイアログ

class CDlgSticktrace : public CFCDlgModelessBase
{
protected:
	using BASE_CLASS = CFCDlgModelessBase;

	DECLARE_DYNAMIC(CDlgSticktrace)

public:
	enum class Mode : LONG
	{
		STOP = 0,
		STOPPING,			// Just issued Stop command to main thread.
		RUN,
		SUSPEND,			// Suspended.
		// PROCEED_NEXT,	// Proceed to next line.
	};

public:
	CDlgSticktrace();   // 標準コンストラクター
	virtual ~CDlgSticktrace();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STICK_TRACE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void SetDebuggerCallback(SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc, void* debuggerCallbackData);

protected:
	static void ThreadForDebbugerCallback(
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData,
		unsigned int dialogId,
		SticktraceDef::DebuggerCommand command,
		std::string strParam1,
		std::string strParam2
	);

public:
	bool APT_Show(bool show);
	bool APT_IsVisible(bool & isVisible);
	bool APT_IsScriptModified(bool & isModified);
	bool APT_SetSource(const char * sandbox, const char * name, const char * src);
	bool APT_IsDebugMode();
	bool APT_IsBreakpoint(const char* name, int lineIndex);
	bool APT_OnSuspended();
	bool APT_OnResumed();
	bool APT_Jump(const char * name, int lineIndex);
	bool APT_NewSession();
	bool APT_OnStart(SticktraceDef::ExecType execType);
	bool APT_OnStop(SticktraceDef::ExecType execType);
//----- 21.05.18 Fukushiro M. 削除始 ()-----
//// 21.05.18 Fukushiro M. 1行追加 ()
//	bool APT_OnErrorStop(const char * message);
//----- 21.05.18 Fukushiro M. 削除終 ()-----
	bool APT_OutputError(const char * message);
	bool APT_OutputDebug(const char * message);
	bool APT_SetWatch(const char * data);
	bool APT_SetVariableNotify(bool succeeded);
	SticktraceDef::SuspendCommand APT_WaitCommandIsSet(StickString & paramA, uint32_t waitMilliseconds);

private:
	HACCEL	m_accelerator;
	CFCTextEdit m_textEditor;
	std::string m_sandbox;
	CFCDdEdit m_output;
	CFCDdEdit m_errorout;
	CFont m_font;					// エディターフォント。
	CImageList m_watchImage;		// ウォッチリストのイメージリスト。

	SticktraceDef::DebuggerCallbackFunc m_DGT_debuggerCallbackFunc;
	void* m_debuggerCallbackData;

	BOOL m_isScriptEditable;

	BOOL m_bIsBtnOnPaneBorder;		// ペインボーダー上でボタンが押されているか？
	CRect m_rtBar;					// ペインボーダー移動中のワーク。ボーダー位置を記録。
	HWND m_hwndWhenBorderMoving;	// Focused window when border moving.

	struct
	{
		LONG isScriptModified;	// 1:Script is modified/0:Not modified.
	} m_scriptModify;

	/// <summary>
	/// Data for synchronous communication from the application thread to the DlgSticktrace thread.
	/// </summary>
	struct InCmd
	{
		enum class Command
		{
			NONE,
			SHOW,
			IS_VISIBLE,
			IS_SCRIPT_MODIFIED,
			SET_SOURCE,
			NEW_SESSION,
			ON_START,
			ON_STOP,
			ON_SUSPENDED,
			ON_RESUMED,
//----- 21.05.18 Fukushiro M. 削除始 ()-----
//// 21.05.18 Fukushiro M. 1行追加 ()
//			ON_ERROR_STOP,
//----- 21.05.18 Fukushiro M. 削除終 ()-----
			JUMP,
			OUTPUT_ERROR,
			OUTPUT_DEBUG,
			SET_WATCH,
			SET_VARIABLE_NOTIFY,
		};

		AutoCS cs;	// Locking object to protect this area.
		AutoCV cv;	// Locking object to protect this area.
		Command command;
		std::string strParam1;
		std::string strParam2;
		std::string strParam3;
		__int64 i64Param1;

		InCmd() : command(Command::NONE) {}
		~InCmd() = default;
	} m_incmd;

	/// <summary>
	/// Data for synchronous communication from the DlgSticktrace thread to the application thread.
	/// </summary>
	struct OutCmd
	{
		AutoCS cs;	// Locking object to protect this area.
		AutoCV cv;	// Locking object to protect this area.
		SticktraceDef::SuspendCommand command;
		std::string strParamA;

		OutCmd() : command(SticktraceDef::SuspendCommand::NONE) {}
		~OutCmd() = default;
	} m_outcmd;

	/// <summary>
	/// Check the breakpoint status fast.
	/// Member variables are refered from main thread and dialog thread both, using Interlocked function.
	/// Application can confirm that breakpoints exist or not.
	/// </summary>
	struct
	{
		LONG suspendMode;		// CDlgSticktrace::Mode::XXXX.
		LONG breakpointCount;	// Total number of breakpoints.
	} m_breakpointFast;

	/// <summary>
	/// Check the line number of breakpoint.
	/// Member variables are refered from main thread and dialog thread both, using AutoCS.
	/// It is not fast.
	/// </summary>
	struct
	{
		AutoCS cs;
		std::vector<__int8> lineIndexToIsBreak;
		std::set<std::pair<int, std::string>> lineIndexAndSourceNameSet;

		/// <summary>
		/// Source name of run-to-cursor.
		/// </summary>
		std::string m_runToCursorSourceName;

		/// <summary>
		/// Line number of run-to-cursor.
		/// -1: Run-to-cursor is not set.
		/// </summary>
		int m_runToCursorLineIndex;
	} m_breakpointSlow;

	struct
	{
		LONG debugMode;		// 0:not debug / 1:debug mode
	} m_debugMode;

	std::vector<std::string> m_sourceNameArray;
	std::unordered_map<std::string, std::pair<std::string, std::wstring>> m_nameToSandSource;

protected:
	virtual void GetWatchedVariables(std::vector<std::string>& vTopLevelName, std::unordered_set<std::string>& stExpandedName) const;
	virtual void UpdateWatchWindow();
	virtual void UpdateScriptIdList();
	virtual void InitLayoutAll();
	virtual CRect GetBorderRect() const;
	virtual void MoveControl(WORD wControlId, const CSize& szMove);
	virtual void MoveBorder(int dx);

	void SetSource(const std::string & sandbox, const std::string & name, const std::wstring & src);
	void Jump(const std::string & name, int lineIndex, CFCTextEdit::MarkerType markerType, bool selectLine);
	bool JumpErrorLocation();
	void NewSession();
	void OnStart(SticktraceDef::ExecType execType);
	void OnStop(SticktraceDef::ExecType execType);
	void OnSuspended();
	void OnResumed();
//----- 21.05.18 Fukushiro M. 削除始 ()-----
//// 21.05.18 Fukushiro M. 1行追加 ()
//	void OnErrorStop(const std::string & message);
//----- 21.05.18 Fukushiro M. 削除終 ()-----
	void OutputError(const std::string & message);
	void OutputDebug(const std::string & message);
	void SetWatch(const std::string & data);
	void SetVariableNotify(bool succeeded);

public:
	afx_msg void OnTcnSelchangeSceTabOutput(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTcnSelchangeSceTabScript(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUserCommand(WPARAM, LPARAM);
	afx_msg LRESULT OnUserBreakpointUpdated(WPARAM, LPARAM);
	afx_msg LRESULT OnUserTextEditMarkerClicked(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedSceBtnDebugContinue();
	afx_msg void OnBnClickedSceBtnDebugBreak();
	afx_msg void OnBnClickedSceBtnDebugStop();
	afx_msg void OnBnClickedSceBtnDebugStepToNext();
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM, LPARAM);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnEditScriptEdit();
	afx_msg void OnUpdateEditScriptEdit(CCmdUI *pCmdUI);
	afx_msg void OnSceEditInputLine();
	afx_msg void OnUpdateEditInputLine(CCmdUI *pCmdUI);
	afx_msg void OnSceEditGotoLine();
	afx_msg void OnUpdateSceEditGotoLine(CCmdUI *pCmdUI);
	afx_msg void OnSceWinKeyword();
	afx_msg void OnUpdateSceWinKeyword(CCmdUI *pCmdUI);
	afx_msg void OnEditFindNextText();
	afx_msg void OnUpdateEditFindNextText(CCmdUI *pCmdUI);
	afx_msg void OnEditFindPrevText();
	afx_msg void OnUpdateEditFindPrevText(CCmdUI *pCmdUI);
	afx_msg void OnSceWinClearError();
	afx_msg void OnUpdateSceWinClearError(CCmdUI *pCmdUI);
	afx_msg void OnSceWinClearOutput();
	afx_msg void OnUpdateSceWinClearOutput(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugToggleBreakpoint();
	afx_msg void OnUpdateSceDebugToggleBreakpoint(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugClearBreakpoint();
	afx_msg void OnUpdateSceDebugClearBreakpoint(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugMode();
	afx_msg void OnUpdateSceDebugMode(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugContinue();
	afx_msg void OnUpdateSceDebugContinue(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugBreak();
	afx_msg void OnUpdateSceDebugBreak(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugStepToNext();
	afx_msg void OnUpdateSceDebugStepToNext(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugRunToCursor();
	afx_msg void OnUpdateSceDebugRunToCursor(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugStop();
	afx_msg void OnUpdateSceDebugStop(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugSetVariable();
	afx_msg void OnUpdateSceDebugSetVariable(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugAddWatch();
	afx_msg void OnUpdateSceDebugAddWatch(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugDeleteWatch();
	afx_msg void OnUpdateSceDebugDeleteWatch(CCmdUI *pCmdUI);
	afx_msg void OnSceWinError();
	afx_msg void OnUpdateSceWinError(CCmdUI *pCmdUI);
	afx_msg void OnSceWinWatch();
	afx_msg void OnUpdateSceWinWatch(CCmdUI *pCmdUI);
	afx_msg void OnSceWinOutput();
	afx_msg void OnUpdateSceWinOutput(CCmdUI *pCmdUI);
	afx_msg void OnSceWinVariableName();
	afx_msg void OnUpdateSceWinVariableName(CCmdUI *pCmdUI);
	afx_msg void OnSceWinVariableValue();
	afx_msg void OnUpdateSceWinVariableValue(CCmdUI *pCmdUI);
	afx_msg void OnHelp();
	afx_msg void OnUpdateHelp(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedSceBtnSetVariable();
	afx_msg void OnBnClickedSceBtnAddWatch();
	afx_msg void OnBnClickedSceBtnDeleteWatch();
	afx_msg void OnBnClickedSceChkDebugMode();
	afx_msg void OnBnClickedSceBtnFindNext();
// 21.05.07 Fukushiro M. 1行追加 ()
	afx_msg void OnBnClickedSceBtnGotoLine();
	afx_msg void OnChangeSceEdtKeyword();
	afx_msg void OnNMClickSceLsvWatch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUserDdEditDblClked(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnUserTextEditCurlineChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLvnItemchangedSceLsvWatch(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnTcnSelchangingSceTabScript(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnNcActivate(BOOL bActive);
//	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnOptionAutoAltkey();
	afx_msg void OnUpdateOptionAutoAltkey(CCmdUI *pCmdUI);
};
