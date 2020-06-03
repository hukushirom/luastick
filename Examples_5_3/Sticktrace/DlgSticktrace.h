#pragma once

#include "DlgModelessBase.h"	// Superclass.
#include "UtilSync.h"
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
		STOPPING,
		RUN,
		SUSPEND,		// Suspended.
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
	bool TC_SetSource(const std::string& sandbox, const std::string& name, const std::string& src);
	bool TC_IsDebugMode();
	bool TC_IsBreakpoint(const char* name, int lineIndex);
	bool TC_OnSuspended();
	bool TC_OnResumed();
	bool TC_Jump(const char * name, int lineIndex);
	bool TC_OnStart();
	bool TC_OnStop();
	bool TC_OutputError(const char * message);
	bool TC_OutputDebug(const char * message);
	bool TC_SetWatch(const std::string& data);
	SticktraceCommand TC_GetCommand(std::string & param, uint32_t waitMilliseconds);

private:
	CFCTextEdit m_textEditor;
	std::string m_sandbox;
	CFCDdEdit m_output;
	CFCDdEdit m_errorout;
	CFont m_font;					// エディターフォント。
	// BOOL m_bIsDebugMode;				// デバッグモード？
	CImageList m_watchImage;			// ウォッチリストのイメージリスト。

	BOOL			m_bIsBtnOnPaneBorder;	// ペインボーダー上でボタンが押されているか？
	CRect			m_rtBar;				// ペインボーダー移動中のワーク。ボーダー位置を記録。
	HWND			m_hwndWhenBorderMoving;	// Focused window when border moving.
	
	/// <summary>
	/// Data for synchronous communication from the application thread to the DlgSticktrace thread.
	/// </summary>
	struct InCmd
	{
		enum class Command
		{
			NONE,
			SET_SOURCE,
			ON_START,
			ON_STOP,
			ON_SUSPENDED,
			ON_RESUMED,
			JUMP,
			OUTPUT_ERROR,
			OUTPUT_DEBUG,
			SET_WATCH,
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
		SticktraceCommand command;
		std::string strParam1;

		OutCmd() : command(SticktraceCommand::NONE) {}
		~OutCmd() = default;
	} m_outcmd;

	struct
	{
		LONG suspendMode;
		LONG breakpointCount;
	} m_breakpointFast;

	struct
	{
		AutoCS cs;
		std::vector<__int8> lineIndexToIsBreak;
		std::set<std::pair<int, std::string>> lineIndexAndSourceNameSet;
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
	void OnStart();
	void OnStop();
	void OnSuspended();
	void OnResumed();
	void OutputError(const std::string & message);
	void OutputDebug(const std::string & message);
	void SetWatch(const std::string & data);
	// void OnStart();
	// void OnStop();

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
	afx_msg void OnSceEditGotoLine();
	afx_msg void OnUpdateSceEditGotoLine(CCmdUI *pCmdUI);
	afx_msg void OnSceWinKeyword();
	afx_msg void OnUpdateSceWinKeyword(CCmdUI *pCmdUI);
	afx_msg void OnEditFindNextText();
	afx_msg void OnUpdateEditFindNextText(CCmdUI *pCmdUI);
	afx_msg void OnEditFindPrevText();
	afx_msg void OnUpdateEditFindPrevText(CCmdUI *pCmdUI);
	afx_msg void OnSceOutwinGotoLocation();
	afx_msg void OnUpdateSceOutwinGotoLocation(CCmdUI *pCmdUI);
	afx_msg void OnSceOutwinClear();
	afx_msg void OnUpdateSceOutwinClear(CCmdUI *pCmdUI);
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
	afx_msg void OnSceDebugUntilCaret();
	afx_msg void OnUpdateSceDebugUntilCaret(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugStop();
	afx_msg void OnUpdateSceDebugStop(CCmdUI *pCmdUI);
	afx_msg void OnSceDebugChangeVariable();
	afx_msg void OnUpdateSceDebugChangeVariable(CCmdUI *pCmdUI);
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
	afx_msg void OnBnClickedSceBtnChangeVariable();
	afx_msg void OnBnClickedSceBtnAddWatch();
	afx_msg void OnBnClickedSceBtnDeleteWatch();
	afx_msg void OnNMClickSceLsvWatch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUserDdEditDblClked(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnUserTextEditCurlineChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedSceChkDebugMode();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLvnItemchangedSceLsvWatch(NMHDR *pNMHDR, LRESULT *pResult);
};
