#include "stdafx.h"
#include "LeException.h"
#include "UtilStr.h"

//----- 17.09.28 Fukushiro M. 削除始 ()-----
//#if 0
//
//#include "resource.h"			// For IDS_XXX.
//#include "DynamicDraw.h"		// For FFApp().
////----------<#DD VIEWER NOT EXPORT>-----------
//#include <algorithm>			// For lower_bound template.
//#include <time.h>				// For time().
//#include "UtilMath.h"			// For hix2histr.etc.
//#include "UtilStr.h"			// For GetResString.FFExtractUntil.
//#include "UtilErr.h"			// For CFCMiscException.
//#include "UtilMisc.h"			// For FFGetCurrentTime.
//#include "UtilStdLib.h"			// For m_const_for.
//#include "FrameWnd.h"			// For FFMainWnd().
//#include "DdComboBox.h"			// For CFCDdComboBox.
////----------</#DD VIEWER NOT EXPORT>-----------
//#include "UtilWin.h"			// For ExecAndWaitApp.
//#include "RegData.h"			// For FFReg()->GetIsLeftHand().
//#include "DynamicDrawView.h"	// For FFView().
//#include "UtilCoord.h"			// For mm2lg.etc.
//
//#endif
//----- 17.09.28 Fukushiro M. 削除終 ()-----

#include "UtilDlg.h"			// This header.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0

//----------<#DD VIEWER NOT EXPORT>-----------
typedef std::map<wchar_t, wchar_t> FTTcTcMap;

static void MyInitHandsTable (FTTcTcMap& mpLeft2Right, FTTcTcMap& mpRight2Left, astring& astrBuff, const wchar_t* tcpPath, long& lCurLine);
static void MyInitUserAccelerator (std::vector<ACCEL>& vAccel, astring& strBuff, const char* cpEndTag, const wchar_t* tcpPath, long& lCurLine, const DWORDSet& stCommandId);

static FTTcTcMap s_mpLeft2Right;	// s_mpLeft2Right[左利き文字] = 右利き文字。
static FTTcTcMap s_mpRight2Left;	// s_mpRight2Left[右利き文字] = 左利き文字。

static std::vector<ACCEL> s_vAccelL;		// 左利き用のショートカットキー設定。
static std::vector<ACCEL> s_vAccelR;		// 右利き用のショートカットキー設定。

//----- 14.09.12 Fukushiro M. 追加始 ()-----
// スクリプトエディター用。
static std::vector<ACCEL> s_vAccelScL;		// 左利き用のショートカットキー設定。
static std::vector<ACCEL> s_vAccelScR;		// 右利き用のショートカットキー設定。
//----- 14.09.12 Fukushiro M. 追加終 ()-----

static std::map<astring, DWORD> s_mpCommandStringToId;	// コマンド文字列->コマンドID変換テーブル。
static std::map<DWORD, astring> s_mpCommandIdToString;	// コマンドID->コマンド文字列変換テーブル。
static std::map<DWORD, WString> s_mpCommandIdToTitle;	// コマンドID->コマンドタイトル変換テーブル。
// 11.11.08 Fukushiro M. 1行追加 ()
static std::map<astring, astring> s_mpOldCommandToNew;	// 旧コマンド文字列->新コマンド文字列。

//----- 11.11.08 Fukushiro M. 追加始 ()-----
//! 旧コマンド名->新コマンド名
//! 要素数を空にしないこと。初期化時にチェックしているため。
static const struct { const char* cpOldName; const char* cpNewName; } COMMAND_OLD_TO_NEW[] =
{
	{ "OPTION_IS_SNAP_EFFECTIVE",   "OPTION_NEXT_SNAP_METHOD" },
}; // COMMAND_OLD_TO_NEW[]
//----- 11.11.08 Fukushiro M. 追加終 ()-----

/*************************************************************************
 * <データ>	COMMAND_NAME_TO_ID
 *
 * <機能>	コマンド文字列とコマンドIDの対応表。
 *
 * <履歴>	09.06.16 Fukushiro M. 作成
 *************************************************************************/
static const struct { const char* cpName; DWORD dwId; } COMMAND_NAME_TO_ID[] =
{
	{ "APP_ABOUT",                               ID_APP_ABOUT },
	{ "APP_EXIT",                                ID_APP_EXIT },
	{ "APP_WIN_EXIT",                            ID_APP_WIN_EXIT },
	{ "ARROWEDIT_PROPERTY",                      ID_ARROWEDIT_PROPERTY },
	{ "ARROWEDIT_SET_DUTY",                      ID_ARROWEDIT_SET_DUTY },
	{ "ARROWEDIT_SET_FRAME",                     ID_ARROWEDIT_SET_FRAME },
// 09.06.13 Fukushiro M. 1行削除 ()
//		{ "BTN_OBJECT_INFO",                         ID_BTN_OBJECT_INFO },
	{ "CANCEL_EDIT_SRVR",                        ID_CANCEL_EDIT_SRVR },
	{ "CHIPEDIT_MODE_COMPOSITE_CHIP",            ID_CHIPEDIT_MODE_COMPOSITE_CHIP },
	{ "CHIPEDIT_MODE_OBJECT_CHIP",               ID_CHIPEDIT_MODE_OBJECT_CHIP },
	{ "CHIPEDIT_PROPERTY",                       ID_CHIPEDIT_PROPERTY },
	{ "CHIPEDIT_SET_DUTY",                       ID_CHIPEDIT_SET_DUTY },
	{ "CMB_BOTH_CODE_FONT_COLOR",                ID_CMB_BOTH_CODE_FONT_COLOR },
	{ "CMB_BOTH_CODE_FONT_NAME",                 ID_CMB_BOTH_CODE_FONT_NAME },
	{ "CMB_BOTH_CODE_FONT_SIZE",                 ID_CMB_BOTH_CODE_FONT_SIZE },
	{ "CMB_CHIPEDIT_MOLIP_DUTY_ID",              ID_CMB_CHIPEDIT_MOLIP_DUTY_ID },
	{ "CMB_CHIPEDIT_TEXT_DUTY_ID",               ID_CMB_CHIPEDIT_TEXT_DUTY_ID },
	{ "CMB_CURRENT_ARROW_SHEET",                 ID_CMB_CURRENT_ARROW_SHEET },
	{ "CMB_CURRENT_LAYER",                       ID_CMB_CURRENT_LAYER },
	{ "CMB_CURRENT_SHEET",                       ID_CMB_CURRENT_SHEET },
	{ "CMB_HTREE_UP_DOWN_KIND",                  ID_CMB_HTREE_UP_DOWN_KIND },
	{ "CMB_HTREE_ZOOM",                          ID_CMB_HTREE_ZOOM },
	{ "CMB_LABEL_LIST",                          ID_CMB_LABEL_LIST },
	{ "CMB_OBJ_BRUSH_COLOR",                     ID_CMB_OBJ_BRUSH_COLOR },
//----- 07.06.09 Fukushiro M. 追加始 ()-----
	{ "CMB_OBJ_PARA_ARROW_B",                    ID_CMB_OBJ_PARA_ARROW_B },
	{ "CMB_OBJ_PARA_ARROW_F",                    ID_CMB_OBJ_PARA_ARROW_F },
//----- 07.06.09 Fukushiro M. 追加終 ()-----
	{ "CMB_OBJ_PEN_COLOR",                       ID_CMB_OBJ_PEN_COLOR },
	{ "CMB_OBJ_PEN_STYLE",                       ID_CMB_OBJ_PEN_STYLE },
	{ "CMB_OBJ_PEN_WEIGHT",                      ID_CMB_OBJ_PEN_WEIGHT },
	{ "CMB_OBJ_REG_BRUSH",                       ID_CMB_OBJ_REG_BRUSH },
	{ "CMB_OBJ_REG_PEN",                         ID_CMB_OBJ_REG_PEN },
	{ "CMB_STORE_CHIP_GROUP",                    ID_CMB_STORE_CHIP_GROUP },
	{ "CMB_VIEW_ZOOM",                           ID_CMB_VIEW_ZOOM },
// 11.11.05 Fukushiro M. 1行追加 ()
	{ "CMB_CURRENT_SNAP",                        ID_CMB_CURRENT_SNAP },
	{ "CONTEXT_HELP",                            ID_CONTEXT_HELP },
// 07.03.19 Fukushiro M. 1行追加 ()
	{ "EDIT_ADD_SELECTION_MEMORY",               ID_EDIT_ADD_SELECTION_MEMORY },
	{ "EDIT_BACKSPACE",                          ID_EDIT_BACKSPACE },
	{ "EDIT_CLEAR_SELECT",                       ID_EDIT_CLEAR_SELECT },
// 07.03.19 Fukushiro M. 1行追加 ()
	{ "EDIT_CLEAR_SELECTION_MEMORY",             ID_EDIT_CLEAR_SELECTION_MEMORY },
	{ "EDIT_COPY",                               ID_EDIT_COPY },
	{ "EDIT_COPY_TEXT",                          ID_EDIT_COPY_TEXT },
	{ "EDIT_CUT",                                ID_EDIT_CUT },
	{ "EDIT_DELETE",                             ID_EDIT_DELETE },
	{ "EDIT_EXCHANGE_HEAD_TAIL",                 ID_EDIT_EXCHANGE_HEAD_TAIL },
	{ "EDIT_FIND_NEXT_TEXT",                     ID_EDIT_FIND_NEXT_TEXT },
	{ "EDIT_FIND_PREV_TEXT",                     ID_EDIT_FIND_PREV_TEXT },
	{ "EDIT_FIND_TEXT",                          ID_EDIT_FIND_TEXT },
	{ "EDIT_FIND_TEXT_ALL",                      ID_EDIT_FIND_TEXT_ALL },
	{ "EDIT_FIND_TEXT_ALL_IN_SELECT",            ID_EDIT_FIND_TEXT_ALL_IN_SELECT },
// 07.07.14 Fukushiro M. 1行追加 ()
	{ "EDIT_INSERT_FROM_IMAGE_FILE",             ID_EDIT_INSERT_FROM_IMAGE_FILE },
	{ "EDIT_PASTE",                              ID_EDIT_PASTE },
	{ "EDIT_PASTE_OVER_AS",                      ID_EDIT_PASTE_OVER_AS },
	{ "EDIT_PASTE_SPECIAL",                      ID_EDIT_PASTE_SPECIAL },
	{ "EDIT_REDO",                               ID_EDIT_REDO },
	{ "EDIT_REPLACE_NEXT_TEXT",                  ID_EDIT_REPLACE_NEXT_TEXT },
	{ "EDIT_REPLACE_PREV_TEXT",                  ID_EDIT_REPLACE_PREV_TEXT },
	{ "EDIT_REPLACE_TEXT",                       ID_EDIT_REPLACE_TEXT },
	{ "EDIT_REPLACE_TEXT_ALL",                   ID_EDIT_REPLACE_TEXT_ALL },
	{ "EDIT_REPLACE_TEXT_ALL_IN_SELECT",         ID_EDIT_REPLACE_TEXT_ALL_IN_SELECT },
	{ "EDIT_SELECT_ALL",                         ID_EDIT_SELECT_ALL },
	{ "EDIT_SELECT_NEXT",                        ID_EDIT_SELECT_NEXT },
	{ "EDIT_SELECT_SAME_KIND_OF_OBJECT",         ID_EDIT_SELECT_SAME_KIND_OF_OBJECT },
//----- 07.03.19 Fukushiro M. 追加始 ()-----
	{ "EDIT_SELECT_SELECTION_MEMORY",            ID_EDIT_SELECT_SELECTION_MEMORY },
	{ "EDIT_SET_SELECTION_MEMORY",               ID_EDIT_SET_SELECTION_MEMORY },
//----- 07.03.19 Fukushiro M. 追加終 ()-----
	{ "EDIT_UNDO",                               ID_EDIT_UNDO },
	{ "FILE_CLOSE_HIDOCUMENT",                   ID_FILE_CLOSE_HIDOCUMENT },
	{ "FILE_CLOSE_OPEN",                         ID_FILE_CLOSE_OPEN },
	{ "FILE_COMEDT_NO_SAVE_EXIT",                ID_FILE_COMEDT_NO_SAVE_EXIT },
	{ "FILE_COMEDT_SAVE_EXIT",                   ID_FILE_COMEDT_SAVE_EXIT },
	{ "FILE_DELETE_HISTORY",                     ID_FILE_DELETE_HISTORY },
	{ "FILE_DOCUMENT_PROPERTY",                  ID_FILE_DOCUMENT_PROPERTY },
	{ "FILE_EXEC_HISTDRAW",                      ID_FILE_EXEC_HISTDRAW },
	{ "FILE_EXEC_HISTDRAW_TARGET",               ID_FILE_EXEC_HISTDRAW_TARGET },
	{ "FILE_EXEC_HISTVIEWER",                    ID_FILE_EXEC_HISTVIEWER },
	{ "FILE_EXEC_HISTVIEWER_TARGET",             ID_FILE_EXEC_HISTVIEWER_TARGET },
	{ "FILE_EXEC_MOLIPDRAW",                     ID_FILE_EXEC_MOLIPDRAW },
	{ "FILE_EXPORT",                             ID_FILE_EXPORT },
	{ "FILE_FORK_HISTORY",                       ID_FILE_FORK_HISTORY },
	{ "FILE_IMPORT",                             ID_FILE_IMPORT },
	{ "FILE_IMPORT_HIDOCUMENT",                  ID_FILE_IMPORT_HIDOCUMENT },
	{ "FILE_JOIN_PROJECT",                       ID_FILE_JOIN_PROJECT },
	{ "FILE_NEW_HIDOCUMENT",                     ID_FILE_NEW_HIDOCUMENT },
	{ "FILE_OPEN",                               ID_FILE_OPEN },
	{ "FILE_OPEN_HIDOCUMENT",                    ID_FILE_OPEN_HIDOCUMENT },
	{ "FILE_PAGE_SETUP",                         ID_FILE_PAGE_SETUP },
	{ "FILE_PRINT",                              ID_FILE_PRINT },
	{ "FILE_PRINT_PREVIEW",                      ID_FILE_PRINT_PREVIEW },
	{ "FILE_PRINT_SETUP",                        ID_FILE_PRINT_SETUP },
	{ "FILE_PROJECT_CLOSE",                      ID_FILE_PROJECT_CLOSE },
	{ "FILE_PROJECT_NEW",                        ID_FILE_PROJECT_NEW },
	{ "FILE_PROJECT_OPEN",                       ID_FILE_PROJECT_OPEN },
	{ "FILE_RECENT1",                            ID_FILE_RECENT1 },
	{ "FILE_SAVE",                               ID_FILE_SAVE },
	{ "FILE_SAVE_AS",                            ID_FILE_SAVE_AS },
	{ "FILE_SAVE_COPY_AS",                       ID_FILE_SAVE_COPY_AS },
	{ "FILE_SAVE_HIDOCUMENT",                    ID_FILE_SAVE_HIDOCUMENT },
	{ "FILE_SAVE_SELECT_AS",                     ID_FILE_SAVE_SELECT_AS },
	{ "FILE_SHEET_SETUP",                        ID_FILE_SHEET_SETUP },
	{ "FILE_UPDATE",                             ID_FILE_UPDATE },
	{ "FILE_WIN_NEW",                            ID_FILE_WIN_NEW },
	{ "FORMAT_FONT",                             ID_FORMAT_FONT },
	{ "GRAPHICS_CHIP_COPY",                      ID_GRAPHICS_CHIP_COPY },
// 06.10.31 Fukushiro M. 1行追加 ()
	{ "GRAPHICS_CHIP_PASTE",                     ID_GRAPHICS_CHIP_PASTE },
	{ "GRAPHICS_EDIT_CHIP",                      ID_GRAPHICS_EDIT_CHIP },
	{ "GRAPHICS_EDIT_CHIP_GROUP",                ID_GRAPHICS_EDIT_CHIP_GROUP },
	{ "GRAPHICS_EDIT_COLOR_TABLE",               ID_GRAPHICS_EDIT_COLOR_TABLE },
	{ "GRAPHICS_EDIT_PICT_ARROW",                ID_GRAPHICS_EDIT_PICT_ARROW },
	{ "GRAPHICS_EDIT_PICTURE_TABLE",             ID_GRAPHICS_EDIT_PICTURE_TABLE },
	{ "GRAPHICS_EDIT_REGBRUSH_TABLE",            ID_GRAPHICS_EDIT_REGBRUSH_TABLE },
	{ "GRAPHICS_EDIT_REGPEN_TABLE",              ID_GRAPHICS_EDIT_REGPEN_TABLE },
	{ "GRAPHICS_REGISTER_CHIP_FROM_FILE",        ID_GRAPHICS_REGISTER_CHIP_FROM_FILE },
	{ "GRAPHICS_SHADOW_LENGTH",                  ID_GRAPHICS_SHADOW_LENGTH },
	{ "GRAPHICS_UNSELECT_CHIP",                  ID_GRAPHICS_UNSELECT_CHIP },
	{ "HELP",                                    ID_HELP },
	{ "HELP_FINDER",                             ID_HELP_FINDER },
// 10.03.05 Fukushiro M. 1行追加 ()
	{ "HELP_DOCUMENT_LINK1",                     ID_HELP_DOCUMENT_LINK1 },
	{ "HELP_TUTORIAL",                           ID_HELP_TUTORIAL },
	{ "HELP_MOLIP_HOMEPAGE",                     ID_HELP_MOLIP_HOMEPAGE },
	{ "HTREE_BRANCH_VIEW",                       ID_HTREE_BRANCH_VIEW },
	{ "HTREE_CREATE_TIME_INDEX",                 ID_HTREE_CREATE_TIME_INDEX },
	{ "HTREE_FIND_PANEL",                        ID_HTREE_FIND_PANEL },
	{ "HTREE_FIND_RESULTS",                      ID_HTREE_FIND_RESULTS },
	{ "HTREE_FONT_LARGE",                        ID_HTREE_FONT_LARGE },
	{ "HTREE_FONT_MEDIUM",                       ID_HTREE_FONT_MEDIUM },
	{ "HTREE_FONT_SMALL",                        ID_HTREE_FONT_SMALL },
	{ "HTREE_JUMP_CURRENT_FIND_RESULT",          ID_HTREE_JUMP_CURRENT_FIND_RESULT },
	{ "HTREE_JUMP_NEXT_FIND_RESULT",             ID_HTREE_JUMP_NEXT_FIND_RESULT },
	{ "HTREE_JUMP_PREV_FIND_RESULT",             ID_HTREE_JUMP_PREV_FIND_RESULT },
	{ "HTREE_MOVE_AFTER",                        ID_HTREE_MOVE_AFTER },
	{ "HTREE_MOVE_BEFORE",                       ID_HTREE_MOVE_BEFORE },
	{ "HTREE_MOVE_HIGHER",                       ID_HTREE_MOVE_HIGHER },
	{ "HTREE_MOVE_LOWER",                        ID_HTREE_MOVE_LOWER },
// 06.03.12 Fukushiro M. 1行削除 ()
//		{ "HTREE_MOVE_LTT_EACH",                     ID_HTREE_MOVE_LTT_EACH },
	{ "HTREE_MOVE_OPERATION_AFTER",              ID_HTREE_MOVE_OPERATION_AFTER },
	{ "HTREE_MOVE_OPERATION_BEFORE",             ID_HTREE_MOVE_OPERATION_BEFORE },
// 06.03.12 Fukushiro M. 1行削除 ()
//		{ "HTREE_MOVE_OPERATION_EACH",               ID_HTREE_MOVE_OPERATION_EACH },
	{ "HTREE_MOVE_SELECTION_AFTER",              ID_HTREE_MOVE_SELECTION_AFTER },
	{ "HTREE_MOVE_SELECTION_BEFORE",             ID_HTREE_MOVE_SELECTION_BEFORE },
// 06.03.12 Fukushiro M. 1行削除 ()
//		{ "HTREE_MOVE_TIME_EACH",                    ID_HTREE_MOVE_TIME_EACH },
	{ "HTREE_PROJECT_VIEW",                      ID_HTREE_PROJECT_VIEW },
	{ "HTREE_SELECT_MOVE_OPERATION",             ID_HTREE_SELECT_MOVE_OPERATION },
	{ "HTREE_TREE_VIEW",                         ID_HTREE_TREE_VIEW },
	{ "HTREE_USE_TIME_INDEX",                    ID_HTREE_USE_TIME_INDEX },

// 08.10.02 Fukushiro M. 1行追加 ()
	{ "LAYOUT_REARRANGE",                        ID_LAYOUT_REARRANGE },
//----- 07.03.27 Fukushiro M. 追加始 ()-----
	{ "LAYOUT_ARRANGEMENT_SETUP",                ID_LAYOUT_ARRANGEMENT_SETUP },
	{ "LAYOUT_SELECT",                           ID_LAYOUT_SELECT },
//----- 07.03.27 Fukushiro M. 追加終 ()-----

	{ "OBJECT_ALIGN_H_BOTTOM",                   ID_OBJECT_ALIGN_H_BOTTOM },
	{ "OBJECT_ALIGN_H_CENTER",                   ID_OBJECT_ALIGN_H_CENTER },
	{ "OBJECT_ALIGN_H_TOP",                      ID_OBJECT_ALIGN_H_TOP },
	{ "OBJECT_ALIGN_V_CENTER",                   ID_OBJECT_ALIGN_V_CENTER },
	{ "OBJECT_ALIGN_V_LEFT",                     ID_OBJECT_ALIGN_V_LEFT },
	{ "OBJECT_ALIGN_V_RIGHT",                    ID_OBJECT_ALIGN_V_RIGHT },
// 07.02.15 Fukushiro M. 1行追加 ()
	{ "OBJECT_ARROW_PROPERTY",                   ID_OBJECT_ARROW_PROPERTY },
	{ "OBJECT_BACK_OBJ",                         ID_OBJECT_BACK_OBJ },
// 07.02.15 Fukushiro M. 1行追加 ()
	{ "OBJECT_BRUSH_PROPERTY",                   ID_OBJECT_BRUSH_PROPERTY },
	{ "OBJECT_CONNECT",                          ID_OBJECT_CONNECT },
	{ "OBJECT_CONVERT_TO_POLY",                  ID_OBJECT_CONVERT_TO_POLY },
	{ "OBJECT_CROSS_SHEET",                      ID_OBJECT_CROSS_SHEET },
	{ "OBJECT_DISCONNECT",                       ID_OBJECT_DISCONNECT },
	{ "OBJECT_FRONT_OBJ",                        ID_OBJECT_FRONT_OBJ },
	{ "OBJECT_GROUP",                            ID_OBJECT_GROUP },
	{ "OBJECT_INSERT_JOINT",                     ID_OBJECT_INSERT_JOINT },
	{ "OBJECT_MIRROR_HORZ",                      ID_OBJECT_MIRROR_HORZ },
	{ "OBJECT_MIRROR_VERT",                      ID_OBJECT_MIRROR_VERT },
	{ "OBJECT_MOST_BACK_OBJ",                    ID_OBJECT_MOST_BACK_OBJ },
	{ "OBJECT_MOST_FRONT_OBJ",                   ID_OBJECT_MOST_FRONT_OBJ },
//----- 14.02.04 Fukushiro M. 変更前 ()-----
//	{ "OBJECT_MOVE_RESIZE",                      ID_OBJECT_MOVE_RESIZE },
//----- 14.02.04 Fukushiro M. 変更後 ()-----
	{ "OBJECT_MOVE",							ID_OBJECT_MOVE },
	{ "OBJECT_RESIZE",							ID_OBJECT_RESIZE },
//----- 14.02.04 Fukushiro M. 変更終 ()-----
	{ "OBJECT_MOVE_SNAP_DOWN",                   ID_OBJECT_MOVE_SNAP_DOWN },
	{ "OBJECT_MOVE_SNAP_LEFT",                   ID_OBJECT_MOVE_SNAP_LEFT },
	{ "OBJECT_MOVE_SNAP_RIGHT",                  ID_OBJECT_MOVE_SNAP_RIGHT },
	{ "OBJECT_MOVE_SNAP_UP",                     ID_OBJECT_MOVE_SNAP_UP },
// 07.02.15 Fukushiro M. 1行追加 ()
	{ "OBJECT_PEN_PROPERTY",                     ID_OBJECT_PEN_PROPERTY },
	{ "OBJECT_PROPERTIES",                       ID_OBJECT_PROPERTIES },
	{ "OBJECT_PUT_RTF_OBJ",                      ID_OBJECT_PUT_RTF_OBJ },
	{ "OBJECT_PUT_TIME_LABEL",                   ID_OBJECT_PUT_TIME_LABEL },
	{ "OBJECT_ROTATE",                           ID_OBJECT_ROTATE },
	{ "OBJECT_ROTATE_LEFT",                      ID_OBJECT_ROTATE_LEFT },
	{ "OBJECT_ROTATE_RIGHT",                     ID_OBJECT_ROTATE_RIGHT },
	{ "OBJECT_SAME_DIST_H",                      ID_OBJECT_SAME_DIST_H },
	{ "OBJECT_SAME_DIST_V",                      ID_OBJECT_SAME_DIST_V },
	{ "OBJECT_SAME_GAP_H",                       ID_OBJECT_SAME_GAP_H },
	{ "OBJECT_SAME_GAP_V",                       ID_OBJECT_SAME_GAP_V },
	{ "OBJECT_SAME_SIZE_X",                      ID_OBJECT_SAME_SIZE_X },
	{ "OBJECT_SAME_SIZE_Y",                      ID_OBJECT_SAME_SIZE_Y },
// 07.08.19 Fukushiro M. 1行追加 ()
	{ "OBJECT_SIZE_TO_PICTURE",                  ID_OBJECT_SIZE_TO_PICTURE },
	{ "OBJECT_SIZE_TO_TEXT",                     ID_OBJECT_SIZE_TO_TEXT },
	{ "OBJECT_TABLE_ADD_COLUMN",                 ID_OBJECT_TABLE_ADD_COLUMN },
	{ "OBJECT_TABLE_ADD_LINE",                   ID_OBJECT_TABLE_ADD_LINE },
	{ "OBJECT_TABLE_DELETE_COLUMN",              ID_OBJECT_TABLE_DELETE_COLUMN },
	{ "OBJECT_TABLE_DELETE_LINE",                ID_OBJECT_TABLE_DELETE_LINE },
	{ "OBJECT_TABLE_INSERT_COLUMN",              ID_OBJECT_TABLE_INSERT_COLUMN },
	{ "OBJECT_TABLE_INSERT_LINE",                ID_OBJECT_TABLE_INSERT_LINE },
	{ "OBJECT_TABLE_MERGE_CELL",                 ID_OBJECT_TABLE_MERGE_CELL },
	{ "OBJECT_TABLE_SPLIT_CELL",                 ID_OBJECT_TABLE_SPLIT_CELL },
	{ "OBJECT_TIME_LABEL_PROPERTY",              ID_OBJECT_TIME_LABEL_PROPERTY },
	{ "OBJECT_UNGROUP",                          ID_OBJECT_UNGROUP },
	{ "OLE_INSERT_NEW",                          ID_OLE_INSERT_NEW },
	{ "OPTION_AUTO_ALTKEY",                      ID_OPTION_AUTO_ALTKEY },
	{ "OPTION_ENV",                              ID_OPTION_ENV },
	{ "OPTION_EXPERT_MODE",                      ID_OPTION_EXPERT_MODE },
	{ "OPTION_GRID_SNAP_SETUP",                  ID_OPTION_GRID_SNAP_SETUP },
	{ "OPTION_HISTORY_MOVE_OPE_LIST",            ID_OPTION_HISTORY_MOVE_OPE_LIST },
// 06.10.11 Fukushiro M. 1行追加 ()
	{ "OPTION_IS_ARROW_GAP_EFFECTIVE",           ID_OPTION_IS_ARROW_GAP_EFFECTIVE },
	{ "OPTION_IS_GRID_VISIBLE",                  ID_OPTION_IS_GRID_VISIBLE },
	{ "OPTION_IS_PAGE_VISIBLE",                  ID_OPTION_IS_PAGE_VISIBLE },
// 11.11.08 Fukushiro M. 1行削除 ()
//	{ "OPTION_IS_SNAP_EFFECTIVE",                ID_OPTION_IS_SNAP_EFFECTIVE },
	{ "OPTION_LAYER_ORDER",                      ID_OPTION_LAYER_ORDER },
	{ "OPTION_LEFT_HANDED",                      ID_OPTION_LEFT_HANDED },
//----- 06.10.31 Fukushiro M. 追加始 ()-----
	{ "OPTION_MOVE_IN_LOWER_LAYER",              ID_OPTION_MOVE_IN_LOWER_LAYER },
	{ "OPTION_MOVE_IN_NEXT_SHEET",               ID_OPTION_MOVE_IN_NEXT_SHEET },
	{ "OPTION_MOVE_IN_PREV_SHEET",               ID_OPTION_MOVE_IN_PREV_SHEET },
	{ "OPTION_MOVE_IN_UPPER_LAYER",              ID_OPTION_MOVE_IN_UPPER_LAYER },
//----- 06.10.31 Fukushiro M. 追加終 ()-----
	{ "OPTION_PAGE_MARGIN_VISIBLE",              ID_OPTION_PAGE_MARGIN_VISIBLE },
	{ "OPTION_POP_HINT_ENABLE",                  ID_OPTION_POP_HINT_ENABLE },
	{ "OPTION_RIGHT_HANDED",                     ID_OPTION_RIGHT_HANDED },
	{ "OPTION_SHEET_ORDER",                      ID_OPTION_SHEET_ORDER },
// 09.06.12 Fukushiro M. 1行追加 ()
	{ "OPTION_TOOLBAR_ENV",                      ID_OPTION_TOOLBAR_ENV },

//----- 11.11.06 Fukushiro M. 追加始 ()-----
	{ "OPTION_SNAP_TO_GRID",                     ID_OPTION_SNAP_TO_GRID },
	{ "OPTION_SNAP_TO_OBJECT",                   ID_OPTION_SNAP_TO_OBJECT },
	{ "OPTION_SNAP_OFF",                         ID_OPTION_SNAP_OFF },
	{ "OPTION_NEXT_SNAP_METHOD",                 ID_OPTION_NEXT_SNAP_METHOD },
//----- 11.11.06 Fukushiro M. 追加終 ()-----
// 14.08.15 Fukushiro M. 1行追加 ()
	{ "OPTION_SCRIPT_EDIT",                      ID_OPTION_SCRIPT_EDIT },

// 06.11.01 Fukushiro M. 1行追加 ()
	{ "OPTION_TOOL_LOCK",                        ID_OPTION_TOOL_LOCK },
	{ "OPTION_USER_REGISTRATION",                ID_OPTION_USER_REGISTRATION },
// 06.10.20 Fukushiro M. 1行削除 ()
//		{ "RBTN_DELETE_HI_FORK_TAG",                 ID_RBTN_DELETE_HI_FORK_TAG },
	{ "RBTN_HTREE_CHANGE_BRANCH_INTO_TOP",       ID_RBTN_HTREE_CHANGE_BRANCH_INTO_TOP },
	{ "RBTN_HTREE_CHANGE_BRANCH_INTO_TRUNK",     ID_RBTN_HTREE_CHANGE_BRANCH_INTO_TRUNK },
	{ "RBTN_HTREE_CHANGE_TRUNK_INTO_BRANCH",     ID_RBTN_HTREE_CHANGE_TRUNK_INTO_BRANCH },
// 06.10.20 Fukushiro M. 1行追加 ()
	{ "RBTN_HTREE_DELETE_HI_FORK_TAG",           ID_RBTN_HTREE_DELETE_HI_FORK_TAG },
	{ "RBTN_HTREE_DELETE_TIME_LABEL",            ID_RBTN_HTREE_DELETE_TIME_LABEL },
	{ "RBTN_HTREE_DOC_DELETE",                   ID_RBTN_HTREE_DOC_DELETE },
	{ "RBTN_HTREE_DOCUMENT_PROPERTY",            ID_RBTN_HTREE_DOCUMENT_PROPERTY },
	{ "RBTN_HTREE_EXEC_HISTDRAW_TARGET",         ID_RBTN_HTREE_EXEC_HISTDRAW_TARGET },
	{ "RBTN_HTREE_EXEC_HISTVIEW_TARGET",         ID_RBTN_HTREE_EXEC_HISTVIEW_TARGET },
	{ "RBTN_HTREE_FORK_HISTORY",                 ID_RBTN_HTREE_FORK_HISTORY },
	{ "RBTN_HTREE_HI_TAG_PROPERTY",              ID_RBTN_HTREE_HI_TAG_PROPERTY },
// 08.12.09 Fukushiro M. 1行追加 ()
	{ "RBTN_HTREE_OPEN_TARGET_POSITION_0",       ID_RBTN_HTREE_OPEN_TARGET_POSITION_0 },
	{ "RBTN_HTREE_PROJECT_PROPERTY",             ID_RBTN_HTREE_PROJECT_PROPERTY },
	{ "RBTN_HTREE_PUT_HI_FORK_TAG",              ID_RBTN_HTREE_PUT_HI_FORK_TAG },
	{ "RBTN_HTREE_TIME_LABEL_PROPERTY",          ID_RBTN_HTREE_TIME_LABEL_PROPERTY },
	{ "RBTN_ITEM_CHANGE",                        ID_RBTN_ITEM_CHANGE },
// 06.10.20 Fukushiro M. 1行追加 ()
	{ "RBTN_ITEM_CHANGE_2",                      ID_RBTN_ITEM_CHANGE_2 },
	{ "RBTN_ITEM_DELETE",                        ID_RBTN_ITEM_DELETE },
	{ "RBTN_ITEM_INSERT_NEXT",                   ID_RBTN_ITEM_INSERT_NEXT },
	{ "RBTN_ITEM_INSERT_PREV",                   ID_RBTN_ITEM_INSERT_PREV },
	{ "RBTN_ITEM_MERGE",                         ID_RBTN_ITEM_MERGE },
	{ "RBTN_ITEM_MOVE_BEGIN",                    ID_RBTN_ITEM_MOVE_BEGIN },
	{ "RBTN_ITEM_MOVE_CANCEL",                   ID_RBTN_ITEM_MOVE_CANCEL },
	{ "RBTN_ITEM_MOVE_NEXT",                     ID_RBTN_ITEM_MOVE_NEXT },
	{ "RBTN_ITEM_MOVE_OTHER",                    ID_RBTN_ITEM_MOVE_OTHER },
	{ "RBTN_ITEM_MOVE_PREV",                     ID_RBTN_ITEM_MOVE_PREV },
	{ "RBTN_ITEM_PASTE",                         ID_RBTN_ITEM_PASTE },
	{ "RBTN_RULER_CLEAR_ALL_TAB_POS",            ID_RBTN_RULER_CLEAR_ALL_TAB_POS },
	{ "RBTN_TOOLOBJ_PROPERTY",                   ID_RBTN_TOOLOBJ_PROPERTY },
	{ "RBTN_TOOLSELF_PROPERTY",                  ID_RBTN_TOOLSELF_PROPERTY },
	{ "SC_CLOSE",                                SC_CLOSE },
	{ "SC_MAXIMIZE",                             SC_MAXIMIZE },
	{ "SC_MINIMIZE",                             SC_MINIMIZE },
	{ "SC_MOVE",                                 SC_MOVE },
	{ "SC_RESTORE",                              SC_RESTORE },
	{ "SC_SIZE",                                 SC_SIZE },
//----- 09.06.13 Fukushiro M. 削除始 ()-----
//		{ "STC_OBJECT_INF_0",                        ID_STC_OBJECT_INF_0 },
//		{ "STC_OBJECT_INF_1",                        ID_STC_OBJECT_INF_1 },
//		{ "STC_OBJECT_INF_2",                        ID_STC_OBJECT_INF_2 },
//		{ "STC_OBJECT_INF_3",                        ID_STC_OBJECT_INF_3 },
//		{ "STC_OBJECT_INF_4",                        ID_STC_OBJECT_INF_4 },
//----- 09.06.13 Fukushiro M. 削除終 ()-----
	{ "STC_CAUTION_ICONS",                       ID_STC_CAUTION_ICONS },
	{ "TEXT_ALIGN_BOTTOM",                       ID_TEXT_ALIGN_BOTTOM },
	{ "TEXT_ALIGN_CENTER",                       ID_TEXT_ALIGN_CENTER },
	{ "TEXT_ALIGN_LEFT",                         ID_TEXT_ALIGN_LEFT },
	{ "TEXT_ALIGN_LEFTRIGHT",                    ID_TEXT_ALIGN_LEFTRIGHT },
	{ "TEXT_ALIGN_RIGHT",                        ID_TEXT_ALIGN_RIGHT },
	{ "TEXT_ALIGN_TOP",                          ID_TEXT_ALIGN_TOP },
	{ "TEXT_ALIGN_V_CENTER",                     ID_TEXT_ALIGN_V_CENTER },
	{ "TEXT_BOTH_CODE_FONT_BOLD",                ID_TEXT_BOTH_CODE_FONT_BOLD },
	{ "TEXT_BOTH_CODE_FONT_ITALIC",              ID_TEXT_BOTH_CODE_FONT_ITALIC },
	{ "TEXT_BOTH_CODE_FONT_UNDERLINE",           ID_TEXT_BOTH_CODE_FONT_UNDERLINE },
	{ "TEXT_DEFLATE_FONT_SIZE",                  ID_TEXT_DEFLATE_FONT_SIZE },
// 06.03.25 Fukushiro M. 1行削除 ()
//		{ "TEXT_FULL_SCREEN_EDIT_MODE",              ID_TEXT_FULL_SCREEN_EDIT_MODE },
	{ "TEXT_INFLATE_FONT_SIZE",                  ID_TEXT_INFLATE_FONT_SIZE },
	{ "TEXT_LINE",                               ID_TEXT_LINE },
	{ "TEXT_MARGIN",                             ID_TEXT_MARGIN },
	{ "TEXT_REMOVE_FROM_OBJECT",                 ID_TEXT_REMOVE_FROM_OBJECT },
// 06.03.25 Fukushiro M. 1行追加 ()
	{ "TEXT_ROTATE_BACKGROUND_COLOR",            ID_TEXT_ROTATE_BACKGROUND_COLOR },
	{ "TEXT_STOP_EDIT",                          ID_TEXT_STOP_EDIT },
	{ "TLCMD_CANCEL",                            ID_TLCMD_CANCEL },
	{ "TLCMD_DUPLICATE_OBJECT",                  ID_TLCMD_DUPLICATE_OBJECT },
	{ "TLCMD_ENCLOSE_AND",                       ID_TLCMD_ENCLOSE_AND },
	{ "TLCMD_ENCLOSE_OR",                        ID_TLCMD_ENCLOSE_OR },
	{ "TLCMD_ENCLOSE_XOR",                       ID_TLCMD_ENCLOSE_XOR },
	{ "TLCMD_MAKE_HORZ_LINK_OUT",                ID_TLCMD_MAKE_HORZ_LINK_OUT },
	{ "TLCMD_MAKE_LINK_IN_OUT",                  ID_TLCMD_MAKE_LINK_IN_OUT },
	{ "TLCMD_MAKE_LINK_OUT",                     ID_TLCMD_MAKE_LINK_OUT },
	{ "TLCMD_MAKE_VERT_LINK_OUT",                ID_TLCMD_MAKE_VERT_LINK_OUT },
	{ "TLCMD_MOVE_OBJECT",                       ID_TLCMD_MOVE_OBJECT },
	{ "TLCMD_POPUPMENU_ENDED",                   ID_TLCMD_POPUPMENU_ENDED },
//----- 13.12.30 Fukushiro M. 追加始 ()-----
	{ "TLCMD_RESIZE_PEN_FONT",                   ID_TLCMD_RESIZE_PEN_FONT },
	{ "TLCMD_RESIZE",                            ID_TLCMD_RESIZE },
//----- 13.12.30 Fukushiro M. 追加終 ()-----
	{ "TOOL_ANY_EDIT",                           ID_TOOL_ANY_EDIT },
	{ "TOOL_ARC",                                ID_TOOL_ARC },
	{ "TOOL_BEZIER_ARROW",                       ID_TOOL_BEZIER_ARROW },
	{ "TOOL_BEZILINE",                           ID_TOOL_BEZILINE },
//----- 06.10.05 Fukushiro M. 追加始 ()-----
	{ "TOOL_BOX_BEZIER_ARROW",                   ID_TOOL_BOX_BEZIER_ARROW },
	{ "TOOL_BOX_POLY_ARROW",                     ID_TOOL_BOX_POLY_ARROW },
//----- 06.10.05 Fukushiro M. 追加終 ()-----
	{ "TOOL_CHIP",                               ID_TOOL_CHIP },
	{ "TOOL_CHIP_PUT",                           ID_TOOL_CHIP_PUT },
	{ "TOOL_EDIT_RICH_TEXT",                     ID_TOOL_EDIT_RICH_TEXT },
	{ "TOOL_LINE",                               ID_TOOL_LINE },
	{ "TOOL_LINK",                               ID_TOOL_LINK },
	{ "TOOL_NOTE_LABEL_PUT",                     ID_TOOL_NOTE_LABEL_PUT },
	{ "TOOL_PEN_LABEL_PUT",                      ID_TOOL_PEN_LABEL_PUT },
	{ "TOOL_POINT",                              ID_TOOL_POINT },
	{ "TOOL_POLY_ARROW",                         ID_TOOL_POLY_ARROW },
	{ "TOOL_POLYLINE",                           ID_TOOL_POLYLINE },
	{ "TOOL_RECT",                               ID_TOOL_RECT },
	{ "TOOL_RICH_TEXT",                          ID_TOOL_RICH_TEXT },
	{ "TOOL_ROTATE",                             ID_TOOL_ROTATE },
	{ "TOOL_RRECT",                              ID_TOOL_RRECT },
	{ "TOOL_SELECT",                             ID_TOOL_SELECT },
	{ "TOOL_TABLE",                              ID_TOOL_TABLE },
	{ "TOOL_TEXT_PUT",                           ID_TOOL_TEXT_PUT },
	{ "TOOL_TIME_LABEL",                         ID_TOOL_TIME_LABEL },
	{ "TOOL_WIDE_BEZIER",                        ID_TOOL_WIDE_BEZIER },
	{ "TOOL_WIDE_BEZIER_ARROW",                  ID_TOOL_WIDE_BEZIER_ARROW },
	{ "TOOL_WIDE_POLY",                          ID_TOOL_WIDE_POLY },
	{ "TOOL_WIDE_POLY_ARROW",                    ID_TOOL_WIDE_POLY_ARROW },
	{ "TOOL_ZOOM",                               ID_TOOL_ZOOM },
	{ "VIEW_APPEND_DSM",                         ID_VIEW_APPEND_DSM },
	{ "VIEW_CHIP_STORE_DOCK_OR_FLOAT",           ID_VIEW_CHIP_STORE_DOCK_OR_FLOAT },
	{ "VIEW_CHIP_STORE_SIDE_CHANGE",             ID_VIEW_CHIP_STORE_SIDE_CHANGE },
	{ "VIEW_CHIP_STORE_VISIBLE",                 ID_VIEW_CHIP_STORE_VISIBLE },
	{ "VIEW_COPY_POSITION",                      ID_VIEW_COPY_POSITION },
	{ "VIEW_CURRENT_DSM",                        ID_VIEW_CURRENT_DSM },
	{ "VIEW_DELETE_DSM",                         ID_VIEW_DELETE_DSM },
	{ "VIEW_DSM_PANEL",                          ID_VIEW_DSM_PANEL },
	{ "VIEW_FUNCTION_KEY_BAR",                   ID_VIEW_FUNCTION_KEY_BAR },
	{ "VIEW_HTREE_DOCK_OR_FLOAT",                ID_VIEW_HTREE_DOCK_OR_FLOAT },
	{ "VIEW_HTREE_SIDE_CHANGE",                  ID_VIEW_HTREE_SIDE_CHANGE },
	{ "VIEW_HTREE_VISIBLE",                      ID_VIEW_HTREE_VISIBLE },
	{ "VIEW_NEXT_DSM",                           ID_VIEW_NEXT_DSM },
	{ "VIEW_OBJ_LABEL_LIST",                     ID_VIEW_OBJ_LABEL_LIST },
	{ "VIEW_OBJ_LABEL_SHOW_MODE",                ID_VIEW_OBJ_LABEL_SHOW_MODE },
	{ "VIEW_PASTE_POSITION",                     ID_VIEW_PASTE_POSITION },
	{ "VIEW_PREV_DSM",                           ID_VIEW_PREV_DSM },
	{ "VIEW_REDRAW",                             ID_VIEW_REDRAW },
	{ "VIEW_STATUS_BAR",                         ID_VIEW_STATUS_BAR },
	{ "VIEW_TIME_LABEL_SHOW_MODE",               ID_VIEW_TIME_LABEL_SHOW_MODE },
//----- 09.06.13 Fukushiro M. 削除始 ()-----
//		{ "VIEW_TOOLBAR_ARROW_DUTY",                 ID_VIEW_TOOLBAR_ARROW_DUTY },
//		{ "VIEW_TOOLBAR_ARROW_EDIT",                 ID_VIEW_TOOLBAR_ARROW_EDIT },
//		{ "VIEW_TOOLBAR_ARROW_TOOLS",                ID_VIEW_TOOLBAR_ARROW_TOOLS },
//		{ "VIEW_TOOLBAR_ARROW_TOOLS2",               ID_VIEW_TOOLBAR_ARROW_TOOLS2 },
//		{ "VIEW_TOOLBAR_CHIP_DUTY",                  ID_VIEW_TOOLBAR_CHIP_DUTY },
//		{ "VIEW_TOOLBAR_CHIP_EDIT",                  ID_VIEW_TOOLBAR_CHIP_EDIT },
//		{ "VIEW_TOOLBAR_CHIP_TOOLS",                 ID_VIEW_TOOLBAR_CHIP_TOOLS },
//		{ "VIEW_TOOLBAR_CHIP_TOOLS2",                ID_VIEW_TOOLBAR_CHIP_TOOLS2 },
//		{ "VIEW_TOOLBAR_COMDRW_TOOLS",               ID_VIEW_TOOLBAR_COMDRW_TOOLS },
//		{ "VIEW_TOOLBAR_COMDRW_TOOLS2",              ID_VIEW_TOOLBAR_COMDRW_TOOLS2 },
//		{ "VIEW_TOOLBAR_FONT",                       ID_VIEW_TOOLBAR_FONT },
//		{ "VIEW_TOOLBAR_HIST_DRAW_MAIN",             ID_VIEW_TOOLBAR_HIST_DRAW_MAIN },
//		{ "VIEW_TOOLBAR_HIST_VIEW",                  ID_VIEW_TOOLBAR_HIST_VIEW },
//		{ "VIEW_TOOLBAR_HIST_VIEW_TOOLS",            ID_VIEW_TOOLBAR_HIST_VIEW_TOOLS },
//		{ "VIEW_TOOLBAR_HTREE_SEARCH",               ID_VIEW_TOOLBAR_HTREE_SEARCH },
//		{ "VIEW_TOOLBAR_LAYER",                      ID_VIEW_TOOLBAR_LAYER },
//		{ "VIEW_TOOLBAR_MAINFRAME",                  ID_VIEW_TOOLBAR_MAINFRAME },
//		{ "VIEW_TOOLBAR_OBJ_INFO",                   ID_VIEW_TOOLBAR_OBJ_INFO },
//		{ "VIEW_TOOLBAR_OBJ_LABEL",                  ID_VIEW_TOOLBAR_OBJ_LABEL },
//		{ "VIEW_TOOLBAR_PEN_BRUSH",                  ID_VIEW_TOOLBAR_PEN_BRUSH },
//		{ "VIEW_TOOLBAR_SHEET",                      ID_VIEW_TOOLBAR_SHEET },
//		{ "VIEW_TOOLBAR_TEXT",                       ID_VIEW_TOOLBAR_TEXT },
//----- 09.06.13 Fukushiro M. 削除終 ()-----
	{ "VIEW_UNLINK_MARK_SHOW_MODE",              ID_VIEW_UNLINK_MARK_SHOW_MODE },

//----- 08.06.04 Fukushiro M. 追加始 ()-----
	{ "VIEW_PAN_LEFT",              			ID_VIEW_PAN_LEFT },
	{ "VIEW_PAN_UP",              				ID_VIEW_PAN_UP },
	{ "VIEW_PAN_RIGHT",              			ID_VIEW_PAN_RIGHT },
	{ "VIEW_PAN_DOWN",              			ID_VIEW_PAN_DOWN },
//----- 08.06.04 Fukushiro M. 追加終 ()-----

	{ "VIEW_ZOOMIN",                             ID_VIEW_ZOOMIN },
	{ "VIEW_ZOOMOUT",                            ID_VIEW_ZOOMOUT },
	{ "WINDOW_ANOTHER1",                         ID_WINDOW_ANOTHER1 },
	{ "WND_EDT_STORE_CHIP_NAME",                 ID_WND_EDT_STORE_CHIP_NAME },
	{ "WND_HORZ_RULER",                          ID_WND_HORZ_RULER },
	{ "WND_HTREE_VIEW",                          ID_WND_HTREE_VIEW },
	{ "WND_MTX_STORE_CHIP_VIEW",                 ID_WND_MTX_STORE_CHIP_VIEW },
	{ "WND_RULER_UNIT",                          ID_WND_RULER_UNIT },
//----- 09.05.27 Fukushiro M. 追加始 ()-----
	{ "WND_TOOLBAR_LEFT",                        ID_WND_TOOLBAR_LEFT },
	{ "WND_TOOLBAR_RIGHT",                       ID_WND_TOOLBAR_RIGHT },
	{ "WND_TOOLBAR_TOP",                         ID_WND_TOOLBAR_TOP },
//----- 09.05.27 Fukushiro M. 追加終 ()-----
	{ "WND_VERT_RULER",                          ID_WND_VERT_RULER },

//----- 14.09.12 Fukushiro M. 追加始 ()-----

	{ "WND_VERT_RULER",                          ID_WND_VERT_RULER },

//----- 14.09.12 Fukushiro M. 追加始 ()-----
	{ "SCE_FILE_SAVE",							ID_SCE_FILE_SAVE },
	{ "SCE_FILE_CHANGE_ID",						ID_SCE_FILE_CHANGE_ID },
	{ "SCE_FILE_ADD_SCRIPT",					ID_SCE_FILE_ADD_SCRIPT },
	{ "SCE_FILE_DELETE_SCRIPT",					ID_SCE_FILE_DELETE_SCRIPT },
	{ "SCE_FILE_START_EXEDITOR",				ID_SCE_FILE_START_EXEDITOR },
	{ "SCE_EDIT_GOTO_LINE",						ID_SCE_EDIT_GOTO_LINE },
	{ "SCE_EDIT_SETTINGS",						ID_SCE_EDIT_SETTINGS },
	{ "SCE_OUTWIN_GOTO_LOCATION",				ID_SCE_OUTWIN_GOTO_LOCATION },
	{ "SCE_OUTWIN_CLEAR",						ID_SCE_OUTWIN_CLEAR },
	{ "SCE_DEBUG_TOGGLE_BREAKPOINT",			ID_SCE_DEBUG_TOGGLE_BREAKPOINT },
	{ "SCE_DEBUG_CLEAR_BREAKPOINT",				ID_SCE_DEBUG_CLEAR_BREAKPOINT },
	{ "SCE_DEBUG_MODE",							ID_SCE_DEBUG_MODE },
	{ "SCE_DEBUG_BREAK",						ID_SCE_DEBUG_BREAK },
	{ "SCE_DEBUG_CONTINUE",						ID_SCE_DEBUG_CONTINUE },
	{ "SCE_DEBUG_STEP_TO_NEXT",					ID_SCE_DEBUG_STEP_TO_NEXT },
	{ "SCE_DEBUG_UNTIL_CARET",					ID_SCE_DEBUG_UNTIL_CARET },
	{ "SCE_DEBUG_STOP",							ID_SCE_DEBUG_STOP },
	{ "SCE_DEBUG_CHANGE_VARIABLE",				ID_SCE_DEBUG_CHANGE_VARIABLE },
	{ "SCE_DEBUG_ADD_WATCH",					ID_SCE_DEBUG_ADD_WATCH },
	{ "SCE_DEBUG_DELETE_WATCH",					ID_SCE_DEBUG_DELETE_WATCH },
	{ "SCE_WIN_SCRIPT_ID",						ID_SCE_WIN_SCRIPT_ID },
	{ "SCE_WIN_SCRIPT_EDITOR",					ID_SCE_WIN_SCRIPT_EDITOR },
	{ "SCE_WIN_OUTPUT",							ID_SCE_WIN_OUTPUT },
	{ "SCE_WIN_WATCH",							ID_SCE_WIN_WATCH },
	{ "SCE_WIN_VARIABLE_NAME",					ID_SCE_WIN_VARIABLE_NAME },
	{ "SCE_WIN_VARIABLE_VALUE",					ID_SCE_WIN_VARIABLE_VALUE },
	{ "SCE_WIN_KEYWORD",						ID_SCE_WIN_KEYWORD },
//----- 14.09.12 Fukushiro M. 追加終 ()-----
//----- 14.09.12 Fukushiro M. 追加終 ()-----

}; // COMMAND_NAME_TO_ID.

