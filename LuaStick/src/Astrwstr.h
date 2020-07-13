#pragma once

#include <string>
#include <codecvt>

class Astrwstr
{
public:
	static auto & Converter()
	{
		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t > converter;
		return converter;
	}

	/// <summary>
	/// Convert UTF-8 text to UTF-16 and '\n' to '\r\n'.
	/// </summary>
	/// <param name="wstr"></param>
	/// <param name="astr"></param>
	/// <returns></returns>
	static std::wstring & astr_to_wstr(std::wstring & wstr, const std::string & astr)
	{
		wstr = Converter().from_bytes(astr);
		size_t start_pos = wstr.find(L'\n', 0);
		if (
			(start_pos != std::wstring::npos)
			&&
			(start_pos == 0 || wstr.at(start_pos - 1) != L'\r')
			)
		{
			do
			{
				wstr.replace(start_pos, 1, L"\r\n");
				start_pos = wstr.find(L'\n', start_pos + 2);
			} while (start_pos != std::wstring::npos);
		}
		return wstr;
	}

	/// <summary>
	/// Convert UTF-16 text to UTF-8 and '\r\n' to '\n'.
	/// </summary>
	/// <param name="wstr"></param>
	/// <param name="astr"></param>
	/// <returns></returns>
	static std::string & wstr_to_astr(std::string & astr, const std::wstring & wstr)
	{
		astr = Converter().to_bytes(wstr);
		size_t start_pos = astr.find("\r\n", 0);
		if (start_pos != std::string::npos)
		{
			do
			{
				astr.replace(start_pos, 2, "\n");
				start_pos = astr.find("\r\n", start_pos + 1);
			} while (start_pos != std::string::npos);
		}
		return astr;
	}
};

