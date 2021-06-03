
// StickTestDlg.h : ヘッダー ファイル
//

#pragma once

// Do not include "Stickrun.h". It must be included via "Stick.h".

#include <string>
#include "UtilSync.h"			// For AutoCS,AutoLeaveCS.
#include "Sticktrace.h"


class CStickTestDlg;

enum : UINT
{
	WM_USER_COMMAND = WM_USER + 100,
};

/// <summary export="true">
/// kn;Array test.
/// ja;配列テスト。
/// en;Array test.
/// </summary>
/// <param name="v1" io="inout">ja;テスト en;test</param>
extern void MyArrayFunc0(
	std::vector<std::string> & v1
);

/// <stick export="true"/>
/// <summary>
/// kn;
/// <para>Array test2-1.</para>
/// <para>Array test2-2.</para>
/// ja;
/// <para>配列テスト2-1。</para>
/// <para>配列テスト2-2。</para>
/// en;
/// <para>Array test2-1.</para>
/// <para>Array test2-2.</para>
/// </summary>
/// <param name="v1" io="inout">test</param>
extern void MyArrayFunc1(
	std::vector<int> & v1
);

/// <stick export="true"/>
/// <summary>
/// kn;
/// <code>
/// Array test2-1.
/// Array test2-2.
/// </code>
/// ja;
/// <code>
/// 配列テスト2-1。
/// 配列テスト2-2。
/// </code>
/// en;
/// <code>
/// Array test2-1.
/// Array test2-2.
/// </code>
/// </summary>
/// <param name="v1" io="inout">test</param>
extern void MyArrayFunc2(
	std::vector<std::wstring> & v1
);

/// <stick export="true" />
/// <summary>
/// Get map.
/// </summary>
/// <param io="in" name="L">lua_State</param>
extern void RawFunc(lua_State * L);

/// <stick export="true"/>
constexpr int HIJ = 10;

/// <stick export="true" ctype="char*" />
#define XXX "HELLO"

/// <stick export="true"/>
class X
{
public:
	/// <stick export="true"/>
	enum class Enum1
	{
		A,
		B,
	};

	/// <stick export="true"/>
	enum Enum2
	{
		CC,
		DD,
	};

	/// <stick export="true"/>
	static constexpr int EFG = 10;
	// constexpr int HIJ = 10;	NG


	/// <stick export="true"/>
	class A
	{
	public:
		/// <stick export="true"/>
		/// <summary>
		/// Initializes a new instance of the <see cref="A"/> class.
		/// </summary>
		/// <param name="a" io="in">a.</param>
		A(int a) : m_a(a) {}
		
		/// <stick export="true"/>
		/// <summary>
		/// </summary>
		/// <param name="x" io="inout"></param>
		void FuncX(enum class Enum1 & x)
		{}

		/// <stick export="true"/>
		/// <summary>
		/// Gets this instance.
		/// </summary>
		/// <returns></returns>
		int Get() const
		{
			return m_a;
		}

		/// <stick export="true"/>
		/// <summary>
		/// Initializes a new instance of the <see cref="A"/> class.
		/// </summary>
		/// <param name="a" io="out">a.</param>
		/// <returns></returns>
		void GetSet(int * a)
		{
			*a = m_a;
		}

		/// <stick export="true"/>
		/// <summary>
		/// </summary>
		/// <param name="a" io="in">a.</param>
		void Set(int a)
		{
		}

		/// <stick export="true"/>
		/// <summary>
		/// </summary>
		/// <param name="a" io="in">a.</param>
		/// <param name="b" io="in">a.</param>
		void Set(int a, int b)
		{
		}

		/// <stick export="true"/>
		static const char * ABC;

		/// <stick export="true"/>
		static constexpr int EFG = 10;
		// constexpr int HIJ = 10;	NG


		int m_a;
	};
	
	/// <stick export="true"/>
	class B
	{
	public:
		/// <stick export="true" />
		/// <summary>
		/// Add1s the specified a.
		/// </summary>
		/// <param name="a" io="out" ltype="X.A">a.</param>
		int Add1(A * & a)
		{
			std::string xxx = XXX;
			a = new A(5);
			a->m_a = 20;
			return 10;
		}

		/// <stick export="true"/>
		/// <summary>
		/// Add1s the specified a.
		/// </summary>
		/// <param name="a" io="in">a.</param>
		void Add2(class A * a)
		{
			a->m_a++;
		}
		
		/// <stick export="true"/>
		/// <summary>
		/// Creates a.
		/// </summary>
		/// <param name="a" io="in">a.</param>
		/// <returns ltype="X.A"></returns>
		static A* CreateA(int a)
		{
			return new A(a);
		}
		