/*************************************************************************
 * <データ>	IDS_NAME_TO_ID
 *
 * <機能>	文字列リソース名とIDの対応表。
 *
 * <履歴>	09.06.16 Fukushiro M. 作成
 *************************************************************************/
static const struct { const char* cpName; DWORD dwId; } IDS_NAME_TO_ID[] =
{
	{ "ALIGN_TOP",						IDS_ALIGN_TOP },
	{ "ALIGN_LEFT",						IDS_ALIGN_LEFT },
	{ "ALIGN_RIGHT",					IDS_ALIGN_RIGHT },
	{ "TOOLBAR_MAIN",					IDS_TOOLBAR_MAIN },
	{ "TOOLBAR_ENVIRONMENT",			IDS_TOOLBAR_ENVIRONMENT },
	{ "TOOLBAR_VIEW",					IDS_TOOLBAR_VIEW },
	{ "TOOLBAR_PEN",					IDS_TOOLBAR_PEN },
	{ "TOOLBAR_BRUSH",					IDS_TOOLBAR_BRUSH },
	{ "TOOLBAR_FONT",					IDS_TOOLBAR_FONT },
	{ "TOOLBAR_TEXT",					IDS_TOOLBAR_TEXT },
	{ "TOOLBAR_SHEET",					IDS_TOOLBAR_SHEET },
	{ "TOOLBAR_LAYER",					IDS_TOOLBAR_LAYER },
	{ "TOOLBAR_ARROW",					IDS_TOOLBAR_ARROW },
	{ "TOOLBAR_LABEL",					IDS_TOOLBAR_LABEL },
	{ "TOOLBAR_TOOLS",					IDS_TOOLBAR_TOOLS },
	{ "TOOLBAR_EDIT_TOOLS",				IDS_TOOLBAR_EDIT_TOOLS },
	{ "TOOLBAR_CREATION_TOOLS",			IDS_TOOLBAR_CREATION_TOOLS },
	{ "TOOLBAR_POLY_TOOLS",				IDS_TOOLBAR_POLY_TOOLS },
	{ "TOOLBAR_POSTING_TOOLS",			IDS_TOOLBAR_POSTING_TOOLS },
	{ "TOOLBAR_HTREE_SEARCH",			IDS_TOOLBAR_HTREE_SEARCH },
	{ "TOOLBAR_CHIP_DUTY",				IDS_TOOLBAR_CHIP_DUTY },
	{ "TOOLBAR_ARROW_DUTY",				IDS_TOOLBAR_ARROW_DUTY },
	{ "TOOLBAR_PARAGRAPH",				IDS_TOOLBAR_PARAGRAPH },
	{ "TOOLBAR_ALIGNMENT",				IDS_TOOLBAR_ALIGNMENT },
	{ "TOOLBAR_BOOKMARK",				IDS_TOOLBAR_BOOKMARK },
}; // IDS_NAME_TO_ID.

/*************************************************************************
 * <関数>	myInitCommandTable
 *
 * <機能>	コマンドテーブル変数を初期化する。
 *
 * <履歴>	09.06.15 Fukushiro M. 作成
 *************************************************************************/
static void myInitCommandTable ()
{
    // コマンド文字列->コマンドID変換テーブル。
	if (s_mpCommandStringToId.empty())
	{
		for (long lC = 0; lC != _countof(COMMAND_NAME_TO_ID); lC++)
			s_mpCommandStringToId[COMMAND_NAME_TO_ID[lC].cpName] = COMMAND_NAME_TO_ID[lC].dwId;
	}
    // コマンドID->コマンド文字列変換テーブル。
	if (s_mpCommandIdToString.empty())
	{
		for (long lC = 0; lC != _countof(COMMAND_NAME_TO_ID); lC++)
			s_mpCommandIdToString[COMMAND_NAME_TO_ID[lC].dwId] = COMMAND_NAME_TO_ID[lC].cpName;
	}
//----- 11.11.08 Fukushiro M. 追加始 ()-----
	// 旧コマンド文字列->新コマンド文字列。
	if (s_mpOldCommandToNew.empty())
	{
		for (long lC = 0; lC != _countof(COMMAND_OLD_TO_NEW); lC++)
			s_mpOldCommandToNew[COMMAND_OLD_TO_NEW[lC].cpOldName] = COMMAND_OLD_TO_NEW[lC].cpNewName;
	}
//----- 11.11.08 Fukushiro M. 追加終 ()-----
} // static void myInitCommandTable ()

/*************************************************************************
 * <関数>	myStrToAccel
 *
 * <機能>	指定の文字をアクセラレータキーに変換する。wKey, byVirt には
 *			値が置換されるのではなく、追加される。
 *
// * <注意>	F1は変換できないことに注意。
 *
 * <履歴>	01.05.05 Fukushiro M. 作成
 *************************************************************************/
static BOOL myStrToAccel (WORD& wKey, BYTE& byVirt, const astring& strEachKey)
{
	if (strEachKey.length() == 1)
	{
		if ('a' <= strEachKey[0] && strEachKey[0] <= 'z')
			wKey = strEachKey[0] - ('a' - 'A');
		else
			wKey = strEachKey[0];
	} else
	if (strEachKey == "f1")
		wKey = VK_F1;
	else
	if (strEachKey == "f2")
		wKey = VK_F2;
	else
	if (strEachKey == "f3")
		wKey = VK_F3;
	else
	if (strEachKey == "f4")
		wKey = VK_F4;
	else
	if (strEachKey == "f5")
		wKey = VK_F5;
	else
	if (strEachKey == "f6")
		wKey = VK_F6;
	else
	if (strEachKey == "f7")
		wKey = VK_F7;
	else
	if (strEachKey == "f8")
		wKey = VK_F8;
	else
	if (strEachKey == "f9")
		wKey = VK_F9;
	else
	if (strEachKey == "f10")
		wKey = VK_F10;
	else
	if (strEachKey == "f11")
		wKey = VK_F11;
	else
	if (strEachKey == "f12")
		wKey = VK_F12;
	else
	if (strEachKey == "f13")
		wKey = VK_F13;
	else
	if (strEachKey == "f14")
		wKey = VK_F14;
	else
	if (strEachKey == "f15")
		wKey = VK_F15;
	else
	if (strEachKey == "insert")
		wKey = VK_INSERT;
	else
	if (strEachKey == "space")
		wKey = VK_SPACE;
	else
	if (strEachKey == "back")
		wKey = VK_BACK;
	else
	if (strEachKey == "delete")
		wKey = VK_DELETE;
	else
	if (strEachKey == "tab")
		wKey = VK_TAB;
	else
	if (strEachKey == "escape")
		wKey = VK_ESCAPE;
	else
	if (strEachKey == "return")
		wKey = VK_RETURN;
	else
//----- 05.03.02 Fukushiro M. 追加始 ()-----
	if (strEachKey == "left")
		wKey = VK_LEFT;
	else
	if (strEachKey == "up")
		wKey = VK_UP;
	else
	if (strEachKey == "right")
		wKey = VK_RIGHT;
	else
	if (strEachKey == "down")
		wKey = VK_DOWN;
	else
//----- 05.03.02 Fukushiro M. 追加終 ()-----
//----- 10.02.01 Fukushiro M. 追加始 ()-----
	if (strEachKey == "prior")
		wKey = VK_PRIOR;
	else
	if (strEachKey == "next")
		wKey = VK_NEXT;
	else
	if (strEachKey == "end")
		wKey = VK_END;
	else
	if (strEachKey == "home")
		wKey = VK_HOME;
	else
	if (strEachKey == "numpad0")
		wKey = VK_NUMPAD0;
	else
	if (strEachKey == "numpad1")
		wKey = VK_NUMPAD1;
	else
	if (strEachKey == "numpad2")
		wKey = VK_NUMPAD2;
	else
	if (strEachKey == "numpad3")
		wKey = VK_NUMPAD3;
	else
	if (strEachKey == "numpad4")
		wKey = VK_NUMPAD4;
	else
	if (strEachKey == "numpad5")
		wKey = VK_NUMPAD5;
	else
	if (strEachKey == "numpad6")
		wKey = VK_NUMPAD6;
	else
	if (strEachKey == "numpad7")
		wKey = VK_NUMPAD7;
	else
	if (strEachKey == "numpad8")
		wKey = VK_NUMPAD8;
	else
	if (strEachKey == "numpad9")
		wKey = VK_NUMPAD9;
	else
	if (strEachKey == "multiply")
		wKey = VK_MULTIPLY;
	else
	if (strEachKey == "add")
		wKey = VK_ADD;
	else
	if (strEachKey == "subtract")
		wKey = VK_SUBTRACT;
	else
	if (strEachKey == "decimal")
		wKey = VK_DECIMAL;
	else
	if (strEachKey == "divide")
		wKey = VK_DIVIDE;
	else
	if (strEachKey == "numlock")
		wKey = VK_NUMLOCK;
	else
//----- 10.02.01 Fukushiro M. 追加終 ()-----
	if (strEachKey == "ctrl")
		byVirt |= FCONTROL;
	else
	if (strEachKey == "shift")
		byVirt |= FSHIFT;
	else
	if (strEachKey == "alt")
		byVirt |= FALT;
	else
		return FALSE;	// エラー。
	return TRUE;
} // myStrToAccel.

/*************************************************************************
 * <関数>	myAccelToStr
 *
 * <機能>	指定のアクセラレータキーを文字に変換する。
 *
 * <注意>	myStrToAccelと違い、F1は変換できることに注意。
 *
 * <履歴>	01.05.05 Fukushiro M. 作成
 *************************************************************************/
static void myAccelToStr (CString& strKey, WORD wKey, BYTE byVirt)
{
	strKey.Empty();
	if (byVirt & FALT) strKey += _T("Alt+");
	if (byVirt & FCONTROL) strKey += _T("Ctrl+");
	if (byVirt & FSHIFT) strKey += _T("Shift+");
	switch (wKey)
	{
	case VK_F1:
		strKey += _T("F1");
		break;
	case VK_F2:
		strKey += _T("F2");
		break;
	case VK_F3:
		strKey += _T("F3");
		break;
	case VK_F4:
		strKey += _T("F4");
		break;
	case VK_F5:
		strKey += _T("F5");
		break;
	case VK_F6:
		strKey += _T("F6");
		break;
	case VK_F7:
		strKey += _T("F7");
		break;
	case VK_F8:
		strKey += _T("F8");
		break;
	case VK_F9:
		strKey += _T("F9");
		break;
	case VK_F10:
		strKey += _T("F10");
		break;
	case VK_F11:
		strKey += _T("F11");
		break;
	case VK_F12:
		strKey += _T("F12");
		break;
	case VK_F13:
		strKey += _T("F13");
		break;
	case VK_F14:
		strKey += _T("F14");
		break;
	case VK_F15:
		strKey += _T("F15");
		break;
	case VK_INSERT:
		strKey += _T("Ins");
		break;
	case VK_SPACE:
		strKey += _T("Space");
		break;
	case VK_BACK:
		strKey += _T("Back");
		break;
	case VK_DELETE:
		strKey += _T("Del");
		break;
	case VK_TAB:
		strKey += _T("Tab");
		break;
	case VK_ESCAPE:
		strKey += _T("Esc");
		break;
	case VK_RETURN:
		strKey += _T("Return");
		break;
//----- 05.03.02 Fukushiro M. 追加始 ()-----
	case VK_LEFT:
		strKey += _T("Left");
		break;
	case VK_UP:
		strKey += _T("Up");
		break;
	case VK_RIGHT:
		strKey += _T("Right");
		break;
	case VK_DOWN:
		strKey += _T("Down");
		break;
//----- 05.03.02 Fukushiro M. 追加終 ()-----
//----- 10.02.01 Fukushiro M. 追加始 ()-----
	case VK_PRIOR:
		strKey += _T("Prior");
		break;
	case VK_NEXT:
		strKey += _T("Next");
		break;
	case VK_END:
		strKey += _T("End");
		break;
	case VK_HOME:
		strKey += _T("Home");
		break;
	case VK_NUMPAD0:
		strKey += _T("Numpad0");
		break;
	case VK_NUMPAD1:
		strKey += _T("Numpad1");
		break;
	case VK_NUMPAD2:
		strKey += _T("Numpad2");
		break;
	case VK_NUMPAD3:
		strKey += _T("Numpad3");
		break;
	case VK_NUMPAD4:
		strKey += _T("Numpad4");
		break;
	case VK_NUMPAD5:
		strKey += _T("Numpad5");
		break;
	case VK_NUMPAD6:
		strKey += _T("Numpad6");
		break;
	case VK_NUMPAD7:
		strKey += _T("Numpad7");
		break;
	case VK_NUMPAD8:
		strKey += _T("Numpad8");
		break;
	case VK_NUMPAD9:
		strKey += _T("Numpad9");
		break;
	case VK_MULTIPLY:
		strKey += _T("Multiply");
		break;
	case VK_ADD:
		strKey += _T("Add");
		break;
	case VK_SUBTRACT:
		strKey += _T("Subtract");
		break;
	case VK_DECIMAL:
		strKey += _T("Decimal");
		break;
	case VK_DIVIDE:
		strKey += _T("Divide");
		break;
	case VK_NUMLOCK:
		strKey += _T("Numlock");
		break;
//----- 10.02.01 Fukushiro M. 追加終 ()-----
	default:
// 08.03.20 Fukushiro M. 1行変更 ()
//		strKey += (const _TCHAR)AStrToTStr((char)wKey);
		strKey += (const _TCHAR)AChrToTChr((char)wKey, CP_ACP);
	}
} // myAccelToStr.
//----------</#DD VIEWER NOT EXPORT>-----------

