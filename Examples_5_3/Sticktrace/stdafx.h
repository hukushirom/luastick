// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows ヘッダーから使用されていない部分を除外します。
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 一部の CString コンストラクターは明示的です。

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE クラス
#include <afxodlgs.h>       // MFC OLE ダイアログ クラス
#include <afxdisp.h>        // MFC オートメーション クラス
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC データベース クラス
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO データベース クラス
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcontrolbars.h>

// Define _STICKTRACE.
#if !defined(_STICKTRACE)
#define _STICKTRACE
#endif // _STICKTRACE

#include "GlobalDefs.h"
#include <unordered_map>

enum : UINT
{
	WM_USER_DDEDIT_DBLCLKED = WM_USER + 100,
	WM_USER_DDEDIT_SETTEXT_NOTIFY,
	WM_USER_TEXT_EDIT_INPUT_TAB,
	WM_USER_TEXT_EDIT_MARKER_CLICKED,
	WM_USER_TEXT_EDIT_CURLINE_CHANGED,
	WM_USER_COMMAND,
	WM_USER_BREAKPOINT_UPDATED,
};

constexpr auto FD_COLREF_ULTLA_LIGHT_GRAY = RGB(0xe8, 0xe8, 0xe8);

extern double GetDisplayPPM();

extern __int64 GetCurrentMillisecTime();


template<typename S, typename T>
bool IsFlagged(S value, T flag)
{
	return ((value & flag) == flag);
}

