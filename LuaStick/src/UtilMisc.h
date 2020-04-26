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

	template<typename T> static const T & FindMapValue(const std::map<std::string, T> & mp, const std::string & key, const T & defaultValue = T())
	{
		const auto i = mp.find(key);
		if (i == mp.end())
			return defaultValue;
		else
			return i->second;
	}
};