/*************************************************************************
 * <関数>	CheckFloatStr
 *
 * <機能>	エディットボックスの値が浮動小数点の文字列で構成されているか
 *			どうかを調べ、それ以外の文字部分を選択状態にする。
 *
 * <引数>	str		:取得した文字列を返す。
 *			pEdit	:エディットボックスを指定。
 *
 * <返値>	FDCS_ERROR	:文字列にエラーがある。
 *			FDCS_EMPTY	:文字列が空。
 *			FDCS_NORMAL	:正常。
 *
 * <履歴>	98.12.15 Fukushiro M. 作成
 *************************************************************************/
FTCheckStr CheckFloatStr (WString& str, CEdit* pEdit)
{
	FFGetWindowText(pEdit, str);
	int iLen = str.GetLength();

	FTCheckStr check = FDCS_EMPTY;
	for (int i = 0; i < iLen; i++)
	{
		if (str.at(i) == L'+' || str.at(i) == L'-' ||
			str.at(i) == L'.' || isdigit(str.at(i)))
		{
			check = FDCS_NORMAL;
		} else if (!iswspace(str.at(i)))
		{
			pEdit->SetSel(i, i + 1);
			return FDCS_ERROR;
		}
	}
	return check;
} // CheckFloatStr

//----------<#DD VIEWER NOT EXPORT>-----------
/*************************************************************************
 * <関数>	CheckFloatStr
 *
 * <機能>	コンボボックス（タイプがドロップダウン）の値が浮動小数点
 *			の文字列で構成されているかどうかを調べ、それ以外の文字部分
 *			を選択状態にする。
 *
 * <引数>	str		:取得した文字列を返す。
 *			pBox	:コンボボックスを指定。
 *
 * <返値>	FDCS_ERROR	:文字列にエラーがある。
 *			FDCS_EMPTY	:文字列が空。
 *			FDCS_NORMAL	:正常。
 *
 * <履歴>	98.12.15 Fukushiro M. 作成
 *************************************************************************/
FTCheckStr CheckFloatStr (WString& str, CComboBox* pBox)
{
	FFGetWindowText(pBox, str);
	int iLen = str.GetLength();

	FTCheckStr check = FDCS_EMPTY;
	for (int i = 0; i < iLen; i++)
	{
		if (str.at(i) == L'+' || str.at(i) == L'-' ||
			str.at(i) == L'.' || isdigit(str.at(i)))
		{
			check = FDCS_NORMAL;
		} else if (!iswspace(str.at(i)))
		{
			pBox->SetEditSel(i, i + 1);
			return FDCS_ERROR;
		}
	}
	return check;
} // CheckFloatStr

/*************************************************************************
 * <関数>	CheckDWORDStr
 *
 * <機能>	エディットボックスの値が指定の整数の文字列で構成されているか
 *			どうかを調べ、それ以外の文字部分を選択状態にする。
 *
 * <引数>	str		:取得した文字列を返す。
 *			pEdit	:エディットボックスを指定。
 *			wBase	:基数を指定。とりあえず2,8,10,16進数をサポート。
 *
 * <返値>	FDCS_ERROR	:文字列にエラーがある。
 *			FDCS_EMPTY	:文字列が空。
 *			FDCS_NORMAL	:正常。
 *
 * <履歴>	01.08.15 Fukushiro M. 作成
 *************************************************************************/
FTCheckStr CheckDWORDStr (WString& str, CEdit* pEdit, WORD wBase)
{
	FFGetWindowText(pEdit, str);
	int iLen = str.GetLength();

	FTCheckStr check = FDCS_EMPTY;
	for (int i = 0; i < iLen; i++)
	{
		if (wBase == 2 &&
			(L'0' <= str.at(i) && str.at(i) <= L'1'))
		{
			check = FDCS_NORMAL;
		} else
		if (wBase == 8 &&
			(L'0' <= str.at(i) && str.at(i) <= L'7'))
		{
			check = FDCS_NORMAL;
		} else
		if (wBase == 10 &&
			(L'0' <= str.at(i) && str.at(i) <= L'9'))
		{
			check = FDCS_NORMAL;
		} else
		if (wBase == 16 &&
			(L'0' <= str.at(i) && str.at(i) <= L'9'))
		{
			check = FDCS_NORMAL;
		} else
		if (wBase == 16 &&
			(L'A' <= str.at(i) && str.at(i) <= L'F'))
		{
			check = FDCS_NORMAL;
		} else
		if (wBase == 16 &&
			(L'a' <= str.at(i) && str.at(i) <= L'f'))
		{
			check = FDCS_NORMAL;
		} else
		if (!iswspace(str.at(i)))
		{
			pEdit->SetSel(i, i + 1);
			return FDCS_ERROR;
		}
	}
	return check;
} // CheckDWORDStr

/*************************************************************************
 * <関数>	SetStdBtnString
 *
 * <機能>	標準のボタン(IDOK,IDCANCEL,etc)の表示文字列を置き換える。
 *
 * <引数>	hWndDlg	:置き換えるボタンを子に持つウィンドウを指定。
 *
 * <履歴>	00.02.12 Fukushiro M. 作成
 *************************************************************************/
void SetStdBtnString (HWND hWndDlg)
{
	for (HWND hWnd = ::GetWindow(hWndDlg, GW_CHILD);
		hWnd != NULL; hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT))
	{
		DWORD dwID = GetDlgCtrlID(hWnd);
		switch (dwID)
		{
		case IDOK:
			FFSetWindowText(hWnd, WString().LoadString(IDS_CTRL_OK));
			break;
		case IDCANCEL:
			FFSetWindowText(hWnd, WString().LoadString(IDS_CTRL_CANCEL));
			break;
		case IDHELP:
			FFSetWindowText(hWnd, WString().LoadString(IDS_CTRL_HELP));
			break;
//----- 00.09.05 Fukushiro M. 追加始 ()-----
		case ID_APPLY_NOW:
			FFSetWindowText(hWnd, WString().LoadString(IDS_CTRL_APPLY_NOW));
			break;
//----- 00.09.05 Fukushiro M. 追加終 ()-----
		}
	}
} // SetStdBtnString.
//----------</#DD VIEWER NOT EXPORT>-----------

/*************************************************************************
 * <関数>	SetUnitString
 *
 * <機能>	ダイアログのコントロールで、ウィンドウテキストが"$@@@"の
 *			場合は、現在の単位（ミリ、インチ）の文字列と置き換える。
 *
 * <引数>	hWndDlg	:ダイアログのウィンドウハンドル。
 *
 * <履歴>	99.05.04 Fukushiro M. 作成
 *************************************************************************/
void SetUnitString (HWND hWndDlg)
{
	// バッファ。"#@@@"が入る十分なサイズ。
// 08.05.20 Fukushiro M. 1行変更 ()
//	_TCHAR tcsBuff[1000];
	_TCHAR tcsBuff[10];
	for (HWND hWnd = ::GetWindow(hWndDlg, GW_CHILD);
		hWnd != NULL; hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT))
	{
		// ウィンドウテキストを取得。
// 08.05.20 Fukushiro M. 1行変更 ()
//		::GetWindowText(hWnd, tcsBuff, sizeof(tcsBuff));
		::GetWindowText(hWnd, tcsBuff, _countof(tcsBuff));
		// テキストが"$@@@"であれば、単位文字列に置き換える。
		if (!_tcscmp(tcsBuff, _T("$@@@")))
// 07.09.17 Fukushiro M. 1行変更 ()
//			SetWindowText(hWnd, WStrToTStr(FFReg()->GetUnitString()));
			SetWindowText(hWnd, WStrToTStr(FFReg()->GetUnitChars()));
	}
} // SetUnitString.

/*************************************************************************
 * <関数>	RightByLeftHand
 *
 * <機能>	ダイアログ中のコントロールの文字を書き換える。
 *			左利きに設定の場合は、コントロールのウィンドウテキストを
 *			右利き用文字列から左利き用文字列に置き換える。
 *
 * <引数>	hWndDlg	:ダイアログのウィンドウハンドル。
 *
 * <履歴>	99.05.04 Fukushiro M. 作成
 *************************************************************************/
void RightByLeftHand (HWND hWndDlg)
{
//----------<#DD VIEWER NOT EXPORT>-----------
	// 右利きに設定の場合は、復帰。
	if (!FFReg()->GetIsLeftHand()) return;
//----- 06.01.21 Fukushiro M. 変更前 ()-----
//	_TCHAR tcsBuff[256];
//	for (HWND hWnd = ::GetWindow(hWndDlg, GW_CHILD);
//		hWnd != NULL; hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT))
//	{
//		// ウィンドウテキストを取得。
//		::GetWindowText(hWnd, tcsBuff, sizeof(tcsBuff));
//		// コントロール文字列中の"(&"文字列の位置を検索。
//		_TCHAR* tcpFind = _tcsstr(tcsBuff, _T("(&"));
//		// 文字列が無い場合は、次のループへ。
//		if (tcpFind == NULL) continue;
//		//----- 文字列が見つかった場合 -----
//		// 左利き用へ変換処理。
//		tcpFind[2] = s_mpRight2Left[tcpFind[2]];
//		// ウィンドウテキストを設定。
//		::SetWindowText(hWnd, tcsBuff);
//	}
//----- 06.01.21 Fukushiro M. 変更後 ()-----
	for (HWND hWnd = ::GetWindow(hWndDlg, GW_CHILD);
		hWnd != NULL; hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT))
	{
		// ウィンドウテキストを取得。
		WString wstr;
		FFGetWindowText(hWnd, wstr);
		// 左利き用へ変換処理。
		if (ChangeToLeftHanded(wstr))
			FFSetWindowText(hWnd, wstr);
	}
//----- 06.01.21 Fukushiro M. 変更終 ()-----
//----------</#DD VIEWER NOT EXPORT>-----------
} // RightByLeftHand.

//----------<#DD VIEWER NOT EXPORT>-----------
/*************************************************************************
 * <関数>	ChangeHanded
 *
 * <機能>	右利き用のメニュー文字と左利き用のメニュー文字を交換する。
 *
 * <引数>	hMenu	:交換するメニュー。
 *			bToLeft	:左利き用に変換する場合は TRUE、右利き用は FALSE。
 *
 * <解説>	再帰呼び出しによって、hMenuからサブメニューを辿り、全ての
 *			メニュー文字列を変更する。
 *
 * <注意>	static関数。
 *
 * <履歴>	99.04.21 Fukushiro M. 作成
 *			00.03.13 Fukushiro M. MainFrame.cppから移動。
 *************************************************************************/
void ChangeHanded (HMENU hMenu)
{
	MENUITEMINFO mii;
	// メニュー数を取得。
	const int iMenuCount = GetMenuItemCount(hMenu);
// 06.01.21 Fukushiro M. 1行削除 ()
//	_TCHAR* tcpFind;
	for (int imi = 0; imi != iMenuCount; imi++)
	{
		_TCHAR tsz[250] = { _T('\0') };
		//----- メニュー文字列とサブメニューハンドルを取得 -----
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_SUBMENU;
		mii.dwTypeData = tsz;
		mii.cch = _countof(tsz);
		GetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
		// サブメニューがある場合は再帰呼び出し。
		if (mii.hSubMenu != NULL) ChangeHanded(mii.hSubMenu);
		// メニュー文字列が無い場合（セパレータ等）は次のループへ。
		if (mii.dwTypeData == NULL || *(mii.dwTypeData) == _T('\0')) continue;

//----- 06.01.21 Fukushiro M. 変更前 ()-----
//		// メニュー文字列中の"("文字列の位置を検索。
//		// 部品メニューで、ツールによっては"&"を取り去っている
//		// 場合があるため。
//		tcpFind = _tcsstr(mii.dwTypeData, _T("("));
//		// 文字列が無い場合は、次のループへ。
//		if (tcpFind == NULL) continue;
//		//----- 文字列が見つかった場合 -----
//		tcpFind++;
//		if (*tcpFind == _T('&')) tcpFind++;
//		if (bToLeft)
//		//----- 左利き用へ変換の場合 -----
//		{
//			// 左利き用へ変換処理。
//			*tcpFind = s_mpRight2Left[*tcpFind];
//		} else
//		//----- 右利き用へ変換の場合 -----
//		{
//			// 左利き用へ変換処理。
//			*tcpFind = s_mpLeft2Right[*tcpFind];
//		}
//----- 06.01.21 Fukushiro M. 変更後 ()-----
		WString wstr = TStrToWStr(mii.dwTypeData);
		if (FFReg()->GetIsLeftHand())
		//----- 左利き用へ変換の場合 -----
		{
			// 左利き用へ変換処理。
//----- 09.06.28 Fukushiro M. 変更前 ()-----
//			if (ChangeToLeftHanded(wstr))
//				FFTcscpy_s(mii.dwTypeData, mii.cch, WStrToTStr(wstr));
//----- 09.06.28 Fukushiro M. 変更後 ()-----
			if (ChangeToLeftHanded(wstr))
				FFTcscpy_s(mii.dwTypeData, _countof(tsz), WStrToTStr(wstr));
//----- 09.06.28 Fukushiro M. 変更終 ()-----
		} else
		//----- 右利き用へ変換の場合 -----
		{
			// 左利き用へ変換処理。
//----- 09.06.28 Fukushiro M. 変更前 ()-----
//			if (ChangeToRightHanded(wstr))
//				FFTcscpy_s(mii.dwTypeData, mii.cch, WStrToTStr(wstr));
//----- 09.06.28 Fukushiro M. 変更後 ()-----
			if (ChangeToRightHanded(wstr))
				FFTcscpy_s(mii.dwTypeData, _countof(tsz), WStrToTStr(wstr));
//----- 09.06.28 Fukushiro M. 変更終 ()-----
		}
//----- 06.01.21 Fukushiro M. 変更終 ()-----
		//----- 変換後の文字列をメニューに設定 -----
		mii.cbSize = sizeof(mii);
// 06.01.21 Fukushiro M. 1行削除 ()
//		mii.dwTypeData = tsz;
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.cch = (UINT)_tcslen(mii.dwTypeData);
		SetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
	}
} // ChangeHanded.

/*************************************************************************
 * <関数>	RightByLeftHand
 *
 * <機能>	左利きに設定になっている場合は、右利き用の文字を左利き用の
 *			文字に交換する。
 *
 * <引数>	tc	:交換する文字を指定。
 *
 * <返値>	交換した文字。
 *
 * <注意>	static関数。
 *
 * <履歴>	00.03.13 Fukushiro M. 作成
 *************************************************************************/
wchar_t RightByLeftHand (wchar_t tc)
{
	// 右利きに設定の場合は、復帰。
	if (!FFReg()->GetIsLeftHand()) return tc;
	// 左利き用へ変換処理。
	return s_mpRight2Left[tc];
} // RightByLeftHand.

/*************************************************************************
 * <関数>	ChangeToLeftHanded
 *
 * <機能>	指定文字列中のショートカット文字を右利きから左利き用に変換する。
 *
 * <引数>	wstr	:文字列を指定。置換して返す。
 *
 * <返値>	置換した場合はTRUE。
 *
 * <解説>	'(' → '&' → X → ')' の順に探して一致すれば文字Xを置換する。
 *			但し、部品メニューで、ツールによっては"&"を取り去っている
 *			場合があるため、下のように'&'が無くても置換対象とする。
 *			'(' → X → ')'
 *
 * <履歴>	06.01.21 Fukushiro M. 作成
 *************************************************************************/
BOOL ChangeToLeftHanded (WString& wstr)
{
#ifdef FD_TOP_ACCESSKEY		// 文頭にアクセスキーが有る場合。
	// & が省略されている場合もあるため、FD_TOP_ACCESSKEY_LEN から 1 を引く。
	if (wstr.GetLength() < FD_TOP_ACCESSKEY_LEN - 1)
		return FALSE;
	// キーの文字部分の位置を求める。
	const int iIndex = (wstr.at(0) == L'&') ? 1 : 0;
	FTTcTcMap::const_iterator i = s_mpRight2Left.find(wstr.at(iIndex));
	if (i != s_mpRight2Left.end())
	//----- 文字に対応する左利き用文字がある場合 -----
	{
		// 文字を置換。
		wstr.at(iIndex) = i->second;
		return TRUE;
	}
	return FALSE;
#else //FD_TOP_ACCESSKEY	// 文中にアクセスキーが有る場合。
	int iC = 0;
	for (;;)
	{
		if (iC == wstr.GetLength()) break;
		if (wstr.at(iC) == L'(')
		{
			iC++;
			if (iC == wstr.GetLength()) break;
			if (wstr.at(iC) == L'&')
			{
				iC++;
				if (iC == wstr.GetLength()) break;
			}
			if (wstr.at(iC) < 0x80)
			{
				iC++;
				if (iC == wstr.GetLength()) break;
				if (wstr.at(iC) == L')')
				{
// 06.02.27 Fukushiro M. 1行変更 ()
//					FTTcTcMap::const_iterator i = s_mpRight2Left.find(_TCHAR(wstr.at(iC - 1)));
					FTTcTcMap::const_iterator i = s_mpRight2Left.find(wstr.at(iC - 1));
					if (i == s_mpRight2Left.end()) break;
					wstr.at(iC - 1) = i->second;
					return TRUE;
				}
			}
		} else
		{
			iC++;
		}
	}
	return FALSE;
#endif //FD_TOP_ACCESSKEY	// 文頭にアクセスキーが有る場合。
} // ChangeToLeftHanded.

/*************************************************************************
 * <関数>	ChangeToRightHanded
 *
 * <機能>	指定文字列中のショートカット文字を左利きから右利き用に変換する。
 *
 * <引数>	wstr	:文字列を指定。置換して返す。
 *
 * <返値>	置換した場合はTRUE。
 *
 * <解説>	'(' → '&' → X → ')' の順に探して一致すれば文字Xを置換する。
 *			但し、部品メニューで、ツールによっては"&"を取り去っている
 *			場合があるため、下のように'&'が無くても置換対象とする。
 *			'(' → X → ')'
 *
 * <履歴>	06.01.21 Fukushiro M. 作成
 *************************************************************************/
BOOL ChangeToRightHanded (WString& wstr)
{
#ifdef FD_TOP_ACCESSKEY		// 文頭にアクセスキーが有る場合。
	// & が省略されている場合もあるため、FD_TOP_ACCESSKEY_LEN から 1 を引く。
	if (wstr.GetLength() < FD_TOP_ACCESSKEY_LEN - 1)
		return FALSE;
	// キーの文字部分の位置を求める。
	const int iIndex = (wstr.at(0) == L'&') ? 1 : 0;
	FTTcTcMap::const_iterator i = s_mpLeft2Right.find(wstr.at(iIndex));
// 08.05.05 Fukushiro M. 1行変更 ()
//	if (i != s_mpRight2Left.end())
	if (i != s_mpLeft2Right.end())
	//----- 文字に対応する左利き用文字がある場合 -----
	{
		// 文字を置換。
		wstr.at(iIndex) = i->second;
		return TRUE;
	}
	return FALSE;
#else //FD_TOP_ACCESSKEY	// 文中にアクセスキーが有る場合。
	int iC = 0;
	for (;;)
	{
		if (iC == wstr.GetLength()) break;
		if (wstr.at(iC) == L'(')
		{
			iC++;
			if (iC == wstr.GetLength()) break;
			if (wstr.at(iC) == L'&')
			{
				iC++;
				if (iC == wstr.GetLength()) break;
			}
			if (wstr.at(iC) < 0x80)
			{
				iC++;
				if (iC == wstr.GetLength()) break;
				if (wstr.at(iC) == L')')
				{
// 06.02.27 Fukushiro M. 1行変更 ()
//					FTTcTcMap::const_iterator i = s_mpLeft2Right.find(_TCHAR(wstr.at(iC - 1)));
					FTTcTcMap::const_iterator i = s_mpLeft2Right.find(wstr.at(iC - 1));
					if (i == s_mpLeft2Right.end()) break;
					wstr.at(iC - 1) = i->second;
					return TRUE;
				}
			}
		} else
		{
			iC++;
		}
	}
	return FALSE;
#endif //FD_TOP_ACCESSKEY	// 文頭にアクセスキーが有る場合。
} // ChangeToRightHanded.

/*************************************************************************
 * <関数>	ShortenMenu
 *
 * <機能>	メニューを短縮形にする。
 *
 * <引数>	hMenu	:変換するメニューを指定。
 *
 * <履歴>	00.10.18 Fukushiro M. 作成
 *************************************************************************/
void ShortenMenu (HMENU hMenu)
{
	std::map<WString, WString> mpLongToShort;
	WString strMenuTbl;
	strMenuTbl.LoadString(IDS_SHORT_MENU);

	while (!strMenuTbl.IsEmpty())
	{
		WString strLongMenu = FFExtractUntil(strMenuTbl, L"\n");
		WString strShortMenu = FFExtractUntil(strMenuTbl, L"\n");
		mpLongToShort[strLongMenu] = strShortMenu;
	}
	MENUITEMINFO mii;
	_TCHAR szMenu[260];
	// メニュー数を取得。
	const int iMenuCount = GetMenuItemCount(hMenu);
	for (int imi = 0; imi < iMenuCount; imi++)
	{
		//----- メニュー文字列とサブメニューハンドルを取得 -----
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE;
		mii.dwTypeData = szMenu;
// 08.05.20 Fukushiro M. 1行変更 ()
//		mii.cch = sizeof(szMenu) / sizeof(szMenu[0]);
		mii.cch = _countof(szMenu);
		GetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
		// メニュー文字列が無い場合（セパレータ等）は次のループへ。
		if (mii.dwTypeData == NULL || *(mii.dwTypeData) == _T('\0')) continue;
#ifdef FD_TOP_ACCESSKEY		// 文頭にアクセスキーが有る場合。
		WString wstrMenu = TStrToWStr(mii.dwTypeData);
		// 文頭に'&'が無い場合は、次のループへ。
		if (wstrMenu.at(0) != L'&') continue;
		//----- 文字列が見つかった場合 -----
		// アクセスキー部を取り出す。
		const WString wstrAccesskey = wstrMenu.Left(FD_TOP_ACCESSKEY_LEN);
		// メニュー文本体を取り出す。
		wstrMenu = wstrMenu.Mid(FD_TOP_ACCESSKEY_LEN);
		// メニュー文本体を変換。
		wstrMenu = mpLongToShort[wstrMenu];
		// アクセスキーを挿入。
		wstrMenu.Insert(0, wstrAccesskey);
		FFTcscpy_s(szMenu, _countof(szMenu), WStrToTStr(wstrMenu));
#else //FD_TOP_ACCESSKEY	// 文中にアクセスキーが有る場合。
		// メニュー文字列中の"(&"文字列の位置を検索。
		_TCHAR* tcpFind = _tcsstr(mii.dwTypeData, _T("(&"));
		// 文字列が無い場合は、次のループへ。
		if (tcpFind == NULL) continue;
		//----- 文字列が見つかった場合 -----
		tcpFind[0] = _T('\0');
		CString strShort = WStrToTStr(mpLongToShort[TStrToWStr(szMenu)]);
		strShort += _T("(&");
		strShort += tcpFind[2];
		FFTcscpy_s(szMenu, _countof(szMenu), strShort);
#endif //FD_TOP_ACCESSKEY	// 文頭にアクセスキーが有る場合。
		//----- 変換後の文字列をメニューに設定 -----
		mii.cbSize = sizeof(mii);
		mii.dwTypeData = szMenu;
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.cch = (UINT)_tcslen(mii.dwTypeData);
		SetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
	}
} // ShortenMenu.

/*************************************************************************
 * <関数>	LengthenMenu
 *
 * <機能>	メニューを通常の長さにする。
 *
 * <引数>	hMenu	:変換するメニューを指定。
 *
 * <履歴>	00.10.18 Fukushiro M. 作成
 *************************************************************************/
void LengthenMenu (HMENU hMenu)
{
	std::map<WString, WString> mpShortToLong;
	WString strMenuTbl;
	strMenuTbl.LoadString(IDS_SHORT_MENU);

	while (!strMenuTbl.IsEmpty())
	{
		WString strLongMenu = FFExtractUntil(strMenuTbl, L"\n");
		WString strShortMenu = FFExtractUntil(strMenuTbl, L"\n");
		mpShortToLong[strShortMenu] = strLongMenu;
	}
	MENUITEMINFO mii;
	_TCHAR szMenu[260];
	// メニュー数を取得。
	const int iMenuCount = GetMenuItemCount(hMenu);
	for (int imi = 0; imi < iMenuCount; imi++)
	{
		//----- メニュー文字列とサブメニューハンドルを取得 -----
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE;
		mii.dwTypeData = szMenu;
// 08.05.20 Fukushiro M. 1行変更 ()
//		mii.cch = sizeof(szMenu) / sizeof(szMenu[0]);
		mii.cch = _countof(szMenu);
		GetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
		// メニュー文字列が無い場合（セパレータ等）は次のループへ。
		if (mii.dwTypeData == NULL || *(mii.dwTypeData) == _T('\0')) continue;
#ifdef FD_TOP_ACCESSKEY		// 文頭にアクセスキーが有る場合。
		WString wstrMenu = TStrToWStr(mii.dwTypeData);
		// 文頭に'&'が無い場合は、次のループへ。
		if (wstrMenu.at(0) != L'&') continue;
		//----- 文字列が見つかった場合 -----
		// アクセスキー部を取り出す。
		const WString wstrAccesskey = wstrMenu.Left(FD_TOP_ACCESSKEY_LEN);
		// メニュー文本体を取り出す。
		wstrMenu = wstrMenu.Mid(FD_TOP_ACCESSKEY_LEN);
		// メニュー文本体を変換。
		wstrMenu = mpShortToLong[wstrMenu];
		// アクセスキーを挿入。
		wstrMenu.Insert(0, wstrAccesskey);
		FFTcscpy_s(szMenu, _countof(szMenu), WStrToTStr(wstrMenu));
#else //FD_TOP_ACCESSKEY	// 文中にアクセスキーが有る場合。
		// メニュー文字列中の"(&"文字列の位置を検索。
		_TCHAR* tcpFind = _tcsstr(mii.dwTypeData, _T("(&"));
		// 文字列が無い場合は、次のループへ。
		if (tcpFind == NULL) continue;
		//----- 文字列が見つかった場合 -----
		tcpFind[0] = _T('\0');
		CString strLong = WStrToTStr(mpShortToLong[TStrToWStr(szMenu)]);
		strLong += _T("(&");
		strLong += tcpFind[2];
		strLong += _T(')');
		FFTcscpy_s(szMenu, _countof(szMenu), strLong);
#endif //FD_TOP_ACCESSKEY	// 文頭にアクセスキーが有る場合。
		//----- 変換後の文字列をメニューに設定 -----
		mii.cbSize = sizeof(mii);
		mii.dwTypeData = szMenu;
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.cch = (UINT)_tcslen(mii.dwTypeData);
		SetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
	}
} // LengthenMenu.

/*************************************************************************
 * <関数>	FFMakeCommandText
 *
 * <機能>	コマンドタイトルとアクセスキーから、コマンド文字列を作成する。
 *
 * <引数>	wcpTitle	:コマンド文字列のタイトルを指定。
 *			wcAccesskey	:アクセスキーを指定。
 *
 * <返値>	コマンド文字列。
 *
 * <解説>	例：wcpTitle = "ファイル", wcAccesskey = 'F' -> "&F. ファイル"
 *
 * <履歴>	06.03.04 Fukushiro M. 作成
 *************************************************************************/
WString FFMakeCommandText (const wchar_t* wcpTitle, wchar_t wcAccesskey)
{
	wchar_t key[2] = { L'\0', L'\0' };
	key[0] = wcAccesskey;
#ifdef FD_ACCESSKEY_UC	// アクセスキーが大文字の場合。
	FFWcsupr_s(key, _countof(key));	// 大文字に。
#else //FD_ACCESSKEY_UC	// アクセスキーが小文字の場合。
	FFWcslwr_s(key, _countof(key));	// 小文字に。
#endif //FD_ACCESSKEY_UC	// アクセスキーが小文字の場合。

	WString wstrText;
#ifdef FD_TOP_ACCESSKEY		// 文頭にアクセスキーが有る場合。
	wstrText.Format(FD_TOP_ACCESSKEY_FORM, wcAccesskey, wcpTitle);
#else //FD_TOP_ACCESSKEY	// 文中にアクセスキーが有る場合。
	wstrText.Format(L"%s (&%c)", wcpTitle, wcAccesskey);
#endif //FD_TOP_ACCESSKEY	// 文頭にアクセスキーが有る場合。
	return wstrText;
} // FFMakeCommandText.

/*************************************************************************
 * <関数>	SetHiListByRealData
 *
 * <機能>	コンボボックスに、ＨＩ座標の表示リストを指定された
 *			実座標データ配列から設定。
 *
 * <引数>	pCmb	:コンボボックスを指定。
 *			iDataSz	:データサイズを指定。
 *			mmaData	:設定するデータを指定。
 *
 * <履歴>	00.02.26 Fukushiro M. 作成
 *			00.09.14 Fukushiro M. 同じ文字列は設定しない。
 *************************************************************************/
void SetHiListByRealData (CComboBox* pCmb, int iDataSz, const mmetol mmaData[])
{
	//----- コンボボックスの候補データ設定 -----
	WString strNum;
	wchar_t tcsBuff[200];
	pCmb->ResetContent();
	for (int iC = 0; iC < iDataSz; iC++)
	{
		if (strNum == hix2histr(tcsBuff, _countof(tcsBuff), hixroundoff(mm2hix(mmaData[iC])))) continue;
		strNum = tcsBuff;
		FFAddString(pCmb ,strNum);
	}
} // SetHiListByRealData.

/*************************************************************************
 * <関数>	SetXtmListByTmsData
 *
 * <機能>	コンボボックスに、ＨＩ座標の表示リストを指定された
 *			実座標データ配列から設定。
 *
 * <引数>	pCmb	:コンボボックスを指定。
 *			iDataSz	:データサイズを指定。
 *			tmsaData:設定するデータを指定。
 *
 * <履歴>	05.05.08 Fukushiro M. 作成
 *************************************************************************/
void SetXtmListByTmsData (CComboBox* pCmb, int iDataSz, const tmsc tmsaData[])
{
	//----- コンボボックスの候補データ設定 -----
	WString strNum;
	wchar_t tcsBuff[200];
	pCmb->ResetContent();
	for (int iC = 0; iC < iDataSz; iC++)
	{
		if (strNum == xtm2tmsstr(tcsBuff, _countof(tcsBuff), tms2xtm(tmsaData[iC]))) continue;
		strNum = tcsBuff;
		FFAddString(pCmb ,strNum);
	}
} // SetXtmListByTmsData.