		/// <stick export="true"/>
		/// <summary>
		/// Deletes a.
		/// </summary>
		/// <param name="a" io="in">a.</param>
		static void DeleteA(A* a)
		{
			delete a;
		}

	};
};


/// <stick export="true"/>
static const X::B* XXBB;

/// <stick export="true"/>
/// <summary>
/// Mies the do.
/// </summary>
/// <param name="text" io="in">The text.</param>
extern void ShowMessage(const wchar_t* text);

/// <stick export="true"/>
/// <summary>
/// Mies the func1.
/// </summary>
/// <param name="wstr1" io="inout">The WSTR1.</param>
/// <param name="wcp2" io="in">The WCP2.</param>
/// <returns></returns>
extern std::wstring MyFunc1(
	std::wstring & wstr1,
	const wchar_t * wcp2
);


inline __int64 defaultX(__int64 a, __int64 b) { return a * b; }

/// <stick export="true"/>
/// <summary>
/// Mies the func1.
/// </summary>
/// <param name="v1" io="inout">The v1.</param>
/// <param name="v2" io="in">The v2.</param>
/// <returns></returns>
extern int MyFunc2(double & v1, __int64 v2 = defaultX(10, 20));

class NM1_NM2
{
public:
	void Func()
	{

	}
};

/// <stick export="true" type="namespace" />
/// <summary>
/// 
/// </summary>
class NM1
{
public:	
	/// <stick export="true"/>
	/// <summary>
	/// Mies the func2.
	/// </summary>
	/// <param name="v1" io="inout">The v1.</param>
	/// <param name="v2" io="in">The v2.</param>
	/// <returns></returns>
	static int MyFunc2(double & v1, __int64 v2);
	
	/// <stick export="true" type="namespace" lname="MMM1" />
	class NM1_NM1
	{
	public:
		/// <stick export="true" ctype="void*" />
		static const int* ARRAY;
		
		/// <stick export="true" />
		/// <summary>
		/// Mies the func0.
		/// </summary>
		/// <param name="array" io="in" ctype="void*">The array.</param>
		static void MyFunc0(const int* array)
		{
			for (int i = 0; i != 3; i++)
			{
				ShowMessage(std::to_wstring(array[i]).c_str());
			}
		}

		/// <stick export="true"/>
		/// <summary>
		/// Mies the func1.
		/// </summary>
		/// <param name="wstr1" io="inout">The WSTR1.</param>
		/// <param name="wcp2" io="in">The WCP2.</param>
		/// <returns></returns>
		static std::wstring MyFunc1(std::wstring & wstr1, const wchar_t * wcp2);
		
		/// <stick export="true" lname="MyFunc2y"/>
		/// <summary>
		/// Mies the func2.
		/// </summary>
		/// <param name="v1" io="inout">The v1.</param>
		/// <param name="v2" io="in">The v2.</param>
		/// <returns></returns>
		static int MyFunc2(double & v1, __int64 v2);

		/// <stick export="true" lname="MyFunc2y"/>
		/// <summary>
		/// Mies the func2.
		/// </summary>
		/// <param name="v1" io="inout">The v1.</param>
		/// <returns></returns>
		static int MyFunc2(double & v1);

		/// <stick export="true" lname="MyFunc2x"/>
		/// <summary>
		/// Mies the func2.
		/// </summary>
		/// <param name="v1" io="inout">The v1.</param>
		/// <returns></returns>
		static int MyFunc2(std::string & v1);

	};

	/// <stick export="true" type="class" />
	class NM1_NM2
	{
	public:		
		/// <stick export="true"/>
		/// <summary>
		/// Initializes a new instance of the <see cref="NM1_NM2"/> class.
		/// </summary>
		NM1_NM2() {}
		
		/// <stick export="true"/>
		/// <summary>
		/// Initializes a new instance of the <see cref="NM1_NM2"/> class.
		/// </summary>
		/// <param name="a" io="inout">a.</param>
		NM1_NM2(int & a)
		{
			a = 500;
		}

		/// <summary>
		/// Finalizes an instance of the <see cref="NM1_NM2"/> class.
		/// </summary>
		~NM1_NM2() {}

		/// <stick export="true"/>
		/// <summary>
		/// Mies the func2.
		/// </summary>
		/// <param name="v1" io="inout">The v1.</param>
		/// <param name="v2" io="in">The v2.</param>
		/// <returns></returns>
		int MyFunc2(double & v1, __int64 v2);
	};

