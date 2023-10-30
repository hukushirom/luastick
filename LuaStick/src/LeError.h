// LeError.h
//

#pragma once

/// <summary>
/// 【注意！！！！！！！】
/// ここの定数、関数はC#のLeError.csと共通。ここを修正する場合、LeError.csの修正が必要。
/// </summary>
class LeError
{
public:
	//! エラーのタイプ。
	enum ErrorCode : __int32
	{
		NONE						= 0,	// エラー無し。
		GENERIC_ERROR				= 1,	// その他のエラー。
		SYSTEM						= 400,	// システムエラー。
		CANNOT_READ_FILE			= 407,	// ファイルからデータが読み出せません。
		CANNOT_WRITE_FILE			= 408,	// ファイルにデータを書き込めません。
		CANNOT_OPEN_FILE			= 410,	// ファイルが開けません。

		OPTION_NOT_SPECIFIED		= 10000,	// コマンドラインオプションが指定されていません。
		OPTION_IS_OUT_OF_RANGE		= 10001,	// コマンドラインオプションの値が範囲外です。
		FILE_NOT_SPECIFIED			= 10010,	// ファイルが指定されていません。
		TAG_OCCURED_UNEXPECTED_PLACE= 11000,	// LuaStick tag occured at the unexpected place.
		UNEXPECTED_EOF				= 11001,	// Application encountered unexpected EOF.
		UNEXPECTED_CHAR				= 11002,	// Application encountered unexpected character.
		REGISTER_SAME_NAME			= 11003,	// Tried to register the same name twice.
		CONVERTER_UNDEFINED			= 11004,	// Type-to-type converter is not defined.
		INVALID_VARIABLE_IO_DEFINED	= 11005,	// The io definition of the variable was invalid.
		WRONG_DEFINED_TAG			= 11006,	// LuaStick tag description is invalid.
		NOT_SUPPORTED				= 11007,	// Unsupported expression occured.
		TYPE_UNDEFINED				= 11008,	// The type is not defined.
// 20.01.18 Fukushiro M. 1行削除 ()
//		CANNOT_EXPORT_LOCAL_INSTANCE = 11009,	// Cannot export the local instance to Lua because of a possibility of instance lost.
	
//----- 19.11.25 Fukushiro M. 削除始 ()-----
//		OUT_OF_HEAP_MEMORY			= 101,	// ヒープ領域が不足しています。
//		DATA_SIZE_TOO_LARGE			= 102,	// データサイズが大きすぎます。
//		TOPNODE_NOT_EXIST_IN_DATA	= 103,	// トップノードが定義されていません。
//		IMAGE_FILE_SIZE_TOO_LARGE	= 104,	// 画像ファイルのサイズが大きすぎます。
//		UNKNOWN_IMAGE_FORMAT		= 105,	// サポートされていない画像形式です。
//		NO_ICONFILE_IN_DATA			= 106,	// データ中にアイコンファイル名が記載されていません。
//		ICONTBL_ICONNAME_DUPLICATED	= 107,	// データ中にアイコン名が２つ以上使われています。
//		CANNOT_OPEN_GRAPH_FILE		= 108,	// グラフデータファイルが開けません。
//		INVALID_LINE				= 109,	// 不正な記述です。
//		UNREGISTERED_ICON_ID		= 110,	// アイコンテーブルデータに未登録のアイコンです。
//		NO_END_OF_TAG				= 111,	// <<end of>>の記述がありません。
//		NODEID_DUPLICATED_IN_DATA	= 112,	// データ中にノードIDが２つ以上使われています。
//		UNREGISTERED_NODE_ID		= 113,	// 未登録のノードIDです。
//		TOPNODE_DUPLICATED_IN_DATA	= 114,	// トップノードが２つ以上定義されています。
//		LOOPED_NODE_HIERARCHY		= 115,	// ノードの階層がループしています。
//		NO_WIDTH_HEIGHT				= 116,	// 幅、高さのいずれかを指定する必要があります。
//		INTERRUPT_TIME_OUT			= 117,	// セッション停止処理はタイムアウトしました。
//		INSUFFICIENT_BUFFER			= 118,	// バッファサイズが不足しています。
//		NODE_LOOP_EXIST_IN_DATA		= 119,	// ノード階層にループがあります。
//		INVALID_PARAMETER			= 120,	// 不正なパラメーターが指定されています。
//		INVALID_GRAPH_ID			= 126,	// 無効なグラフIDです。
//		TOO_LONG_PATH_NAME			= 128,	// パス名が長すぎます。
//		FILE_ACCESS_DENIED			= 129,	// ファイルにアクセスできません。
//		FILE_EXIST					= 130,	// ファイルが既に存在します。
//		IMAGE_SIZE_TOO_LARGE		= 131,	// 画像サイズが大きすぎます。
//		OUT_OF_SYSTEM_MEMORY		= 136,	// システムメモリーが不足しています。
//		FOLDER_CREATION_FAILED		= 137,	// フォルダーの作成に失敗しました。
//		FILE_DELETION_FAILED		= 138,	// ファイル、フォルダーの削除に失敗しました。
//		FOLDER_EXIST				= 139,	// フォルダーが既に存在します。
//		CODE_CONVERSION_FAILED		= 140,	// 文字コードの変換に失敗しました。
//		CANNOT_CREATE_IMAP_FILE		= 141,	// 画像マップファイルが作成できません。
//		CANNOT_WRITE_IMAP_FILE		= 142,	// 画像マップファイルに書き込みができません。
//		FILE_NOT_FOUND				= 143,	// ファイルが見つかりません。
//		TOO_LONG_PATH_EXT			= 146,	// ファイル拡張子が長すぎます。
//		WRONG_COLOR_FORM			= 147,	// 色指定の形式が間違っています。
//		IMAGE_ENCODER_NOT_EXIST		= 148,	// イメージエンコーダーが有りません。
//		UNREGISTERED_IMAGE_ENCODER	= 149,	// 未登録のイメージエンコーダーです。
//		OBJECT_BUSY					= 153,	// オブジェクトが使用中です。
//		NOT_IMPLEMENTED				= 155,	// 機能がインプリメントされていません。
//		WIN32_ERROR					= 156,	// WIN32のエラーです。
//		WRONG_STATE					= 157,	// オブジェクトの状態が不良です。
//		ABORTED						= 158,	// アボートしました。
//		VALUE_OVERFLOW				= 160,	// 数値がオーバーフローしました。
//		FONT_FAMILY_NOT_FOUND		= 163,	// 指定されたフォント名のフォントが見つかりません。
//		FONT_STYLE_NOT_FOUND		= 164,	// 指定されたフォントスタイルのフォントが見つかりません。
//		NOT_TRUE_TYPE_FONT			= 165,	// 指定されたフォントはTrueTypeではありません。
//		UNSUPPORTED_GDIPLUS_VERSION	= 166,	// システムにインストールされているGDI+とバージョンが異なります。
//		GDIPLUS_NOT_INITIALIZED		= 167,	// システムにGDI+がインストールされていません。
//		PROPERTY_NOT_FOUND			= 168,	// 指定されたプロパティは画像に含まれていません。
//		PROPERTY_NOT_SUPPORTED		= 169,	// 指定されたプロパティは画像でサポートされていません。
//		CANNOT_OPEN_ICON_TABLE_FILE	= 170,	// アイコンテーブルファイルが開けません。
//		UNLOCKED_GRAPH				= 171,	// ロックされていないグラフです。
//		TOO_LONG_NODE_ID			= 172,	// ノードIDが長すぎます。
//		SYSTEM						= 400,	// システムエラー。
//		COMMUNICATION_SYSTEM		= 401,	// 通信システムエラー。
//		TIMEOUT						= 402,	// タイムアウト。
//		NO_CHANNEL					= 403,	// 空チャンネルが有りません。
//		INTERRUPTED					= 404,	// 中断指示。
//		BUFFER_SIZE_TOO_SMALL		= 405,	// バッファーサイズが小さすぎます。
//		ALREADY_INITIALIZED			= 406,	// 初期化済みです。
//		CANNOT_READ_FILE			= 407,	// ファイルからデータが読み出せません。
//		CANNOT_WRITE_FILE			= 408,	// ファイルにデータを書き込めません。
//		CANNOT_OPEN_FILE			= 410,	// ファイルが開けません。
//		UNKNOWN_COMMAND				= 411,	// 未定義のコマンドです。
//		CHANNEL_COUNT_TOO_MANY		= 412,	// チャネル数が多すぎます。
//		CORRUPTED_HDD_DATA			= 413,	// HDDに保存されたデータが破損しています。
//		ICON_DRAWING_SIZE_TOO_LARGE	= 414,	// アイコンの描画サイズが大きすぎます。
//		TOO_SMALL_SYSTEM_MEMORY		= 415,	// システムメモリーが不足しています。
//		RELEASE_GRAPH_FOR_MEMORY_SHORTAGE	= 416,	// メモリー不足のためグラフを解放しました。
//		GETTING_IP_ADDRESS_FAILED	= 417,	// IPアドレスの取得が失敗しました。
//		NOT_ALLOWED_IP_ADDRESS		= 418,	// このPCには実行ライセンスがありません。
//		CANNOT_START_ENGINE_EXE		= 419,	// エンジンEXEが起動できません。
//
//		INVALID_COMMAND_LINE		= 1001,		// 不正なコマンドラインが指定されています。
//		SAME_ENGINE_ALREADY_EXIST	= 1002,		// 同じIDのエンジンが既に稼働しています。
//		ENGINE_TERMINATION_FAILED	= 1003,		// エンジンの停止に失敗しました。
//		DISCONNECTED_NODE_EXIST		= 1004,		// メインツリーと接続されていないノードが存在します。
//		STYLEID_DUPLICATED_IN_DATA	= 1005,		// データ中にスタイルIDが２つ以上使われています。
//		ICON_TEXT_PROTRUDE_BEYOND_NODE = 1006,	// アイコンまたはテキストがノードフレームから大きくはみ出しています。
//		INCOMPATIBLE_ATTR_DESIGNATED = 1007,	// 同時に選択できない属性が指定されています。
//		DESIGNATED_STYLE_NOT_REGISTERED = 1008,	// 未登録のスタイルが指定されています。
//		STYLE_ATTRIBUTES_CONFLICTED	= 1009,		// 矛盾するスタイル属性が指定されています。
//		DELETED_ICON_GROUP_ACCESSED	= 1010,		// 削除されたアイコングループにアクセスしました。
//		CANNOT_DELETE_REFERRED_ICON_GROUP = 1011,		// 参照されているアイコングループは削除できません。
//		ICON_GROUP_NOT_FOUND		= 1012,		// 指定されたアイコングループは存在しません。
//		RESOURCE_LOCK_FAILED		= 1013,		// リソースのロックに失敗しました。
//		CANNOT_START_ENGINE_KEEPER	= 1014,		// エンジンキーパーが起動できません。
//
//		// 【注意！！！！！！！】
//		// ここの定数、関数はC#のLeError.csと共通。ここを修正する場合、LeError.csの修正が必要。
//----- 19.11.25 Fukushiro M. 削除終 ()-----
	}; // enum ErrorCode