/*************************************************************************
 * <関数>	SetAngleListByAngleData
 *
 * <機能>	コンボボックスに、angleの表示リストを指定されたデータ配列から設定。
 *
 * <引数>	pCmb	:コンボボックスを指定。
 *			iDataSz	:データサイズを指定。
 *			daData	:設定するデータを指定。
 *
 * <履歴>	05.04.26 Fukushiro M. 作成
 *************************************************************************/
void SetAngleListByAngleData (CComboBox* pCmb, int iDataSz, const angle daData[])
{
	//----- コンボボックスの候補データ設定 -----
	WString strNum;
	pCmb->ResetContent();
	for (int iC = 0; iC < iDataSz; iC++)
	{
		strNum.Format(L"%.4f", daData[iC]);
		FFAddString(pCmb ,strNum);
	}
} // SetAngleListByAngleData.

/*************************************************************************
 * <関数>	FFGetLBText
 *
 * <機能>	GetLBTextのUNICODE版。
 *
 * <履歴>	02.02.10 Fukushiro M. 作成
 *************************************************************************/
void FFGetLBText (const CComboBox* pCmb, int nIndex, WString& rString)
{
	CString strBuff;
	pCmb->GetLBText(nIndex, strBuff);
	rString = TStrToWStr(strBuff);
} // FFGetLBText.

//----- 09.05.09 Fukushiro M. 追加始 ()-----
/*************************************************************************
 * <関数>	FFGetLBText
 *
 * <機能>	GetLBTextのUNICODE版。
 *
 * <履歴>	09.05.09 Fukushiro M. 作成
 *************************************************************************/
void FFGetLBText (const CFCDdComboBox* pCmb, int nIndex, WString& rString)
{
	pCmb->GetLBText(nIndex, rString);
} // FFGetLBText.
//----- 09.05.09 Fukushiro M. 追加終 ()-----

/*************************************************************************
 * <関数>	FFSetItemText
 *
 * <機能>	SetItemTextのUNICODE版。
 *
 * <履歴>	02.02.10 Fukushiro M. 作成
 *************************************************************************/
BOOL FFSetItemText (CListCtrl* pCtrl, int nItem, int nSubItem, const wchar_t* lpszText)
{
	return pCtrl->SetItemText(nItem, nSubItem, WStrToTStr(lpszText));
} // FFSetItemText.

/*************************************************************************
 * <関数>	FFSetItemText
 *
 * <機能>	SetItemTextのUNICODE版。
 *
 * <履歴>	02.02.10 Fukushiro M. 作成
 *************************************************************************/
BOOL FFSetItemText (CTreeCtrl* pCtrl, HTREEITEM hItem, const wchar_t* lpszItem)
{
	return pCtrl->SetItemText(hItem, WStrToTStr(lpszItem));
} // FFSetItemText.

/*************************************************************************
 * <関数>	FFInsertColumn
 *
 * <機能>	InsertColumnのUNICODE版。
 *
 * <履歴>	02.02.10 Fukushiro M. 作成
 *************************************************************************/
int FFInsertColumn (CListCtrl* pCtrl, int nCol, const wchar_t* lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	return pCtrl->InsertColumn(nCol, WStrToTStr(lpszColumnHeading), nFormat, nWidth, nSubItem);
} // FFInsertColumn.

/*************************************************************************
 * <関数>	FFDeleteCurSel
 *
 * <機能>	選択された項目を削除する。
 *
 * <引数>	pListBox	:リストボックスを指定。
 *
 * <返値>	成功した場合はTRUE。
 *
 * <履歴>	09.06.08 Fukushiro M. 作成
 *************************************************************************/
BOOL FFDeleteCurSel (CListBox* pListBox)
{
	int iCurSel = pListBox->GetCurSel();
	if (iCurSel == LB_ERR)
		return FALSE;
	pListBox->DeleteString(iCurSel);

	// リストボックスは、スクロールバーが表示されている時に、
	// 項目が削除されてスクロールバーが非表示となっても、
	// スクロール位置を補正しないため、項目の前半がウィンドウ
	// 上方に隠れた状態となってしまう。
	// これを防ぐために、以下でスクロール位置を補正する。
	CRect rtClient;
	pListBox->GetClientRect(rtClient);
	const int iHeight = rtClient.Height();
	const int iItemHeight = pListBox->GetItemHeight(0);
	// 表示領域に表示可能な項目数を算出。
	const int iLineCount = (iHeight / iItemHeight) + ((iHeight % iItemHeight) ? 1 : 0);
	if (pListBox->GetCount() < pListBox->GetTopIndex() + iLineCount)
	//----- 最後の項目が表示可能領域の底辺より上にある場合 -----
	{
		// 表示可能領域の一番上の項目のインデックスを補正。
		int iNewTopIndex = pListBox->GetCount() - iLineCount;
		if (iNewTopIndex < 0) iNewTopIndex = 0;
		pListBox->SetTopIndex(iNewTopIndex);
	}

	// 削除された位置が選択されるようにする。
	if (pListBox->GetCount() == 0)
	{
		iCurSel = LB_ERR;
	} else
	{
		if (iCurSel == pListBox->GetCount())
			iCurSel = pListBox->GetCount() - 1;
	}
	pListBox->SetCurSel(iCurSel);
	return TRUE;
} // BOOL FFDeleteCurSel (CListBox* pListBox)

/*************************************************************************
 * <関数>	FFBringUpCurSel
 *
 * <機能>	選択された項目を一つ上に移動する。
 *
 * <引数>	pListBox	:リストボックスを指定。
 *
 * <返値>	成功した場合はTRUE。
 *
 * <履歴>	09.06.08 Fukushiro M. 作成
 *************************************************************************/
BOOL FFBringUpCurSel (CListBox* pListBox)
{
	int iSelToolbar = pListBox->GetCurSel();
	if (iSelToolbar != LB_ERR &&
		iSelToolbar != 0)
	//----- 選択されていて、且つ項目が最初ではない場合 -----
	{
		const WString wstrText = FFGetText(pListBox, iSelToolbar);
		const DWORD_PTR uiItemData = pListBox->GetItemData(iSelToolbar);
		pListBox->DeleteString(iSelToolbar);
		iSelToolbar = FFInsertString(pListBox, iSelToolbar - 1, wstrText);
		pListBox->SetItemData(iSelToolbar, uiItemData);
		pListBox->SetCurSel(iSelToolbar);
		return TRUE;
	}
	return FALSE;
} // BOOL FFBringUpCurSel (CListBox* pListBox)

/*************************************************************************
 * <関数>	FFBringDownCurSel
 *
 * <機能>	選択された項目を一つ下に移動する。
 *
 * <引数>	pListBox	:リストボックスを指定。
 *
 * <返値>	成功した場合はTRUE。
 *
 * <履歴>	09.06.08 Fukushiro M. 作成
 *************************************************************************/
BOOL FFBringDownCurSel (CListBox* pListBox)
{
	int iSelToolbar = pListBox->GetCurSel();
	if (iSelToolbar != LB_ERR &&
		iSelToolbar != pListBox->GetCount() - 1)
	//----- 選択されていて、且つ項目が最後ではない場合 -----
	{
		const WString wstrText = FFGetText(pListBox, iSelToolbar);
		const DWORD_PTR uiItemData = pListBox->GetItemData(iSelToolbar);
		pListBox->DeleteString(iSelToolbar);
		iSelToolbar = FFInsertString(pListBox, iSelToolbar + 1, wstrText);
		pListBox->SetItemData(iSelToolbar, uiItemData);
		pListBox->SetCurSel(iSelToolbar);
		return TRUE;
	}
	return FALSE;
} // BOOL FFBringDownCurSel (CListBox* pListBox)

//********************************************************************************************
/*!
 * @brief	CListCtrlの選択された行のインデックスを返す。
 *			CListCtrlはLVS_SINGLESELでなければならない。
 *
 * @author	Fukushiro M.
 * @date	2014/08/28(木) 23:32:14
 *
 * @param[in]	const CListCtrl*	pListCtrl	リストコントロール。
 *
 * @return	int	!=-1:選択インデックス/-1:非選択
 */
//********************************************************************************************
int FFGetCurSel (const CListCtrl* pListCtrl)
{
	ASSERT(FFIsFlagged(pListCtrl->GetStyle(), LVS_SINGLESEL));
//----- 14.09.16 Fukushiro M. 変更前 ()-----
//	for (int iC = 0; iC != pListCtrl->GetItemCount(); iC++)
//	{
//		if (FFIsFlagged(pListCtrl->GetItemState(iC, LVIS_SELECTED), LVIS_SELECTED))
//			return iC;
//	}
//	return -1;
//----- 14.09.16 Fukushiro M. 変更後 ()-----
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (pos == NULL) return -1;
	return pListCtrl->GetNextSelectedItem(pos);
//----- 14.09.16 Fukushiro M. 変更終 ()-----
} // int FFGetCurSel (const CListCtrl* pListCtrl)

//********************************************************************************************
/*!
 * @brief	CListCtrlの行を選択する。
 *			CListCtrlはLVS_SINGLESELでなければならない。
 *
 * @author	Fukushiro M.
 * @date	2014/08/28(木) 23:32:11
 *
 * @param[in,out]	CListCtrl*	pListCtrl	リストコントロール。
 * @param[in]		int			index		選択行のインデックス。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void FFSetCurSel (CListCtrl* pListCtrl, int index)
{
	ASSERT(FFIsFlagged(pListCtrl->GetStyle(), LVS_SINGLESEL));
	for (int iC = 0; iC != pListCtrl->GetItemCount(); iC++)
		pListCtrl->SetItemState(iC, (iC == index) ? LVIS_SELECTED|LVIS_FOCUSED : 0, LVIS_SELECTED|LVIS_FOCUSED);
} // void FFSetCurSel (CListCtrl* pListCtrl, int index)

//********************************************************************************************
/*!
 * @brief	リストコントロールのA行とB行を入れ替える。
 *			リストコントロールはレポート形式でなければならない。
 *			選択はA行からB行へ移動する。
 *
 * @author	Fukushiro M.
 * @date	2014/08/28(木) 23:50:53
 *
 * @param[in,out]	CListCtrl*	pListCtrl	リストコントロール。
 * @param[in]		int			indexA		Aの行番号。
 * @param[in]		int			indexB		Bの行番号。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void FFExchangeItem (CListCtrl* pListCtrl, int indexA, int indexB)
{
	WStringVector vItemA;
	WStringVector vItemB;
	for (int iC = 0; iC != pListCtrl->GetHeaderCtrl()->GetItemCount(); iC++)
	{
		vItemA.push_back(pListCtrl->GetItemText(indexA, iC));
		vItemB.push_back(pListCtrl->GetItemText(indexB, iC));
	}
	for (int iC = 0; iC != pListCtrl->GetHeaderCtrl()->GetItemCount(); iC++)
	{
		pListCtrl->SetItemText(indexA, iC, vItemB[iC]);
		pListCtrl->SetItemText(indexB, iC, vItemA[iC]);
	}
	// 選択を変更。
	FFSetCurSel(pListCtrl, indexB);
	// 選択が表示されるようスクロール。
	FFScrollItemVisible(pListCtrl, indexB);
} // void FFExchangeItem (CListCtrl* pListCtrl, int indexA, int indexB)

//********************************************************************************************
/*!
 * @brief	CListCtrlの指定された行が表示されるように上下方向にスクロールする。
 * @author	Fukushiro M.
 * @date	2014/07/09(水) 09:57:48
 *
 * @param[in,out]	CListCtrl*	pListCtrl	リストコントロール。
 * @param[in]		int			index		項目のインデックス。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void FFScrollItemVisible (CListCtrl* pListCtrl, int index)
{
	//
	//       ┌─A0┌…………………………………┐
	//       │    ：         rtItem(A)        ：
	//   移動│    └…………………………………┘
	//       │
	//       │    ┌─────────────┐
	//       │    │          Header          │
	//       └─→├─────────────┤
	//             │                          │
	//             │                          │
	//             │                          │
	//             │         rtClient         │
	//             │                          │
	//             │                          │
	//       ┌─→│                          │┬
	//       │    │                          ││rtItem.Height
	//   移動│    └─────────────┘┴
	//       │
	//       └─B0┌…………………………………┐
	//             ：         rtItem(B)        ：
	//             └…………………………………┘
	//

	// 表示枠を取得。
	CRect rtItem;
	pListCtrl->GetItemRect(index, rtItem, LVIR_BOUNDS);
	// クライアント領域を取得。
	CRect rtClient;
	pListCtrl->GetClientRect(rtClient);
	// ヘッダーのクライアント領域を取得。
	CRect rtHeaderClient;
	pListCtrl->GetHeaderCtrl()->GetWindowRect(rtHeaderClient);
	pListCtrl->ScreenToClient(rtHeaderClient);
	// リストコントロールのトップをヘッダーのボトムに設定。
	rtClient.top = rtHeaderClient.bottom;

	// スクロール量を計算。
	CSize szScroll(0, 0);
	if (rtItem.top < rtClient.top)
		szScroll.cy = rtItem.top - rtClient.top;
	else
	if (rtClient.bottom < rtItem.bottom)
		szScroll.cy = rtItem.top - (rtClient.bottom - rtItem.Height());

	// CListCtrl::SetItemPositionはレポートビューでは使えない。CListCtrl::Scrollを使う。
	pListCtrl->Scroll(szScroll);
} // void FFScrollItemVisible (CListCtrl* pListCtrl, int index)

/*************************************************************************
 * <関数>	FindPropPage
 *
 * <機能>	指定されたクラスのページオブジェクトを探し出す。
 *
 * <引数>	pSheet	:親シートを指定。
 *			pClass	:検索対象クラスを指定。
 *
 * <返値>	最初に見つかったクラスオブジェクトを返す。見つからない場合は
 *			NULL を返す。
 *
 * <履歴>	00.02.14 Fukushiro M. 作成
 *************************************************************************/
CPropertyPage* FindPropPage (const CPropertySheet* pSheet, CRuntimeClass* pClass)
{
	if (pSheet == NULL) return NULL;
	for (int iC = 0; iC < pSheet->GetPageCount(); iC++)
	{
		CPropertyPage* pPage = pSheet->GetPage(iC);
		if (pPage->IsKindOf(pClass)) return pPage;
	}
	return NULL;
} // FindPropPage.
//----------</#DD VIEWER NOT EXPORT>-----------

/*************************************************************************
 * <関数>	FFJustifyWindow
 *
 * <機能>	ダイアログ上の指定の座標がマウスカーソル上に来るように
 *			ダイアログを移動する。
 *
 * <引数>	hWnd		:ダイアログウィンドウを指定。
 *			hWndOffset	:ダイアログ上の基準座標の部品を指定。
 *
 * <解説>	pOffsetがNULLの場合は、ダイアログ中央がマウス位置に。
 *
 * <履歴>	01.01.22 Fukushiro M. 作成
 *************************************************************************/
void FFJustifyWindow (HWND hWnd, HWND hWndOffset)
{
	CRect rect;
	GetWindowRect(hWndOffset, rect);
	CPoint ptCenter = rect.CenterPoint();
	ScreenToClient(hWnd, &ptCenter);
	FFJustifyWindow(hWnd, &ptCenter);
} // FFJustifyWindow.

/*************************************************************************
 * <関数>	FFJustifyWindow
 *
 * <機能>	ダイアログ上の指定の座標がマウスカーソル上に来るように
 *			ダイアログを移動する。
 *
 * <引数>	hWnd	:ダイアログウィンドウを指定。
 *			pOffset	:ダイアログ上の基準座標を指定。無い場合は NULL。
 *
 * <解説>	pOffsetがNULLの場合は、ダイアログ中央がマウス位置に。
 *
 * <履歴>	00.12.31 Fukushiro M. 作成
 *************************************************************************/
void FFJustifyWindow (HWND hWnd, CPoint* pOffset)
{
	static __time32_t	stTime = 0;
	static CPoint	stptOffset(0, 0);

	// ダイアログをマウス位置に移動するか？
	if (!FFReg()->GetIsShowDlgNearMouse()) return;

	CPoint ptOffset;
	CRect rtDialog;
	GetWindowRect(hWnd, rtDialog);
	if (pOffset == NULL)
	{
		ptOffset = rtDialog.CenterPoint();
	} else
	{
		ptOffset = *pOffset;
		ClientToScreen(hWnd, &ptOffset);
	}
	// マウスカーソル位置を取得。
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	// オフセット値を計算。
	ptOffset = ptMouse - ptOffset;
	//----- 短い時間にダイアログを連続して表示する場合に、ダイアログが移動しないように -----
	// 現在の時間（秒）を取得。
	__time32_t timeNew = _time32(NULL);
	// 前回のダイアログ表示から3秒未満ならば、同じ位置にダイアログを表示。
	if (timeNew - stTime < 3)
		ptOffset = stptOffset;
	// staticの情報を更新。
	stTime = timeNew;
	stptOffset = ptOffset;
	// ダイアログ座標をマウス位置に移動。
	rtDialog += ptOffset;

	// モニターに矩形が収まるよう補正。
	rtDialog += FFJustifyToMonitor(rtDialog);

	// ダイアログを移動。
	SetWindowPos(hWnd, NULL, rtDialog.left, rtDialog.top, 0, 0,
						SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOOWNERZORDER|
						SWP_NOSENDCHANGING|SWP_NOSIZE|SWP_NOZORDER);
} // FFJustifyWindow.

//----------<#DD VIEWER NOT EXPORT>-----------
/*************************************************************************
 * <関数>	FFUpdateTextComboList
 *
 * <機能>	コンボボックスのテキスト枠のテキストを、リスト最上位に追加する。
 *			リスト中に同じテキストがある場合は削除。
 *
 * <引数>	pBox	:リストボックスを指定。
 *
 * <履歴>	01.01.23 Fukushiro M. 作成
 *************************************************************************/
void FFUpdateTextComboList (CComboBox* pBox)
{
	WString wstrText;
	FFGetWindowText(pBox, wstrText);
//----- 05.06.24 Fukushiro M. 変更前 ()-----
//	int iIndex = pBox->FindStringExact(-1, WStrToTStr(wstrText));
//----- 05.06.24 Fukushiro M. 変更後 ()-----
	// CComboBox::FindStringExactは、大文字と小文字を区別せず検索するため使えない。
	int iIndex;
	for (iIndex = 0; iIndex != pBox->GetCount(); iIndex++)
	{
		WString wstrList;
		FFGetLBText(pBox, iIndex, wstrList);
		if (wstrList == wstrText) break;
	}
//----- 05.06.24 Fukushiro M. 変更終 ()-----
	// リスト途中に同じ文字列がある場合は削除。
	if (iIndex != pBox->GetCount())
		pBox->DeleteString(iIndex);
	// リストトップに文字列挿入。
	pBox->InsertString(0, WStrToTStr(wstrText));
	pBox->SetCurSel(0);
} // FFUpdateTextComboList.

/*************************************************************************
 * <関数>	FFGetDlgItemText
 *
 * <機能>	コントロールからUNICODEで文字列を取得する。
 *
 * <引数>	pWnd	:ダイアログのウィンドウを指定。
 *			dwCtrl	:コントロールIDを指定。
 *			str		:文字列を返す。
 *
 * <履歴>	01.04.18 Fukushiro M. 作成
 *************************************************************************/
// 14.08.18 Fukushiro M. 1行変更 ()
//void FFGetDlgItemText (CWnd* pWnd, DWORD dwCtrl, WString& wstr)
void FFGetDlgItemText (const CWnd* pWnd, DWORD dwCtrl, WString& wstr)
{
	CString str;
	pWnd->GetDlgItemText(dwCtrl, str);
// 02.06.25 Fukushiro M. 1行変更 (SJISにないUNICODE文字列の貼り付けを可能にする)
//	wstr = TStrToWStr(str);
	wstr = TStrToWStr(str);
} // FFGetDlgItemText.

/*************************************************************************
 * <関数>	FFGetDlgItemText
 *
 * <機能>	コントロールからUNICODEで文字列を取得する。
 *
 * <引数>	hwnd	:ダイアログのウィンドウを指定。
 *			dwCtrl	:コントロールIDを指定。
 *			str		:文字列を返す。
 *
 * <履歴>	06.03.20 Fukushiro M. 作成
 *************************************************************************/
void FFGetDlgItemText (HWND hwnd, DWORD dwCtrl, WString& wstr)
{
	FFGetWindowText(::GetDlgItem(hwnd, dwCtrl), wstr, CP_ACP);
} // FFGetDlgItemText.

//----------</#DD VIEWER NOT EXPORT>-----------

/*************************************************************************
 * <関数>	FFSetDlgItemText
 *
 * <機能>	コントロールにUNICODEで文字列を設定する。
 *
 * <引数>	pWnd	:ダイアログのウィンドウを指定。
 *			dwCtrl	:コントロールIDを指定。
 *			wcpText	:文字列を指定。
 *
 * <履歴>	01.04.18 Fukushiro M. 作成
 *************************************************************************/
void FFSetDlgItemText (CWnd* pWnd, DWORD dwCtrl, const wchar_t* wcpText)
{
	pWnd->SetDlgItemText(dwCtrl, WStrToTStr(wcpText));
} // FFSetDlgItemText.

/*************************************************************************
 * <関数>	FFSetDlgItemText
 *
 * <機能>	コントロールにUNICODEで文字列を設定する。
 *
 * <引数>	hwnd	:ダイアログのウィンドウを指定。
 *			dwCtrl	:コントロールIDを指定。
 *			wcpText	:文字列を指定。
 *
 * <履歴>	05.10.11 Fukushiro M. 作成
 *************************************************************************/
void FFSetDlgItemText (HWND hwnd, DWORD dwCtrl, const wchar_t* wcpText)
{
	SetDlgItemText(hwnd, dwCtrl, WStrToTStr(wcpText));
} // FFSetDlgItemText.

//----- 07.10.30 Fukushiro M. 追加始 ()-----
/*************************************************************************
 * <関数>	FFSetDlgItemTextEx
 *
 * <機能>	コントロールにUNICODEで文字列を設定する。
 *
 * <引数>	hwnd	:ダイアログのウィンドウを指定。
 *			dwCtrl	:コントロールIDを指定。
 *			wcpText	:文字列を指定。
 *
 * <履歴>	07.10.30 Fukushiro M. 作成
 *************************************************************************/
void FFSetDlgItemTextEx (HWND hwnd, DWORD dwCtrl, const wchar_t* wcpText)
{
	const CString str = WStrToTStr(wcpText);
	HWND hwndItem = GetDlgItem(hwnd, dwCtrl);
	HWND hwndFocus = GetFocus();
	if (hwndItem == hwndFocus)
	//----- フォーカス中のコントロールを修正する場合 -----
	{
		// 例えば最小値が0.001の欄に0.02を入力する場合を考える。
		// "0.0"まで入力した場合、カレット位置は "0.0|" となっているが、次の瞬間に
		// この関数を使って最小値が補完されるので"0.001"に変わる。このとき、カレット位置
		// から右を全て選択されれば"0.001"となり、続けて"2"を入力すればOK。
		//                             ~~
		// 便利だが、自身のコントロールのEN_CHANGEのイベント処理中に範囲選択をすると
		// 問題が生じる場合がある。
		// 例えば"10"の"1"をDELキーで削除した場合、"0.001"の全体が選択されるが、
		// なぜか次の瞬間に全テキストが削除されてしまう。
		// これを防ぐため、範囲選択EM_SETSELはPostMessageで送信する。

		// 現在のカレット位置を取得。
		DWORD dwSel1;
		DWORD dwSel2;
		SendMessage(hwndItem, EM_GETSEL, (WPARAM)&dwSel1, (LPARAM)&dwSel2);
		const int iCurPos = GetSmallest((int)dwSel1, (int)dwSel2);
		// テキストを設定。
		SetDlgItemText(hwnd, dwCtrl, str);
		// 現在のカレット位置が新しいテキスト中に収まるなら、現在位置〜終端までを選択状態。
//----- 14.07.17 Fukushiro M. 変更前 ()-----
//		if (0 <= iCurPos && iCurPos <= str.GetLength())
//			SendMessage(hwndItem, EM_SETSEL, (WPARAM)iCurPos, (LPARAM)str.GetLength());
//----- 14.07.17 Fukushiro M. 変更後 ()-----
		if (0 <= iCurPos && iCurPos <= str.GetLength())
			PostMessage(hwndItem, EM_SETSEL, (WPARAM)iCurPos, (LPARAM)str.GetLength());
//----- 14.07.17 Fukushiro M. 変更終 ()-----
	} else
	{
		// テキストを設定。
		SetDlgItemText(hwnd, dwCtrl, str);
	}
} // FFSetDlgItemTextEx.

/*************************************************************************
 * <関数>	FFSetDlgItemComboTextEx
 *
 * <機能>	コントロールにUNICODEで文字列を設定する。
 *
 * <引数>	hwnd	:ダイアログのウィンドウを指定。
 *			dwCtrl	:コントロールIDを指定。
 *			wcpText	:文字列を指定。
 *
 * <履歴>	07.10.30 Fukushiro M. 作成
 *************************************************************************/
void FFSetDlgItemComboTextEx (HWND hwnd, DWORD dwCtrl, const wchar_t* wcpText)
{
	const CString str = WStrToTStr(wcpText);
	HWND hwndItem = GetDlgItem(hwnd, dwCtrl);
	// コンボボックスからエディットボックスを取得。
	COMBOBOXINFO cbi;
	memset(&cbi, 0, sizeof(cbi));
	cbi.cbSize = sizeof(cbi);
	GetComboBoxInfo(hwndItem, &cbi);
	HWND hwndFocus = GetFocus();
	if (cbi.hwndItem == hwndFocus)
	//----- フォーカス中のコントロールを修正する場合 -----
	{
		// 例えば最小値が0.001の欄に0.02を入力する場合を考える。
		// "0.0"まで入力した場合、カレット位置は "0.0|" となっているが、次の瞬間に
		// この関数を使って最小値が補完されるので"0.001"に変わる。このとき、カレット位置
		// から右を全て選択されれば"0.001"となり、続けて"2"を入力すればOK。
		//                             ~~
		// 便利だが、自身のコントロールのCBN_EDITCHANGEのイベント処理中に範囲選択をすると
		// 問題が生じる場合がある。
		// 例えば"10"の"1"をDELキーで削除した場合、"0.001"の全体が選択されるが、
		// なぜか次の瞬間に全テキストが削除されてしまう。
		// これを防ぐため、範囲選択CB_SETEDITSELはPostMessageで送信する。

		// 現在のカレット位置を取得。
		DWORD dwSel1;
		DWORD dwSel2;
		SendMessage(hwndItem, CB_GETEDITSEL, (WPARAM)&dwSel1, (LPARAM)&dwSel2);
		const int iCurPos = GetSmallest((int)dwSel1, (int)dwSel2);
		// テキストを設定。
		SetDlgItemText(hwnd, dwCtrl, str);
		// 現在のカレット位置が新しいテキスト中に収まるなら、現在位置〜終端までを選択状態。
//----- 14.07.17 Fukushiro M. 変更前 ()-----
//		if (0 <= iCurPos && iCurPos <= str.GetLength())
//			SendMessage(hwndItem, CB_SETEDITSEL, (WPARAM)0, MAKELPARAM(iCurPos, str.GetLength()));
//----- 14.07.17 Fukushiro M. 変更後 ()-----
		if (0 <= iCurPos && iCurPos <= str.GetLength())
			PostMessage(hwndItem, CB_SETEDITSEL, (WPARAM)0, MAKELPARAM(iCurPos, str.GetLength()));
//----- 14.07.17 Fukushiro M. 変更終 ()-----
	} else
	{
		// テキストを設定。
		SetDlgItemText(hwnd, dwCtrl, str);
	}
} // FFSetDlgItemComboTextEx.
//----- 07.10.30 Fukushiro M. 追加終 ()-----

#endif

//********************************************************************************************
/*!
 * @brief	EditコントロールにUndoを実行する。
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 09:51:25
 *
 * @param[in]	CEdit*							edit			Undo対象のEditコントロール。
 * @param[in]	const std::vector<FCDiffRecW>&	vUndoBuffer		Undoバッファ。
 * @param[in]	int								iCurUndoBuffer	Undoバッファの現在位置。Undo対象の
 *																バッファの1つ後を指す。
 *
 * @return	int	新しいUndoバッファの位置。
 */
//********************************************************************************************
int UtilDlg::UndoEdit (CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer, int iCurUndoBuffer)
{
	if (iCurUndoBuffer == 0) return iCurUndoBuffer;
	iCurUndoBuffer--;
	const FCDiffRecW& rec = vUndoBuffer[iCurUndoBuffer];
	if (rec.cmd == FCDiffRecW::INS)
	//----- 挿入の場合 -----
	{
		edit->SetSel(rec.begin, rec.begin + rec.text.length());
		edit->ReplaceSel(L"");
	} else
	//----- 削除の場合 -----
	{
		edit->SetSel(rec.begin, rec.begin);
		edit->ReplaceSel(rec.text.c_str());
	}
	return iCurUndoBuffer;
} // FFUndoEdit

//********************************************************************************************
/*!
 * @brief	EditコントロールにRedoを実行する。
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 09:51:25
 *
 * @param[in]	CEdit*							edit			Redo対象のEditコントロール。
 * @param[in]	const std::vector<FCDiffRecW>&	vUndoBuffer		Undoバッファ。
 * @param[in]	int								iCurUndoBuffer	Undoバッファの現在位置。Redo対象の
 *																バッファの1つ前を指す。
 *
 * @return	int	新しいUndoバッファの位置。
 */
//********************************************************************************************
int UtilDlg::RedoEdit (CEdit* edit, const std::vector<FCDiffRecW>& vUndoBuffer, int iCurUndoBuffer)
{
	if (vUndoBuffer.size() == iCurUndoBuffer) return iCurUndoBuffer;
	const FCDiffRecW& rec = vUndoBuffer[iCurUndoBuffer];
	if (rec.cmd == FCDiffRecW::INS)
	//----- 挿入の場合 -----
	{
		edit->SetSel(rec.begin, rec.begin);
		edit->ReplaceSel(rec.text.c_str());
	} else
	//----- 削除の場合 -----
	{
		edit->SetSel(rec.begin, rec.begin + rec.text.length());
		edit->ReplaceSel(L"");
	}
	return iCurUndoBuffer + 1;
} // FFRedoEdit

#if 0
//----- 10.02.18 Fukushiro M. 削除始 ()-----
///*************************************************************************
// * <関数>	FFInsertDlgItemComboTextEx
// *
// * <機能>	コンボエディットボックスコントロールのカレット位置に文字列を
// *			挿入する。
// *
// * <引数>	hwnd	:ダイアログのウィンドウを指定。
// *			dwCtrl	:コンボエディットボックスコントロールIDを指定。
// *			wcpText	:文字列を指定。
// *
// * <解説>	コンボエディットボックスのテキストを編集中の場合は、カレット
// *			位置にテキストを挿入する。編集中以外の場合は、最後に追加する。
// *
// * <履歴>	10.02.17 Fukushiro M. 作成
// *************************************************************************/
//void FFInsertDlgItemComboTextEx (HWND hwnd, DWORD dwCtrl, const wchar_t* wcpText)
//{
//	const CString str = WStrToTStr(wcpText);
//	// コンボボックスのハンドルを取得。
//	HWND hwndItem = GetDlgItem(hwnd, dwCtrl);
//	// コンボボックスからエディットボックスを取得。
//	COMBOBOXINFO cbi;
//	memset(&cbi, 0, sizeof(cbi));
//	cbi.cbSize = sizeof(cbi);
//	GetComboBoxInfo(hwndItem, &cbi);
//	if (cbi.hwndItem != NULL)
//	{
//		// 現在のカレット位置を取得。
//		DWORD dwSel1;
//		DWORD dwSel2;
//		SendMessage(hwndItem, CB_GETEDITSEL, (WPARAM)&dwSel1, (LPARAM)&dwSel2);
//		const int iCurPos = GetSmallest((int)dwSel1, (int)dwSel2);
//		SendMessage(cbi.hwndItem, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)(const _TCHAR*)str);
//	} else
//	{
//		// テキストを設定。
////		SetDlgItemText(hwnd, dwCtrl, str);
//	}
//} // FFInsertDlgItemComboTextEx.
//----- 10.02.18 Fukushiro M. 削除終 ()-----

