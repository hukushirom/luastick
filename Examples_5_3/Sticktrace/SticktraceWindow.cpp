// SticktraceWindow.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include <thread>
#include "Resource.h"		// For IDD_STICK_TRACE.
#include "RegBase.h"
#include "DlgSticktrace.h"
#include "Sticktrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class __declspec(dllexport) SticktraceWindow
{
private:
	static SticktraceWindow* New(
		const wchar_t* companyName,
		const wchar_t* packageName,
		const wchar_t* applicationName,
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	);
	static void Delete(SticktraceWindow* mtw);
	SticktraceWindow(
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	);
	~SticktraceWindow();
	bool Show(bool show);
	bool IsVisible(bool & isVisible);
	bool SetSource(const std::string& sandbox, const std::string& name, const std::string& source);
	bool IsDebugMode();
	bool IsBreakpoint(const char* name, int lineIndex);
	bool OnSuspended();
	bool OnResumed();
	bool Jump(const char * name, int lineIndex);
	bool NewSession();
	bool OnStart();
	bool OnStop();
	bool OutputError(const char* message);
	bool OutputDebug(const char* message);
	bool SetWatch(const std::string& data);
	bool SetVariableNotify(bool succeeded);
	SticktraceDef::SuspendCommand WaitCommandIsSet(std::string & paramA, uint32_t waitMilliseconds);

// 20.06.10  1行削除 ()
//	void Create(unsigned int dialogId);
	void Destroy();
	static void StaticThreadFunc(
		SticktraceWindow* stickTrace,
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	);
	void ThreadFunc(
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
		void* debuggerCallbackData
	);

private:
	CDlgSticktrace* m_stickTraceDlg;
	LONG m_threadId;
	LONG m_threadTerminateFlag;
};


SticktraceWindow* SticktraceWindow::New(
	const wchar_t* companyName,
	const wchar_t* packageName,
	const wchar_t* applicationName,
	unsigned int dialogId,
	SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
	void* debuggerCallbackData
)
{
	FCRegBase::SetApplicationInfo(companyName, packageName, applicationName);
	return new SticktraceWindow(dialogId, DGT_debuggerCallbackFunc, debuggerCallbackData);
}

void SticktraceWindow::Delete(SticktraceWindow* mtw)
{
	delete mtw;
}

SticktraceWindow::SticktraceWindow(
	unsigned int dialogId,
	SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
	void* debuggerCallbackData
)
	: m_stickTraceDlg(nullptr)
	, m_threadId(0)
	, m_threadTerminateFlag(0)
{
	if (::InterlockedCompareExchange(&m_threadId, 0, 0) != 0) return;
	std::thread{ SticktraceWindow::StaticThreadFunc, this, dialogId, DGT_debuggerCallbackFunc, debuggerCallbackData }.detach();
	for (int i = 0; i != 100 && ::InterlockedCompareExchange(&m_threadId, 0, 0) == 0; i++)
		::Sleep(10);
}

SticktraceWindow::~SticktraceWindow()
{
	Destroy();
}

void SticktraceWindow::StaticThreadFunc(
	SticktraceWindow* stickTrace,
	unsigned int dialogId,
	SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
	void* debuggerCallbackData
)
{
	stickTrace->ThreadFunc(dialogId, DGT_debuggerCallbackFunc, debuggerCallbackData);
}

void SticktraceWindow::ThreadFunc(
	unsigned int dialogId,
	SticktraceDef::DebuggerCallbackFunc DGT_debuggerCallbackFunc,
	void* debuggerCallbackData
)
{
	m_stickTraceDlg = new CDlgSticktrace();
	m_stickTraceDlg->SetDialogId(dialogId);
	m_stickTraceDlg->SetDebuggerCallback(DGT_debuggerCallbackFunc, debuggerCallbackData);
	m_stickTraceDlg->Create(IDD_STICK_TRACE);
	m_stickTraceDlg->ShowWindow(SW_HIDE);
	// スレッドIDを設定。後で強制終了できるよう。CSロック前なのでInterlockedで設定。
	::InterlockedExchange(&m_threadId, (LONG)::GetCurrentThreadId());
	while (::InterlockedCompareExchange(&m_threadTerminateFlag, 0, 0) == 0)
		AfxGetApp()->PumpMessage();
	m_stickTraceDlg->DestroyWindow();
	delete m_stickTraceDlg;
	m_stickTraceDlg = nullptr;
	// スレッドIDを解除。
	::InterlockedExchange(&m_threadId, 0);
	::InterlockedExchange(&m_threadTerminateFlag, 0);
}

