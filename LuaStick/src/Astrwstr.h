#pragma once

#include <string>
// 22.09.06 Fukushiro M. 1行削除 (追加)
//#include <codecvt>

class Astrwstr
{
public:
//----- 22.09.06 Fukushiro M. 削除始 (追加)-----
//	static auto & Converter()
//	{
//		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t > converter;
//		return converter;
//	}
//----- 22.09.06 Fukushiro M. 削除終 (追加)-----

	/// <summary>
	/// Convert UTF-8 text to UTF-16 and '\n' to '\r\n'.
	/// </summary>
	/// <param name="wstr"></param>
	/// <param name="astr"></param>
	/// <returns></returns>
	static std::wstring & astr_to_wstr(std::wstring & wstr, const std::string & astr)
	{
//----- 22.09.06 Fukushiro M. 変更前 ()-----
//		wstr = Converter().from_bytes(astr);
//		size_t start_pos = wstr.find(L'\n', 0);
//		if (
//			(start_pos != std::wstring::npos)
//			&&
//			(start_pos == 0 || wstr.at(start_pos - 1) != L'\r')
//			)
//		{
//			do
//			{
//				wstr.replace(start_pos, 1, L"\r\n");
//				start_pos = wstr.find(L'\n', start_pos + 2);
//			} while (start_pos != std::wstring::npos);
//		}
//----- 22.09.06 Fukushiro M. 変更後 ()-----
		if (!astr.empty())
		{
			std::vector<wchar_t> vBuff(astr.length() + 1);
			const auto writeLen = MultiByteToWideChar(CP_UTF8, 0, astr.c_str(), (int)astr.length(), vBuff.data(), (int)vBuff.size());
			vBuff[writeLen] = L'\0';
			wstr = vBuff.data();

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
		}
		else
		{
			wstr.clear();
		}
//----- 22.09.06 Fukushiro M. 変更終 ()-----
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
//----- 22.09.06 Fukushiro M. 変更前 ()-----
//		astr = Converter().to_bytes(wstr);
//		size_t start_pos = astr.find("\r\n", 0);
//		if (start_pos != std::string::npos)
//		{
//			do
//			{
//				astr.replace(start_pos, 2, "\n");
//				start_pos = astr.find("\r\n", start_pos + 1);
//			} while (start_pos != std::string::npos);
//		}
//----- 22.09.06 Fukushiro M. 変更後 ()-----
		if (!wstr.empty())
		{
			auto iBuffSz = (wstr.length() + 1) * sizeof(wchar_t);
			std::vector<char> vBuff(iBuffSz);
			auto writtenLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), vBuff.data(), (int)vBuff.size(), nullptr, nullptr);
			if (writtenLen == 0)
			{
				//----- The size might become more than twice of the source size -----
				iBuffSz = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), nullptr, 0, nullptr, nullptr);
				vBuff.resize(iBuffSz);
				writtenLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), vBuff.data(), (int)vBuff.size(), nullptr, nullptr);
			}
			vBuff[writtenLen] = L'\0';
			astr = vBuff.data();

			size_t start_pos = astr.find("\r\n", 0);
			if (start_pos != std::string::npos)
			{
				do
				{
					astr.replace(start_pos, 2, "\n");
					start_pos = astr.find("\r\n", start_pos + 1);
				} while (start_pos != std::string::npos);
			}
		}
		else
		{
			astr.clear();
		}
//----- 22.09.06 Fukushiro M. 変更終 ()-----
		return astr;
	}
};