/*************************************************************************
 * <関数>	FFPopupSubmenu
 *
 * <機能>	指定のサブメニューをポップアップする。
 *
 * <引数>	scpt		:ポップアップする位置（通常はマウス位置）を指定。
 *						 スクリーン上の座標値で指定。
 *			dwMenuId	:メニューIDを指定。
 *			iSubmenuIdx	:サブメニューのインデックスを指定。dwMenuIdの
 *						 何番目か？
 *			pParent		:メニューのコマンドメッセージ送付先を指定。
 *			dwTopCommand:優先するコマンドIDを指定。メニューのトップに
 *						 表示される。指定しない場合は DWORD(-1)を設定。
 *
 * <解説>	メニューのフォントを計算し、マウスがメニュー最初のアイテム上に
 *			来るようにメニューをポップアップする。
 *
 * <履歴>	01.04.24 Fukushiro M. 作成
 *************************************************************************/
void FFPopupSubmenu (CPoint scpt, DWORD dwMenuId, int iSubmenuIdx, CWnd* pParent, DWORD dwTopCommand)
{
	CMenu menu;
	if (menu.LoadMenu(dwMenuId))
	{
//----- 06.02.24 Fukushiro M. 追加始 ()-----
		// フレーム以外のウィンドウを指定する場合は、OnCmdMsgの実行を登録する。
		if (pParent != NULL && pParent != FFMainWnd()->GetWnd())
			FFMainWnd()->SetOnCmdMsgWnd(pParent->m_hWnd);
//----- 06.02.24 Fukushiro M. 追加終 ()-----

		CMenu* pPopup = menu.GetSubMenu(iSubmenuIdx);
		ASSERT(pPopup != NULL);
		if (dwTopCommand != DWORD(-1))
		{
			//----- メニュー文字列を取得 -----
			_TCHAR tsz[250] = { _T('\0') };
			MENUITEMINFO mii;
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE;
			mii.dwTypeData = tsz;
// 08.05.20 Fukushiro M. 1行変更 ()
//			mii.cch = sizeof(tsz) / sizeof(tsz[0]);
			mii.cch = _countof(tsz);
			if (pPopup->GetMenuItemInfo(dwTopCommand, &mii) && tsz[0] != _T('\0'))
			{
#ifdef FD_TOP_ACCESSKEY		// 文頭にアクセスキーが有る場合。
				// メニュー文字列中の"&X."を検索。
				WString wstrText = TStrToWStr(tsz);
				if (wstrText.GetAt(0) == L'&')
				{
					// 先頭のアクセスキーを除く。
					wstrText = wstrText.Mid(FD_TOP_ACCESSKEY_LEN);
					// サブメニューのトップに "(&X)"を除いたメニューを挿入。
					pPopup->InsertMenu(0, MF_STRING | MF_BYPOSITION, dwTopCommand, WStrToTStr(wstrText));
				}
#else //FD_TOP_ACCESSKEY	// 文中にアクセスキーが有る場合。
				// メニュー文字列中の"(&"文字列の位置を検索。
				_TCHAR* tcpFind = _tcsstr(tsz, _T("(&"));
				if (tcpFind != NULL)
				{
					// サブメニューのトップに "(&X)"を除いたメニューを挿入。
					*tcpFind = _T('\0');
					pPopup->InsertMenu(0, MF_STRING | MF_BYPOSITION, dwTopCommand, tsz);
				}
#endif //FD_TOP_ACCESSKEY	// 文頭にアクセスキーが有る場合。
			}
		}

//----------<#DD VIEWER NOT EXPORT>-----------
		// 左利きに設定ならば、メニューを変更。
		if (FFReg()->GetIsLeftHand())
			::ChangeHanded(pPopup->m_hMenu);
//----- 14.09.12 Fukushiro M. 追加始 ()-----
		const ACCEL* pAccel;
		long lAccel;
		FFGetUserAccelerator(pAccel, lAccel);
//----- 14.09.12 Fukushiro M. 追加終 ()-----
		// ショートカットキー情報を書き込む。
		::FFAddShortcutText(pPopup->m_hMenu, pAccel, lAccel);
//----------</#DD VIEWER NOT EXPORT>-----------

		//---- GUIフォントのサイズからオフセットを計算 -----
		CFont font;
		font.CreateStockObject(DEFAULT_GUI_FONT);
//----- 06.05.22 Fukushiro M. 変更前 ()-----
//		LOGFONT lf = {0};
//----- 06.05.22 Fukushiro M. 変更後 ()-----
		// memset で初期化すること。 = {0} ではうまく初期化できない場合があった。
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
//----- 06.05.22 Fukushiro M. 変更終 ()-----

		font.GetLogFont(&lf);
		scpt.x -= 3 * labs(lf.lfHeight);
		scpt.y -= 2 + labs(lf.lfHeight) / 2;	// 最初の2は、メニューのフレーム幅。
		//----- メニューがフレームをはみ出さないように調整 -----

		// ウインドウ最大にしてチップストア左はじでマウス右クリックで
		// メニューが変な位置に表示されるのを修正。
		scpt += FFJustifyToMonitor(scpt);
// 06.02.24 Fukushiro M. 1行変更 ()
//		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, scpt.x, scpt.y, pParent);
		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, scpt.x, scpt.y, FFMainWnd()->GetWnd());

//----- 06.03.09 Fukushiro M. 追加始 ()-----
		// OnCmdMsgの登録を解除する。
		// PostMessageで送付することで、コマンド選択後にキャンセルさせることができる。
		FFMainWnd()->PostMessage(WM_COMMAND, ID_RBTN_END_MARK, 0);
//----- 06.03.09 Fukushiro M. 追加終 ()-----

//----- 06.02.25 Fukushiro M. 削除始 ()-----
////----- 06.02.24 Fukushiro M. 追加始 ()-----
//		// OnCmdMsgの登録を解除する。
//		if (pParent != NULL && pParent != FFMainWnd()->GetWnd())
//			FFMainWnd()->ResetOnCmdMsgWnd(pParent);
////----- 06.02.24 Fukushiro M. 追加終 ()-----
//----- 06.02.25 Fukushiro M. 削除終 ()-----
	}


} // FFPopupSubmenu.

/*************************************************************************
 * <関数>	FFPopupSubmenu
 *
 * <機能>	指定のサブメニューをポップアップする。
 *
 * <引数>	mmpt		:ポップアップする位置（通常はマウス位置）を指定。
 *			dwMenuId	:メニューIDを指定。
 *			iSubmenuIdx	:サブメニューのインデックスを指定。dwMenuIdの
 *						 何番目か？
 *			pParent		:メニューのコマンドメッセージ送付先を指定。
 *			dwTopCommand:優先するコマンドIDを指定。メニューのトップに
 *						 表示される。指定しない場合は DWORD(-1)を設定。
 *
 * <解説>	メニューのフォントを計算し、マウスがメニュー最初のアイテム上に
 *			来るようにメニューをポップアップする。
 *
 * <履歴>	01.04.24 Fukushiro M. 作成
 *************************************************************************/
void FFPopupSubmenu (const DBLPoint& mmpt, DWORD dwMenuId, int iSubmenuIdx, CWnd* pParent, DWORD dwTopCommand)
{
	// デバイス座標。
	CPoint scpt = mm2lg(mmpt);
	// デバイス座標へ変換。
	FFView()->LPtoDP(scpt);
	// スクリーン座標へ変換。
	FFView()->ClientToScreen(&scpt);
	FFPopupSubmenu(scpt, dwMenuId, iSubmenuIdx, pParent, dwTopCommand);
} // FFPopupSubmenu.

//----------<#DD VIEWER NOT EXPORT>-----------

/*************************************************************************
 * <関数>	myExtractMenuCommand
 *
 * <機能>	メニューからコマンドIDの一覧を取り出す。
 *
 * <履歴>	01.05.06 Fukushiro M. 作成
 *************************************************************************/
static void myExtractMenuCommand (DWORDSet& stCommandId, HMENU hMenu)
{
	MENUITEMINFO mii;
	// メニュー数を取得。
	const int iMenuCount = GetMenuItemCount(hMenu);
	for (int imi = 0; imi < iMenuCount; imi++)
	{
		//----- メニュー文字列とサブメニューハンドルを取得 -----
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID | MIIM_SUBMENU;
		GetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
		// サブメニューがある場合は再帰呼び出し。
		if (mii.hSubMenu != NULL)
			myExtractMenuCommand(stCommandId, mii.hSubMenu);
		else
			stCommandId.insert(mii.wID);
	}
} // myExtractMenuCommand.

/*************************************************************************
 * <関数>	FFInitKeyboardTable
 *
 * <機能>	キーテーブル（ショートカットキー、1文字メニュー）の初期化を
 *			行う。
 *
 * <履歴>	01.05.05 Fukushiro M. 作成
 *************************************************************************/
void FFInitKeyboardTable (const wchar_t* tcpPath, HMENU hMenu, HMENU hScMenu)
{
	static const char* s_cpDefaultTable =
		"<handed exchange>\n"
		"Q->P\n"
		"W->O\n"
		"E->I\n"
		"R->U\n"
		"T->Y\n"
		"A->;\n"
		"S->L\n"
		"D->K\n"
		"F->J\n"
		"G->H\n"
		"Z->/\n"
		"X->.\n"
		"C->,\n"
		"V->M\n"
		"B->N\n"
		"1->=\n"
		"2->-\n"
		"3->0\n"
		"4->9\n"
		"5->8\n"
		"6->7\n"
		"</handed exchange>\n"
		"<right shortcut key>\n"
		"ESCAPE,		TLCMD_CANCEL\n"
		"BACK,			EDIT_BACKSPACE\n"
		"CTRL+SPACE,	OPTION_NEXT_SNAP_METHOD\n"
		"SHIFT+SPACE,	OPTION_IS_ARROW_GAP_EFFECTIVE\n"
		"ALT+SPACE,		OPTION_POP_HINT_ENABLE\n"
		"ALT+LEFT,		OBJECT_MOVE_SNAP_LEFT\n"
		"ALT+UP,		OBJECT_MOVE_SNAP_UP\n"
		"ALT+RIGHT,		OBJECT_MOVE_SNAP_RIGHT\n"
		"ALT+DOWN,		OBJECT_MOVE_SNAP_DOWN\n"

		"LEFT,			OBJECT_MOVE_SNAP_LEFT\n"
		"UP,			OBJECT_MOVE_SNAP_UP\n"
		"RIGHT,			OBJECT_MOVE_SNAP_RIGHT\n"
		"DOWN,			OBJECT_MOVE_SNAP_DOWN\n"
		"CTRL+LEFT,		VIEW_PAN_LEFT\n"
		"CTRL+UP,		VIEW_PAN_UP\n"
		"CTRL+RIGHT,	VIEW_PAN_RIGHT\n"
		"CTRL+DOWN,		VIEW_PAN_DOWN\n"
		"CTRL+ALT+LEFT,	VIEW_PAN_LEFT\n"
		"CTRL+ALT+UP,	VIEW_PAN_UP\n"
		"CTRL+ALT+RIGHT,VIEW_PAN_RIGHT\n"
		"CTRL+ALT+DOWN,	VIEW_PAN_DOWN\n"

		"CTRL+C,		EDIT_COPY\n"
		"CTRL+INSERT,	EDIT_COPY\n"
		"SHIFT+DELETE,	EDIT_CUT\n"
		"CTRL+X,		EDIT_CUT\n"
		"DELETE,		EDIT_DELETE\n"
		"CTRL+V,		EDIT_PASTE\n"
		"SHIFT+INSERT,	EDIT_PASTE\n"
		"CTRL+R,		EDIT_REDO\n"
		"CTRL+Y,		EDIT_REDO\n"
		"CTRL+A,		EDIT_SELECT_ALL\n"
		"ALT+BACK,		EDIT_UNDO\n"
		"CTRL+Z,		EDIT_UNDO\n"
		"CTRL+Q,		EDIT_SELECT_NEXT\n"
		"CTRL+S,		FILE_COMEDT_SAVE_EXIT\n"
		"CTRL+S,		FILE_SAVE_HIDOCUMENT\n"
		"CTRL+S,		FILE_SAVE\n"
		"CTRL+S,		FILE_UPDATE\n"
		"CTRL+R,		HTREE_MOVE_AFTER\n"
		"ALT+BACK,		HTREE_MOVE_BEFORE\n"
		"CTRL+Z,		HTREE_MOVE_BEFORE\n"
		"CTRL+W,		HTREE_MOVE_OPERATION_BEFORE\n"
		"CTRL+E,		HTREE_MOVE_OPERATION_AFTER\n"
		"F1,			HELP\n"					// メニューにF1を表示させるため。無くても動作する。
		"F2,			EDIT_FIND_TEXT\n"
		"CTRL+F,		EDIT_FIND_TEXT\n"
		"F3,			EDIT_FIND_NEXT_TEXT\n"
		"F4,			EDIT_FIND_PREV_TEXT\n"
		"F5,			EDIT_REPLACE_TEXT\n"
		"F6,			EDIT_REPLACE_NEXT_TEXT\n"
		"F7,			EDIT_REPLACE_PREV_TEXT\n"
		"F8,			EDIT_COPY\n"
		"F9,			EDIT_PASTE\n"
		"F10,			EDIT_SELECT_NEXT\n"
		"F11,			OBJECT_CONNECT\n"
		"F12,			OBJECT_DISCONNECT\n"
		"SHIFT+F1,		CONTEXT_HELP\n"
		"SHIFT+F2,		FORMAT_FONT\n"
		"SHIFT+F3,		TEXT_ALIGN_TOP\n"
		"SHIFT+F4,		TEXT_ALIGN_V_CENTER\n"
		"SHIFT+F5,		TEXT_ALIGN_BOTTOM\n"
		"SHIFT+F6,		TEXT_ALIGN_LEFT\n"
		"SHIFT+F7,		TEXT_ALIGN_CENTER\n"
		"SHIFT+F8,		TEXT_ALIGN_RIGHT\n"
		"SHIFT+F9,		TEXT_ALIGN_LEFTRIGHT\n"
		"SHIFT+F10,		TEXT_MARGIN\n"
		"SHIFT+F11,		TEXT_LINE\n"
		"SHIFT+F12,		TEXT_ROTATE_BACKGROUND_COLOR\n"
		"CTRL+F1,		OBJECT_SAME_SIZE_X\n"
		"CTRL+F2,		OBJECT_SAME_SIZE_Y\n"
		"CTRL+F3,		OBJECT_ALIGN_H_TOP\n"
		"CTRL+F4,		OBJECT_ALIGN_H_CENTER\n"
		"CTRL+F5,		OBJECT_ALIGN_H_BOTTOM\n"
		"CTRL+F6,		OBJECT_ALIGN_V_LEFT\n"
		"CTRL+F7,		OBJECT_ALIGN_V_CENTER\n"
		"CTRL+F8,		OBJECT_ALIGN_V_RIGHT\n"
		"CTRL+F9,		OBJECT_SAME_GAP_V\n"
		"CTRL+F10,		OBJECT_SAME_GAP_H\n"
		"CTRL+F11,		OBJECT_SAME_DIST_V\n"
		"CTRL+F12,		OBJECT_SAME_DIST_H\n"
		"ALT+F1,		VIEW_FUNCTION_KEY_BAR\n"
		"ALT+F2,		VIEW_COPY_POSITION\n"
		"ALT+F3,		VIEW_PASTE_POSITION\n"
		// ALT+F4 はアプリ終了用なので使わない。
		"ALT+F5,		OBJECT_SIZE_TO_TEXT\n"
		"ALT+F6,		EDIT_REPLACE_TEXT_ALL\n"
		"ALT+F7,		OBJECT_ROTATE_RIGHT\n"
		"ALT+F8,		OBJECT_ROTATE_LEFT\n"
		"ALT+F9,		OBJECT_MIRROR_VERT\n"
		"ALT+F10,		OBJECT_MIRROR_HORZ\n"
		"ALT+F11,		EDIT_UNDO\n"
		"ALT+F12,		EDIT_REDO\n"
		"SHIFT+CTRL+F1,	VIEW_CHIP_STORE_VISIBLE\n"
		"SHIFT+CTRL+F2,	OPTION_AUTO_ALTKEY\n"
		"SHIFT+CTRL+F3,	OBJECT_GROUP\n"
		"SHIFT+CTRL+F4,	OBJECT_UNGROUP\n"
		"SHIFT+CTRL+F5,	VIEW_REDRAW\n"
		"SHIFT+CTRL+F6,	VIEW_ZOOMIN\n"
		"SHIFT+CTRL+F7,	VIEW_ZOOMOUT\n"
		"SHIFT+CTRL+F8,	EDIT_SELECT_ALL\n"
		"SHIFT+CTRL+F9,	EDIT_CLEAR_SELECT\n"
		"SHIFT+CTRL+F10,FILE_SHEET_SETUP\n"
		"SHIFT+CTRL+F11,FILE_PAGE_SETUP\n"
		"SHIFT+CTRL+F12,FILE_PRINT_PREVIEW\n"
		"CTRL+ALT+F6,	GRAPHICS_EDIT_COLOR_TABLE\n"
		"CTRL+ALT+F7,	GRAPHICS_EDIT_REGPEN_TABLE\n"
		"CTRL+ALT+F8,	GRAPHICS_EDIT_REGBRUSH_TABLE\n"
		"CTRL+ALT+F9,	GRAPHICS_EDIT_PICTURE_TABLE\n"
		"CTRL+ALT+F10,	GRAPHICS_EDIT_PICT_ARROW\n"
		"CTRL+ALT+F11,	GRAPHICS_EDIT_CHIP_GROUP\n"
		"CTRL+ALT+F12,	GRAPHICS_EDIT_CHIP\n"
		"SHIFT+ALT+F9,	OPTION_IS_GRID_VISIBLE\n"
		"SHIFT+ALT+F10,	OPTION_IS_PAGE_VISIBLE\n"
		"</right shortcut key>\n"
		"<left shortcut key>\n"
		"ESCAPE,		TLCMD_CANCEL\n"
		"BACK,			EDIT_BACKSPACE\n"
		"CTRL+SPACE,	OPTION_NEXT_SNAP_METHOD\n"
		"SHIFT+SPACE,	OPTION_IS_ARROW_GAP_EFFECTIVE\n"
		"ALT+SPACE,		OPTION_POP_HINT_ENABLE\n"
		"ALT+LEFT,		OBJECT_MOVE_SNAP_LEFT\n"
		"ALT+UP,		OBJECT_MOVE_SNAP_UP\n"
		"ALT+RIGHT,		OBJECT_MOVE_SNAP_RIGHT\n"
		"ALT+DOWN,		OBJECT_MOVE_SNAP_DOWN\n"

		"LEFT,			OBJECT_MOVE_SNAP_LEFT\n"
		"UP,			OBJECT_MOVE_SNAP_UP\n"
		"RIGHT,			OBJECT_MOVE_SNAP_RIGHT\n"
		"DOWN,			OBJECT_MOVE_SNAP_DOWN\n"
		"CTRL+LEFT,		VIEW_PAN_LEFT\n"
		"CTRL+UP,		VIEW_PAN_UP\n"
		"CTRL+RIGHT,	VIEW_PAN_RIGHT\n"
		"CTRL+DOWN,		VIEW_PAN_DOWN\n"
		"CTRL+ALT+LEFT,	VIEW_PAN_LEFT\n"
		"CTRL+ALT+UP,	VIEW_PAN_UP\n"
		"CTRL+ALT+RIGHT,VIEW_PAN_RIGHT\n"
		"CTRL+ALT+DOWN,	VIEW_PAN_DOWN\n"

		"CTRL+J,		EDIT_COPY\n"
		"CTRL+INSERT,	EDIT_COPY\n"
		"SHIFT+DELETE,	EDIT_CUT\n"
		"CTRL+K,		EDIT_CUT\n"
		"DELETE,		EDIT_DELETE\n"
		"CTRL+L,		EDIT_PASTE\n"
		"SHIFT+INSERT,	EDIT_PASTE\n"
		"CTRL+P,		EDIT_REDO\n"
		"CTRL+Y,		EDIT_REDO\n"
		"CTRL+M,		EDIT_SELECT_ALL\n"
		"ALT+BACK,		EDIT_UNDO\n"
		"CTRL+O,		EDIT_UNDO\n"
		"CTRL+/,		EDIT_SELECT_NEXT\n"
		"CTRL+I,		FILE_COMEDT_SAVE_EXIT\n"
		"CTRL+I,		FILE_SAVE_HIDOCUMENT\n"
		"CTRL+I,		FILE_SAVE\n"
		"CTRL+I,		FILE_UPDATE\n"
		"CTRL+P,		HTREE_MOVE_AFTER\n"
		"CTRL+Y,		HTREE_MOVE_AFTER\n"
		"ALT+BACK,		HTREE_MOVE_BEFORE\n"
		"CTRL+O,		HTREE_MOVE_BEFORE\n"
		"CTRL+U,		HTREE_MOVE_OPERATION_BEFORE\n"
		"CTRL+N,		HTREE_MOVE_OPERATION_AFTER\n"
		"F1,			HELP\n"					// メニューにF1を表示させるため。無くても動作する。
		"F2,			EDIT_FIND_TEXT\n"
		"CTRL+F,		EDIT_FIND_TEXT\n"
		"F3,			EDIT_FIND_NEXT_TEXT\n"
		"F4,			EDIT_FIND_PREV_TEXT\n"
		"F5,			EDIT_REPLACE_TEXT\n"
		"F6,			EDIT_REPLACE_NEXT_TEXT\n"
		"F7,			EDIT_REPLACE_PREV_TEXT\n"
		"F8,			EDIT_COPY\n"
		"F9,			EDIT_PASTE\n"
		"F10,			EDIT_SELECT_NEXT\n"
		"F11,			OBJECT_CONNECT\n"
		"F12,			OBJECT_DISCONNECT\n"
		"SHIFT+F1,		CONTEXT_HELP\n"
		"SHIFT+F2,		FORMAT_FONT\n"
		"SHIFT+F3,		TEXT_ALIGN_TOP\n"
		"SHIFT+F4,		TEXT_ALIGN_V_CENTER\n"
		"SHIFT+F5,		TEXT_ALIGN_BOTTOM\n"
		"SHIFT+F6,		TEXT_ALIGN_LEFT\n"
		"SHIFT+F7,		TEXT_ALIGN_CENTER\n"
		"SHIFT+F8,		TEXT_ALIGN_RIGHT\n"
		"SHIFT+F9,		TEXT_ALIGN_LEFTRIGHT\n"
		"SHIFT+F10,		TEXT_MARGIN\n"
		"SHIFT+F11,		TEXT_LINE\n"
		"SHIFT+F12,		TEXT_ROTATE_BACKGROUND_COLOR\n"
		"CTRL+F1,		OBJECT_SAME_SIZE_X\n"
		"CTRL+F2,		OBJECT_SAME_SIZE_Y\n"
		"CTRL+F3,		OBJECT_ALIGN_H_TOP\n"
		"CTRL+F4,		OBJECT_ALIGN_H_CENTER\n"
		"CTRL+F5,		OBJECT_ALIGN_H_BOTTOM\n"
		"CTRL+F6,		OBJECT_ALIGN_V_LEFT\n"
		"CTRL+F7,		OBJECT_ALIGN_V_CENTER\n"
		"CTRL+F8,		OBJECT_ALIGN_V_RIGHT\n"
		"CTRL+F9,		OBJECT_SAME_GAP_V\n"
		"CTRL+F10,		OBJECT_SAME_GAP_H\n"
		"CTRL+F11,		OBJECT_SAME_DIST_V\n"
		"CTRL+F12,		OBJECT_SAME_DIST_H\n"
		"ALT+F1,		VIEW_FUNCTION_KEY_BAR\n"
		"ALT+F2,		VIEW_COPY_POSITION\n"
		"ALT+F3,		VIEW_PASTE_POSITION\n"
		// ALT+F4 はアプリ終了用なので使わない。
		"ALT+F5,		OBJECT_SIZE_TO_TEXT\n"
		"ALT+F6,		EDIT_REPLACE_TEXT_ALL\n"
		"ALT+F7,		OBJECT_ROTATE_RIGHT\n"
		"ALT+F8,		OBJECT_ROTATE_LEFT\n"
		"ALT+F9,		OBJECT_MIRROR_VERT\n"
		"ALT+F10,		OBJECT_MIRROR_HORZ\n"
		"ALT+F11,		EDIT_UNDO\n"
		"ALT+F12,		EDIT_REDO\n"
		"SHIFT+CTRL+F1,	VIEW_CHIP_STORE_VISIBLE\n"
		"SHIFT+CTRL+F2,	OPTION_AUTO_ALTKEY\n"
		"SHIFT+CTRL+F3,	OBJECT_GROUP\n"
		"SHIFT+CTRL+F4,	OBJECT_UNGROUP\n"
		"SHIFT+CTRL+F5,	VIEW_REDRAW\n"
		"SHIFT+CTRL+F6,	VIEW_ZOOMIN\n"
		"SHIFT+CTRL+F7,	VIEW_ZOOMOUT\n"
		"SHIFT+CTRL+F8,	EDIT_SELECT_ALL\n"
		"SHIFT+CTRL+F9,	EDIT_CLEAR_SELECT\n"
		"SHIFT+CTRL+F10,FILE_SHEET_SETUP\n"
		"SHIFT+CTRL+F11,FILE_PAGE_SETUP\n"
		"SHIFT+CTRL+F12,FILE_PRINT_PREVIEW\n"
		"CTRL+ALT+F6,	GRAPHICS_EDIT_COLOR_TABLE\n"
		"CTRL+ALT+F7,	GRAPHICS_EDIT_REGPEN_TABLE\n"
		"CTRL+ALT+F8,	GRAPHICS_EDIT_REGBRUSH_TABLE\n"
		"CTRL+ALT+F9,	GRAPHICS_EDIT_PICTURE_TABLE\n"
		"CTRL+ALT+F10,	GRAPHICS_EDIT_PICT_ARROW\n"
		"CTRL+ALT+F11,	GRAPHICS_EDIT_CHIP_GROUP\n"
		"CTRL+ALT+F12,	GRAPHICS_EDIT_CHIP\n"
		"SHIFT+ALT+F9,	OPTION_IS_GRID_VISIBLE\n"
		"SHIFT+ALT+F10,	OPTION_IS_PAGE_VISIBLE\n"
		"</left shortcut key>\n"
//----- 14.09.12 Fukushiro M. 追加始 ()-----
		"<sceditor right shortcut key>\n"
		"CTRL+C,		EDIT_COPY\n"
		"CTRL+INSERT,	EDIT_COPY\n"
		"SHIFT+DELETE,	EDIT_CUT\n"
		"CTRL+X,		EDIT_CUT\n"
		"DELETE,		EDIT_DELETE\n"
		"CTRL+V,		EDIT_PASTE\n"
		"SHIFT+INSERT,	EDIT_PASTE\n"
		"CTRL+S,		SCE_FILE_SAVE\n"
		"F5,			SCE_DEBUG_CONTINUE\n"
		"F6,			SCE_DEBUG_UNTIL_CARET\n"
		"F9,			SCE_DEBUG_TOGGLE_BREAKPOINT\n"
		"F10,			SCE_DEBUG_STEP_TO_NEXT\n"
		"SHIFT+F5,		SCE_DEBUG_STOP\n"
		"F2,			SCE_WIN_KEYWORD\n"
		"CTRL+F,		SCE_WIN_KEYWORD\n"
		"F3,			EDIT_FIND_NEXT_TEXT\n"
		"F4,			EDIT_FIND_PREV_TEXT\n"
		"ALT+BACK,		EDIT_UNDO\n"
		"CTRL+Z,		EDIT_UNDO\n"
		"ALT+F11,		EDIT_UNDO\n"
		"CTRL+R,		EDIT_REDO\n"
		"CTRL+Y,		EDIT_REDO\n"
		"ALT+F12,		EDIT_REDO\n"
		"</sceditor right shortcut key>\n"
		"<sceditor left shortcut key>\n"
		"CTRL+J,		EDIT_COPY\n"
		"CTRL+INSERT,	EDIT_COPY\n"
		"SHIFT+DELETE,	EDIT_CUT\n"
		"CTRL+K,		EDIT_CUT\n"
		"DELETE,		EDIT_DELETE\n"
		"CTRL+L,		EDIT_PASTE\n"
		"SHIFT+INSERT,	EDIT_PASTE\n"
		"CTRL+I,		SCE_FILE_SAVE\n"
		"F5,			SCE_DEBUG_CONTINUE\n"
		"F6,			SCE_DEBUG_UNTIL_CARET\n"
		"F9,			SCE_DEBUG_TOGGLE_BREAKPOINT\n"
		"F10,			SCE_DEBUG_STEP_TO_NEXT\n"
		"SHIFT+F5,		SCE_DEBUG_STOP\n"
		"F2,			SCE_WIN_KEYWORD\n"
		"CTRL+F,		SCE_WIN_KEYWORD\n"
		"F3,			EDIT_FIND_NEXT_TEXT\n"
		"F4,			EDIT_FIND_PREV_TEXT\n"
		"ALT+BACK,		EDIT_UNDO\n"
		"CTRL+O,		EDIT_UNDO\n"
		"ALT+F11,		EDIT_UNDO\n"
		"CTRL+P,		EDIT_REDO\n"
		"CTRL+Y,		EDIT_REDO\n"
		"ALT+F12,		EDIT_REDO\n"
		"</sceditor left shortcut key>\n";
//----- 14.09.12 Fukushiro M. 追加終 ()-----

	static const char* s_cpDefaultHistViewTable =
		"<handed exchange>\n"
		"Q->P\n"
		"W->O\n"
		"E->I\n"
		"R->U\n"
		"T->Y\n"
		"A->;\n"
		"S->L\n"
		"D->K\n"
		"F->J\n"
		"G->H\n"
		"Z->/\n"
		"X->.\n"
		"C->,\n"
		"V->M\n"
		"B->N\n"
		"1->=\n"
		"2->-\n"
		"3->0\n"
		"4->9\n"
		"5->8\n"
		"6->7\n"
		"</handed exchange>\n"
		"<right shortcut key>\n"
		"ESCAPE,		TLCMD_CANCEL\n"
		"CTRL+SPACE,	OPTION_NEXT_SNAP_METHOD\n"
		"SHIFT+SPACE,	OPTION_IS_ARROW_GAP_EFFECTIVE\n"
		"ALT+SPACE,		OPTION_POP_HINT_ENABLE\n"
		"ALT+LEFT,		OBJECT_MOVE_SNAP_LEFT\n"
		"ALT+UP,		OBJECT_MOVE_SNAP_UP\n"
		"ALT+RIGHT,		OBJECT_MOVE_SNAP_RIGHT\n"
		"ALT+DOWN,		OBJECT_MOVE_SNAP_DOWN\n"

		"LEFT,			OBJECT_MOVE_SNAP_LEFT\n"
		"UP,			OBJECT_MOVE_SNAP_UP\n"
		"RIGHT,			OBJECT_MOVE_SNAP_RIGHT\n"
		"DOWN,			OBJECT_MOVE_SNAP_DOWN\n"
		"CTRL+LEFT,		VIEW_PAN_LEFT\n"
		"CTRL+UP,		VIEW_PAN_UP\n"
		"CTRL+RIGHT,	VIEW_PAN_RIGHT\n"
		"CTRL+DOWN,		VIEW_PAN_DOWN\n"
		"CTRL+ALT+LEFT,	VIEW_PAN_LEFT\n"
		"CTRL+ALT+UP,	VIEW_PAN_UP\n"
		"CTRL+ALT+RIGHT,VIEW_PAN_RIGHT\n"
		"CTRL+ALT+DOWN,	VIEW_PAN_DOWN\n"

		"CTRL+C,		EDIT_COPY\n"
		"CTRL+INSERT,	EDIT_COPY\n"
		"DELETE,		EDIT_DELETE\n"
		"CTRL+A,		EDIT_SELECT_ALL\n"
		"CTRL+Q,		EDIT_SELECT_NEXT\n"
		"CTRL+R,		HTREE_MOVE_AFTER\n"
		"ALT+BACK,		HTREE_MOVE_BEFORE\n"
		"CTRL+Z,		HTREE_MOVE_BEFORE\n"
		"CTRL+W,		HTREE_MOVE_OPERATION_BEFORE\n"
		"CTRL+E,		HTREE_MOVE_OPERATION_AFTER\n"
		"F1,			HELP\n"					// メニューにF1を表示させるため。無くても動作する。
		"F2,			HTREE_MOVE_BEFORE\n"
		"F3,			HTREE_MOVE_AFTER\n"
		"F4,			HTREE_MOVE_OPERATION_BEFORE\n"
		"F5,			HTREE_MOVE_OPERATION_AFTER\n"
		"F6,			HTREE_MOVE_HIGHER\n"
		"F7,			HTREE_MOVE_LOWER\n"
		"F8,			HTREE_MOVE_SELECTION_BEFORE\n"
		"F9,			HTREE_MOVE_SELECTION_AFTER\n"
		"F10,			HTREE_FIND_PANEL\n"
		"F11,			HTREE_FIND_RESULTS\n"
		"F12,			HTREE_JUMP_NEXT_FIND_RESULT\n"
		"SHIFT+F1,		CONTEXT_HELP\n"
		"SHIFT+F2,		EDIT_FIND_TEXT\n"
		"CTRL+F,		EDIT_FIND_TEXT\n"
		"SHIFT+F3,		EDIT_FIND_NEXT_TEXT\n"
		"SHIFT+F4,		EDIT_FIND_PREV_TEXT\n"
		"SHIFT+F5,		EDIT_FIND_TEXT_ALL\n"
		"SHIFT+F6,		EDIT_FIND_TEXT_ALL_IN_SELECT\n"
		"SHIFT+F7,		EDIT_COPY_TEXT\n"
		"SHIFT+F8,		EDIT_COPY\n"
		"SHIFT+F9,		EDIT_SELECT_ALL\n"
		"SHIFT+F10,		EDIT_SELECT_NEXT\n"
		"SHIFT+F11,		EDIT_SELECT_SAME_KIND_OF_OBJECT\n"
		"SHIFT+F12,		EDIT_CLEAR_SELECT\n"
		"CTRL+F1,		VIEW_HTREE_VISIBLE\n"
		"CTRL+F2,		HTREE_PROJECT_VIEW\n"
		"CTRL+F3,		HTREE_TREE_VIEW\n"
		"CTRL+F4,		HTREE_BRANCH_VIEW\n"
		"CTRL+F5,		OPTION_AUTO_ALTKEY\n"
		"ALT+F1,		VIEW_FUNCTION_KEY_BAR\n"
		"ALT+F2,		VIEW_COPY_POSITION\n"
		"ALT+F3,		VIEW_PASTE_POSITION\n"
		// ALT+F4 はアプリ終了用なので使わない。
		"ALT+F5,		VIEW_REDRAW\n"
		"ALT+F6,		VIEW_ZOOMIN\n"
		"ALT+F7,		VIEW_ZOOMOUT\n"
		"ALT+F8,		EDIT_SELECT_ALL\n"
		"ALT+F9,		EDIT_CLEAR_SELECT\n"
		"ALT+F10,		OPTION_IS_GRID_VISIBLE\n"
		"ALT+F11,		OPTION_IS_PAGE_VISIBLE\n"
		"ALT+F12,		FILE_PRINT_PREVIEW\n"
		"SHIFT+CTRL+F2,	OPTION_AUTO_ALTKEY\n"
		"SHIFT+CTRL+F5,	VIEW_REDRAW\n"
		"SHIFT+CTRL+F6,	VIEW_ZOOMIN\n"
		"SHIFT+CTRL+F7,	VIEW_ZOOMOUT\n"
		"SHIFT+CTRL+F8,	EDIT_SELECT_ALL\n"
		"SHIFT+CTRL+F9,	EDIT_CLEAR_SELECT\n"
		"SHIFT+CTRL+F12,FILE_PRINT_PREVIEW\n"
		"SHIFT+ALT+F9,	OPTION_IS_GRID_VISIBLE\n"
		"SHIFT+ALT+F10,	OPTION_IS_PAGE_VISIBLE\n"
		"</right shortcut key>\n"
		"<left shortcut key>\n"
		"ESCAPE,		TLCMD_CANCEL\n"
		"CTRL+SPACE,	OPTION_NEXT_SNAP_METHOD\n"
		"SHIFT+SPACE,	OPTION_IS_ARROW_GAP_EFFECTIVE\n"
		"ALT+SPACE,		OPTION_POP_HINT_ENABLE\n"
		"ALT+LEFT,		OBJECT_MOVE_SNAP_LEFT\n"
		"ALT+UP,		OBJECT_MOVE_SNAP_UP\n"
		"ALT+RIGHT,		OBJECT_MOVE_SNAP_RIGHT\n"
		"ALT+DOWN,		OBJECT_MOVE_SNAP_DOWN\n"

		"LEFT,			OBJECT_MOVE_SNAP_LEFT\n"
		"UP,			OBJECT_MOVE_SNAP_UP\n"
		"RIGHT,			OBJECT_MOVE_SNAP_RIGHT\n"
		"DOWN,			OBJECT_MOVE_SNAP_DOWN\n"
		"CTRL+LEFT,		VIEW_PAN_LEFT\n"
		"CTRL+UP,		VIEW_PAN_UP\n"
		"CTRL+RIGHT,	VIEW_PAN_RIGHT\n"
		"CTRL+DOWN,		VIEW_PAN_DOWN\n"
		"CTRL+ALT+LEFT,	VIEW_PAN_LEFT\n"
		"CTRL+ALT+UP,	VIEW_PAN_UP\n"
		"CTRL+ALT+RIGHT,VIEW_PAN_RIGHT\n"
		"CTRL+ALT+DOWN,	VIEW_PAN_DOWN\n"

		"CTRL+J,		EDIT_COPY\n"
		"CTRL+INSERT,	EDIT_COPY\n"
		"DELETE,		EDIT_DELETE\n"
		"CTRL+M,		EDIT_SELECT_ALL\n"
		"CTRL+/,		EDIT_SELECT_NEXT\n"
		"CTRL+P,		HTREE_MOVE_AFTER\n"
		"CTRL+Y,		HTREE_MOVE_AFTER\n"
		"ALT+BACK,		HTREE_MOVE_BEFORE\n"
		"CTRL+O,		HTREE_MOVE_BEFORE\n"
		"CTRL+U,		HTREE_MOVE_OPERATION_BEFORE\n"
		"CTRL+N,		HTREE_MOVE_OPERATION_AFTER\n"
		"F1,			HELP\n"					// メニューにF1を表示させるため。無くても動作する。
		"F2,			HTREE_MOVE_BEFORE\n"
		"F3,			HTREE_MOVE_AFTER\n"
		"F4,			HTREE_MOVE_OPERATION_BEFORE\n"
		"F5,			HTREE_MOVE_OPERATION_AFTER\n"
		"F6,			HTREE_MOVE_HIGHER\n"
		"F7,			HTREE_MOVE_LOWER\n"
		"F8,			HTREE_MOVE_SELECTION_BEFORE\n"
		"F9,			HTREE_MOVE_SELECTION_AFTER\n"
		"F10,			HTREE_FIND_PANEL\n"
		"F11,			HTREE_FIND_RESULTS\n"
		"F12,			HTREE_JUMP_NEXT_FIND_RESULT\n"
		"SHIFT+F1,		CONTEXT_HELP\n"
		"SHIFT+F2,		EDIT_FIND_TEXT\n"
		"CTRL+F,		EDIT_FIND_TEXT\n"
		"SHIFT+F3,		EDIT_FIND_NEXT_TEXT\n"
		"SHIFT+F4,		EDIT_FIND_PREV_TEXT\n"
		"SHIFT+F5,		EDIT_FIND_TEXT_ALL\n"
		"SHIFT+F6,		EDIT_FIND_TEXT_ALL_IN_SELECT\n"
		"SHIFT+F7,		EDIT_COPY_TEXT\n"
		"SHIFT+F8,		EDIT_COPY\n"
		"SHIFT+F9,		EDIT_SELECT_ALL\n"
		"SHIFT+F10,		EDIT_SELECT_NEXT\n"
		"SHIFT+F11,		EDIT_SELECT_SAME_KIND_OF_OBJECT\n"
		"SHIFT+F12,		EDIT_CLEAR_SELECT\n"
		"CTRL+F1,		VIEW_HTREE_VISIBLE\n"
		"CTRL+F2,		HTREE_PROJECT_VIEW\n"
		"CTRL+F3,		HTREE_TREE_VIEW\n"
		"CTRL+F4,		HTREE_BRANCH_VIEW\n"
		"CTRL+F5,		OPTION_AUTO_ALTKEY\n"
		"ALT+F1,		VIEW_FUNCTION_KEY_BAR\n"
		"ALT+F2,		VIEW_COPY_POSITION\n"
		"ALT+F3,		VIEW_PASTE_POSITION\n"
		// ALT+F4 はアプリ終了用なので使わない。
		"ALT+F5,		VIEW_REDRAW\n"
		"ALT+F6,		VIEW_ZOOMIN\n"
		"ALT+F7,		VIEW_ZOOMOUT\n"
		"ALT+F8,		EDIT_SELECT_ALL\n"
		"ALT+F9,		EDIT_CLEAR_SELECT\n"
		"ALT+F10,		OPTION_IS_GRID_VISIBLE\n"
		"ALT+F11,		OPTION_IS_PAGE_VISIBLE\n"
		"ALT+F12,		FILE_PRINT_PREVIEW\n"
		"SHIFT+CTRL+F2,	OPTION_AUTO_ALTKEY\n"
		"SHIFT+CTRL+F5,	VIEW_REDRAW\n"
		"SHIFT+CTRL+F6,	VIEW_ZOOMIN\n"
		"SHIFT+CTRL+F7,	VIEW_ZOOMOUT\n"
		"SHIFT+CTRL+F8,	EDIT_SELECT_ALL\n"
		"SHIFT+CTRL+F9,	EDIT_CLEAR_SELECT\n"
		"SHIFT+CTRL+F12,FILE_PRINT_PREVIEW\n"
		"SHIFT+ALT+F9,	OPTION_IS_GRID_VISIBLE\n"
		"SHIFT+ALT+F10,	OPTION_IS_PAGE_VISIBLE\n"
		"</left shortcut key>\n";


	try
	{
		// s_mpLeft2Right[左利き文字] = 右利き文字。
		s_mpLeft2Right.clear();
		// s_mpRight2Left[右利き文字] = 左利き文字。
		s_mpRight2Left.clear();
		// 左利き用のショートカットキー設定。
		s_vAccelL.clear();
		// 右利き用のショートカットキー設定。
		s_vAccelR.clear();
//----- 14.09.12 Fukushiro M. 追加始 ()-----
		// 左利き用のショートカットキー設定。
		s_vAccelScL.clear();
		// 右利き用のショートカットキー設定。
		s_vAccelScR.clear();
//----- 14.09.12 Fukushiro M. 追加終 ()-----

		astring strBuff;
		long lCurLine = 1;
		long lNewLine = lCurLine;

		//----- strBuff に、キー割り当てデータを読み込む -----

		if (tcpPath != NULL && *tcpPath != L'\0')
		//----- ファイル名が設定されている場合 -----
		{
			CFile file(WStrToTStr(tcpPath), CFile::modeRead | CFile::shareDenyNone);
			DWORD dwLen = (DWORD)file.GetLength();
			strBuff.assign(dwLen + 10, '\0');
			dwLen = file.Read(FFBeginPtr(strBuff), (UINT)strBuff.size());
			strBuff.resize(dwLen);
			file.Close();
		} else
		//----- ファイル名が設定されていない場合 -----
		{
			tcpPath = L"";
			// 履歴ビューアの場合は特別。
			if ((FFApp()->GetAppMode() & FD_APPMODE_MASK) == FD_APPMODE_HISTORYVIEWER)
				strBuff = s_cpDefaultHistViewTable;
			else
				strBuff = s_cpDefaultTable;
		}
		astring strLine;
		// メニューに含まれるコマンドを取り出す。
		DWORDSet stCommandId;
		myExtractMenuCommand(stCommandId, hMenu);
//----- 14.09.12 Fukushiro M. 追加始 ()-----
		// メニューに含まれるコマンドを取り出す。
		DWORDSet stScCommandId;
		myExtractMenuCommand(stScCommandId, hScMenu);
//----- 14.09.12 Fukushiro M. 追加終 ()-----

		while (!strBuff.empty())
		{
			// 新しい行を設定。
			lCurLine = lNewLine;
			// 1行読み込み。改行は取り除く。
			long lLineCount;
			strLine = ::GetLine(strBuff, lLineCount);
			// 新しい行を計算。
			lNewLine = lCurLine + lLineCount;
			// 両側の空白・改行を取り除く。
			TrimLeft(strLine);
			TrimRight(strLine);
			// 行に何も無い場合は行をスキップ。
			if (strLine.empty()) continue;
			// #で始まる場合は行をスキップ。
			if (strLine[0] == '#') continue;
			// 全て小文字に。
			FFStrlwr_s(FFBeginPtr(strLine), strLine.size() + 1);
			if (strLine[0] == '<')
			{
				if (strLine == "<handed exchange>")
					MyInitHandsTable(s_mpLeft2Right, s_mpRight2Left, strBuff, tcpPath, lNewLine);
				else
				if (strLine == "<left shortcut key>")
					MyInitUserAccelerator(s_vAccelL, strBuff, "</left shortcut key>", tcpPath, lNewLine, stCommandId);
				else
				if (strLine == "<right shortcut key>")
					MyInitUserAccelerator(s_vAccelR, strBuff, "</right shortcut key>", tcpPath, lNewLine, stCommandId);
				else
				if (strLine == "<sceditor left shortcut key>")
					MyInitUserAccelerator(s_vAccelScL, strBuff, "</sceditor left shortcut key>", tcpPath, lNewLine, stScCommandId);
				else
				if (strLine == "<sceditor right shortcut key>")
					MyInitUserAccelerator(s_vAccelScR, strBuff, "</sceditor right shortcut key>", tcpPath, lNewLine, stScCommandId);
				else
				{
					FFThrowMiscException(IDS_ERR_SYNTAX_ERROR,
										tcpPath, lCurLine,
										(const wchar_t*)AStrToWStr(strLine.c_str(), CP_ACP),
                                        L"");
				}
			}
		}
		// 新しい行を設定。
		lCurLine = lNewLine;
	} catch (CException* e)
	//----- ファイルエラー処理 -----
	{
		// エラーコード取得。
		e->ReportError();
		e->Delete();
	}
} // FFInitKeyboardTable.

