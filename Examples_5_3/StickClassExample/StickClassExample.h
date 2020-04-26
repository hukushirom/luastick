
// StickClassExample.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CStickClassExampleApp:
// このクラスの実装については、StickClassExample.cpp を参照してください
//

/// <stick export="true" type="inconstructible" lname="StickClassExampleApp" super="-" />
/// <summary>
/// Application class.
/// </summary>
class CStickClassExampleApp : public CWinApp
{
public:
	CStickClassExampleApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

	/// <stick export="true"/>
	/// <summary>
	/// Output the message to the message window.
	/// </summary>
	/// <param io="in" name="message">message</param>
	virtual void Output (const wchar_t * message);

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CStickClassExampleApp theApp;

/// <stick export="true"/>
/// <summary>
/// Get application object.
/// </summary>
/// <returns>Application object</returns>
inline CStickClassExampleApp * App() { return &theApp; }