	/// <stick export="true" type="class" super="NM1_NM2" />
	class NM1_NM3 : public NM1_NM2
	{
	public:
		/// <stick export="true"/>
		/// <summary>
		/// Mies the func2.
		/// </summary>
		/// <param name="v1" io="inout">The v1.</param>
		/// <param name="v2" io="in">The v2.</param>
		/// <returns></returns>
		int MyFunc3(double & v1, __int64 v2);

		/// <stick export="true"/>
		/// <summary>
		/// Mies the func2.
		/// </summary>
		/// <param name="v1" io="inout">The v1.</param>
		/// <returns></returns>
		int MyFunc3(double & v1);
	};

};

/// <stick export="true" type="class" />
class C : public ::NM1::NM1_NM3
{
};

/// <stick export="true" />
/// <summary>
/// 
/// </summary>
namespace NM2
{
	/// <stick export="true"/>
	/// <summary>
	/// Mies the func2.
	/// </summary>
	/// <param name="v1" io="inout">The v1.</param>
	/// <param name="v2" io="in">The v2.</param>
	/// <returns></returns>
	int MyFunc2(double & v1, __int64 v2);
};


/// <stick export="true" />
/// <summary>
/// 
/// </summary>
/// <seealso cref="CDialogEx" />
class CStickTestDlg : public CDialogEx
{
// コンストラクション
public:
	CStickTestDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STICKTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

public:	
	/// <summary export="true">
	/// Debugs the output.
	/// </summary>
	/// <param name="message" io="in">The message.</param>
	virtual void DebugOutput(const char* message);

protected:
	virtual void OnSaveScript(const std::string & name, const std::string & code);

// 実装
protected:
	HICON m_hIcon;

	Stickrun* m_stickrun1;
	Sticktrace* m_sticktrace1;
	Stickrun* m_stickrun2;
	Sticktrace* m_sticktrace2;

	/// <summary>
	/// Data for synchronous communication from the application thread to the DlgSticktrace thread.
	/// </summary>
	struct InCmd
	{
		AutoCS cs;	// Locking object to protect this area.
		AutoCV cv;	// Locking object to protect this area.
		SticktraceDef::DebuggerCommand command;
		std::string strParam1;
		std::string strParam2;
		InCmd() : command(SticktraceDef::DebuggerCommand::NONE) {}
		~InCmd() = default;
		void Clear()
		{
			command = SticktraceDef::DebuggerCommand::NONE;
			strParam1.clear();
			strParam2.clear();
		}
	} m_incmd;

	/// <summary>
	/// ja; コールバック関数。デバッガーウィンドウから実行される。
	/// en; Callback function. It is called from the debugger window.
	/// </summary>
	/// <param name="command" io="in">
	/// 	ja; コマンド。コールバックのタイプを指定する。
	/// 	en; Command. It specifies the type of callback.
	/// </param>
	/// <param name="param" io="in">Parameters for the command.</param>
	/// <returns>true:Accepted/false:Timeout</returns>
	virtual bool DGT_DebuggerCallback(
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackParam* param
	);

	static bool DGT_DebuggerCallback(
		unsigned int dialogId,
		SticktraceDef::DebuggerCallbackParam* param,
		void* userData
	);

	static void OnScriptCallback(
		lua_State * luaState,
		lua_Debug * luaDebug,
		void * userData,
		SticktraceDef::Mode mode,
		Sticktrace* sticktrace
	);
	
	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM, LPARAM);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnTest1();
	afx_msg void OnBnClickedBtnTest2();
	afx_msg void OnBnClickedBtnTest3();
	afx_msg void OnBnClickedBtnTest4();
	afx_msg void OnBnClickedBtnTest5();
	afx_msg void OnBnClickedBtnTest6();
	afx_msg void OnBnClickedBtnTest7();
	afx_msg void OnBnClickedBtnShowEditor();
	afx_msg void OnBnClickedBtnHideEditor();
	afx_msg void OnBnClickedBtnStopScript();
	afx_msg void OnBnClickedBtnResumeScript();
	afx_msg void OnBnClickedBtnSuspendScript();
	afx_msg void OnBnClickedBtnNextScript();
	afx_msg void OnBnClickedBtnTest8();
	afx_msg void OnBnClickedBtnInitStickrun();
	afx_msg void OnBnClickedBtnTest9();
	afx_msg LRESULT OnUserCommand(WPARAM, LPARAM);
	virtual void OnOK();
	virtual void OnCancel();
};

/// <stick export="true" />
/// <summary>
/// To the stick test dialog.
/// </summary>
/// <param name="data" io="in">The data.</param>
/// <returns ltype="CStickTestDlg"></returns>
extern CStickTestDlg* ToStickTestDlg(void* data);
