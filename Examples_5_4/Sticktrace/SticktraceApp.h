// SticktraceApp.h : Sticktrace.DLL のメイン ヘッダー ファイル
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CSticktraceApp
// このクラスの実装に関しては SticktraceWindow.cpp を参照してください。
//

class CSticktraceApp : public CWinApp
{
public:
	CSticktraceApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
