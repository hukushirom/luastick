#pragma once

class UtilMisc
{
public:	
	/// <summary>
	/// Parses the option arguments and gets option params and source files.<para/>
	/// e.g. LuaStick.exe -out "c:\abc\hello" "c:\abc\xyz.h" "c:\abc\pqr.h"<para/>
	///     argc=5, argv={"LuaStick.exe", "-out", "c:\abc\hello", "c:\abc\xyz.h", "c:\abc\pqr.h"}<para/>
	/// -> optionToValue={"out" -> "c:\abc\hello"}<para/>
	/// -> sourceFiles={"c:\abc\xyz.h", "c:\abc\pqr.h"}
	/// </summary>
	/// <param name="optionToValue">The option to value hash.</param>
	/// <param name="sourceFiles">The option files.</param>
	/// <param name="argc">The argc.</param>
	/// <param name="argv">The argv.</param>
	static void ParseOptionArgs(
		std::unordered_map<std::wstring, std::wstring> & optionToValue,
		std::vector<std::wstring> & sourceFiles,
		int argc,
		const wchar_t * const argv[]
	)
	{
		std::wstring option = L"";
		for (int i = 1; i != argc; i++)
		{
			if (argv[i][0] == L'-')
			{	//----- "-"で始まる文字列の場合 -----
				// オプションモードを設定。
				option = argv[i] + 1;
				optionToValue[option] = L"";
			}
			else
			{	//----- "-"以外で始まる文字列の場合 -----
				if (!option.empty())
				{	//----- オプションモードの場合 -----
					optionToValue[option] = argv[i];
					option = L"";
				}
				else
				{	//----- 通常モードの場合 -----
					sourceFiles.emplace_back(argv[i]);
				}
			}
		}
		if (!option.empty())
			ThrowLeException(LeError::OPTION_NOT_SPECIFIED, option);	// オプションが指定されていません。
	}

	/// <summary>
	/// Find a value to match to the specified key and return the value.
	/// If the key is not found, it returns the default value.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="mp">Source map</param>
	/// <param name="key">Key</param>
	/// <param name="defaultValue">Default value</param>
	/// <returns>Matched value</returns>
	template<typename T>
	static const T & FindMapValue(const std::map<std::string, T> & mp, const std::string & key, const T & defaultValue = T())
	{
		const auto i = mp.find(key);
		if (i == mp.end())
			return defaultValue;
		else
			return i->second;
	}

	/// <summary>
	/// Try to parse the specified text as an integer value.
	/// text must be string of a number.
	/// e.g. "10" -> OK   "10H" -> NG
	/// </summary>
	/// <param name="value">Parsed value</param>
	/// <param name="text">Source text</param>
	/// <param name="base">Base number</param>
	/// <returns>true:Success/false:failed</returns>
	static bool TryParse(int & value, const char * text, int base)
	{
		char * end_ptr;
		auto v = strtol(text, &end_ptr, base);
		if (end_ptr[0] != '\0')
			return false;
		value = (int)v;
		return true;
	}
};

