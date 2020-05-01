// SticktraceWindow.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include <thread>
#include "Resource.h"		// For IDD_STICK_TRACE.
#include "DlgSticktrace.h"
#include "Sticktrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class _SticktraceWindow
{
public:
	_SticktraceWindow();
	~_SticktraceWindow();
	void Create();
	void Destroy();
	bool Show(bool show);
	bool SetSource(const std::string& name, const std::string& source);
	bool IsDebugMode();
	bool IsBreakpoint(const char* name, int lineIndex);
//----- 17.10.20 Fukushiro M. 削除始 ()-----
//	bool IsSuspended();
//	int GetMode();
//----- 17.10.20 Fukushiro M. 削除終 ()-----
	bool OnSuspended();
	bool OnResumed();
	bool Jump(const char * name, int lineIndex);
	bool OnStart();
	bool OnStop();
	bool OutputError(const char* message);
	bool OutputDebug(const char* message);
	bool SetWatch(const std::string& data);
	SticktraceCommand GetCommand(std::string & param, uint32_t waitMilliseconds);

	static void StaticThreadFunc(_SticktraceWindow* stickTrace);
	void ThreadFunc();
private:
	CDlgSticktrace* m_stickTraceDlg;
	LONG m_threadId;
	LONG m_threadTerminateFlag;
};

class __declspec(dllexport) SticktraceWindow
{
private:
	static SticktraceWindow* New()
	{
		return new SticktraceWindow();
	}

	static void Delete(SticktraceWindow* mtw)
	{
		delete mtw;
	}

	SticktraceWindow()
		: m_stickTraceWindow(new _SticktraceWindow{})
	{}

	~SticktraceWindow()
	{
		delete m_stickTraceWindow;
		m_stickTraceWindow = nullptr;
	}

	bool Show(bool show)
	{
		return m_stickTraceWindow->Show(show);
	}

	bool SetSource(const std::string& name, const std::string& source)
	{
		return m_stickTraceWindow->SetSource(name, source);
	}

	bool IsDebugMode()
	{
		return m_stickTraceWindow->IsDebugMode();
	}

	bool IsBreakpoint(const char* name, int lineIndex)
	{
		return m_stickTraceWindow->IsBreakpoint(name, lineIndex);
	}
	//----- 17.10.20 Fukushiro M. 削除始 ()-----
//	bool IsSuspended()
//	{
//		return m_stickTraceWindow->IsSuspended();
//	}
//
//	int GetMode()
//	{
//		return m_stickTraceWindow->GetMode();
//	}
//----- 17.10.20 Fukushiro M. 削除終 ()-----

	bool OnSuspended()
	{
		return m_stickTraceWindow->OnSuspended();
	}

	bool OnResumed()
	{
		return m_stickTraceWindow->OnResumed();
	}

	bool Jump(const char * name, int lineIndex)
	{
		return m_stickTraceWindow->Jump(name, lineIndex);
	}

	bool SticktraceWindow::OnStart()
	{
		return m_stickTraceWindow->OnStart();
	}

	bool SticktraceWindow::OnStop()
	{
		return m_stickTraceWindow->OnStop();
	}

	bool OutputError(const char* message)
	{
		return m_stickTraceWindow->OutputError(message);
	}

	bool OutputDebug(const char* message)
	{
		return m_stickTraceWindow->OutputDebug(message);
	}

	bool SetWatch(const std::string& data)
	{
		return m_stickTraceWindow->SetWatch(data);
	}
	
	SticktraceCommand GetCommand(std::string & param, uint32_t waitMilliseconds)
	{
		return m_stickTraceWindow->GetCommand(param, waitMilliseconds);
	}


private:
	_SticktraceWindow* m_stickTraceWindow;
};


_SticktraceWindow::_SticktraceWindow()
	:m_stickTraceDlg(nullptr)
	,m_threadId(0)
	,m_threadTerminateFlag(0)
{
	Create();
}

_SticktraceWindow::~_SticktraceWindow()
{
	Destroy();
}

void _SticktraceWindow::StaticThreadFunc(_SticktraceWindow* stickTrace)
{
	stickTrace->ThreadFunc();
}

void _SticktraceWindow::ThreadFunc()
{
	m_stickTraceDlg = new CDlgSticktrace();
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

void _SticktraceWindow::Create()
{
	if (::InterlockedCompareExchange(&m_threadId, 0, 0) != 0) return;
	std::thread{ _SticktraceWindow::StaticThreadFunc, this }.detach();
	for (int i = 0; i != 100 && ::InterlockedCompareExchange(&m_threadId, 0, 0) == 0; i++)
		::Sleep(10);
}

void _SticktraceWindow::Destroy()
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

bool _SticktraceWindow::Show(bool show)
{
	auto threadId = (DWORD)::InterlockedCompareExchange(&m_threadId, 0, 0);
	if (threadId == 0) return false;
	if (show)
	{
		if (::IsWindow(m_stickTraceDlg->GetSafeHwnd()) && !m_stickTraceDlg->IsWindowVisible())
			m_stickTraceDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		if (::IsWindow(m_stickTraceDlg->GetSafeHwnd()) && m_stickTraceDlg->IsWindowVisible())
			m_stickTraceDlg->ShowWindow(SW_HIDE);
	}
	return true;
}

bool _SticktraceWindow::SetSource(const std::string& name, const std::string& source)
{
	return m_stickTraceDlg->TC_SetSource(name, source);
	// m_stickTraceDlg->PostMessage(WM_USER_COMMAND);
}

bool _SticktraceWindow::IsDebugMode()
{
	return m_stickTraceDlg->TC_IsDebugMode();
}

bool _SticktraceWindow::IsBreakpoint(const char* name, int lineIndex)
{
	return m_stickTraceDlg->TC_IsBreakpoint(name, lineIndex);
}

//----- 17.10.20 Fukushiro M. 削除始 ()-----
//bool _SticktraceWindow::IsSuspended()
//{
//	return m_stickTraceDlg->TC_IsSuspended();
//}
//
//int _SticktraceWindow::GetMode()
//{
//	return m_stickTraceDlg->TC_GetMode();
//}
//----- 17.10.20 Fukushiro M. 削除終 ()-----

bool _SticktraceWindow::OnSuspended()
{
	return m_stickTraceDlg->TC_OnSuspended();
}

bool _SticktraceWindow::OnResumed()
{
	return m_stickTraceDlg->TC_OnResumed();
}

bool _SticktraceWindow::Jump(const char * name, int lineIndex)
{
	return m_stickTraceDlg->TC_Jump(name, lineIndex);
}

bool _SticktraceWindow::OnStart()
{
	return m_stickTraceDlg->TC_OnStart();
}

bool _SticktraceWindow::OnStop()
{
	return m_stickTraceDlg->TC_OnStop();
}

bool _SticktraceWindow::OutputError(const char * message)
{
	return m_stickTraceDlg->TC_OutputError(message);
}

bool _SticktraceWindow::OutputDebug(const char * message)
{
	return m_stickTraceDlg->TC_OutputDebug(message);
}

bool _SticktraceWindow::SetWatch(const std::string& data)
{
	return m_stickTraceDlg->TC_SetWatch(data);
}


SticktraceCommand _SticktraceWindow::GetCommand(std::string & param, uint32_t waitMilliseconds)
{
	return m_stickTraceDlg->TC_GetCommand(param, waitMilliseconds);
}