/*************************************************************************
 * <関数>	FFInitHandsTable
 *
 * <機能>	1文字メニューの文字交換テーブルを初期化する。
 *
 * <引数>	strBuff	:テーブル初期化用のデータを指定。
 *			tcpPath	:定義ファイル名を指定。
 *			lCurLine:現在の行番号を指定。更新して返す。
 *
 * <履歴>	01.05.05 Fukushiro M. 作成
 *************************************************************************/
// 14.09.12 Fukushiro M. 1行変更 ()
//void FFInitHandsTable (FTTcTcMap& mpLeft2Right, FTTcTcMap& mpRight2Left, astring& astrBuff, const wchar_t* tcpPath, long& lCurLine)
static void MyInitHandsTable (FTTcTcMap& mpLeft2Right, FTTcTcMap& mpRight2Left, astring& astrBuff, const wchar_t* tcpPath, long& lCurLine)
{
	// 既にデータが設定されている場合はエラー。
	ASSERT(mpLeft2Right.empty());

	static const struct
	{
		wchar_t tcRight;		// 右利きデータ。
		wchar_t tcLeft;		// 左利きデータ。
	} DEFAULT_HANDS_TBL[] =
	{
#ifdef FD_ACCESSKEY_UC	// アクセスキーが大文字の場合。
		{ L'Q', L'P' },
		{ L'W', L'O' },
		{ L'E', L'I' },
		{ L'R', L'U' },
		{ L'T', L'Y' },
		{ L'A', L';' },
		{ L'S', L'L' },
		{ L'D', L'K' },
		{ L'F', L'J' },
		{ L'G', L'H' },
		{ L'Z', L'/' },
		{ L'X', L'.' },
		{ L'C', L',' },
		{ L'V', L'M' },
		{ L'B', L'N' },
		{ L'1', L'=' },
		{ L'2', L'-' },
		{ L'3', L'0' },
		{ L'4', L'9' },
		{ L'5', L'8' },
		{ L'6', L'7' },
#else //FD_ACCESSKEY_UC	// アクセスキーが小文字の場合。
		{ L'q', L'p' },
		{ L'w', L'o' },
		{ L'e', L'i' },
		{ L'r', L'u' },
		{ L't', L'y' },
		{ L'a', L';' },
		{ L's', L'l' },
		{ L'd', L'k' },
		{ L'f', L'j' },
		{ L'g', L'h' },
		{ L'z', L'/' },
		{ L'x', L'.' },
		{ L'c', L',' },
		{ L'v', L'm' },
		{ L'b', L'n' },
		{ L'1', L'=' },
		{ L'2', L'-' },
		{ L'3', L'0' },
		{ L'4', L'9' },
		{ L'5', L'8' },
		{ L'6', L'7' },
#endif //FD_ACCESSKEY_UC	// アクセスキーが小文字の場合。
	};
	//----- 右利き->左利きテーブルにデフォルトデータを登録 -----
	const int iTblSize = _countof(DEFAULT_HANDS_TBL);
	for (int iC = 0; iC != iTblSize; iC++)
	{
		// mpRight2Left[右利き文字] = 左利き文字。
		mpRight2Left[DEFAULT_HANDS_TBL[iC].tcRight] = DEFAULT_HANDS_TBL[iC].tcLeft;
	}
	astring astrLine;
	long lNewLine = lCurLine;
	// 1行読み込み。改行は取り除く。
	while (!astrBuff.empty())
	{
		// 新しい行を設定。
		lCurLine = lNewLine;
		// 1行読み込み。改行は取り除く。
		long lLineCount;
		astrLine = ::GetLine(astrBuff, lLineCount);
		// 新しい行を計算。
		lNewLine = lCurLine + lLineCount;
		// 両側の空白・改行を取り除く。
		TrimLeft(astrLine);
		TrimRight(astrLine);
		// 行に何も無い場合は行をスキップ。
		if (astrLine.empty()) continue;
		// #で始まる場合はコメント行なのでスキップ。
		if (astrLine[0] == '#') continue;
#ifdef FD_ACCESSKEY_UC	// アクセスキーが大文字の場合。
		// 全て大文字に。
// 08.01.22 Fukushiro M. 1行変更 ()
//		_strupr(astrLine.begin());
		FFStrupr_s(FFBeginPtr(astrLine), astrLine.size() + 1);
#else //FD_ACCESSKEY_UC	// アクセスキーが小文字の場合。
		// 全て小文字に。
// 08.01.22 Fukushiro M. 1行変更 ()
//		_strlwr(astrLine.begin());
		_strlwr(FFBeginPtr(astrLine));
#endif //FD_ACCESSKEY_UC	// アクセスキーが小文字の場合。
		// 終了タグ </handed exchange> の場合は終了。
		if (_stricmp(astrLine.c_str(), "</handed exchange>") == 0) break;
		// 終了タグ </end> の場合は終了。
		if (_stricmp(astrLine.c_str(), "</end>") == 0) break;
		// 行の文字数が４以外の場合は行をスキップ。
		if (astrLine.length() != 4)
		{
			// %s(%d):"%s" 左記の記述形式に誤りがあります。
			FFThrowMiscException(	IDS_ERR_SYNTAX_ERROR,
									tcpPath, lCurLine,
									(const wchar_t*)AStrToWStr(astrLine.c_str(), CP_ACP),
                                    L"");
		}
		if (astrLine[0] == '<')
		{
			FFThrowMiscException(	IDS_ERR_SYNTAX_ERROR,
									tcpPath, lCurLine,
									(const wchar_t*)AStrToWStr(astrLine.c_str(), CP_ACP),
                                    L"");
		}
		AStrToWStr wstr(astrLine.c_str(), CP_ACP);
		// mpRight2Left[右利き文字] = 左利き文字。
		mpRight2Left[((const wchar_t*)wstr)[0]] = ((const wchar_t*)wstr)[3];
	}
	// 新しい行を設定。
	lCurLine = lNewLine;

	//----- 左利き->右利きテーブルを作成 -----
	FTTcTcMap::const_iterator iCc;
	for (iCc = mpRight2Left.begin(); iCc != mpRight2Left.end(); iCc++)
	{
		// mpLeft2Right[左利き文字] = 右利き文字。
		mpLeft2Right[iCc->second] = iCc->first;
	}
} // MyInitHandsTable

/////////////////////////////////////////////////////////////////////////////
// コマンド文字列変換

/*************************************************************************
 * <関数>	FFCommandStringToId
 *
 * <機能>	指定されたコマンド名の文字列をコマンドＩＤに変換する。
 *
 * <引数>	cpCommandString	:コマンド名文字列を指定。大文字、小文字の
 *							 どちらでもよい。
 *
 * <返値>	コマンドＩＤ。対応するコマンドが存在しない場合はDWORD(-1)。
 *
 * <解説>	コマンド名は、"ID_XXXXXX" の "ID_" を取り除いた部分。
 *			例：コマンド名 "EDIT_COPY" -> コマンドＩＤ ID_EDIT_COPY
 *
 * <履歴>	05.07.21 Fukushiro M. 作成
 *************************************************************************/
DWORD FFCommandStringToId (const char* cpCommandString)
{
	// コマンドテーブルを初期化。
	myInitCommandTable();
	// 大文字に変換。
	astring astr(cpCommandString);
//----- 10.02.02 Fukushiro M. 追加始 ()-----
	// 空文字の場合は該当コマンド無しを返す。
	if (astr.empty()) return DWORD(-1);
//----- 10.02.02 Fukushiro M. 追加終 ()-----
	FFStrupr_s(FFBeginPtr(astr), astr.size() + 1);
	std::map<astring, DWORD>::const_iterator i = s_mpCommandStringToId.find(astr.c_str());
	if (i == s_mpCommandStringToId.end()) return DWORD(-1);
	return (*i).second;
} // FFCommandStringToId.

/*************************************************************************
 * <関数>	FFCommandIdToString
 *
 * <機能>	指定されたコマンドIDをコマンド文字列に変換する。
 *
 * <引数>	dwCommandId	:コマンドIDを指定。
 *
 * <返値>	コマンド文字列。見つからない場合はNULL。
 *
 * <解説>	FFCommandStringToId関数の解説を参照。
 *
 * <履歴>	09.06.15 Fukushiro M. 作成
 *************************************************************************/
const char* FFCommandIdToString (DWORD dwCommandId)
{
	// コマンドテーブルを初期化。
	myInitCommandTable();
	std::map<DWORD, astring>::const_iterator i = s_mpCommandIdToString.find(dwCommandId);
	if (i == s_mpCommandIdToString.end()) return NULL;
	return (*i).second.c_str();
} // const char* FFCommandIdToString (DWORD dwCommandId)

/*************************************************************************
 * <関数>	FFCommandIdToTitle
 *
 * <機能>	指定されたコマンドIDのコマンドタイトルを取得する。
 *          プラグインで登録されるコマンドにも対応する。
 *
 * <引数>	dwCommandId	:コマンドIDを指定。
 *			bSecond		:長いタイトルの場合はTRUE、短いタイトルの場合は
 *						 FALSEを指定。
 *
 * <返値>	コマンドタイトル。見つからない場合はNULL。
 *
 * <解説>	FFCommandStringToId関数の解説を参照。
 *
 * <履歴>	09.06.15 Fukushiro M. 作成
 *************************************************************************/
WString FFCommandIdToTitle (DWORD dwCommandId, BOOL bSecond)
{
	WString wstrTitle;
	wstrTitle.LoadString(dwCommandId);
	if (wstrTitle.IsEmpty())
	{
		std::map<DWORD, WString>::const_iterator i = s_mpCommandIdToTitle.find(dwCommandId);
		if (i != s_mpCommandIdToTitle.end())
			wstrTitle = (*i).second;
	}
	if (bSecond)
	{
		FFExtractUntil(wstrTitle, L'\n');
		return wstrTitle;
	} else
	{
		return FFExtractUntil(wstrTitle, L'\n');
	}
} // WString FFCommandIdToTitle (DWORD dwCommandId)

/*************************************************************************
 * <関数>	FFRegisterCommand
 *
 * <機能>	指定されたコマンド名とコマンドID、コマンドタイトルのセット
 *			を登録する。
 *
 * <引数>	cpCommandString	:コマンド名文字列を指定。
 *			dwCommandId		:コマンドIDを指定。
 *			wcpCommandTitle	:コマンドタイトルを指定。
 *
 * <解説>	コマンドタイトルは"長いタイトル\n短いタイトル"の形式で指定。
 *			例："File open\nOpen"
 *
 * <履歴>	09.06.15 Fukushiro M. 作成
 *************************************************************************/
void FFRegisterCommand (const char* cpCommandString,
						DWORD dwCommandId,
						const wchar_t* wcpCommandTitle)
{
	// コマンドテーブルを初期化。
	myInitCommandTable();
	if (s_mpCommandStringToId.find(cpCommandString) != s_mpCommandStringToId.end())
		FFThrowMiscUnknown();
	if (s_mpCommandIdToString.find(dwCommandId) != s_mpCommandIdToString.end())
		FFThrowMiscUnknown();
	s_mpCommandStringToId[cpCommandString] = dwCommandId;
	s_mpCommandIdToString[dwCommandId] = cpCommandString;
	s_mpCommandIdToTitle[dwCommandId] = wcpCommandTitle;
} // void FFRegisterCommand (const char* cpCommandString)

/*************************************************************************
 * <関数>	FFUnregisterCommand
 *
 * <機能>	指定されたコマンド名の登録を解除する。
 *
 * <引数>	cpCommandString	:コマンド名文字列を指定。
 *
 * <履歴>	09.06.15 Fukushiro M. 作成
 *************************************************************************/
void FFUnregisterCommand (const char* cpCommandString)
{
	// コマンドテーブルを初期化。
	myInitCommandTable();
	const DWORD dwCommandId = FFCommandStringToId(cpCommandString);
	s_mpCommandStringToId.erase(cpCommandString);
	s_mpCommandIdToString.erase(dwCommandId);
	s_mpCommandIdToTitle.erase(dwCommandId);
} // void FFUnregisterCommand (const char* cpCommandString)

//----- 11.11.08 Fukushiro M. 追加始 ()-----
//********************************************************************************************
/*!
 * @brief	指定されたコマンドがエラーの
 *
 *
 * @author	Fukushiro M.
 * @date	2011/11/08(火) 09:49:57
 *
 * @param[in]	const char*	cpCommandString	文字列。
 *
 * @return	DWORD	
 */
//********************************************************************************************
WString FFCommandStringToErrorMessage (const char* cpCommandString)
{
    WString wstrMessage(L"");
    // コマンドテーブルを初期化。
    myInitCommandTable();
    // 大文字に変換。
    astring astr(cpCommandString);
    FFStrupr_s(FFBeginPtr(astr), astr.size() + 1);
    std::map<astring, astring>::const_iterator i = s_mpOldCommandToNew.find(astr.c_str());
    if (i != s_mpOldCommandToNew.end())
    {
        // "コマンド「%s」はサポートされなくなりました。\r\nコマンド「%s」をお使いください。"
        // "The command [%s] is not supported now.\r\nUse the command [%s]."
        wstrMessage.Format( IDS_ERR_MSG_COMMAND_REPLACED,
                            (const wchar_t*)AStrToWStr((*i).first.c_str(), CP_ACP),
                            (const wchar_t*)AStrToWStr((*i).second.c_str(), CP_ACP));
    } else
    {
        // "コマンド「%s」はサポートされていません。"
        // "The command [%s] is not supported."
        wstrMessage.Format( IDS_ERR_MSG_UNSUPPORTED_COMMAND,
                            (const wchar_t*)AStrToWStr(astr.c_str(), CP_ACP));
    }
    return wstrMessage;
} // FFCommandStringToErrorMessage (const char* cpCommandString)
//----- 11.11.08 Fukushiro M. 追加終 ()-----

/////////////////////////////////////////////////////////////////////////////
// リソース文字列変換

/*************************************************************************
 * <関数>	FFIdsStringToId
 *
 * <機能>	指定された文字列リソース名の文字列をIDに変換する。
 *
 * <引数>	cpIdsString	:文字列リソース名を指定。大文字、小文字の
 *						 どちらでもよい。
 *
 * <返値>	ID。対応するリソースが存在しない場合はDWORD(-1)。
 *
 * <解説>	リソース名は、"IDS_XXXXXX" の "IDS_" を取り除いた部分。
 *			例："TITLE" -> ID=IDS_TITLE
 *
 * <履歴>	09.06.16 Fukushiro M. 作成
 *************************************************************************/
DWORD FFIdsStringToId (const char* cpIdsString)
{
	// 文字列->ID変換テーブル。
	static std::map<astring, DWORD> s_mpStringToId;
	if (s_mpStringToId.empty())
	{
		// テーブルを初期化。
		for (long lC = 0; lC != _countof(IDS_NAME_TO_ID); lC++)
			s_mpStringToId[IDS_NAME_TO_ID[lC].cpName] = IDS_NAME_TO_ID[lC].dwId;
	}
	// 大文字に変換。
	astring astr(cpIdsString);
	FFStrupr_s(FFBeginPtr(astr), astr.size() + 1);
	std::map<astring, DWORD>::const_iterator i = s_mpStringToId.find(astr.c_str());
	if (i == s_mpStringToId.end()) return DWORD(-1);
	return (*i).second;
} // DWORD FFIdsStringToId (const char* cpIdsString)

/*************************************************************************
 * <関数>	FFIdsIdToString
 *
 * <機能>	指定された文字列リソースIDを文字列リソース名に変換する。
 *
 * <引数>	dwIdsId	:IDを指定。
 *
 * <返値>	リソース名文字列。見つからない場合はNULL。
 *
 * <解説>	FFIdsStringToId関数の解説を参照。
 *
 * <履歴>	09.06.16 Fukushiro M. 作成
 *************************************************************************/
const char* FFIdsIdToString (DWORD dwIdsId)
{
	// ID->文字列変換テーブル。
	static std::map<DWORD, astring> s_mpIdToString;
	if (s_mpIdToString.empty())
	{
		// テーブルを初期化。
		for (long lC = 0; lC != _countof(IDS_NAME_TO_ID); lC++)
			s_mpIdToString[IDS_NAME_TO_ID[lC].dwId] = IDS_NAME_TO_ID[lC].cpName;
	}
	std::map<DWORD, astring>::const_iterator i = s_mpIdToString.find(dwIdsId);
	if (i == s_mpIdToString.end()) return NULL;
	return (*i).second.c_str();
} // const char* FFIdsIdToString (DWORD dwIdsId)

/////////////////////////////////////////////////////////////////////////////

/*************************************************************************
 * <関数>	MyInitUserAccelerator
 *
 * <機能>	ユーザアクセラレータテーブルを初期化する。
 *
 * <引数>	strBuff	:テーブル初期化用のデータを指定。
 *			bLeft	:左利きならば TRUE を指定。
 *			tcpPath	:定義ファイル名を指定。
 *			lCurLine:現在の行番号を指定。更新して返す。
 *
 * <履歴>	01.05.05 Fukushiro M. 作成
 *			05.07.21 Fukushiro M. 修正。
 *************************************************************************/