	//********************************************************************************************
	/*!
	 * @brief	エラーコードに対応するエラーメッセージを取得する。
	 * @author	Fukushiro M.
	 * @date	2010/09/13(月) 17:09:52
	 *
	 * @param[in]	ErrorCode	_errorCode	エラーコード。
	 *
	 * @return	const wchar_t*	エラーメッセージ。
	 */
	//********************************************************************************************
	static const wchar_t* GetErrorCodeMessage (ErrorCode errorCode)
	{
		switch (errorCode)
		{
#if defined(FD_JAPANESE)

		case LeError::NONE:							return L"エラー無し";
		case LeError::GENERIC_ERROR:				return L"その他のエラー";
		case LeError::SYSTEM:						return L"システムエラー";
		case LeError::CANNOT_READ_FILE:				return L"ファイルからデータが読み出せません";
		case LeError::CANNOT_WRITE_FILE:			return L"ファイルにデータを書き込めません";
		case LeError::CANNOT_OPEN_FILE:				return L"ファイルが開けません";

		case LeError::OPTION_NOT_SPECIFIED:			return L"コマンドラインオプションが指定されていません。";
		case LeError::OPTION_IS_OUT_OF_RANGE:		return L"コマンドラインオプションの値が範囲外です。";

		case LeError::FILE_NOT_SPECIFIED:			return L"ファイルが指定されていません。";
		case LeError::TAG_OCCURED_UNEXPECTED_PLACE:	return L"LuaStickのタグが間違った場所で使われています。";
		case LeError::UNEXPECTED_EOF:				return L"予期しないEOFです。";
		case LeError::UNEXPECTED_CHAR:				return L"予期しない文字が使われています。";
		case LeError::REGISTER_SAME_NAME:			return L"同じ名前で２回登録しています";
		case LeError::CONVERTER_UNDEFINED:			return L"型変換コンバーターが定義されていません。";
		case LeError::INVALID_VARIABLE_IO_DEFINED:	return L"変数のio定義が間違っています。";
		case LeError::WRONG_DEFINED_TAG:			return L"LuaStickのタグに間違いがあります。";
		case LeError::NOT_SUPPORTED:				return L"サポートしていない構文です。";
		case LeError::TYPE_UNDEFINED:				return L"サポートしていないタイプです。";
// 20.01.18 Fukushiro M. 1行削除 ()
//		case LeError::CANNOT_EXPORT_LOCAL_INSTANCE:	return L"ローカルインスタンスは失われる可能性があるためLuaにエクスポートできません。";

//----- 19.11.25 Fukushiro M. 削除始 ()-----
//		case LeError::OUT_OF_HEAP_MEMORY:			return L"ヒープ領域が不足しています";
//		case LeError::DATA_SIZE_TOO_LARGE:			return L"データサイズが大きすぎます";
//		case LeError::TOPNODE_NOT_EXIST_IN_DATA:	return L"トップノードが定義されていません";
//		case LeError::IMAGE_FILE_SIZE_TOO_LARGE:	return L"画像ファイルのサイズが大きすぎます";
//		case LeError::UNKNOWN_IMAGE_FORMAT:			return L"サポートされていない画像形式です";
//		case LeError::NO_ICONFILE_IN_DATA:			return L"データ中にアイコンファイル名が記載されていません";
//		case LeError::ICONTBL_ICONNAME_DUPLICATED:	return L"データ中にアイコン名が２つ以上使われています";
//		case LeError::CANNOT_OPEN_GRAPH_FILE:		return L"グラフデータファイルが開けません";
//		case LeError::INVALID_LINE:					return L"不正な記述です";
//		case LeError::UNREGISTERED_ICON_ID:			return L"アイコンテーブルデータに未登録のアイコンです";
//		case LeError::NO_END_OF_TAG:				return L"end of>>の記述がありません";
//		case LeError::NODEID_DUPLICATED_IN_DATA:	return L"データ中にノードIDが２つ以上使われています";
//		case LeError::UNREGISTERED_NODE_ID:			return L"未登録のノードIDです";
//		case LeError::TOPNODE_DUPLICATED_IN_DATA:	return L"トップノードが２つ以上定義されています";
//		case LeError::LOOPED_NODE_HIERARCHY:		return L"ノードの階層がループしています";
//		case LeError::NO_WIDTH_HEIGHT:				return L"幅、高さのいずれかを指定する必要があります";
//		case LeError::INTERRUPT_TIME_OUT:			return L"セッション停止処理はタイムアウトしました";
//		case LeError::INSUFFICIENT_BUFFER:			return L"バッファサイズが不足しています";
//		case LeError::NODE_LOOP_EXIST_IN_DATA:		return L"ノード階層にループがあります";
//		case LeError::INVALID_PARAMETER:			return L"不正なパラメーターが指定されています";
//		case LeError::INVALID_GRAPH_ID:				return L"無効なグラフIDです";
//		case LeError::TOO_LONG_PATH_NAME:			return L"パス名が長すぎます";
//		case LeError::FILE_ACCESS_DENIED:			return L"ファイルにアクセスできません";
//		case LeError::FILE_EXIST:					return L"ファイルが既に存在します";
//		case LeError::IMAGE_SIZE_TOO_LARGE:			return L"画像サイズが大きすぎます";
//		case LeError::OUT_OF_SYSTEM_MEMORY:			return L"システムメモリーが不足しています";
//		case LeError::FOLDER_CREATION_FAILED:		return L"フォルダーの作成に失敗しました";
//		case LeError::FILE_DELETION_FAILED:			return L"ファイル、フォルダーの削除に失敗しました";
//		case LeError::FOLDER_EXIST:					return L"フォルダーが既に存在します";
//		case LeError::CODE_CONVERSION_FAILED:		return L"文字コードの変換に失敗しました";
//		case LeError::CANNOT_CREATE_IMAP_FILE:		return L"画像マップファイルが作成できません";
//		case LeError::CANNOT_WRITE_IMAP_FILE:		return L"画像マップファイルに書き込みができません";
//		case LeError::FILE_NOT_FOUND:				return L"ファイルが見つかりません";
//		case LeError::TOO_LONG_PATH_EXT:			return L"ファイル拡張子が長すぎます";
//		case LeError::WRONG_COLOR_FORM:				return L"色指定の形式が間違っています";
//		case LeError::IMAGE_ENCODER_NOT_EXIST:		return L"イメージエンコーダーが有りません";
//		case LeError::UNREGISTERED_IMAGE_ENCODER:	return L"未登録のイメージエンコーダーです";
//		case LeError::OBJECT_BUSY:					return L"オブジェクトが使用中です";
//		case LeError::NOT_IMPLEMENTED:				return L"機能がインプリメントされていません";
//		case LeError::WIN32_ERROR:					return L"WIN32のエラーです";
//		case LeError::WRONG_STATE:					return L"オブジェクトの状態が不良です";
//		case LeError::ABORTED:						return L"アボートしました";
//		case LeError::VALUE_OVERFLOW:				return L"数値がオーバーフローしました";
//		case LeError::FONT_FAMILY_NOT_FOUND:		return L"指定されたフォント名のフォントが見つかりません";
//		case LeError::FONT_STYLE_NOT_FOUND:			return L"指定されたフォントスタイルのフォントが見つかりません";
//		case LeError::NOT_TRUE_TYPE_FONT:			return L"指定されたフォントはTrueTypeではありません";
//		case LeError::UNSUPPORTED_GDIPLUS_VERSION:	return L"システムにインストールされているGDI+とバージョンが異なります";
//		case LeError::GDIPLUS_NOT_INITIALIZED:		return L"システムにGDI+がインストールされていません";
//		case LeError::PROPERTY_NOT_FOUND:			return L"指定されたプロパティは画像に含まれていません";
//		case LeError::PROPERTY_NOT_SUPPORTED:		return L"指定されたプロパティは画像でサポートされていません";
//		case LeError::CANNOT_OPEN_ICON_TABLE_FILE:	return L"アイコンテーブルファイルが開けません";
//		case LeError::UNLOCKED_GRAPH:				return L"ロックされていないグラフです";
//		case LeError::TOO_LONG_NODE_ID:				return L"ノードIDが長すぎます";
//		case LeError::SYSTEM:						return L"システムエラー";
//		case LeError::COMMUNICATION_SYSTEM:			return L"通信システムエラー";
//		case LeError::TIMEOUT:						return L"タイムアウト";
//		case LeError::NO_CHANNEL:					return L"空チャンネルが有りません";
//		case LeError::INTERRUPTED:					return L"中断指示";
//		case LeError::BUFFER_SIZE_TOO_SMALL:		return L"バッファーサイズが小さすぎます";
//		case LeError::ALREADY_INITIALIZED:			return L"初期化済みです";
//		case LeError::CANNOT_READ_FILE:				return L"ファイルからデータが読み出せません";
//		case LeError::CANNOT_WRITE_FILE:			return L"ファイルにデータを書き込めません";
//		case LeError::CANNOT_OPEN_FILE:				return L"ファイルが開けません";
//		case LeError::UNKNOWN_COMMAND:				return L"未定義のコマンドです";
//		case LeError::CHANNEL_COUNT_TOO_MANY:		return L"チャネル数が多すぎます";
//		case LeError::CORRUPTED_HDD_DATA:			return L"HDDに保存されたデータが破損しています";
//		case LeError::ICON_DRAWING_SIZE_TOO_LARGE:	return L"アイコンの描画サイズが大きすぎます";
//		case LeError::TOO_SMALL_SYSTEM_MEMORY:		return L"システムメモリーが不足しています";
//		case LeError::RELEASE_GRAPH_FOR_MEMORY_SHORTAGE:	return L"メモリー不足のためグラフを解放しました";
//		case LeError::GETTING_IP_ADDRESS_FAILED:	return L"IPアドレスの取得が失敗しました";
//		case LeError::NOT_ALLOWED_IP_ADDRESS:		return L"このPCには実行ライセンスがありません";
//		case LeError::CANNOT_START_ENGINE_EXE:		return L"エンジンEXEが起動できません。";
//
//		case LeError::INVALID_COMMAND_LINE:			return L"不正なコマンドラインが指定されています。";
//		case LeError::SAME_ENGINE_ALREADY_EXIST:	return L"同じIDのエンジンが既に稼働しています。";
//		case LeError::ENGINE_TERMINATION_FAILED:	return L"エンジンの停止に失敗しました。";
//		case LeError::DISCONNECTED_NODE_EXIST:		return L"メインツリーと接続されていないノードが存在します。";
//		case LeError::STYLEID_DUPLICATED_IN_DATA:	return L"データ中にスタイルIDが２つ以上使われています。";
//		case LeError::ICON_TEXT_PROTRUDE_BEYOND_NODE:	return L"アイコンまたはテキストがノードフレームから大きくはみ出しています。";
//		case LeError::INCOMPATIBLE_ATTR_DESIGNATED:	return L"同時に選択できない属性が指定されています。";
//		case LeError::DESIGNATED_STYLE_NOT_REGISTERED:	return L"未登録のスタイルが指定されています。";
//		case LeError::STYLE_ATTRIBUTES_CONFLICTED:	return L"矛盾するスタイル属性が指定されています。";
//		case LeError::DELETED_ICON_GROUP_ACCESSED:	return L"削除されたアイコングループにアクセスしました。";
//		case LeError::CANNOT_DELETE_REFERRED_ICON_GROUP:	return L"参照されているアイコングループは削除できません。";
//		case LeError::ICON_GROUP_NOT_FOUND:			return L"指定されたアイコングループは存在しません。";
//		case LeError::RESOURCE_LOCK_FAILED:			return L"リソースのロックに失敗しました。";
//		case LeError::CANNOT_START_ENGINE_KEEPER:	return L"エンジンキーパーが起動できません。";
//
//		// 【注意！！！！！！！】
//		// ここの定数、関数はC#のLeError.csと共通。ここを修正する場合、LeError.csの修正が必要。
//----- 19.11.25 Fukushiro M. 削除終 ()-----

#elif defined(FD_ENGLISH)

		case LeError::NONE:							return L"No Error";
		case LeError::GENERIC_ERROR:				return L"Other Errors";
		case LeError::SYSTEM:						return L"System Error";
		case LeError::CANNOT_READ_FILE:				return L"Data cannot be read out from the file";
		case LeError::CANNOT_WRITE_FILE:			return L"Data cannot been written in the file";
		case LeError::CANNOT_OPEN_FILE:				return L"The file cannot be opened";

		case LeError::OPTION_NOT_SPECIFIED:			return L"Command line option is not specified.";
		case LeError::OPTION_IS_OUT_OF_RANGE:		return L"Value of command line is out of range.";

		case LeError::FILE_NOT_SPECIFIED:			return L"File is not specified.";
		case LeError::TAG_OCCURED_UNEXPECTED_PLACE:	return L"LuaStick tag occured at the unexpected place.";
		case LeError::UNEXPECTED_EOF:				return L"Application encountered unexpected EOF.";
		case LeError::UNEXPECTED_CHAR:				return L"Application encountered unexpected character.";
		case LeError::REGISTER_SAME_NAME:			return L"Tried to register the same name twice.";
		case LeError::CONVERTER_UNDEFINED:			return L"Type-to-type converter is not defined.";
		case LeError::INVALID_VARIABLE_IO_DEFINED:	return L"The io definition of the variable was invalid.";
		case LeError::WRONG_DEFINED_TAG:			return L"LuaStick tag description is invalid.";
		case LeError::NOT_SUPPORTED:				return L"Unsupported expression occured.";
		case LeError::TYPE_UNDEFINED:				return L"Unsupported type occured.";
// 20.01.18 Fukushiro M. 1行削除 ()
//		case LeError::CANNOT_EXPORT_LOCAL_INSTANCE:	return L"Cannot export the local instance to Lua because of a possibility of instance lost.";

//----- 19.11.25 Fukushiro M. 削除始 ()-----
//		case LeError::OUT_OF_HEAP_MEMORY:			return L"Heap area is too small";
//		case LeError::DATA_SIZE_TOO_LARGE:			return L"Too large data size";
//		case LeError::TOPNODE_NOT_EXIST_IN_DATA:	return L"Top node is not defined";
//		case LeError::IMAGE_FILE_SIZE_TOO_LARGE:	return L"Too large image file size";
//		case LeError::UNKNOWN_IMAGE_FORMAT:			return L"This is an unsupported image format";
//		case LeError::NO_ICONFILE_IN_DATA:			return L"Icon file name is not stated in the data";
//		case LeError::ICONTBL_ICONNAME_DUPLICATED:	return L"More than one icon name is used in the data";
//		case LeError::CANNOT_OPEN_GRAPH_FILE:		return L"Graph data file cannot be opened";
//		case LeError::INVALID_LINE:					return L"Wrong statement";
//		case LeError::UNREGISTERED_ICON_ID:			return L"This is an unregistered icon In the icon table data";
//		case LeError::NO_END_OF_TAG:				return L"end of >> is not stated";
//		case LeError::NODEID_DUPLICATED_IN_DATA:	return L"More than one node ID is used in the data";
//		case LeError::UNREGISTERED_NODE_ID:			return L"This node ID has not yet registered";
//		case LeError::TOPNODE_DUPLICATED_IN_DATA:	return L"More than one top node is defined";
//		case LeError::LOOPED_NODE_HIERARCHY:		return L"Hierarchy of node is looping";
//		case LeError::NO_WIDTH_HEIGHT:				return L"Selection for width or height is required";
//		case LeError::INTERRUPT_TIME_OUT:			return L"Session stop process is time out";
//		case LeError::INSUFFICIENT_BUFFER:			return L"Buffer size is too small";
//		case LeError::NODE_LOOP_EXIST_IN_DATA:		return L"A loop is in the node hierarchy";
//		case LeError::INVALID_PARAMETER:			return L"Wrong argument is being selected";
//		case LeError::INVALID_GRAPH_ID:				return L"This graph ID is invalid";
//		case LeError::TOO_LONG_PATH_NAME:			return L"Too long pass name";
//		case LeError::FILE_ACCESS_DENIED:			return L"The file cannot be accessed";
//		case LeError::FILE_EXIST:					return L"This file has already existed";
//		case LeError::IMAGE_SIZE_TOO_LARGE:			return L"Too large image sizes";
//		case LeError::OUT_OF_SYSTEM_MEMORY:			return L"System memory is low";
//		case LeError::FOLDER_CREATION_FAILED:		return L"Creation of the folder has been failed";
//		case LeError::FILE_DELETION_FAILED:			return L"Deletion of file or folder has been failed";
//		case LeError::FOLDER_EXIST:					return L"This folder has already existed";
//		case LeError::CODE_CONVERSION_FAILED:		return L"Conversion of character codes failed";
//		case LeError::CANNOT_CREATE_IMAP_FILE:		return L"Image map file cannot be created";
//		case LeError::CANNOT_WRITE_IMAP_FILE:		return L"Image map file cannot be written";
//		case LeError::FILE_NOT_FOUND:				return L"The file cannot be found";
//		case LeError::TOO_LONG_PATH_EXT:			return L"Too long file extension";
//		case LeError::WRONG_COLOR_FORM:				return L"The format of the color designation is wrong";
//		case LeError::IMAGE_ENCODER_NOT_EXIST:		return L"Image encoder does not exist";
//		case LeError::UNREGISTERED_IMAGE_ENCODER:	return L"This image encoder has not yet registered";
//		case LeError::OBJECT_BUSY:					return L"Object is in use";
//		case LeError::NOT_IMPLEMENTED:				return L"Function has not yet been implemented";
//		case LeError::WIN32_ERROR:					return L"It is WIN32 error";
//		case LeError::WRONG_STATE:					return L"Object condition is in a failure condition";
//		case LeError::ABORTED:						return L"Aborted";
//		case LeError::VALUE_OVERFLOW:				return L"The number is overflowed";
//		case LeError::FONT_FAMILY_NOT_FOUND:		return L"The font with the name designated cannot be found";
//		case LeError::FONT_STYLE_NOT_FOUND:			return L"The font with the style designated cannot be found";
//		case LeError::NOT_TRUE_TYPE_FONT:			return L"The font designated is not TrueType";
//		case LeError::UNSUPPORTED_GDIPLUS_VERSION:	return L"This is different from the version of GDI+ installed in the system";
//		case LeError::GDIPLUS_NOT_INITIALIZED:		return L"GDI+ has not been installed in the system";
//		case LeError::PROPERTY_NOT_FOUND:			return L"The property designated is not included in image";
//		case LeError::PROPERTY_NOT_SUPPORTED:		return L"The property designated is not supported by image";
//		case LeError::CANNOT_OPEN_ICON_TABLE_FILE:	return L"The icon table file cannot be opened";
//		case LeError::UNLOCKED_GRAPH:				return L"This is an unlocked graph";
//		case LeError::TOO_LONG_NODE_ID:				return L"Too long node ID";
//		case LeError::SYSTEM:						return L"System Error";
//		case LeError::COMMUNICATION_SYSTEM:			return L"Correspondence System Error";
//		case LeError::TIMEOUT:						return L"Time Out";
//		case LeError::NO_CHANNEL:					return L"No empty channel";
//		case LeError::INTERRUPTED:					return L"Cease Direction";
//		case LeError::BUFFER_SIZE_TOO_SMALL:		return L"Too small buffer size";
//		case LeError::ALREADY_INITIALIZED:			return L"Already Initialized";
//		case LeError::CANNOT_READ_FILE:				return L"Data cannot be read out from the file";
//		case LeError::CANNOT_WRITE_FILE:			return L"Data cannot been written in the file";
//		case LeError::CANNOT_OPEN_FILE:				return L"The file cannot be opened";
//		case LeError::UNKNOWN_COMMAND:				return L"This command has not yet defined";
//		case LeError::CHANNEL_COUNT_TOO_MANY:		return L"Too many channels";
//		case LeError::CORRUPTED_HDD_DATA:			return L"Data saved HDD is damaged";
//		case LeError::ICON_DRAWING_SIZE_TOO_LARGE:	return L"The drawing size of the icon is too large";
//		case LeError::TOO_SMALL_SYSTEM_MEMORY:		return L"System memory is low";
//		case LeError::RELEASE_GRAPH_FOR_MEMORY_SHORTAGE:	return L"Chat has been released due to shortage of memory";
//		case LeError::GETTING_IP_ADDRESS_FAILED:	return L"Obtaining of IP address failed";
//		case LeError::NOT_ALLOWED_IP_ADDRESS:		return L"This PC has no conduct license";
//		case LeError::CANNOT_START_ENGINE_EXE:		return L"Engine executable cannot be activated";
//
//		case LeError::INVALID_COMMAND_LINE:			return L"Invalid command line is designated";
//		case LeError::SAME_ENGINE_ALREADY_EXIST:	return L"Same id engine already exists";
//		case LeError::ENGINE_TERMINATION_FAILED:	return L"Engine termination failed";
//		case LeError::DISCONNECTED_NODE_EXIST:		return L"Some node disconnected from main tree exists";
//		case LeError::STYLEID_DUPLICATED_IN_DATA:	return L"More than one style ID is used in the data";
//		case LeError::ICON_TEXT_PROTRUDE_BEYOND_NODE:	return L"Icon or text protrudes beyond node";
//		case LeError::INCOMPATIBLE_ATTR_DESIGNATED:	return L"Incompatible attributes are designated";
//		case LeError::DESIGNATED_STYLE_NOT_REGISTERED:	return L"Designated style was not registered";
//		case LeError::STYLE_ATTRIBUTES_CONFLICTED:	return L"Conflicted style attributes are designated";
//		case LeError::DELETED_ICON_GROUP_ACCESSED:	return L"Deleted icon group was accessed";
//		case LeError::CANNOT_DELETE_REFERRED_ICON_GROUP: return L"Referred icon group cannot be deleted";
//		case LeError::ICON_GROUP_NOT_FOUND:			return L"The icon group designated is not found";
//		case LeError::RESOURCE_LOCK_FAILED:			return L"Acquiring resource lock was failed";
//		case LeError::CANNOT_START_ENGINE_KEEPER:	return L"EngineKeeper executable cannot be activated";
//
//		// 【注意！！！！！！！】
//		// ここの定数、関数はC#のLeError.csと共通。ここを修正する場合、LeError.csの修正が必要。
//----- 19.11.25 Fukushiro M. 削除終 ()-----
#else // defined(FD_ENGLISH)

#error "言語が指定されていません"

#endif // defined(FD_JAPANESE)

		default:									return L"";
		}
	} // GetErrorCodeMessage.

