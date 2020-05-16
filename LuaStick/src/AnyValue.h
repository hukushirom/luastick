#pragma once

#include "Astrwstr.h"

class AnyValue
{
private:
	enum class Type : __int16
	{
		NONE = 0,
		INT16,
		INT32,
		INT64,
		UINT16,
		UINT32,
		UINT64,
		DWORD,
		FLOAT,
		DOUBLE,
		BOOLEAN,
		CHARP,
		WCHARP,
	};

public:
	AnyValue() : m_type(Type::NONE) {};
	AnyValue(const __int16 & value)
		: m_type(Type::INT16)
		, m_int16(value)
	{}
	AnyValue(const __int32 & value)
		: m_type(Type::INT32)
		, m_int32(value)
	{}
	AnyValue(const __int64 & value)
		: m_type(Type::INT64)
		, m_int64(value)
	{}
	AnyValue(const uint16_t & value)
		: m_type(Type::UINT16)
		, m_uint16(value)
	{}
	AnyValue(const uint32_t & value)
		: m_type(Type::UINT32)
		, m_uint32(value)
	{}
	AnyValue(const uint64_t & value)
		: m_type(Type::UINT64)
		, m_uint64(value)
	{}
	AnyValue(const DWORD & value)
		: m_type(Type::DWORD)
		, m_dword(value)
	{}
	AnyValue(const float & value)
		: m_type(Type::FLOAT)
		, m_float(value)
	{}
	AnyValue(const double & value)
		: m_type(Type::DOUBLE)
		, m_double(value)
	{}
	AnyValue(const bool & value)
		: m_type(Type::BOOLEAN)
		, m_bool(value)
	{}
	AnyValue(const char * value)
		: m_type(Type::CHARP)
		, m_charp(value)
	{}
	AnyValue(const wchar_t * value)
		: m_type(Type::WCHARP)
		, m_wcharp(value)
	{}
	AnyValue(const std::string & value)
		: m_type(Type::CHARP)
		, m_charp(value.c_str())
	{}
	AnyValue(const std::wstring & value)
		: m_type(Type::WCHARP)
		, m_wcharp(value.c_str())
	{}

	bool IsNoValue() const
	{
		return (m_type == Type::NONE);
	}

	const char * GetCharP() const
	{
		if (m_type != Type::CHARP)
			throw "";
		return m_charp;
	}

	const wchar_t * GetWCharP() const
	{
		if (m_type != Type::WCHARP)
			throw "";
		return m_wcharp;
	}

	__int16 GetInt16() const
	{
		if (m_type != Type::INT16)
			throw "";
		return m_int16;
	}

	__int32 GetInt32() const
	{
		if (m_type != Type::INT32)
			throw "";
		return m_int32;
	}

	__int64 GetInt64() const
	{
		if (m_type != Type::INT64)
			throw "";
		return m_int64;
	}

	uint16_t GetUint16() const
	{
		if (m_type != Type::UINT16)
			throw "";
		return m_uint16;
	}

	uint32_t GetUint32() const
	{
		if (m_type != Type::UINT32)
			throw "";
		return m_uint32;
	}

	uint64_t GetUint64() const
	{
		if (m_type != Type::UINT64)
			throw "";
		return m_uint64;
	}

	DWORD GetDword() const
	{
		if (m_type != Type::DWORD)
			throw "";
		return m_dword;
	}

	float GetFloat() const
	{
		if (m_type != Type::FLOAT)
			throw "";
		return m_float;
	}

	double GetDouble() const
	{
		if (m_type != Type::DOUBLE)
			throw "";
		return m_double;
	}

	bool GetBoolean() const
	{
		if (m_type != Type::BOOLEAN)
			throw "";
		return m_bool;
	}

	std::string ToAString() const
	{
		switch (m_type)
		{
		case Type::INT16:
			return std::to_string(m_int16);
		case Type::INT32:
			return std::to_string(m_int32);
		case Type::INT64:
			return std::to_string(m_int64);
		case Type::UINT16:
			return std::to_string(m_uint16);
		case Type::UINT32:
			return std::to_string(m_uint32);
		case Type::UINT64:
			return std::to_string(m_uint64);
		case Type::DWORD:
			return std::to_string(m_dword);
		case Type::FLOAT:
			return std::to_string(m_float);
		case Type::DOUBLE:
			return std::to_string(m_double);
		case Type::BOOLEAN:
			return m_bool ? "true" : "false";
		case Type::CHARP:
			return m_charp;
		case Type::WCHARP:
		{
			std::string str;
			Astrwstr::wstr_to_astr(str, m_wcharp);
// 20.05.08  1行変更 ()
//			return str.c_str();
			return str;
		}
		default:	// case Type::NONE:
			return "";
		}
	}

	std::wstring ToWString() const
	{
		switch (m_type)
		{
		case Type::INT16:
			return std::to_wstring(m_int16);
		case Type::INT32:
			return std::to_wstring(m_int32);
		case Type::INT64:
			return std::to_wstring(m_int64);
		case Type::UINT16:
			return std::to_wstring(m_uint16);
		case Type::UINT32:
			return std::to_wstring(m_uint32);
		case Type::UINT64:
			return std::to_wstring(m_uint64);
		case Type::DWORD:
			return std::to_wstring(m_dword);
		case Type::FLOAT:
			return std::to_wstring(m_float);
		case Type::DOUBLE:
			return std::to_wstring(m_double);
		case Type::BOOLEAN:
			return m_bool ? L"true" : L"false";
		case Type::CHARP:
		{
			std::wstring wstr;
			Astrwstr::astr_to_wstr(wstr, m_charp);
// 20.05.08  1行変更 ()
//			return wstr.c_str();
			return wstr;
		}
		case Type::WCHARP:
			return m_wcharp;
		default:	// case Type::NONE:
			return L"";
		}
	}

	/// <summary>
	/// パラメーターで指定された値を文字列に変換し、カンマ「,」で接続する。
	/// </summary>
	/// <param name="value0"></param>
	/// <param name="value1"></param>
	/// <param name="value2"></param>
	/// <param name="value3"></param>
	/// <param name="value4"></param>
	/// <param name="value5"></param>
	/// <param name="value6"></param>
	/// <param name="value7"></param>
	/// <param name="value8"></param>
	/// <param name="value9"></param>
	/// <returns></returns>
	static std::wstring GetJoinWString(
		AnyValue value0 = AnyValue(),
		AnyValue value1 = AnyValue(),
		AnyValue value2 = AnyValue(),
		AnyValue value3 = AnyValue(),
		AnyValue value4 = AnyValue(),
		AnyValue value5 = AnyValue(),
		AnyValue value6 = AnyValue(),
		AnyValue value7 = AnyValue(),
		AnyValue value8 = AnyValue(),
		AnyValue value9 = AnyValue()
	)
	{
		const AnyValue * values[]{ &value0, &value1, &value2, &value3, &value4, &value5, &value6, &value7, &value8, &value9 };
		std::wstring result;
		for (auto value : values)
		{
			if (value->IsNoValue()) break;
			result += value->ToWString() + L",";
		}
		if (!result.empty())
			result.erase(result.end() - 1);
		return result;
	}

private:
	Type m_type;
	union
	{
		__int16 m_int16;
		__int32 m_int32;
		__int64 m_int64;
		uint16_t m_uint16;
		uint32_t m_uint32;
		uint64_t m_uint64;
		DWORD m_dword;
		float m_float;
		double m_double;
		bool m_bool;
		const char * m_charp;
		const wchar_t * m_wcharp;
	};
}; // class AnyValue.

#define NoValue AnyValue()