static void MyInitUserAccelerator (std::vector<ACCEL>& vAccel, astring& strBuff, const char* cpEndTag, const wchar_t* tcpPath,
							long& lCurLine, const DWORDSet& stCommandId)
{
	long lNewLine = lCurLine;
//----- 14.09.12 Fukushiro M. 削除始 ()-----
//	char* cpEndTag;
//	if (bLeft)
//		cpEndTag = "</left shortcut key>";
//	else
//		cpEndTag = "</right shortcut key>";
//----- 14.09.12 Fukushiro M. 削除終 ()-----

	astring astrLine;
	astring astrKey;
	astring astrEachKey;
	astring astrCommand;
	// 1行読み込み。改行は取り除く。
	while (!strBuff.empty())
	{
		// 新しい行を設定。
		lCurLine = lNewLine;
		// 1行読み込み。改行は取り除く。
		long lLineCount;
		astrLine = ::GetLine(strBuff, lLineCount);
		// 新しい行を計算。
		lNewLine = lCurLine + lLineCount;
		// 最初・最後の空白・改行を取り除く。
		TrimLeft(astrLine);
		TrimRight(astrLine);
		// 行に何も無い場合は行をスキップ。
		if (astrLine.empty()) continue;
		// #で始まる場合は行をスキップ。
		if (astrLine[0] == '#') continue;
		// 全て小文字に。
// 08.01.22 Fukushiro M. 1行変更 ()
//		_strlwr(astrLine.begin());
		FFStrlwr_s(FFBeginPtr(astrLine), astrLine.size() + 1);
		// 終了タグ </XXX shortcut key> の場合は終了。
		if (astrLine == cpEndTag) break;
		// 終了タグ </end> の場合は終了。
		if (astrLine == "</end>") break;
		// その他のタグならばエラー。
		if (astrLine[0] == '<')
		{
			FFThrowMiscException(IDS_ERR_SYNTAX_ERROR,
								tcpPath, lCurLine,
								(const wchar_t*)AStrToWStr(astrLine.c_str(), CP_ACP),
                                L"");
		}
//----- 10.02.02 Fukushiro M. 変更前 ()-----
//		astrKey = ::FFExtractUntil(astrLine, ',');
//		astrCommand = ::FFExtractUntil(astrLine, ',');
//----- 10.02.02 Fukushiro M. 変更後 ()-----
		astrCommand = astrLine;
		astrKey = ::FFExtractUntil(astrCommand, ',');
//----- 10.02.02 Fukushiro M. 変更終 ()-----
		// 両側の空白・改行を取り除く。
		TrimLeft(astrKey);
		TrimRight(astrKey);
		// 両側の空白・改行を取り除く。
		TrimLeft(astrCommand);
		TrimRight(astrCommand);

		// コマンド名からコマンドＩＤを取得。
		DWORD dwCommandId = FFCommandStringToId(astrCommand.c_str());

		if (dwCommandId == DWORD(-1))
		//----- コマンド名の文字列がどれとも一致しない場合 -----
		{
			FFThrowMiscException(	IDS_ERR_SYNTAX_ERROR,
									tcpPath, lCurLine,
									(const wchar_t*)AStrToWStr(astrLine.c_str(), CP_ACP),
                                    FFCommandStringToErrorMessage(astrCommand.c_str()).c_str());
		}
		// 現在のメニューに含まれないコマンドは、アクセラレータに追加しない。
		if (stCommandId.find(dwCommandId) == stCommandId.end()) continue;

		WORD wKey = WORD(-1);
		BYTE byVirt = FVIRTKEY;
		while (!astrKey.empty())
		{
			astrEachKey = ::FFExtractUntil(astrKey, '+');
			if (!myStrToAccel(wKey, byVirt, astrEachKey))
			{
				FFThrowMiscException(IDS_ERR_SYNTAX_ERROR,
									tcpPath, lCurLine,
									(const wchar_t*)AStrToWStr(astrLine.c_str(), CP_ACP),
                                    L"");
			}
		}
		if (wKey == WORD(-1))
		{
			FFThrowMiscException(IDS_ERR_SYNTAX_ERROR,
								tcpPath, lCurLine, L"", L"");
		}

		ACCEL accel;
		accel.fVirt = byVirt;
		accel.key = wKey;
		accel.cmd = (WORD)dwCommandId;

//----- 14.09.12 Fukushiro M. 変更前 ()-----
//		if (bLeft)
//			s_vAccelL.push_back(accel);
//		else
//			s_vAccelR.push_back(accel);
//----- 14.09.12 Fukushiro M. 変更後 ()-----
		vAccel.push_back(accel);
//----- 14.09.12 Fukushiro M. 変更終 ()-----
	}
	// 新しい行を設定。
	lCurLine = lNewLine;
} // MyInitUserAccelerator.

/*************************************************************************
 * <関数>	FFGetUserAccelerator
 *
 * <機能>	ユーザ定義のアクセラレータを返す。
 *
 * <履歴>	01.05.03 Fukushiro M. 作成
 *************************************************************************/
long FFGetUserAccelerator (const ACCEL*& pAccel, long& lAccel)
{
	if (FFReg()->GetIsLeftHand())
	{
		pAccel = FFBeginPtr(s_vAccelL);
		lAccel = (long)s_vAccelL.size();
	} else
	{
		pAccel = FFBeginPtr(s_vAccelR);
		lAccel = (long)s_vAccelR.size();
	}
    return lAccel;
} // FFGetUserAccelerator.

//********************************************************************************************
/*!
 * @brief	スクリプトエディターのユーザー定義アクセラレーターを返す。
 * @author	Fukushiro M.
 * @date	2014/09/12(金) 22:19:24
 *
 * @param[out]	const ACCEL*&	pAccel	アクセラレーター。
 * @param[out]	long&			lAccel	アクセラレーター数。
 * @param[in]	BOOL			bLeft	TRUE:左利き用/FALSE:右利き用。
 *
 * @return	long	アクセラレーター数。
 */
//********************************************************************************************
long FFGetScUserAccelerator (const ACCEL*& pAccel, long& lAccel)
{
	if (FFReg()->GetIsLeftHand())
	{
		pAccel = FFBeginPtr(s_vAccelScL);
		lAccel = (long)s_vAccelScL.size();
	} else
	{
		pAccel = FFBeginPtr(s_vAccelScR);
		lAccel = (long)s_vAccelScR.size();
	}
    return lAccel;
} // FFGetScUserAccelerator

/*************************************************************************
 * <関数>	myAddShortcutText
 *
 * <機能>	メニューにショートカットテキストを書き込む。
 *
 * <履歴>	01.05.05 Fukushiro M. 作成
 *************************************************************************/
static void myAddShortcutText (HMENU hMenu, const std::map<DWORD, CString>& mpCmdToName)
{
	std::map<DWORD, CString>::const_iterator iCn;
	MENUITEMINFO mii;
	// メニュー数を取得。
	const int iMenuCount = GetMenuItemCount(hMenu);
	_TCHAR* tcpFind;
	for (int imi = 0; imi < iMenuCount; imi++)
	{
		_TCHAR tsz[250] = { _T('\0') };
		//----- メニュー文字列とサブメニューハンドルを取得 -----
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
		mii.dwTypeData = tsz;
// 08.05.20 Fukushiro M. 1行変更 ()
//		mii.cch = sizeof(tsz) / sizeof(tsz[0]);
		mii.cch = _countof(tsz);
		GetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);

		// サブメニューがある場合は再帰呼び出し。
		if (mii.hSubMenu != NULL)
		{
			myAddShortcutText(mii.hSubMenu, mpCmdToName);
			continue;
		}
		// メニュー文字列が無い場合（セパレータ等）は次のループへ。
		if (tsz == NULL || *tsz == _T('\0')) continue;
		// メニュー文字列中の"\t"文字列の位置を検索。
		tcpFind = _tcsstr(tsz, _T("\t"));
		// 文字列がある場合はタブを消去。
		if (tcpFind != NULL) *tcpFind = _T('\0');

		iCn = mpCmdToName.find(mii.wID);
		if (iCn != mpCmdToName.end())
		{
			FFTcscat_s(tsz, _countof(tsz), _T("\t"));
			FFTcscat_s(tsz, _countof(tsz), (*iCn).second);
		}
		//----- 変換後の文字列をメニューに設定 -----
		mii.cbSize = sizeof(mii);
		mii.dwTypeData = tsz;
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.cch = (UINT)_tcslen(mii.dwTypeData);
		SetMenuItemInfo(hMenu, imi, MF_BYPOSITION, &mii);
	}
} // myAddShortcutText.

/*************************************************************************
 * <関数>	FFAddShortcutText
 *
 * <機能>	メニューにショートカットテキストを書き込む。
 *
 * <履歴>	01.05.05 Fukushiro M. 作成
 *************************************************************************/
void FFAddShortcutText (HMENU hMenu, const ACCEL* pAccel, long lAccel)
{
//----- 14.09.12 Fukushiro M. 削除始 ()-----
//	const ACCEL* pAccel;
//	long lAccel;
//	FFGetUserAccelerator(pAccel, lAccel);
//----- 14.09.12 Fukushiro M. 削除終 ()-----

	CString strKey;
	std::map<DWORD, CString> mpCmdToName;
	std::map<DWORD, CString>::iterator iCn;
	for (long lC = 0; lC != lAccel; lC++)
	{
		myAccelToStr(strKey, pAccel[lC].key, pAccel[lC].fVirt);
		iCn = mpCmdToName.find(pAccel[lC].cmd);
		if (iCn == mpCmdToName.end())
		{
			mpCmdToName[pAccel[lC].cmd] = strKey;
		} else
		{
			(*iCn).second += _T(",");
			(*iCn).second += strKey;
		}
	}
	ACCEL accel[100];
	HACCEL hAccel = FFMainWnd()->GetDefaultAccelerator();
	int iAccelSz = ::CopyAcceleratorTable(hAccel, accel, 100);
	for (long lC = 0; lC != iAccelSz; lC++)
	{
		myAccelToStr(strKey, accel[lC].key, accel[lC].fVirt);
		iCn = mpCmdToName.find(accel[lC].cmd);
		if (iCn == mpCmdToName.end())
		{
			mpCmdToName[accel[lC].cmd] = strKey;
		} else
		{
			(*iCn).second += _T(",");
			(*iCn).second += strKey;
		}
	}
	myAddShortcutText(hMenu, mpCmdToName);
} // FFAddShortcutText.

/*************************************************************************
 * <関数>	FFShowMsgPanel
 *
 * <機能>	メッセージパネルを表示。
 *
 * <引数>	dwId	:メッセージパネルを識別するためのIDを指定。同じ
 *					 パネルを続けて表示させないために必要。
 *			tcpMsg	:表示するメッセージを指定。dwIdで指定するリソース文字列
 *					 を表示する場合は NULL を指定。
 *			hWnd	:呼び出し元のウィンドウを指定。アプリケーションの
 *					 フレームの場合はNULLを指定。
 *			lSurvivalMsec
 *					:メッセージパネルを表示する時間を指定。msec。
 *
 * <履歴>	01.06.23 Fukushiro M. 作成
 *************************************************************************/
// 10.02.02 Fukushiro M. 1行変更 ()
//void FFShowMsgPanel (DWORD dwId, const wchar_t* tcpMsg)
// 10.02.13 Fukushiro M. 1行変更 ()
//void FFShowMsgPanel (DWORD dwId, const wchar_t* tcpMsg, long lSurvivalMsec)
void FFShowMsgPanel (DWORD dwId,
					const wchar_t* tcpMsg,
					HWND hWnd,
					long lSurvivalMsec,
					const CPoint ptShow)
{
	static DWORD	s_dwLastId = DWORD(-1);
	static FDTIME	s_fdtmLast = 0;
	static CPoint	s_ptLast(0,0);

	// 現在の時間（10,000,000分の1秒単位）を取得。
	FDTIME fdtmNow = FFGetCurrentTime();
	if (FDTIME(fdtmNow - s_fdtmLast) < FDTIME(10000000))
	//----- 前回のパネル表示から1秒未満の場合 -----
	{
		TRACE(_T("NO\r\n"));
		// 前回のパネル表示から1秒未満の同じパネルならば、パネルを表示せず復帰。
		if (s_dwLastId == dwId) return;
		// パネル表示位置を下にずらす。
		s_ptLast.y += 50;
	} else
	//----- 前回のパネル表示から1秒以上の場合 -----
	{
		TRACE(_T("YES\r\n"));
//----- 06.02.24 Fukushiro M. 変更前 ()-----
//		//----- 画面のサイズを取得 -----
//		HDC hAttribDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
//		if (hAttribDC != NULL)
//		{
//			int iDisplayW = GetDeviceCaps(hAttribDC, HORZRES);
//			int iDisplayH = GetDeviceCaps(hAttribDC, VERTRES);
//			::DeleteDC(hAttribDC);
//			//----- 画面中央を、ダイアログ表示位置のデフォルトに -----
//			s_ptLast.x = iDisplayW / 2;
//			s_ptLast.y = iDisplayH / 2;
//		}
//----- 06.02.24 Fukushiro M. 変更後 ()-----
		if (ptShow.x == LONG_MAX)
		{
			// 以前は画面中央をデフォルトにしていたが、
			// マルチモニターでは動作が変。
			// アプリフレームの中央をデフォルトに設定。
			s_ptLast = FFGetWindowRect(FFMainWnd()->GetWnd()).CenterPoint();
		} else
		{
			s_ptLast = ptShow;
		}
//----- 06.02.24 Fukushiro M. 変更終 ()-----
	}
	// staticの情報を更新。
	s_dwLastId = dwId;
	s_fdtmLast = fdtmNow;
	//----- メッセージパネルの表示 -----
	WString wstrMessage;
	if (tcpMsg != NULL)
		wstrMessage = tcpMsg;
	else
		wstrMessage.LoadString(dwId);
	WString strArg;
//----- 10.02.02 Fukushiro M. 変更前 ()-----
//	strArg.Format(	L"x=%d y=%d h=%lu m=1000 %s",
//					s_ptLast.x,
//					s_ptLast.y,
//					FFMainWnd()->GetSafeHwnd(),
//					wstrMessage.c_str());
//----- 10.02.02 Fukushiro M. 変更後 ()-----
	strArg.Format(	L"x=%d y=%d h=%lu m=%d %s",
					s_ptLast.x,
					s_ptLast.y,
// 10.02.13 Fukushiro M. 1行変更 ()
//					FFMainWnd()->GetSafeHwnd(),
					(hWnd == NULL) ? FFMainWnd()->GetSafeHwnd() : hWnd,
					lSurvivalMsec,
					wstrMessage.c_str());
//----- 10.02.02 Fukushiro M. 変更終 ()-----
	ExecAndWaitApp(L"MsgPanel.exe", strArg, 5000);
} // FFShowMsgPanel.

/*************************************************************************
 * <関数>	FFGetFileDialogFilterString
 *
 * <機能>	ファイルダイアログのフィルタ文字列（m_ofn.lpstrFilter）を取得。
 *
 * <引数>	strFilter	:フィルタ文字列を返す。
 *			bisSave		:保存ダイアログの場合は TRUE を指定。
 *
 * <履歴>	03.06.10 Fukushiro M. 作成
 *************************************************************************/
void FFGetFileDialogFilterString (CString& strFilter, BOOL bIsSave)
{
	// ファイルフォーマットのフィルタ文字列を読み込む。
	// "Dynamic Draw Standard Format\n.mdrw\nDynamic Draw Portable Format\n.mdpf\n ・・・"
	WString wstrFilterName;
	if (!bIsSave)
	//----- 読み込みの場合 -----
	{
		wstrFilterName.LoadString(IDS_CTRL_LOAD_SAVE_FILTER);
		//----- ファイルダイアログのフィルタ文字列を作成 ------
		// 例 保存に対して、すべての文書と、バージョン名が追加されている。
		// "すべてのDynamic Draw文書 (*.mdrw;*.mdpf;…)\0*.mdrw;*.mdpf;…\0Dynamic Draw Standard Format 4.0 (*.mdrw)\0
		// *.mdrw\0Dynamic Draw Portable Format 4.0 (*.mdpf)\0*.mdpf\0 ・・・\0\0"

		// 全てのファイルの拡張子。例「*.mdrw;*.mfdpf;*.mdvf」
		CString strAllExts;
		// フィルタ文字列を設定。
		while (!wstrFilterName.IsEmpty())
		{
			WString wstrTypeName = FFExtractUntil(wstrFilterName, L"\n");
			WString wstrExtension = FFExtractUntil(wstrFilterName, L"\n");
			strAllExts += _T('*');
			strAllExts += WStrToTStr(wstrExtension);
			strAllExts += _T(';');
		}
		strAllExts.TrimRight(_T(';'));

		// strFilter += _T("すべてのDynamic Drawファイル (*.mdrw;*.mdpf)");
		// IDS_CTRL_ALL_DOCUMENT = すべてのDynamic Draw文書。
		strFilter.LoadString(IDS_CTRL_ALL_DOCUMENT);
		strFilter += _T(" (");
		strFilter += strAllExts;
		strFilter += _T(")");
		strFilter += _T('\0');
		strFilter += strAllExts;
		strFilter += _T('\0');
	}

	wstrFilterName.LoadString(IDS_CTRL_LOAD_SAVE_FILTER);
	// フィルタ文字列を設定。
	while (!wstrFilterName.IsEmpty())
	{
		WString wstrTypeName = FFExtractUntil(wstrFilterName, L"\n");
		WString wstrExtension = FFExtractUntil(wstrFilterName, L"\n");

		strFilter += WStrToTStr(wstrTypeName);
		strFilter += _T(" (*");
		strFilter += WStrToTStr(wstrExtension);
		strFilter += _T(")");
		strFilter += _T('\0');
		strFilter += _T("*");
		strFilter += WStrToTStr(wstrExtension);
		strFilter += _T('\0');
	}
	strFilter += _T('\0');
} // FFGetFileDialogFilterString.
//----------</#DD VIEWER NOT EXPORT>-----------

#endif

/*************************************************************************
 * <関数>	FFInitDlgLayout
 *
 * <機能>	ダイアログのコントロールの初期位置によって、レイアウト情報
 *			mpLayoutInfo を設定する。
 *
 * <引数>	mpLayoutInfo	:レイアウト情報。設定して返す。
 *			pWnd			:ダイアログウィンドウを指定。
 *			lControlSize	:コントロールの数を指定。
 *			aControlId		:コントロールのID配列を指定。
 *			dwOffsetFlag	:コントロールがダイアログのどの辺に追従するか
 *							 指定。
 *
 * <解説>	FFInitDlgLayout関数、FFInitDlgLayout関数は、リサイズ可能な
 *			ダイアログにおいて、リサイズ時のコントロールの再配置を
 *			自動化する。
 *			FFInitDlgLayout関数は、コントロールの再配置に必要な
 *			レイアウト情報をIdToDlgLayoutRecMap型変数に蓄積する。
 *			FFInitDlg関数は、コントロールの再配置を実行する。
 *			下図の場合を考える。
 *
 *             ┏━━━━━━━━━━━━━━━━━┓
 *             ┃┌───────────────┐┃
 *             ┃│                              │┃
 *             ┃│            IDC_A             │┃
 *             ┃│                              │┃
 *             ┃└───────────────┘┃
 *             ┃┌───┐    ┌───┐┌───┐┃
 *             ┃│IDC_B │    │IDC_C ││IDC_D │┃
 *             ┃└───┘    └───┘└───┘┃
 *             ┗━━━━━━━━━━━━━━━━━┛ 
 *                             ┌─┐                 
 *                           ┌┘  └┐               
 *                            ＼    ／                
 *                              ＼／                  
 *          ┏━━━━━━━━━━━━━━━━━━━━━┓
 *          ┃┌───────────────────┐┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃│                 IDC_A                │┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃│                                      │┃
 *          ┃└───────────────────┘┃
 *          ┃┌───┐            ┌───┐┌───┐┃
 *          ┃│IDC_B │            │IDC_C ││IDC_D │┃
 *          ┃└───┘            └───┘└───┘┃
 *          ┗━━━━━━━━━━━━━━━━━━━━━┛
 *
 *			IDC_A：ダイアログの四方に合わせてリサイズ
 *			IDC_B：ダイアログの左下に合わせて移動
 *			IDC_C：ダイアログの右下に合わせて移動
 *			IDC_D：ダイアログの右下に合わせて移動
 *
 *			この場合は、以下のように、FFInitDlgLayout関数を３回実行する。
 *			--------------------------------------------------------
 *			OnInitDialog ()
 *			{
 *				const long aId1[] = { IDC_A }
 *				const long aId2[] = { IDC_B }
 *				const long aId3[] = { IDC_C, IDC_D }
 *				FFInitDlgLayout(mpInfo, pWnd, lSize1, aId1,
 *								FCDlgLayoutRec::HOOK_LEFT |
 *								FCDlgLayoutRec::HOOK_TOP |
 *								FCDlgLayoutRec::HOOK_RIGHT |
 *								FCDlgLayoutRec::HOOK_BOTTOM);
 *				FFInitDlgLayout(mpInfo, pWnd, lSize2, aId2,
 *								FCDlgLayoutRec::HOOK_LEFT |
 *								FCDlgLayoutRec::HOOK_BOTTOM);
 *				FFInitDlgLayout(mpInfo, pWnd, lSize3, aId3,
 *								FCDlgLayoutRec::HOOK_RIGHT |
 *								FCDlgLayoutRec::HOOK_BOTTOM);
 *			}
 *			--------------------------------------------------------
 *
 *			コントロールの再配置は以下のようにFFDlgLayout関数を１回実行する。
 *			--------------------------------------------------------
 *			OnSize (int cx, int cy)
 *			{
 *				FFDlgLayout(mpInfo, pWnd, CSize(cx, cy));
 *			}
 *			--------------------------------------------------------
 *
 * <履歴>	05.11.11 Fukushiro M. 作成
 *************************************************************************/
void UtilDlg::InitDlgLayout (	IdToDlgLayoutRecMap& mpLayoutInfo,
						HWND hWnd,
						long lControlSize,
						const long aControlId[],
						DWORD dwOffsetFlag)
{
	const CWnd* pWnd = CWnd::FromHandle(hWnd);
	// ダイアログのウィンドウ領域を取得。
	CRect rtParent;
	pWnd->GetClientRect(rtParent);
	pWnd->ClientToScreen(rtParent);
	long lC;
	for (lC = 0; lC != lControlSize; lC++)
	{
		FCDlgLayoutRec rec(dwOffsetFlag, CRect(LONG_MAX, LONG_MAX, LONG_MAX, LONG_MAX));
		// コントロールのウィンドウ領域をスクリーン座標系で取得。
		CRect rtControl;
		pWnd->GetDlgItem(aControlId[lC])->GetWindowRect(rtControl);
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_LEFT)
			rec.m_rtDistance.left = rtControl.left - rtParent.left;
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_TOP)
			rec.m_rtDistance.top = rtControl.top - rtParent.top;
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_RIGHT)
			rec.m_rtDistance.right = rtParent.right - rtControl.right;
		if (dwOffsetFlag & FCDlgLayoutRec::HOOK_BOTTOM)
			rec.m_rtDistance.bottom = rtParent.bottom - rtControl.bottom;
		mpLayoutInfo[aControlId[lC]] = rec;
	}
} // InitDlgLayout.

/*************************************************************************
 * <関数>	FFDlgLayout
 *
 * <機能>	ダイアログのコントロールを再配置する。
 *
 * <引数>	mpLayoutInfo	:レイアウト情報を指定。
 *			pWnd			:ダイアログウィンドウを指定。
 *			szWnd			:新しいダイアログウィンドウサイズを指定
 *
 * <解説>	FFInitDlgLayout関数、FFInitDlgLayout関数は、リサイズ可能な
 *			ダイアログにおいて、リサイズ時のコントロールの再配置を
 *			自動化する。
 *			詳しくはFFInitDlgLayout関数の解説を参照。
 *
 * <履歴>	05.11.11 Fukushiro M. 作成
 *************************************************************************/
void UtilDlg::DlgLayout (	const IdToDlgLayoutRecMap& mpLayoutInfo,
					HWND hWnd)
{
	CWnd* pWnd = CWnd::FromHandle(hWnd);
	// ダイアログのウィンドウ領域を取得。
	CRect rtParent;
	pWnd->GetClientRect(rtParent);

	IdToDlgLayoutRecMap::const_iterator iInfo;
	for (iInfo = mpLayoutInfo.begin(); iInfo != mpLayoutInfo.end(); iInfo++)
	{
		// コントロールの矩形サイズを取得。
		CRect rtControl;
		pWnd->GetDlgItem(iInfo->first)->GetWindowRect(rtControl);
		pWnd->ScreenToClient(rtControl);

		const CSize szControl(rtControl.Width(), rtControl.Height());
		// コントロールの矩形を設定。
		rtControl.SetRect(LONG_MAX, LONG_MAX, LONG_MAX, LONG_MAX);
		if (iInfo->second.m_rtDistance.left != LONG_MAX)
			rtControl.left = rtParent.left + iInfo->second.m_rtDistance.left;
		if (iInfo->second.m_rtDistance.top != LONG_MAX)
			rtControl.top = rtParent.top + iInfo->second.m_rtDistance.top;
		if (iInfo->second.m_rtDistance.right != LONG_MAX)
			rtControl.right = rtParent.right - iInfo->second.m_rtDistance.right;
		if (iInfo->second.m_rtDistance.bottom != LONG_MAX)
			rtControl.bottom = rtParent.bottom - iInfo->second.m_rtDistance.bottom;
		// デフォルトの座標を補正。
		if (rtControl.left == LONG_MAX)
			rtControl.left = rtControl.right - szControl.cx;
		else
		if (rtControl.right == LONG_MAX)
			rtControl.right = rtControl.left + szControl.cx;
		if (rtControl.top == LONG_MAX)
			rtControl.top = rtControl.bottom - szControl.cy;
		else
		if (rtControl.bottom == LONG_MAX)
			rtControl.bottom = rtControl.top + szControl.cy;

		// コントロールをダイアログサイズ変更に合わせて移動。
		pWnd->GetDlgItem(iInfo->first)->MoveWindow(rtControl);
		// コントロールを再描画。MoveWindow では、サイズが変わらない場合、
		// 中身の再描画をしないよう(移動前の場所の再描画はする)なので、
		// ここで強制的に再描画する。
		pWnd->GetDlgItem(iInfo->first)->RedrawWindow();
	}
} // DlgLayout.

#if 0

//----- 07.11.01 Fukushiro M. 追加始 ()-----
/*************************************************************************
 * <関数>	FFGetRevisedDlgItemXtmc
 *
 * <機能>	ダイアログの入力欄用の、xtmc値補正関数。
 *
 * <引数>	xtmCurValue		:入力欄の現在値を指定。
 *			xtmMinValue		:下限値を指定。
 *			bIsIncludeMinEq	:xtmMinValueを下限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			xtmMaxValue		:上限値を指定。
 *			bIsIncludeMaxEq	:xtmMaxValueを上限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			bIsSimpleRevise	:単純に上限、下限に補正する場合は真を指定。
 *
 * <返値>	補正後の値。
 *
 * <解説>	ダイアログの入力枠の下限・上限が(10,100)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 *
 * <履歴>	07.11.01 Fukushiro M. 作成
 *************************************************************************/
xtmc FFGetRevisedDlgItemXtmc (	xtmc xtmCurValue,
								xtmc xtmMinValue,
								BOOL bIsIncludeMinEq,
								xtmc xtmMaxValue,
								BOOL bIsIncludeMaxEq,
								BOOL bIsSimpleRevise)
{
	if (!bIsIncludeMinEq) xtmMinValue++;
	if (!bIsIncludeMaxEq) xtmMaxValue--;
	if (0 <= xtmCurValue)
	//----- xtmCurValue が正の場合 -----
	{
		// 正の場合、後ろに数値を加えることで、値は増えるのみ。
		// まず、上限を超えた場合は上限に補正。
		if (xtmMaxValue < xtmCurValue) return xtmMaxValue;
		// 下限〜上限に収まる場合は変更しない。
		if (xtmMinValue <= xtmCurValue) return xtmCurValue;

		//----- 下限を下回る場合 -----

		// 単純補正の場合は、下限値に補正。
		if (bIsSimpleRevise) return xtmMinValue;
		// 値が０の場合は下限値に補正。上限、下限とも正の値。
		if (xtmCurValue == 0) return xtmMinValue;
		// 小数点の場合は下限値に補正。
		if (xtmCurValue < 10000) return xtmMinValue;
		// 下限値を上回るまで、後ろに０を付けくわえる。
		while (xtmCurValue < xtmMinValue)
			xtmCurValue *= 10;
		// 上限を超えてしまった場合は、下限値に補正。
		if (xtmMaxValue < xtmCurValue) return xtmMinValue;
		// 補正後の値を返す。
		return xtmCurValue;
	} else
	//----- xtmCurValue が負の場合 -----
	{
		// 負の場合、後ろに数値を加えることで、値は減るのみ。
		// まず、下限を下回る場合は下限に補正。
		if (xtmCurValue < xtmMinValue) return xtmMinValue;
		// 上限〜下限に収まる場合は変更しない。
		if (xtmCurValue <= xtmMaxValue) return xtmCurValue;

		//----- 上限を上回る場合 -----

		// 単純補正の場合は、上限値に補正。
		if (bIsSimpleRevise) return xtmMaxValue;
		// 小数点の場合は上限値に補正。
		if (-10000 < xtmCurValue) return xtmMaxValue;
		// 上限値を下回るまで、後ろに０を付けくわえる。
		while (xtmMaxValue < xtmCurValue)
			xtmCurValue *= 10;
		// 下限を下回ってしまった場合は、上限値に補正。
		if (xtmCurValue < xtmMinValue) return xtmMaxValue;
		// 補正後の値を返す。
		return xtmCurValue;
	}
} // FFGetRevisedDlgItemXtmc.

//********************************************************************************************
/*!
 * @brief	ダイアログの入力欄用の、xtmc値補正関数。
 *
 *			ダイアログの入力枠の下限・上限が(10,100)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 * @author	Fukushiro M.
 * @date	2014/02/17(月) 18:19:30
 *
 * @param[in]	xtmc8	xtm8CurValue	入力欄の現在値を指定。
 * @param[in]	xtmc8	xtm8MinValue	下限値を指定。
 * @param[in]	BOOL	bIsIncludeMinEq	xtm8MinValueを下限値に含む場合は真、含まない場合は偽を指定。
 * @param[in]	xtmc8	xtm8MaxValue	上限値を指定。
 * @param[in]	BOOL	bIsIncludeMaxEq	xtm8MaxValueを上限値に含む場合は真、含まない場合は偽を指定。
 * @param[in]	BOOL	bIsSimpleRevise	単純に上限、下限に補正する場合は真を指定。
 *
 * @return	xtmc8	補正後の値。
 */
//********************************************************************************************
xtmc8 FFGetRevisedDlgItemXtmc8 (xtmc8 xtm8CurValue,
								xtmc8 xtm8MinValue,
								BOOL bIsIncludeMinEq,
								xtmc8 xtm8MaxValue,
								BOOL bIsIncludeMaxEq,
								BOOL bIsSimpleRevise)
{
	if (!bIsIncludeMinEq) xtm8MinValue++;
	if (!bIsIncludeMaxEq) xtm8MaxValue--;
	if (0 <= xtm8CurValue)
	//----- xtm8CurValue が正の場合 -----
	{
		// 正の場合、後ろに数値を加えることで、値は増えるのみ。
		// まず、上限を超えた場合は上限に補正。
		if (xtm8MaxValue < xtm8CurValue) return xtm8MaxValue;
		// 下限〜上限に収まる場合は変更しない。
		if (xtm8MinValue <= xtm8CurValue) return xtm8CurValue;

		//----- 下限を下回る場合 -----

		// 単純補正の場合は、下限値に補正。
		if (bIsSimpleRevise) return xtm8MinValue;
		// 値が０の場合は下限値に補正。上限、下限とも正の値。
		if (xtm8CurValue == 0) return xtm8MinValue;
		// 小数点の場合は下限値に補正。
		if (xtm8CurValue < 100000000) return xtm8MinValue;
		// 下限値を上回るまで、後ろに０を付けくわえる。
		while (xtm8CurValue < xtm8MinValue)
			xtm8CurValue *= 10;
		// 上限を超えてしまった場合は、下限値に補正。
		if (xtm8MaxValue < xtm8CurValue) return xtm8MinValue;
		// 補正後の値を返す。
		return xtm8CurValue;
	} else
	//----- xtm8CurValue が負の場合 -----
	{
		// 負の場合、後ろに数値を加えることで、値は減るのみ。
		// まず、下限を下回る場合は下限に補正。
		if (xtm8CurValue < xtm8MinValue) return xtm8MinValue;
		// 上限〜下限に収まる場合は変更しない。
		if (xtm8CurValue <= xtm8MaxValue) return xtm8CurValue;

		//----- 上限を上回る場合 -----

		// 単純補正の場合は、上限値に補正。
		if (bIsSimpleRevise) return xtm8MaxValue;
		// 小数点の場合は上限値に補正。
		if (-100000000 < xtm8CurValue) return xtm8MaxValue;
		// 上限値を下回るまで、後ろに０を付けくわえる。
		while (xtm8MaxValue < xtm8CurValue)
			xtm8CurValue *= 10;
		// 下限を下回ってしまった場合は、上限値に補正。
		if (xtm8CurValue < xtm8MinValue) return xtm8MaxValue;
		// 補正後の値を返す。
		return xtm8CurValue;
	}
} // FFGetRevisedDlgItemXtmc8.

//********************************************************************************************
/*!
 * @brief	ダイアログの入力欄用の、double値補正関数。
 *
 *			ダイアログの入力枠の下限・上限が(10.0,100.0)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 * @author	Fukushiro M.
 * @date	2014/02/17(月) 15:58:22
 *
 * @param[in]	double	dCurValue		入力欄の現在値を指定。
 * @param[in]	double	dMinValue		下限値を指定。
 * @param[in]	BOOL	bIsIncludeMinEq	dMinValueを下限値に含む場合は真、含まない場合は偽を指定。
 * @param[in]	double	dMaxValue		上限値を指定。
 * @param[in]	BOOL	bIsIncludeMaxEq	dMaxValueを上限値に含む場合は真、含まない場合は偽を指定。
 * @param[in]	BOOL	bIsSimpleRevise	単純に上限、下限に補正する場合は真を指定。
 *
 * @return	double	補正後の値。
 */
