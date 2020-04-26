#pragma once

#include "UtilSync.h"
#include "LeError.h"
#include "AnyValue.h"

/// <summary>
/// エラーメッセージを管理する。マルチスレッドで処理が実行されるため、例外をスローしてもスレッド呼出し元でキャッチできない。
/// そのため、ここにエラーを保存し、そのエラーコードをスレッド間で渡す。
/// </summary>
class _ErrorManager
{
private:	
	/// <summary>
	/// 蓄積できるメッセージ数。
	/// </summary>
	static constexpr size_t MESSAGE_COUNT = 100;

public:
	/// <summary>
	/// コンストラクター
	/// </summary>
	_ErrorManager()
		: m_nextErrorId(0)
	{}

	~_ErrorManager()
	{}
	
	/// <summary>
	/// エラー情報をエラーバッファに保存する。
	/// </summary>
	/// <param name="errorCode">エラーコード</param>
	/// <param name="lineNumber">エラー発生のソースコード行番号</param>
	/// <param name="filePath">エラー発生のソースコードファイルパス</param>
	/// <param name="params">パラメーター（オプション）</param>
	/// <returns>エラーID</returns>
	__int32 SetError(
		LeError::ErrorCode errorCode,
		__int32 lineNumber,
		const std::wstring & filePath,
		const std::wstring & params
	)
	{
		AutoLeaveCS acs(m_cs);
		const auto errorId = m_nextErrorId;
		m_nextErrorId++;
		if (m_nextErrorId == MESSAGE_COUNT)
			m_nextErrorId = 0;
		m_errorBuffer[errorId].m_errorCode = errorCode;
		m_errorBuffer[errorId].m_lineNumber = lineNumber;
		m_errorBuffer[errorId].m_filePath = filePath;
		m_errorBuffer[errorId].m_params = params;
		return errorId;
	}
	
	/// <summary>
	/// 指定されたエラーIDのエラー情報を返す。
	/// </summary>
	/// <param name="errorCode">エラーコード</param>
	/// <param name="lineNumber">エラー発生のソースコード行番号</param>
	/// <param name="filePath">エラー発生のソースコードファイルパス</param>
	/// <param name="params">パラメーター（オプション）</param>
	/// <param name="errorId">エラーID</param>
	void GetError(
		LeError::ErrorCode & errorCode,
		__int32 & lineNumber,
		std::wstring & filePath,
		std::wstring & params,
		__int32 errorId
	)
	{
		AutoLeaveCS acs(m_cs);
		errorCode = m_errorBuffer[errorId].m_errorCode;
		lineNumber = m_errorBuffer[errorId].m_lineNumber;
		filePath = m_errorBuffer[errorId].m_filePath;
		params = m_errorBuffer[errorId].m_params;
	}

private:
	/// <summary>
	/// エラーバッファの配列構成要素
	/// </summary>
	struct ErrorRec
	{
		ErrorRec()
			: m_errorCode(LeError::ErrorCode::NONE)
			, m_lineNumber(0)
		{}
		LeError::ErrorCode m_errorCode;	// エラーコード
		__int32 m_lineNumber;			// エラー発生のソースコード行番号
		std::wstring m_filePath;		// エラー発生のソースコードファイルパス
		std::wstring m_params;			// パラメーター（オプション）
	};

	// 次のエラーID。最後まで行ったら0に戻る。
	__int32 m_nextErrorId;
	// エラーバッファ。配列。
	ErrorRec m_errorBuffer[MESSAGE_COUNT];
	// CriticalSection。GetError、SetErrorの排他処理。
	AutoCS m_cs;
};

template<typename T> _ErrorManager & __ErrorManager()
{
	static _ErrorManager errorManager;
	return errorManager;
}

#define ErrorManager __ErrorManager<void>()

/// <summary>
/// マルチスレッド対応のエラー例外クラス。
/// 内部的には__int32のエラーIDのみ保存している。
/// 詳細なエラー情報は、エラーIDに対応した共有のエラーバッファに保存されている。
/// 例外はスレッドの呼出し元へは飛ばないため、スレッドがエラーIDを呼出し元に返すようにする。
/// 呼出し元はそのエラーIDからLeExceptionを再作成し、スローする。
/// </summary>
class BaseException
{
public:
	BaseException(__int32 errorId)
	 	: m_errorId(errorId)
	{}
//	BaseException()
//		: m_errorId(-1)
//	{}

	~BaseException() {}

	std::wstring GetMessage() const
	{
		LeError::ErrorCode errorCode;
		__int32 lineNumber;
		std::wstring filePath;
		std::wstring params;
		ErrorManager.GetError(
			errorCode,
			lineNumber,
			filePath,
			params,
			m_errorId
		);
		return LeError::GetErrorMessage(errorCode, filePath.c_str(), lineNumber, params.c_str());
	}

	LeError::ErrorCode GetErrorCode() const
	{
		LeError::ErrorCode errorCode;
		__int32 lineNumber;
		std::wstring filePath;
		std::wstring params;
		ErrorManager.GetError(
			errorCode,
			lineNumber,
			filePath,
			params,
			m_errorId
		);
		return errorCode;
	}

	__int32 GetErrorId() const
	{
		return m_errorId;
	}

	void OutErrorMessage(std::wostream& wstream) const
	{
		wstream << GetMessage() << std::endl;
	}

protected:
	__int32 m_errorId;
};

class LeException : public BaseException
{
public:
	LeException(const std::wstring & filePath, __int32 lineNumber, LeError::ErrorCode errorCode, const std::wstring & params)
		: BaseException(ErrorManager.SetError(errorCode, lineNumber, filePath, params))
	{}
};

class PsException : public BaseException
{
public:
	PsException(const std::wstring & filePath, __int32 lineNumber, LeError::ErrorCode errorCode, const std::wstring & params)
		: BaseException(ErrorManager.SetError(errorCode, lineNumber, filePath, params))
	{}
};


#if defined(_DEBUG)

#define ThrowLeException(ec, ...)	{ DebugBreak(); throw LeException(__WFILE__, __LINE__, ec, AnyValue::GetJoinWString(__VA_ARGS__)); }
#define ThrowLeSystemError(...)		{ DebugBreak(); throw LeException(__WFILE__, __LINE__, LeError::ErrorCode::SYSTEM, AnyValue::GetJoinWString(__VA_ARGS__)); }
#define ThrowLeGenericError(...)	{ DebugBreak(); throw LeException(__WFILE__, __LINE__, LeError::ErrorCode::GENERIC_ERROR, AnyValue::GetJoinWString(__VA_ARGS__)); }
#define CerrLeException(ec, ...)	{ DebugBreak(); LeException(__WFILE__, __LINE__, ec, AnyValue::GetJoinWString(__VA_ARGS__)).OutErrorMessage(std::wcerr); }

#else

#define ThrowLeException(ec, ...)	{ throw LeException(__WFILE__, __LINE__, ec, AnyValue::GetJoinWString(__VA_ARGS__)); }
#define ThrowLeSystemError(...)		{ throw LeException(__WFILE__, __LINE__, LeError::ErrorCode::SYSTEM, AnyValue::GetJoinWString(__VA_ARGS__)); }
#define ThrowLeGenericError(...)	{ throw LeException(__WFILE__, __LINE__, LeError::ErrorCode::GENERIC_ERROR, AnyValue::GetJoinWString(__VA_ARGS__)); }
#define CerrLeException(ec, ...)	{ LeException(__WFILE__, __LINE__, ec, AnyValue::GetJoinWString(__VA_ARGS__)).OutErrorMessage(std::wcerr); }

#endif