	static std::wstring GetErrorMessage(ErrorCode errorCode, const wchar_t * srcFileName, __int32 errorLine, const wchar_t* params)
	{
		try
		{
			return (params == nullptr || *params == L'\0') ?
					std::to_wstring((__int32)errorCode) + L":" +
					LeError::GetErrorCodeMessage(errorCode) + L":" +
					srcFileName + L":" +
					std::to_wstring(errorLine)
					:
					std::to_wstring((__int32)errorCode) + L":" +
					LeError::GetErrorCodeMessage(errorCode) + L":" +
					srcFileName + L":" +
					std::to_wstring(errorLine) + L":" +
					params
					;
		}
		catch (...)
		{
			// エラー処理で使われる関数なので、例外を発生させないようにする。
		}
		return std::wstring();
	}

	static std::wstring SystemErrorToString(DWORD messageId)
	{
		std::wstring message;
		try
		{
			LPWSTR lpMsgBuf;
			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				messageId,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&lpMsgBuf,
				0,
				nullptr);
			message = lpMsgBuf;
			// Free the buffer.
			::LocalFree(lpMsgBuf);
		}
		catch (...)
		{
			// エラー処理で使われる関数なので、例外を発生させないようにする。
		}
		return message;
	}
}; // class LeError

using ErrorCode = LeError::ErrorCode;