//********************************************************************************************
double FFGetRevisedDlgItemDouble (	double dCurValue,
									double dMinValue,
									BOOL bIsIncludeMinEq,
									double dMaxValue,
									BOOL bIsIncludeMaxEq,
									BOOL bIsSimpleRevise)
{
	// 上限、下限ともに値に含まれる必要がある。
	ASSERT(bIsIncludeMinEq);
	ASSERT(bIsIncludeMaxEq);

	if (0 <= dCurValue)
	//----- dCurValue が正の場合 -----
	{
		// 正の場合、後ろに数値を加えることで、値は増えるのみ。
		// まず、上限を超えた場合は上限に補正。
		if (dMaxValue < dCurValue) return dMaxValue;
		// 下限〜上限に収まる場合は変更しない。
		if (dMinValue <= dCurValue) return dCurValue;

		//----- 下限を下回る場合 -----

		// 単純補正の場合は、下限値に補正。
		if (bIsSimpleRevise) return dMinValue;
		// 値が０の場合は下限値に補正。上限、下限とも正の値。
		if (almost0(dCurValue)) return dMinValue;
		// 小数点の場合は下限値に補正。
		if (dCurValue < 1.0) return dMinValue;
		// 下限値を上回るまで、後ろに０を付けくわえる。
		while (dCurValue < dMinValue)
			dCurValue *= 10.0;
		// 上限を超えてしまった場合は、下限値に補正。
		if (dMaxValue < dCurValue) return dMinValue;
		// 補正後の値を返す。
		return dCurValue;
	} else
	//----- dCurValue が負の場合 -----
	{
		// 負の場合、後ろに数値を加えることで、値は減るのみ。
		// まず、下限を下回る場合は下限に補正。
		if (dCurValue < dMinValue) return dMinValue;
		// 上限〜下限に収まる場合は変更しない。
		if (dCurValue <= dMaxValue) return dCurValue;

		//----- 上限を上回る場合 -----

		// 単純補正の場合は、上限値に補正。
		if (bIsSimpleRevise) return dMaxValue;
		// 小数点の場合は上限値に補正。
		if (-1.0 < dCurValue) return dMaxValue;
		// 上限値を下回るまで、後ろに０を付けくわえる。
		while (dMaxValue < dCurValue)
			dCurValue *= 10.0;
		// 下限を下回ってしまった場合は、上限値に補正。
		if (dCurValue < dMinValue) return dMaxValue;
		// 補正後の値を返す。
		return dCurValue;
	}
} // FFGetRevisedDlgItemDouble.

/*************************************************************************
 * <関数>	FFGetRevisedDlgItemHixc
 *
 * <機能>	ダイアログの入力欄用の、hixc値補正関数。
 *
 * <引数>	hixCurValue		:入力欄の現在値を指定。
 *			hixMinValue		:下限値を指定。
 *			bIsIncludeMinEq	:hixMinValueを下限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			hixMaxValue		:上限値を指定。
 *			bIsIncludeMaxEq	:hixMaxValueを上限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			bIsSimpleRevise	:単純に上限、下限に補正する場合は真を指定。
 *
 * <返値>	補正後の値。
 *
 * <解説>	ダイアログの入力枠の上限・下限が(10,100)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 *
 * <履歴>	07.11.01 Fukushiro M. 作成
 *************************************************************************/
hixc FFGetRevisedDlgItemHixc (	hixc hixCurValue,
								hixc hixMinValue,
								BOOL bIsIncludeMinEq,
								hixc hixMaxValue,
								BOOL bIsIncludeMaxEq,
								BOOL bIsSimpleRevise)
{
	if (!bIsIncludeMinEq) hixMinValue++;
	if (!bIsIncludeMaxEq) hixMaxValue--;
	if (0 <= hixCurValue)
	//----- hixCurValue が正の場合 -----
	{
		// 正の場合、後ろに数値を加えることで、値は増えるのみ。
		// まず、上限を超えた場合は上限に補正。
		if (hixMaxValue < hixCurValue) return hixMaxValue;
		// 下限〜上限に収まる場合は変更しない。
		if (hixMinValue <= hixCurValue) return hixCurValue;

		//----- 下限を下回る場合 -----

		// 単純補正の場合は、下限値に補正。
		if (bIsSimpleRevise) return hixMinValue;
		// 値が０の場合は下限値に補正。上限、下限とも正の値。
		if (hixCurValue == 0) return hixMinValue;
		// 小数点の場合は下限値に補正。
		if (hixCurValue < (hixc)FFReg()->GetUnitToHixCoeff()) return hixMinValue;
		// 下限値を上回るまで、後ろに０を付けくわえる。
		while (hixCurValue < hixMinValue)
			hixCurValue *= 10;
		// 上限を超えてしまった場合は、下限値に補正。
		if (hixMaxValue < hixCurValue) return hixMinValue;
		// 補正後の値を返す。
		return hixCurValue;
	} else
	//----- hixCurValue が負の場合 -----
	{
		// 負の場合、後ろに数値を加えることで、値は減るのみ。
		// まず、下限を下回る場合は下限に補正。
		if (hixCurValue < hixMinValue) return hixMinValue;
		// 上限〜下限に収まる場合は変更しない。
		if (hixCurValue <= hixMaxValue) return hixCurValue;

		//----- 上限を上回る場合 -----

		// 単純補正の場合は、上限値に補正。
		if (bIsSimpleRevise) return hixMaxValue;
		// 小数点の場合は上限値に補正。
		if (-(hixc)FFReg()->GetUnitToHixCoeff() < hixCurValue) return hixMaxValue;
		// 上限値を下回るまで、後ろに０を付けくわえる。
		while (hixMaxValue < hixCurValue)
			hixCurValue *= 10;
		// 下限を下回ってしまった場合は、上限値に補正。
		if (hixCurValue < hixMinValue) return hixMaxValue;
		// 補正後の値を返す。
		return hixCurValue;
	}
} // FFGetRevisedDlgItemHixc.

/*************************************************************************
 * <関数>	FFGetRevisedDlgItemDblpnt
 *
 * <機能>	ダイアログの入力欄用の、dblpnt値補正関数。
 *
 * <引数>	llCurValue	:入力欄の現在値を指定。
 *			llMinValue	:下限値を指定。
 *			bIsIncludeMinEq	:llMinValueを下限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			llMaxValue	:上限値を指定。
 *			bIsIncludeMaxEq	:llMaxValueを上限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			bIsSimpleRevise	:単純に上限、下限に補正する場合は真を指定。
 *
 * <返値>	補正後の値。
 *
 * <注意>	bIsIncludeMinEq,bIsIncludeMaxEq の両値はTRUEとして処理される。
 *			上限値・下限値が返る場合があるので、FALSEの場合は制限を超える。
 *
 * <解説>	ダイアログの入力枠の上限・下限が(10,100)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 *
 * <履歴>	07.11.01 Fukushiro M. 作成
 *************************************************************************/
dblpnt FFGetRevisedDlgItemDblpnt (	dblpnt dbptCurValue,
									dblpnt dbptMinValue,
									BOOL bIsIncludeMinEq,
									dblpnt dbptMaxValue,
									BOOL bIsIncludeMaxEq,
									BOOL bIsSimpleRevise)
{
	if (0 <= dbptCurValue)
	//----- dbptCurValue が正の場合 -----
	{
		// 正の場合、後ろに数値を加えることで、値は増えるのみ。
		// まず、上限を超えた場合は上限に補正。
		if (dbptMaxValue < dbptCurValue) return dbptMaxValue;
		// 下限〜上限に収まる場合は変更しない。
		if (dbptMinValue <= dbptCurValue) return dbptCurValue;

		//----- 下限を下回る場合 -----

		// 単純補正の場合は、下限値に補正。
		if (bIsSimpleRevise) return dbptMinValue;
		// 値が０の場合は下限値に補正。上限、下限とも正の値。
		if (almost0(dbptCurValue)) return dbptMinValue;
		// 下限値を上回るまで、後ろに０を付けくわえる。
		while (dbptCurValue < dbptMinValue)
			dbptCurValue *= 10.0;
		// 上限を超えてしまった場合は、下限値に補正。
		if (dbptMaxValue < dbptCurValue) return dbptMinValue;
		// 補正後の値を返す。
		return dbptCurValue;
	} else
	//----- dbptCurValue が負の場合 -----
	{
		// 負の場合、後ろに数値を加えることで、値は減るのみ。
		// まず、下限を下回る場合は下限に補正。
		if (dbptCurValue < dbptMinValue) return dbptMinValue;
		// 上限〜下限に収まる場合は変更しない。
		if (dbptCurValue <= dbptMaxValue) return dbptCurValue;

		//----- 上限を上回る場合 -----

		// 単純補正の場合は、上限値に補正。
		if (bIsSimpleRevise) return dbptMaxValue;
		// 値が０の場合は上限値に補正。上限、下限とも負の値。
		if (almost0(dbptCurValue)) return dbptMaxValue;
		// 上限値を下回るまで、後ろに０を付けくわえる。
		while (dbptMaxValue < dbptCurValue)
			dbptCurValue *= 10.0;
		// 下限を下回ってしまった場合は、上限値に補正。
		if (dbptCurValue < dbptMinValue) return dbptMaxValue;
		// 補正後の値を返す。
		return dbptCurValue;
	}
} // FFGetRevisedDlgItemDblpnt.

/*************************************************************************
 * <関数>	FFGetRevisedDlgItem__int64
 *
 * <機能>	ダイアログの入力欄用の、__int64値補正関数。
 *
 * <引数>	llCurValue	:入力欄の現在値を指定。
 *			llMinValue	:下限値を指定。
 *			bIsIncludeMinEq	:llMinValueを下限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			llMaxValue	:上限値を指定。
 *			bIsIncludeMaxEq	:llMaxValueを上限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			bIsSimpleRevise	:単純に上限、下限に補正する場合は真を指定。
 *
 * <返値>	補正後の値。
 *
 * <解説>	ダイアログの入力枠の上限・下限が(10,100)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 *
 * <履歴>	07.11.01 Fukushiro M. 作成
 *************************************************************************/
__int64 FFGetRevisedDlgItem__int64 (__int64 llCurValue,
									__int64 llMinValue,
									BOOL bIsIncludeMinEq,
									__int64 llMaxValue,
									BOOL bIsIncludeMaxEq,
									BOOL bIsSimpleRevise)
{
	if (!bIsIncludeMinEq) llMinValue++;
	if (!bIsIncludeMaxEq) llMaxValue--;
	if (0 <= llCurValue)
	//----- llCurValue が正の場合 -----
	{
		// 正の場合、後ろに数値を加えることで、値は増えるのみ。
		// まず、上限を超えた場合は上限に補正。
		if (llMaxValue < llCurValue) return llMaxValue;
		// 下限〜上限に収まる場合は変更しない。
		if (llMinValue <= llCurValue) return llCurValue;

		//----- 下限を下回る場合 -----

		// 単純補正の場合は、下限値に補正。
		if (bIsSimpleRevise) return llMinValue;
		// 値が０の場合は下限値に補正。上限、下限とも正の値。
		if (llCurValue == 0) return llMinValue;
		// 下限値を上回るまで、後ろに０を付けくわえる。
		while (llCurValue < llMinValue)
			llCurValue *= 10;
		// 上限を超えてしまった場合は、下限値に補正。
		if (llMaxValue < llCurValue) return llMinValue;
		// 補正後の値を返す。
		return llCurValue;
	} else
	//----- llCurValue が負の場合 -----
	{
		// 負の場合、後ろに数値を加えることで、値は減るのみ。
		// まず、下限を下回る場合は下限に補正。
		if (llCurValue < llMinValue) return llMinValue;
		// 上限〜下限に収まる場合は変更しない。
		if (llCurValue <= llMaxValue) return llCurValue;

		//----- 上限を上回る場合 -----

		// 単純補正の場合は、上限値に補正。
		if (bIsSimpleRevise) return llMaxValue;
		// 上限値を下回るまで、後ろに０を付けくわえる。
		while (llMaxValue < llCurValue)
			llCurValue *= 10;
		// 下限を下回ってしまった場合は、上限値に補正。
		if (llCurValue < llMinValue) return llMaxValue;
		// 補正後の値を返す。
		return llCurValue;
	}
} // FFGetRevisedDlgItem__int64.

/*************************************************************************
 * <関数>	FFGetRevisedDlgItemDWORD
 *
 * <機能>	ダイアログの入力欄用の、DWORD値補正関数。
 *
 * <引数>	llCurValue	:入力欄の現在値を指定。
 *			llMinValue	:下限値を指定。
 *			bIsIncludeMinEq	:llMinValueを下限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			llMaxValue	:上限値を指定。
 *			bIsIncludeMaxEq	:llMaxValueを上限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			bIsSimpleRevise	:単純に上限、下限に補正する場合は真を指定。
 *
 * <返値>	補正後の値。
 *
 * <解説>	ダイアログの入力枠の上限・下限が(10,100)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 *
 * <履歴>	07.11.01 Fukushiro M. 作成
 *************************************************************************/
DWORD FFGetRevisedDlgItemDWORD (DWORD dwCurValue,
								DWORD dwMinValue,
								BOOL bIsIncludeMinEq,
								DWORD dwMaxValue,
								BOOL bIsIncludeMaxEq,
								BOOL bIsSimpleRevise)
{
	return (DWORD)FFGetRevisedDlgItem__int64(	__int64(dwCurValue),
												__int64(dwMinValue),
												bIsIncludeMinEq,
												__int64(dwMaxValue),
												bIsIncludeMaxEq,
												bIsSimpleRevise);
} // FFGetRevisedDlgItemDWORD.

/*************************************************************************
 * <関数>	FFGetRevisedDlgItemLong
 *
 * <機能>	ダイアログの入力欄用の、long値補正関数。
 *
 * <引数>	llCurValue	:入力欄の現在値を指定。
 *			llMinValue	:下限値を指定。
 *			bIsIncludeMinEq	:llMinValueを下限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			llMaxValue	:上限値を指定。
 *			bIsIncludeMaxEq	:llMaxValueを上限値に含む場合は真、含まない
 *							 場合は偽を指定。
 *			bIsSimpleRevise	:単純に上限、下限に補正する場合は真を指定。
 *
 * <返値>	補正後の値。
 *
 * <解説>	ダイアログの入力枠の上限・下限が(10,100)の場合、'53'をキーで
 *			入力しようとして'5'を打った瞬間に、下限値を下回るため '10' に
 *			補正されてしまう。これではキー入力で'53'は入力できない。
 *			'5'を打った瞬間に、'50'に補正されるようにすれば、'53'は入力
 *			可能。（FFSetDlgItemTextEx関数によって、カレット位置は保存
 *			されるため、'5|0'のようになる）
 *			このような補正値を取得する。
 *
 * <履歴>	07.11.01 Fukushiro M. 作成
 *************************************************************************/
long FFGetRevisedDlgItemLong (	long lCurValue,
								long lMinValue,
								BOOL bIsIncludeMinEq,
								long lMaxValue,
								BOOL bIsIncludeMaxEq,
								BOOL bIsSimpleRevise)
{
	return (long)FFGetRevisedDlgItem__int64(__int64(lCurValue),
											__int64(lMinValue),
											bIsIncludeMinEq,
											__int64(lMaxValue),
											bIsIncludeMaxEq,
											bIsSimpleRevise);
} // FFGetRevisedDlgItemLong.
//----- 07.11.01 Fukushiro M. 追加終 ()-----

/*************************************************************************
 * <関数>	FFTabFrameInit
 *
 * <機能>	タブコントロールとフレームの初期化を行う。
 *
 * <引数>	vtc				:設定情報を返す。
 *			hWndDlg			:ダイアログのウィンドウを指定。
 *			lTabControlId	:タブコントロールのIDを指定。
 *			iFrameCount		:aFrameIdの配列サイズを指定。
 *			aFrameId		:フレームのコントロールID配列を指定。
 *							 配列の要素はタブコントロールの選択に一致する。
 *
 * <解説>	下図のダイアログリソースがある場合、FrameAに囲われたコントロール、
 *			FrameBに囲まれたコントロールを、Tabコントロール上に移動させる。
 *
 *           Dialog
 *          ┌───────────────┐
 *          │                              │
 *          │                              │
 *          │  ┌───┐                  │
 *          │  │      ├───┬───┐  │
 *          │  │      │      │      │  │
 *          │┌┘      └───┴───┴┐│
 *          ││                          ││
 *          ││    Tab                   ││
 *          ││                          ││
 *          ││                          ││
 *          │└─────────────┘│
 *          │          ┌──┐┌────┐│
 *          │          │ OK ││ Cancel ││
 *          │          └──┘└────┘│
 *          └───────────────┘
 *
 *                FrameA
 *               ┌─────────────┐
 *               │┌──┐     ┌────┐ │
 *               ││    │     │        │ │
 *               │└──┘     │        │ │
 *               │ ┌──┐    └────┘ │
 *               │ │    │                 │
 *               │ └──┘                 │
 *               └─────────────┘
 *
 *                FrameB
 *               ┌─────────────┐
 *               │         ┌──────┐ │
 *               │         │            │ │
 *               │         │            │ │
 *               │ ┌──┐│            │ │
 *               │ │    │└──────┘ │
 *               │ └──┘                 │
 *               └─────────────┘
 *
 * <履歴>	08.08.05 Fukushiro M. 作成
 *************************************************************************/
void FFTabFrameInit (	FTTabFrameData& vtc,
						HWND hWndDlg,
						long lTabControlId,
						int iFrameCount,
						const int aFrameId[])
{
	vtc.clear();
	vtc.resize(iFrameCount);

	// タブコントロールの描画領域を取得する。
	CRect rtTab;
	GetWindowRect(GetDlgItem(hWndDlg, lTabControlId), &rtTab);
	FFScreenToClient(hWndDlg, &rtTab);
	((CTabCtrl*)CWnd::FromHandle(GetDlgItem(hWndDlg, lTabControlId)))->AdjustRect(FALSE, rtTab);

	// 各フレームの領域を取得する。
	std::vector<CRect> vrtFrame(iFrameCount);
	for (int iTabIdx = 0; iTabIdx != iFrameCount; iTabIdx++)
	{
		GetWindowRect(GetDlgItem(hWndDlg, aFrameId[iTabIdx]), &vrtFrame[iTabIdx]);
		FFScreenToClient(hWndDlg, &vrtFrame[iTabIdx]);
	}

	// ダイアログ中の全コントロールをループ。
	for (HWND hWndItem = ::GetWindow(hWndDlg, GW_CHILD);
		 hWndItem != NULL;
		 hWndItem = ::GetNextWindow(hWndItem, GW_HWNDNEXT))
	{
		// コントロールのIDを取得。
		const int iCtrlId = GetDlgCtrlID(hWndItem);
		// コントロールがフレームの場合はスキップ。
		BOOL bIsSkip = FALSE;
		for (int iTabIdx = 0; !bIsSkip && iTabIdx != iFrameCount; iTabIdx++)
		{
			if (iCtrlId == aFrameId[iTabIdx])
				bIsSkip = TRUE;
		}
		if (bIsSkip) continue;

		// コントロールの領域を取得。
		CRect rtItem;
		::GetWindowRect(hWndItem, &rtItem);
		FFScreenToClient(hWndDlg, &rtItem);

		// フレームをループ。
		for (int iTabIdx = 0; iTabIdx != iFrameCount; iTabIdx++)
		{
			// コントロールがフレームに含まれる場合。
			if (CRect().IntersectRect(vrtFrame[iTabIdx], rtItem))
			{
				// コントロールのテキストを取得。
				WString wstrText;
				FFGetWindowText(hWndItem, wstrText);
				// コントロール情報を設定。
//----- 10.02.20 Fukushiro M. 変更前 ()-----
//				vtc[iTabIdx].vid.push_back(iCtrlId);
//				vtc[iTabIdx].vwstrText.push_back(wstrText);
//----- 10.02.20 Fukushiro M. 変更後 ()-----
				vtc[iTabIdx][iCtrlId] = wstrText;
//----- 10.02.20 Fukushiro M. 変更終 ()-----
				// コントロールをタブ上に移動する。
				::MoveWindow(hWndItem,
							rtTab.left + (rtItem.left - vrtFrame[iTabIdx].left),
							rtTab.top + (rtItem.top - vrtFrame[iTabIdx].top),
							rtItem.Width(),
							rtItem.Height(),
							FALSE);
				break;
			}
		}
	}
} // FFTabFrameInit.

/*************************************************************************
 * <関数>	FFTabFrameSelChanged
 *
 * <機能>	タブコントロールの選択が変更されたときに実行する。
 *
 * <引数>	vtc				:設定情報を指定。
 *			hWndDlg			:ダイアログのウィンドウを指定。
 *			lTabControlId	:タブコントロールのIDを指定。
 *
 * <解説>	フレーム中の各コントロールの表示状態を切り替えて、選択中のタブ
 *			に一致するコントロールが表示されるようにする。
 *			また、非表示になったコントロールはアクセスキーがヒットしないよう、
 *			ウィンドウテキストを無効な値に書き換える。
 *
 * <履歴>	08.08.05 Fukushiro M. 作成
 *************************************************************************/
void FFTabFrameSelChanged (	const FTTabFrameData& vtc,
							HWND hWndDlg,
							long lTabControlId)
{
	// 新しく表示されるタブのインデックス。
	const int iTabCurSel = ((CTabCtrl*)CWnd::FromHandle(GetDlgItem(hWndDlg, lTabControlId)))->GetCurSel();
	for (int iTabIdx = 0; iTabIdx != vtc.size(); iTabIdx++)
	{
		if (iTabIdx == iTabCurSel)
		//----- 新しく表示されるタブに属するコントロール -----
		{
//----- 10.02.21 Fukushiro M. 変更前 ()-----
//			for (int iCC = 0; iCC != vtc[iC].vid.size(); iCC++)
//			{
//				ShowWindow(GetDlgItem(hWndDlg, vtc[iC].vid[iCC]), SW_SHOW);
//				FFSetWindowText(GetDlgItem(hWndDlg, vtc[iC].vid[iCC]), vtc[iC].vwstrText[iCC]);
//			}
//----- 10.02.21 Fukushiro M. 変更後 ()-----
			m_const_for (FTCtrlIdToTextMap, iIdTx, vtc[iTabIdx])
			{
				ShowWindow(GetDlgItem(hWndDlg, (*iIdTx).first), SW_SHOW);
				FFSetWindowText(GetDlgItem(hWndDlg, (*iIdTx).first), (*iIdTx).second);
			}
//----- 10.02.21 Fukushiro M. 変更終 ()-----
		} else
		//----- 新しく表示されるタブ以外のタブに属するコントロール -----
		{
//----- 10.02.21 Fukushiro M. 変更前 ()-----
//			for (int iCC = 0; iCC != vtc[iC].vid.size(); iCC++)
//			{
//				ShowWindow(GetDlgItem(hWndDlg, vtc[iC].vid[iCC]), SW_HIDE);
//				FFSetWindowText(GetDlgItem(hWndDlg, vtc[iC].vid[iCC]), L"");
//			}
//----- 10.02.21 Fukushiro M. 変更後 ()-----
			m_const_for (FTCtrlIdToTextMap, iIdTx, vtc[iTabIdx])
			{
				ShowWindow(GetDlgItem(hWndDlg, (*iIdTx).first), SW_HIDE);
				FFSetWindowText(GetDlgItem(hWndDlg, (*iIdTx).first), L"");
			}
//----- 10.02.21 Fukushiro M. 変更終 ()-----
		}
	}
} // FFTabFrameSelChanged.

//----- 10.02.21 Fukushiro M. 削除始 ()-----
//BOOL FFTabFrameGetItemText (const FTTabFrameData& vtc,
//							long lChildControlId,
//							WString& wstrText)
//{
//	m_const_for (FTTabFrameData, i, vtc)
//	{
//		for (int iC = 0; iC != (*i).vid.size(); iC++)
//		{
//			if ((*i).vid[iC] == lChildControlId)
//			{
//				wstrText = (*i).vwstrText[iC];
//				return TRUE;
//			}
//		}
//	}
//	return FALSE;
//} // FFTabFrameGetItemText.
//----- 10.02.21 Fukushiro M. 削除終 ()-----

/*************************************************************************
 * <関数>	FFTabFrameSetItemText
 *
 * <機能>	指定されたコントロールのテキストを書き換える。
 *
 * <引数>	vtc				:設定情報を指定。
 *			hWndDlg			:ダイアログのウィンドウを指定。
 *			lChildControlId	:テキストを変更したいコントロールのIDを指定。
 *			wcpText			:変更後のテキストを指定。
 *
 * <返値>	指定したコントロールが存在する場合はTRUE。
 *
 * <解説>	非表示中の場合は変更しない。但し、次に表示されたときには変更後
 *			のテキストになっている。
 *
 * <履歴>	08.08.05 Fukushiro M. 作成
 *************************************************************************/
BOOL FFTabFrameSetItemText (FTTabFrameData& vtc,
							HWND hWndDlg,
							long lChildControlId,
							const wchar_t* wcpText)
{
	for (int iTabIdx = 0; iTabIdx != vtc.size(); iTabIdx++)
	{
		FTCtrlIdToTextMap& ctrlIdToTextMap = vtc[iTabIdx];
//----- 10.02.21 Fukushiro M. 変更前 ()-----
//		for (int iC = 0; iC != (*i).vid.size(); iC++)
//		{
//			if ((*i).vid[iC] == lChildControlId)
//			{
//				(*i).vwstrText[iC] = wcpText;
//				// 表示中ならばテキストを設定。アクセスキーの影響があるため、
//				// 非表示中にテキストを変更してはならない。
//				if (IsWindowVisible(GetDlgItem(hWndDlg, lChildControlId)))
//					FFSetDlgItemText(hWndDlg, lChildControlId, wcpText);
//				return TRUE;
//			}
//		}
//----- 10.02.21 Fukushiro M. 変更後 ()-----
		FTCtrlIdToTextMap::iterator iIdTx = ctrlIdToTextMap.find(lChildControlId);
		if (iIdTx != ctrlIdToTextMap.end())
		{
			(*iIdTx).second = wcpText;
			// 表示中ならばテキストを設定。アクセスキーの影響があるため、
			// 非表示中にテキストを変更してはならない。
			if (IsWindowVisible(GetDlgItem(hWndDlg, lChildControlId)))
				FFSetDlgItemText(hWndDlg, lChildControlId, wcpText);
			return TRUE;
		}
//----- 10.02.21 Fukushiro M. 変更終 ()-----
	}
	return FALSE;
} // FFTabFrameSetItemText.

/*************************************************************************
 * <関数>	FFTabFrameCtlColor
 *
 * <機能>	タブコントロールが置かれたダイアログのOnCtlColor関数から実行する。
 *
 * <引数>	vtc				:設定情報を指定。
 *			hWndDlg			:ダイアログのウィンドウを指定。
 *			lTabControlId	:タブコントロールのIDを指定。
 *			lChildControlId	:コントロールのIDを指定。
 *			nCtlColor		:コントロールのタイプを指定。
 *
 * <返値>	コントロール背景用ブラシ。変更の必要が無い場合はNULL。
 *
 * <解説>	タブコントロールの背景は白だが、スタティックやチェックボタンの
 *			背景はダイアログ背景と同じ（グレー）なので、上に乗せると背景が
 *			違う色になってしまう。
 *			このため、スタティック、ボタンコントロールの背景を白にする。
 *			タブコントロールの背景色を取得したかったが、取得する方法が
 *			見つからない。
 *			ウィンドウクラス登録のブラシも取得したが正しい値ではなかった（常に白）。
 *			GetSysColor とも連動していない。
 *			タブコントロールの子に付け替えてもダメ。
 *			やむなく白と決め打ちにした。
 *
 * <履歴>	10.02.21 Fukushiro M. 作成
 *************************************************************************/
HBRUSH FFTabFrameCtlColor (	const FTTabFrameData& vtc,
							HWND hWndDlg,
							long lTabControlId,
							long lChildControlId,
							UINT nCtlColor)
{
	// タブのインデックス。
	const int iTabCurSel = ((CTabCtrl*)CWnd::FromHandle(GetDlgItem(hWndDlg, lTabControlId)))->GetCurSel();
	const FTCtrlIdToTextMap& ctrlIdToTextMap = vtc[iTabCurSel];
	switch (nCtlColor)
	{
	case CTLCOLOR_BTN:		// ボタン コントロール
	case CTLCOLOR_STATIC:	// 静的コントロール
		if (ctrlIdToTextMap.find(lChildControlId) != ctrlIdToTextMap.end())
			return (HBRUSH)GetStockObject(WHITE_BRUSH);
		break;
	}
	return NULL;
} // FFTabFrameCtlColor.

//********************************************************************************************
/*!
 * @brief	コントロールを適切な大きさにリサイズする。
 *			コントロールに設定されているテキストに合わせてコントロールを適切な大きさにリサイズする。
 * @author	Fukushiro M.
 * @date	2014/09/09(火) 17:32:20
 *
 * @param[in]	CWnd*	control	コントロール。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void FFResizeToIdeal (CWnd* control)
{
	CSize sz = FFGetIdealSize(control);
	// ダイアログをリサイズ。
	control->SetWindowPos(NULL, 0, 0, sz.cx, sz.cy,
						SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOOWNERZORDER|
						SWP_NOSENDCHANGING|SWP_NOMOVE|SWP_NOZORDER);
} // FFResizeToIdeal

//********************************************************************************************
/*!
 * @brief	コントロールの適切な大きさを計算する。
 *			コントロールに設定されているテキストに合わせたコントロールサイズを計算する。
 * @author	Fukushiro M.
 * @date	2014/09/09(火) 18:42:56
 *
 * @param[in]	CWnd*	control	コントロール。
 *
 * @return	CSize	
 */
//********************************************************************************************
CSize FFGetIdealSize (CWnd* control)
{
	static const int DEFAULT_PADDING_X = 4;
	static const int DEFAULT_PADDING_Y = 4;

	if (control->IsKindOf(RUNTIME_CLASS(CButton)))
	{
		CButton* button = (CButton*)control;
		switch (button->GetButtonStyle() & BS_TYPEMASK)
		{
		case BS_DEFPUSHBUTTON:
		case BS_PUSHBUTTON:			// 通常ボタン
		  {
			// ボタンの最適な幅を計算。GetIdealSizeはPUSHBUTTON以外では正常に動作しない。
			CSize sz;
			((CButton*)control)->GetIdealSize(&sz);
			// 計算値はぎりぎりサイズなので少し加算する。
			return sz + CSize(DEFAULT_PADDING_X, DEFAULT_PADDING_Y);
		  }
		}
	}

	CSize szDefault;
	{
		// CButtonにおいて、GetIdealSizeはPUSHBUTTON以外では正常に動作しない。このため、文字から計算。
		CDC* pDC = control->GetDC();
		CFont* pFontOld = pDC->SelectObject(control->GetFont());
		// コントロールのテキストを取得。
		WString wstrText;
		FFGetWindowText(control, wstrText);
		GetTextExtentPoint32(pDC->m_hDC, wstrText, wstrText.GetLength(), &szDefault);
		pDC->SelectObject(pFontOld);
		control->ReleaseDC(pDC);
	}

	if (control->IsKindOf(RUNTIME_CLASS(CButton)))
	{
		CButton* button = (CButton*)control;
		switch (button->GetButtonStyle() & BS_TYPEMASK)
		{
		case BS_RADIOBUTTON:		// ラジオボタン
		case BS_AUTORADIOBUTTON:	// ラジオボタン
		case BS_CHECKBOX:			// チェックボタン
			// ボタンサイズ(20px)と少し加算する。
			return szDefault + CSize(20 + DEFAULT_PADDING_X, DEFAULT_PADDING_Y);
		default:	/// BS_GROUPBOX:			// グループボックス
			return szDefault + CSize(DEFAULT_PADDING_X, DEFAULT_PADDING_Y);
		}
	} else
	if (control->IsKindOf(RUNTIME_CLASS(CStatic)))
	{
		return szDefault;
	} else
	if (control->IsKindOf(RUNTIME_CLASS(CEdit)))
	{
		return CSize(D2L(szDefault.cx * 1.2), D2L(szDefault.cy * 1.5));
	} else
	{
		return szDefault + CSize(DEFAULT_PADDING_X, DEFAULT_PADDING_Y);
	}
} // FFGetIdealSize
#endif