//----- 20.06.10  削除始 ()-----
//void SticktraceWindow::Create(unsigned int dialogId)
//{
//	if (::InterlockedCompareExchange(&m_threadId, 0, 0) != 0) return;
//	std::thread{ SticktraceWindow::StaticThreadFunc, this, dialogId }.detach();
//	for (int i = 0; i != 100 && ::InterlockedCompareExchange(&m_threadId, 0, 0) == 0; i++)
//		::Sleep(10);
//}
//----- 20.06.10  削除終 ()-----

void SticktraceWindow::Destroy()
{
	auto threadId = (DWORD)::InterlockedCompareExchange(&m_threadId, 0, 0);
	if (threadId == 0) return;
	auto threadHandle = ::OpenThread(THREAD_ALL_ACCESS, false, threadId);
	::InterlockedExchange(&m_threadTerminateFlag, 1);

#if defined(_DEBUG)
	static constexpr int WAIT_COUNT = 5000;
#else
	static constexpr int WAIT_COUNT = 50;
#endif
	DWORD result = WAIT_TIMEOUT;
	for (int i = 0; i != WAIT_COUNT && result == WAIT_TIMEOUT; i++)
	{
		// Post the WM_USER as a dummy message to work the PumpMessage function.
		if (::IsWindow(m_stickTraceDlg->GetSafeHwnd()))
			m_stickTraceDlg->PostMessage(WM_USER);
		result = ::WaitForSingleObject(threadHandle, 100);
	}
	if (result == WAIT_TIMEOUT)
	{
		::TerminateThread(threadHandle, 0);
		::CloseHandle(threadHandle);
		// delete m_stickTraceDlg はできない。スレッドを強制停止したため。
	}
	m_stickTraceDlg = nullptr;
	::InterlockedExchange(&m_threadId, 0);
	::InterlockedExchange(&m_threadTerminateFlag, 0);
}

bool SticktraceWindow::Show(bool show)
{
	return m_stickTraceDlg->APT_Show(show);
}

bool SticktraceWindow::IsVisible(bool & isVisible)
{
	if (m_stickTraceDlg == nullptr) return false;
	return m_stickTraceDlg->APT_IsVisible(isVisible);
}

bool SticktraceWindow::SetSource(const std::string& sandbox, const std::string& name, const std::string& source)
{
	return m_stickTraceDlg->APT_SetSource(sandbox, name, source);
}

bool SticktraceWindow::IsDebugMode()
{
	return m_stickTraceDlg->APT_IsDebugMode();
}

bool SticktraceWindow::IsBreakpoint(const char* name, int lineIndex)
{
	return m_stickTraceDlg->APT_IsBreakpoint(name, lineIndex);
}

bool SticktraceWindow::OnSuspended()
{
	return m_stickTraceDlg->APT_OnSuspended();
}

bool SticktraceWindow::OnResumed()
{
	return m_stickTraceDlg->APT_OnResumed();
}

bool SticktraceWindow::Jump(const char * name, int lineIndex)
{
	return m_stickTraceDlg->APT_Jump(name, lineIndex);
}

bool SticktraceWindow::NewSession()
{
	return m_stickTraceDlg->APT_NewSession();
}

bool SticktraceWindow::OnStart()
{
	return m_stickTraceDlg->APT_OnStart();
}

bool SticktraceWindow::OnStop()
{
	return m_stickTraceDlg->APT_OnStop();
}

bool SticktraceWindow::OutputError(const char * message)
{
	return m_stickTraceDlg->APT_OutputError(message);
}

bool SticktraceWindow::OutputDebug(const char * message)
{
	return m_stickTraceDlg->APT_OutputDebug(message);
}

bool SticktraceWindow::SetWatch(const std::string& data)
{
	return m_stickTraceDlg->APT_SetWatch(data);
}

bool SticktraceWindow::SetVariableNotify(bool succeeded)
{
	return m_stickTraceDlg->APT_SetVariableNotify(succeeded);
}

SticktraceDef::SuspendCommand SticktraceWindow::WaitCommandIsSet(std::string & paramA, uint32_t waitMilliseconds)
{
	return m_stickTraceDlg->APT_WaitCommandIsSet(paramA, waitMilliseconds);
}
