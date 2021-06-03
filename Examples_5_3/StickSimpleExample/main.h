#pragma once

// main.h

// Do not include "Stickrun.h". It must be included via "Stick.h".
#include <map>

// Converter definition for LuaStick
/// <stickconv type1="std::map<std::string,__int64>" type2="std::map<std::wstring,int>" type1to2="myconv::map_to_map<std::wstring,int,std::string,__int64>" type2to1="myconv::map_to_map<std::string,__int64,std::wstring,int>" />
/// <stickconv type1="std::unordered_map<std::string,__int64>" type2="std::map<std::string,__int64>" type1to2="myconv::uomap_to_map" type2to1="myconv::map_to_uomap" />

// Exception definition for LuaStick


/// <stick export="true"/>
/// <summary>
/// Namespace.
/// </summary>
namespace myspace
{
	/// <stick export="true"/>
	/// <summary>
	/// Print message.
	/// </summary>
	/// <param name="message" io="in">message</param>
	/// <exception cref="std::exception &" />
	extern void Print(char const * message);

	/// <stick export="true"/>
	/// <summary>
	/// Add message-1 and message-2 together.
	/// </summary>
	/// <param name="v1" io="in">message-1</param>
	/// <param name="v2" io="in">message-2</param>
	/// <returns>result</returns>
	extern std::string Add(const std::string & v1, std::string const & v2);

	/// <stick export="true"/>
	/// <summary>
	/// Add v2 to each member of v1.
	/// </summary>
	/// <param name="v1" io="inout">string array.</param>
	/// <param name="v2" io="in">string</param>
	extern void ArrayAdd(std::vector<std::wstring> & v1, const std::wstring & v2);

	/// <stick export="true"/>
	/// <summary>
	/// Add v2 to each member's value of v1.
	/// </summary>
	/// <param name="v1" io="inout">string->int hash.</param>
	/// <param name="v2" io="in">value</param>
	extern void HashAdd(std::map<std::wstring, int> & v1, int v2);
}

/// <summary>
/// Converters
/// </summary>
namespace myconv
{
	template<typename K2, typename V2, typename K1, typename V1>
	static void map_to_map(std::map<K2, V2> & dst, const std::map<K1, V1> & src)
	{
		dst.clear();
		for (const auto & kv : src)
			dst[kv.first] = (V2)kv.second;
	}

	template<>
	static void map_to_map(std::map<std::wstring, int> & dst, const std::map<std::string, __int64> & src)
	{
		dst.clear();
		for (const auto & kv : src)
		{
			std::wstring key2;
			Sticklib::T_to_U<std::wstring, std::string>(key2, kv.first);
			dst[key2] = (int)kv.second;
		}
	}

	template<>
	static void map_to_map(std::map<std::string, __int64> & dst, const std::map<std::wstring, int> & src)
	{
		dst.clear();
		for (const auto & kv : src)
		{
			std::string key2;
			Sticklib::T_to_U<std::string, std::wstring>(key2, kv.first);
			dst[key2] = (__int64)kv.second;
		}
	}

	template<typename K, typename V>
	static void uomap_to_map(std::map<K, V> & dst, const std::unordered_map<K, V> & src)
	{
		dst.clear();
		for (const auto & kv : src)
			dst[kv.first] = kv.second;
	}

	template<typename K, typename V>
	static void map_to_uomap(std::unordered_map<K, V> & dst, const std::map<K, V> & src)
	{
		dst.clear();
		for (const auto & kv : src)
			dst[kv.first] = kv.second;
	}
}