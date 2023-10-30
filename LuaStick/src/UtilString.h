#pragma once

#include <string>
#include <unordered_set>
#include "AnyValue.h"
#include "LeException.h"

/// <summary>
/// Class for string functions.
/// </summary>
class UtilString
{
public:
	/// <summary>
	/// Format string.
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="format">Format.</param>
	/// <param name="...args">Arguments.</param>
	/// <returns></returns>
	template<typename ... Args>
	static std::string Format(const std::string & format, Args ... args)
	{
		std::vector<char> buffer(std::snprintf(nullptr, 0, format.c_str(), args ...) + 1);
		std::snprintf(buffer.data(), buffer.size(), format.c_str(), args ...);
		return std::string(buffer.data(), buffer.size() - 1);
	}

	/// <summary>
	/// Format string.
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="format">Format.</param>
	/// <param name="...args">Arguments.</param>
	/// <returns></returns>
	template<typename ... Args>
	static std::wstring Format(const std::wstring & format, Args ... args)
	{
		std::vector<wchar_t> buffer(std::swprintf(nullptr, 0, format.c_str(), args ...) + 1);
		std::swprintf(buffer.data(), buffer.size(), format.c_str(), args ...);
		return std::wstring(buffer.data(), buffer.size() - 1);
	}

//----- 21.05.18 Fukushiro M. 削除始 ()-----
//	/// <summary>
//	/// Skips the space.
//	/// </summary>
//	/// <param name="text">The text.</param>
//	/// <returns>next text pointer</returns>
//	static const wchar_t* SkipSpace(const wchar_t* text)
//	{
//		while (*text == L' ') text++;
//		return text;
//	}
//
//	/// <summary>
//	/// Skips the space.
//	/// </summary>
//	/// <param name="text">The text.</param>
//	/// <returns>next text pointer</returns>
//	static const char* SkipSpace(const char* text)
//	{
//		while (*text == ' ') text++;
//		return text;
//	}
//----- 21.05.18 Fukushiro M. 削除終 ()-----

	/// <summary>
	/// Skips the space and tab.
	/// </summary>
	/// <param name="text">The text.</param>
	/// <returns>next text pointer</returns>
	static const char* SkipSpaceTab(const char* text)
	{
		while (*text == ' ' || *text == '\t') text++;
		return text;
	}

//----- 21.05.18 Fukushiro M. 追加始 ()-----
	/// <summary>
	/// Skips the space and tab.
	/// </summary>
	/// <param name="text">The text.</param>
	/// <returns>next text pointer</returns>
	static const wchar_t* SkipSpaceTab(const wchar_t* text)
	{
		while (*text == L' ' || *text == L'\t') text++;
		return text;
	}
//----- 21.05.18 Fukushiro M. 追加終 ()-----

	/// <summary>
	/// Skips until next space.
	/// 次の空白までスキップする。
	/// </summary>
	/// <param name="m_singleText">The m_singleText.</param>
	/// <returns></returns>
	static const wchar_t* SkipUntilSpace(const wchar_t* text)
	{
		while (*text != L'\0' && *text != L' ') text++;
		return text;
	}

	/// <summary>
	/// Skips until next double quoartation.
	/// 次のダブルクォートまでスキップする。
	/// </summary>
	/// <param name="m_singleText">The m_singleText.</param>
	/// <returns></returns>
	static const wchar_t* SkipUntilDq(const wchar_t* text)
	{
		while (*text != L'\0' && *text != L'"') text++;
		return text;
	}

	/// <summary>
	/// Parses the command line.
	/// コマンドラインを解析する。オプション(-xx)のxxは小文字に変換される。
	/// 例：-id GL1 -gf "C:/temp/LE" -gt 3600 -cn 4
	///    optionToValue = {"id" -> "GL1", "gf" -> "C:/temp/LE", "gt" -> "3600", "cn" -> "4"}
	/// 例：-id -gt 3600 -cn 4
	///    optionToValue = {"id" -> "", "gt" -> "3600", "cn" -> "4"}
	/// 例：-id -gt 3600 4
	///    エラー。falseを返す。
	/// </summary>
	/// <param name="optionToValue">The option to value.</param>
	/// <param name="commandLine">The command line.</param>
	/// <returns>true:成功/false:エラー</returns>
	static bool ParseCommandLine(std::map<std::wstring, std::wstring>& optionToValue, const wchar_t* commandLine)
	{
		bool isError = false;
		std::wstring optBuff;
// 21.05.18 Fukushiro M. 1行変更 ()
//		for (const wchar_t* cmdLineTop = commandLine; *cmdLineTop != L'\0'; cmdLineTop = UtilString::SkipSpace(cmdLineTop))
		for (const wchar_t* cmdLineTop = commandLine; *cmdLineTop != L'\0'; cmdLineTop = UtilString::SkipSpaceTab(cmdLineTop))
		{
			if (*cmdLineTop == L'-')
			{
				//---- 先頭が-の場合（オプション名） -----
				cmdLineTop++;	// '-'をスキップ。
				auto optLength = UtilString::SkipUntilSpace(cmdLineTop) - cmdLineTop;
				optBuff.assign(cmdLineTop, cmdLineTop + optLength);
				cmdLineTop += optLength;
				optionToValue[optBuff] = L"";
			}
			else
			{
				//----- 先頭が-以外の場合（オプション値） -----
				// オプション名が未指定の場合はエラー
				if (optBuff.empty()) return false;
				std::wstring argBuff;
				if (*cmdLineTop == L'"')
				{
					//----- オプション値がダブルクォートで始まる場合 -----
					cmdLineTop++;	// '"'をスキップ。
					// 次のダブルクォートまでスキップし、ダブルクォートで囲まれた範囲の長さを計算。
					auto argLength = UtilString::SkipUntilDq(cmdLineTop) - cmdLineTop;
					// ダブルクォートの中身をargBuffへ複写。
					argBuff.assign(cmdLineTop, cmdLineTop + argLength);
					cmdLineTop += argLength;
					// 終わりのダブルクォートをスキップ。
					if (*cmdLineTop == L'"') cmdLineTop++;
				}
				else
				{
					//----- オプション値がダブルクォート以外で始まる場合 -----
					auto argLength = UtilString::SkipUntilSpace(cmdLineTop) - cmdLineTop;
					// 次の空白までスキップし、空白で囲まれた範囲の長さを計算。
					argBuff.assign(cmdLineTop, cmdLineTop + argLength);
					cmdLineTop += argLength;
				}
				// オプション名を小文字に変換。
				std::transform(optBuff.begin(), optBuff.end(), optBuff.begin(), ::tolower);
				// オプション名->オプション値を登録。
				optionToValue[optBuff] = argBuff;
				optBuff.clear();
			}
		}
		return true;
	}	

	static std::string TrimLeftCR(std::string str)
	{
		std::string::iterator i;
		for (i = str.begin(); i != str.end(); i++)
		{
			if (*i != '\r' &&
				*i != '\n') break;
		}
		str.erase(str.begin(), i);
		return str;
	}

	static std::string TrimLeftSpace(std::string str)
	{
		std::string::iterator i;
		for (i = str.begin(); i != str.end(); i++)
		{
			if (*i != ' ' &&
				*i != '\t') break;
		}
		str.erase(str.begin(), i);
		return str;
	}

	/// 	/// <summary>
	/// Deletes first single CR and last single CR from the string.
	/// </summary>
	/// <param name="str">string</param>
	/// <returns>Modified string</returns>
	static std::string Trim1CR(std::string str)
	{
		if (2 <= str.length() && str.front() == '\r' && str.at(1) == '\n')
			str.erase(str.begin(), str.begin() + 2);	// Delete first "\r\n"
		else if (1 <= str.length() && (str.front() == '\n' || str.front() == '\r'))
			str.erase(str.begin());	// Delete first "\r" or "\n"

		if (2 <= str.length() && str.at(str.length() - 2) == '\r' && str.back() == '\n')
			str.erase(str.end() - 2, str.end());	// Delete last "\r\n"
		else if (1 <= str.length() && (str.back() == '\n' || str.back() == '\r'))
			str.erase(str.end() - 1);	// Delete last "\r" or "\n"

		return str;
	}

	static std::string& TrimLeft(std::string& str)
	{
		std::string::iterator i;
		for (i = str.begin(); i != str.end(); i++)
		{
			if (*i != ' ' &&
				*i != '\t' &&
				*i != '\r' &&
				*i != '\n') break;
		}
		str.erase(str.begin(), i);
		return str;
	}

	static std::string& TrimRight(std::string& str)
	{
		int i;
		for (i = (int)str.length() - 1; i != -1; i--)
		{
			if (str.at(i) != ' ' &&
				str.at(i) != '\t' &&
				str.at(i) != '\r' &&
				str.at(i) != '\n') break;
		}
		str.erase(i + 1);
		return str;
	}

	static std::string& TrimRight(std::string& str, char chTarget1, char chTarget2 = 0, char chTarget3 = 0, char chTarget4 = 0)
	{
		int i;
		for (i = (int)str.length() - 1; i != -1; i--)
		{
			if (str.at(i) == chTarget1) continue;
			if (str.at(i) == chTarget2) continue;
			if (str.at(i) == chTarget3) continue;
			if (str.at(i) == chTarget4) continue;
			break;
		}
		str.erase(i + 1);
		return str;
	}

	static std::string& Trim(std::string& str)
	{
		UtilString::TrimLeft(str);
		UtilString::TrimRight(str);
		return str;
	}

	static std::wstring& TrimLeft(std::wstring& str)
	{
		std::wstring::iterator i;
		for (i = str.begin(); i != str.end(); i++)
		{
			if (*i != L' ' &&
				*i != L'\t' &&
				*i != L'\r' &&
				*i != L'\n') break;
		}
		str.erase(str.begin(), i);
		return str;
	}

	static std::wstring& TrimRight(std::wstring& str)
	{
		int i;
		for (i = (int)str.length() - 1; i != -1; i--)
		{
			if (str.at(i) != L' ' &&
				str.at(i) != L'\t' &&
				str.at(i) != L'\r' &&
				str.at(i) != L'\n') break;
		}
		str.erase(i + 1);
		return str;
	}

	static std::wstring& Trim(std::wstring& str)
	{
		UtilString::TrimLeft(str);
		UtilString::TrimRight(str);
		return str;
	}

//----- 21.06.03 Fukushiro M. 変更前 ()-----
//	static std::string & Right(std::string& str, size_t length)
//	{
//		if (length < str.length())
//			str.erase(0, str.length() - length);
//		return str;
//	}
//	
//	static std::wstring & Right(std::wstring& str, size_t length)
//	{
//		if (length < str.length())
//			str.erase(0, str.length() - length);
//		return str;
//	}
//----- 21.06.03 Fukushiro M. 変更後 ()-----
	static std::string Right(const std::string& str, size_t length)
	{
		return (length < str.length()) ? str.substr(str.length() - length) : str;
	}
	
	static std::wstring Right(const std::wstring& str, size_t length)
	{
		return (length < str.length()) ? str.substr(str.length() - length) : str;
	}
//----- 21.06.03 Fukushiro M. 変更終 ()-----
	
	static std::string & SimpleReplace(std::string & str, const std::string & oldStr, const std::string & newStr)
	{
		size_t nnn = 0;
		for (;;)
		{
			nnn = str.find(oldStr, nnn);
			if (nnn == std::string::npos) break;
			str.replace(nnn, oldStr.size(), newStr);
			nnn += newStr.size();
		}
		return str;
	}

	static std::string Replace(
		std::string str,
		const char * old01 = nullptr, AnyValue new01 = NoValue,
		const char * old02 = nullptr, AnyValue new02 = NoValue,
		const char * old03 = nullptr, AnyValue new03 = NoValue,
		const char * old04 = nullptr, AnyValue new04 = NoValue,
		const char * old05 = nullptr, AnyValue new05 = NoValue,
		const char * old06 = nullptr, AnyValue new06 = NoValue,
		const char * old07 = nullptr, AnyValue new07 = NoValue,
		const char * old08 = nullptr, AnyValue new08 = NoValue,
		const char * old09 = nullptr, AnyValue new09 = NoValue,
		const char * old10 = nullptr, AnyValue new10 = NoValue,
		const char * old11 = nullptr, AnyValue new11 = NoValue,
		const char * old12 = nullptr, AnyValue new12 = NoValue,
		const char * old13 = nullptr, AnyValue new13 = NoValue,
		const char * old14 = nullptr, AnyValue new14 = NoValue,
		const char * old15 = nullptr, AnyValue new15 = NoValue,
		const char * old16 = nullptr, AnyValue new16 = NoValue,
		const char * old17 = nullptr, AnyValue new17 = NoValue,
		const char * old18 = nullptr, AnyValue new18 = NoValue,
		const char * old19 = nullptr, AnyValue new19 = NoValue,
		const char * old20 = nullptr, AnyValue new20 = NoValue
	)
	{
		const char * OLD[] =
		{
			old01,
			old02,
			old03,
			old04,
			old05,
			old06,
			old07,
			old08,
			old09,
			old10,
			old11,
			old12,
			old13,
			old14,
			old15,
			old16,
			old17,
			old18,
			old19,
			old20,
		};
		const AnyValue NEW[] =
		{
			new01,
			new02,
			new03,
			new04,
			new05,
			new06,
			new07,
			new08,
			new09,
			new10,
			new11,
			new12,
			new13,
			new14,
			new15,
			new16,
			new17,
			new18,
			new19,
			new20,
		};
		for (size_t i = 0; i != _countof(OLD) && OLD[i] != nullptr; i++)
			SimpleReplace(str, OLD[i], NEW[i].ToAString());
		return str;
	}

//----- 19.11.27 Fukushiro M. 削除始 ()-----
//	static std::string Replace(
//		std::string str,
//		const std::vector<std::pair<std::string, std::string>> & oldNewArray
//	)
//	{
//		for (const auto & old_new : oldNewArray)
//			SimpleReplace(str, old_new.first, old_new.second);
//		return str;
//	}
//----- 19.11.27 Fukushiro M. 削除終 ()-----

	static std::string ReplaceAsHtml(
		std::string str,
		const char * old01 = nullptr, AnyValue new01 = NoValue,
		const char * old02 = nullptr, AnyValue new02 = NoValue,
		const char * old03 = nullptr, AnyValue new03 = NoValue,
		const char * old04 = nullptr, AnyValue new04 = NoValue,
		const char * old05 = nullptr, AnyValue new05 = NoValue,
		const char * old06 = nullptr, AnyValue new06 = NoValue,
		const char * old07 = nullptr, AnyValue new07 = NoValue,
		const char * old08 = nullptr, AnyValue new08 = NoValue,
		const char * old09 = nullptr, AnyValue new09 = NoValue,
		const char * old10 = nullptr, AnyValue new10 = NoValue,
		const char * old11 = nullptr, AnyValue new11 = NoValue,
		const char * old12 = nullptr, AnyValue new12 = NoValue,
		const char * old13 = nullptr, AnyValue new13 = NoValue,
		const char * old14 = nullptr, AnyValue new14 = NoValue,
		const char * old15 = nullptr, AnyValue new15 = NoValue,
		const char * old16 = nullptr, AnyValue new16 = NoValue,
		const char * old17 = nullptr, AnyValue new17 = NoValue,
		const char * old18 = nullptr, AnyValue new18 = NoValue,
		const char * old19 = nullptr, AnyValue new19 = NoValue,
		const char * old20 = nullptr, AnyValue new20 = NoValue
	)
	{
		const char * OLD[] =
		{
			old01,
			old02,
			old03,
			old04,
			old05,
			old06,
			old07,
			old08,
			old09,
			old10,
			old11,
			old12,
			old13,
			old14,
			old15,
			old16,
			old17,
			old18,
			old19,
			old20,
		};
		const AnyValue NEW[] =
		{
			new01,
			new02,
			new03,
			new04,
			new05,
			new06,
			new07,
			new08,
			new09,
			new10,
			new11,
			new12,
			new13,
			new14,
			new15,
			new16,
			new17,
			new18,
			new19,
			new20,
		};
		for (size_t i = 0; i != _countof(OLD) && OLD[i] != nullptr; i++)
			SimpleReplace(str, OLD[i], UtilString::ReplaceToHtml(NEW[i].ToAString()));
		return str;
	}

	static std::string ReplaceToHtml(const std::string & str)
	{
//----- 20.03.04  変更前 ()-----
//		return Replace(str, "&", "&amp;", "<", "&lt;", ">", "&gt;");
//----- 20.03.04  変更後 ()-----
// 20.03.08  1行変更 ()
//		return Replace(Replace(str, "&", "&amp;", "<", "&lt;", ">", "&gt;"), "\r\n", "<br/>", "\n", "<br/>", "\r", "<br/>");
		return Replace(
			str,
			"&", "&amp;",
			"<", "&lt;",
			">", "&gt;",
			"\r\n", "<br/>",
			"\n", "<br/>",
			"\r", "<br/>",
			"\a", "<",
			"\b", ">",
			"\f", "\r\n"
		);
//----- 20.03.04  変更終 ()-----
	}

	static const char * SkipOneDqChar(const char * cp)
	{
		if (*cp == '\\')
			cp++;
		if (*cp == '\0')
			ThrowLeSystemError();
		return cp + 1;
	}
		
	/// <summary>
	/// Skips the doube-quoted sentence.
	/// e.g. if cp=A then return B.
	/// -----------------------------
	/// '  "abc \" efg"  '
	///  ^A            ^B
	/// -----------------------------
	/// This function does not support raw string literal.
	/// </summary>
	/// <param name="cp">The cp.</param>
	/// <returns></returns>
	static const char * SkipDqSentence(const char * cp)
	{
		// Skip spaces in front of '\"'
// 21.05.18 Fukushiro M. 1行変更 ()
//		cp = UtilString::SkipSpace(cp);
		cp = UtilString::SkipSpaceTab(cp);
		if (*cp != '\"')
			ThrowLeSystemError();
		cp++;	// Skip '"'
		while (*cp != '\"')
			cp = UtilString::SkipOneDqChar(cp);
		cp++;	// Skip '\"'
		return cp;
	}

	static const char * SkipName(const char * cp)
	{
		// Skip alphabet and number and '-' and '_'
		while (
			('0' <= *cp && *cp <= '9') ||
			('A' <= *cp && *cp <= 'Z') ||
			('a' <= *cp && *cp <= 'z') ||
			(*cp == '_') ||
			(*cp == '-')
		)
			cp++;
		return cp;
	}
	
	/// <summary>
	/// Reads c++ one item.
	/// Item is Split at the following point.
	/// {   }   (   )   ,   =   ;   \r\n    "~"     R"(~)"    //~     /*~*/    #~
	/// </summary>
	/// <param name="text">The text end.</param>
	/// <returns></returns>
	static const char* SkipOneItem(const char * text, int & lineNumber)
	{
		enum class Mode
		{
			FirstSpace,		// [^  ~]
			Regular,
			SlashComment,	// [// ~]
			AsterComment,	// [/* ~]
			RegularString,	// [" ~]
			RawString1,		// [R" ~RawString1~ ( ~RawString2~]
			RawString2,		// [R" ~RawString1~ ( ~RawString2~]
			PreProcessor,	// [# ~]
			PreProcessor2,	// [# ~ \ ~PreProcessor2~ ]
		} mode = Mode::FirstSpace;

		const char * textTop = text;
		std::string rawStrTerminater;
		bool isEndOfItem = false;
		while (!isEndOfItem)
		{
			switch (mode)
			{
			case Mode::FirstSpace:
				switch (*text)
				{
				case ' ':
				case '\t':
					text += 1;
					break;
				case '#':
					mode = Mode::PreProcessor;
					text += 1;
					break;
				default:
					mode = Mode::Regular;
					break;
				}
				break;
			case Mode::Regular:
				switch (*text)
				{
				case '/':
					if (text[1] == '/')
					{	// when "// ~"
						if (text == textTop)
						{	// if first text
							// // xxx xxx xxx xxx xxx xxx
							// ^don't cut here           ^cut here
							mode = Mode::SlashComment;
							text += 2;
						}
						else
						{	// if second or later text
							// xxx xxx // xx xx
							//         ^cut here
							isEndOfItem = true;
						}
					}
					else if (text[1] == '*')
					{
						if (text == textTop)
						{	// if first text
							// /* xxx xxx xxx xxx xxx xxx */
							// ^don't cut here              ^cut here
							mode = Mode::AsterComment;
							text += 2;
						}
						else
						{	// if second or later text
							// xxx xxx /* xx xx
							//         ^cut here
							isEndOfItem = true;
						}
					}
					else
					{
						text += 1;
					}
					break;
				case '\"':
					mode = Mode::RegularString;
					text++;
					break;
				case 'R':
					if (text[1] == '\"')
					{	// when start a raw string.
						// xxx xxx R"***( xx xx
						//         ^here
						mode = Mode::RawString1;
						text += 2;
						rawStrTerminater = ')';
					}
					else
					{
						text++;
					}
					break;
				case '\r':
					if (text == textTop)
					{
						lineNumber += 1;
						text += 2;
					}
					isEndOfItem = true;
					break;
				case '\n':
					if (text == textTop)
					{
						lineNumber += 1;
						text += 1;
					}
					isEndOfItem = true;
					break;
				case '(':
				case ')':
				case '{':
				case '}':
				case ';':
				case '=':
				case ',':
					// xxx xxx ( xx xx
					//         ^^cut both
					if (text == textTop)
						text += 1;
					isEndOfItem = true;
					break;
				case '\0':
					isEndOfItem = true;
					break;
				default:
					text++;
					break;
				}
				break;
			case Mode::SlashComment:	// [// ~]
				switch (*text)
				{
				case '\r':
					mode = Mode::Regular;
					// text += 2;
					isEndOfItem = true;
					break;
				case '\n':
					mode = Mode::Regular;
					// text += 1;
					isEndOfItem = true;
					break;
				case '\0':
					mode = Mode::Regular;
					isEndOfItem = true;
					break;
				default:
					text += 1;
					break;
				}
				break;
			case Mode::AsterComment:	// [/* ~]
				switch (*text)
				{
				case '*':
					if (text[1] == '/')
					{
						mode = Mode::Regular;
						text += 2;
						isEndOfItem = true;
					}
					else
					{
						text += 1;
					}
					break;
				case '\r':
					lineNumber += 1;
					text += 2;
					break;
				case '\n':
					lineNumber += 1;
					text += 1;
					break;
				case '\0':
					ThrowLeException(LeError::UNEXPECTED_EOF, "Unterminated comment exists");
					break;
				default:
					text++;
					break;
				}
				break;
			case Mode::RegularString:	// [" ~]
				switch (*text)
				{
				case '\\':
					if (text[1] == '\0')
						ThrowLeException(LeError::UNEXPECTED_EOF, "Unterminated string exists");
					text += 2;
					break;
				case '\"':
					mode = Mode::Regular;
					text += 1;
					break;
				case '\0':
					ThrowLeException(LeError::UNEXPECTED_EOF, "Unterminated string exists");
					break;
				default:
					text++;
					break;
				}
				break;
			case Mode::RawString1:		// [R" ~RawString1~ ( ~RawString2~]
				// R"xxxx( yyyyyyy )xxxx"
				//   ^here
				switch (*text)
				{
				case ' ':
				case ')':
				case '\\':
				case '\t':
					ThrowLeException(LeError::UNEXPECTED_CHAR, std::string(text, 1));
					break;
				case '\r':
				case '\n':
					ThrowLeException(LeError::UNEXPECTED_CHAR, "Unexpected new line");
					break;
				case '(':
					mode = Mode::RawString2;	// [R" ~RawString1~ ( ~RawString2~]
					rawStrTerminater += '\"';
					text += 1;
					break;
				case '\0':
					ThrowLeException(LeError::UNEXPECTED_EOF, "Unterminated string exists");
					break;
				default:
					rawStrTerminater += *text;
					text++;
					break;
				}
				break;
			case Mode::RawString2:		// [R" ~RawString1~ ( ~RawString2~]
			{
				// R"xxxx( yyyyyyy )xxxx"
				//         ^here
				switch (*text)
				{
				case '\0':
					ThrowLeException(LeError::UNEXPECTED_EOF, "Unterminated string exists");
					break;
				case '\r':
					lineNumber += 1;
					text += 2;
					break;
				case '\n':
					lineNumber += 1;
					text += 1;
					break;
				default:
					if (strncmp(text, rawStrTerminater.c_str(), rawStrTerminater.length()) == 0)
					{
						mode = Mode::Regular;
						text += rawStrTerminater.length();
					}
					else
					{
						text += 1;
					}
					break;
				}
				break;
			}
			case Mode::PreProcessor:	// [# ~]
				switch (*text)
				{
				case '\\':
					// #define ~ \      C++ do not allow to write any comment after \.
					mode = Mode::PreProcessor2;
					text += 1;
					break;
				case '\r':
					isEndOfItem = true;
					break;
				case '\n':
					isEndOfItem = true;
					break;
				case '\0':
					isEndOfItem = true;
					break;
				default:
					text += 1;
					break;
				}
				break;
			case Mode::PreProcessor2:	// [# ~ \ ~PreProcessor2~ ]
										// #define ~ \      C++ do not allow to write any comment after \.
				switch (*text)
				{
				case ' ':
				case '\t':
					text += 1;
					break;
				case '\r':
					mode = Mode::PreProcessor;
					lineNumber += 1;
					text += 2;
					break;
				case '\n':
					mode = Mode::PreProcessor;
					lineNumber += 1;
					text += 1;
					break;
				case '\0':
					ThrowLeException(LeError::UNEXPECTED_EOF);
					break;
				default:
					mode = Mode::PreProcessor;
					break;
				}
				break;
			}
		}
		return text;
	}

	/// <summary>
	/// <para> Reads the one sentence.                            </para>
	/// <para> 1.Cut at the next char of ';'.                     </para>
	/// <para> 2.Cut at the front of '{' and next of '{'.         </para>
	/// <para> 3.Cut at the front of '}' and next of '}'.         </para>
	/// <para> 4.Comment is one sentence.                         </para>
	/// <para> 5.Comment is erased when inside of parenthesis.    </para>
	/// <para> 6.Do not cut at '\r' and '\n' characters.          </para>
	/// </summary>
	/// <param name="argument">The argument.</param>
	/// <returns></returns>
	static const char * ReadOneSentence(std::string & buffer, const char * text, int & lineNumber)
	{
		buffer.clear();
		if (*text == '\0') return text;
		int parenthesisDepth = 0;	// depth=0 ( depth=1 ( depth=2 ) depth=1 ) depth=0
		bool isEndOfItem = false;
		while (!isEndOfItem)
		{
			text = UtilString::SkipSpaceTab(text);
			auto nextPtr = UtilString::SkipOneItem(text, lineNumber);
			if (nextPtr == text) break;
			if (parenthesisDepth == 0)
			{	// when outside of parentheses.
				switch (*text)
				{
				case '(':
					parenthesisDepth++;
					buffer.append(text, nextPtr);
					text = nextPtr;
					break;
				case ')':
					ThrowLeException(LeError::UNEXPECTED_CHAR, ")");
					break;
				case ';':	// At the front of next sentence.
				case '#':	// At the front of macro.
					buffer.append(text, nextPtr);
					text = nextPtr;
					isEndOfItem = true;
					break;
				case '/':
					if (text[1] == '/' || text[1] == '*')
					{	//----- At the front of comment -----
						if (buffer.empty())
						{
							buffer.append(text, nextPtr);
							text = nextPtr;
						}
						isEndOfItem = true;
					}
					else
					{
						buffer.append(text, nextPtr);
						text = nextPtr;
					}
					break;
				case '{':
				case '}':
					if (buffer.empty())
					{
						buffer.append(text, nextPtr);
						text = nextPtr;
					}
					isEndOfItem = true;
					break;
				case '\r':
				case '\n':
					text = nextPtr;
					break;
				default:
					// argument += ' ';
					buffer.append(text, nextPtr);
					text = nextPtr;
					break;
				}
			}
			else
			{	// when inside of parentheses.
				switch (*text)
				{
				case '(':
					parenthesisDepth++;
					buffer.append(text, nextPtr);
					break;
				case ')':
					parenthesisDepth--;
					buffer.append(text, nextPtr);
					break;
				case '\r':
				case '\n':
					break;
				default:
					if (strncmp(text, "//", 2) != 0 && strncmp(text, "/*", 2) != 0)
					{
						// argument += ' ';
						buffer.append(text, nextPtr);
					}
					break;
				}
				text = nextPtr;
			}
		}
		return text;
	}

//----- 19.12.16 Fukushiro M. 変更前 ()-----
//	/// <summary>
//	/// Skips the formulae.
//	/// Example.
//	/// double & v1,
//	/// __int64 v3 = defaultY{10, 20},
//	/// ^text      ^return
//	/// __int64 v3 = defaultY{10, 20},
//	///             ^text            ^return
//	/// __int64 v4 = (10 + 20) * ABX, bool v5 = 10 == 20 );
//	///             ^text           ^return
//	/// __int64 v4 = (10 + 20) * ABX, bool v5 = 10 == 20 );
//	///                              ^text               ^return
//	/// std::vector<int> v = std::vector<int>(10), ...
//	/// std::map<int,int> v, ...
//	/// std::vector<std::basic_string<char>> & v, ...
//	/// bool i = XX < 0, ...
//	/// int i = XX < 0 ? 10 : 20, ...
//	/// </summary>
//	/// <param name="text">The text.</param>
//	/// <returns></returns>
//	static const char * SkipFormulae(const char * text, int & lineNumber)
//	{
//		int parenthesisA = 0;	// (
//		int parenthesisB = 0;	// {
//		for (;;)
//		{
//			//            defaultX(10, 20),
//			//            (10 + 20) * defaultX{10, 20}
//			//        text^^nextPtr
//			text = UtilString::SkipSpaceTab(text);
//			if (
//				*text == ',' ||
//				*text == '=' ||
//				*text == ')' ||
//				*text == '\0'
//				)
//			{
//				if (parenthesisA == 0 && parenthesisB == 0) break;
//			}
//			auto nextPtr = UtilString::SkipOneItem(text, lineNumber);
//			if (*text == '(') parenthesisA++;
//			if (*text == ')') parenthesisA--;
//			if (*text == '{') parenthesisB++;
//			if (*text == '}') parenthesisB--;
//			text = nextPtr;
//		}
//		return text;
//	}
//----- 19.12.16 Fukushiro M. 変更後 ()-----
	/// <summary>
	/// Skips a formulae.
	/// e.g.
	///-----------------------------------------------------
	///  +Good
	///     double & v1, ...
	///     ^text      ^return
	///-----------------------------------------------------
	///  +Good
	///     __int64 v3 = defaultY{10, 20},
	///   ^text                          ^return
	///-----------------------------------------------------
	///  +Good
	///     __int64 v3 = defaultY{10, 20},
	///                 ^text            ^return
	///-----------------------------------------------------
	///  +Good
	///     __int64 v4 = (10 + 20) * ABX, bool v5 = 10 == 20 );
	///   ^text                         ^return
	///-----------------------------------------------------
	///  +Good
	///     __int64 v4 = (10 + 20) * ABX, bool v5 = 10 == 20 );
	///                                  ^text               ^return
	///-----------------------------------------------------
	///  +Good
	///     std::vector<int> v = std::vector<int>(10), ...
	///     ^text                                    ^return
	///-----------------------------------------------------
	///  +Good
	///     std::map<int,int> v, ...
	///     ^text              ^return
	///-----------------------------------------------------
	///  +Good
	///     std::vector<std::basic_string<char>> & v, ...
	///     ^text                                   ^return
	///-----------------------------------------------------
	///  +Not good
	///     bool i = XX < 0, bool j = XX > 0 ,...
	///     ^text                            ^return
	///               |
	///               V
	///  +Good
	///     bool i = (XX < 0), bool j = (XX > 0) ,...
	///     ^text            ^return
	///-----------------------------------------------------
	///  +Not good
	///     int i = XX < 0 ? 10 : 20, int j )
	///     ^text                           ^return
	///               |
	///               V
	///  +Good
	///     int i = (XX < 0) ? 10 : 20, int j )
	///     ^text                     ^return
	///
	/// </summary>
	/// <param name="text">The text.</param>
	/// <returns>Next text pointer</returns>
	static const char * SkipFormulae(const char * text, int & lineNumber)
	{
		std::vector<char> parenthesis;
		for (;;)
		{
			//            defaultX(10, 20),
			//            (10 + 20) * defaultX{10, 20}
			//        text^^nextPtr
			text = UtilString::SkipSpaceTab(text);
			if (
				*text == ',' ||
				*text == ')' ||
				*text == '\0'
				)
			{
				if (parenthesis.empty()) break;
			}
			switch (*text)
			{
			case '(':
			case '{':
			case '[':
			case '<':
				parenthesis.emplace_back(*text);
				break;
			case ')':
				while (!parenthesis.empty())
				{
					auto c = parenthesis.back();
					parenthesis.pop_back();
					if (c == '(') break;
				}
				break;
			case '}':
				while (!parenthesis.empty())
				{
					auto c = parenthesis.back();
					parenthesis.pop_back();
					if (c == '{') break;
				}
				break;
			case ']':
				while (!parenthesis.empty())
				{
					auto c = parenthesis.back();
					parenthesis.pop_back();
					if (c == '[') break;
				}
				break;
			case '>':
				while (!parenthesis.empty())
				{
					auto c = parenthesis.back();
					parenthesis.pop_back();
					if (c == '<') break;
				}
				break;
			}
			text++;
		}
		return text;
	}
//----- 19.12.16 Fukushiro M. 変更終 ()-----

	/// <summary>
	/// Extract one argument from argumentsText.
	/// e.g. argumentsText="double & v1, __int64 v2 = defaultX(10, 20), __int64 v3 = defaultY{10, 20}, __int64 v4 = (10 + 20) * ABX, bool v5 = 10 == 20);"
	/// </summary>
	/// <param name="argument"></param>
	/// <param name="defaultValue"></param>
	/// <param name="argumentsText"></param>
	/// <param name="fileName"></param>
	/// <param name="lineNumber"></param>
	/// <returns></returns>
	static const char * ReadOneArgument(
		std::string & argument,
		std::string & defaultValue,
		const char * argumentsText,
		const wchar_t * fileName,
		int & lineNumber
	)
	{
//		static const std::regex REX(R"((.+)\b\=\b(.+))");
		static const std::regex REX(R"((.+[^=])=([^=].+))");


		auto nextPtr = UtilString::SkipFormulae(argumentsText, lineNumber);
		std::string oneArgument(argumentsText, nextPtr);
		std::smatch results;
		if (std::regex_search(oneArgument, results, REX))
		{	//----- if onArgument="int i = 10", results[1]="int i ", results[2]="10" -----
			argument = results[1].str();
			defaultValue = results[2].str();
		}
		else
		{
			argument = oneArgument;
			defaultValue.clear();
		}
		return nextPtr;
	}

	/// <summary>
	/// Splits the source into items at the separator point.
	/// e.g.
	/// 'ABC::EFG::::XYZ::' -> {"ABC","EFG","","XYZ",""}
	/// </summary>
	/// <param name="items">The items.</param>
	/// <param name="source">The source.</param>
	/// <param name="separator">The separator.</param>
	/// <returns></returns>
	static std::vector<std::string> & Split(std::vector<std::string> & items, const std::string & source, const std::string & separator)
	{
		items.clear();
		if (!source.empty())
		{
			size_t pos = 0;
			for (;;)
			{
				const auto nextpos = source.find(separator, pos);
				if (nextpos == std::string::npos) break;
				items.emplace_back(source.substr(pos, nextpos - pos));
				pos = nextpos + separator.length();
			}
			items.emplace_back(source.substr(pos));
		}
		return items;
	}

	/// <summary>
	/// Splits the source into items at the separator point.
	/// e.g.
	/// 'ABC:EFG::XYZ:' -> {"ABC","EFG","","XYZ",""}
	/// </summary>
	/// <param name="items">The items.</param>
	/// <param name="source">The source.</param>
	/// <param name="separator">The separator.</param>
	/// <returns></returns>
	static std::vector<std::string> & Split(std::vector<std::string> & items, const std::string & source, char separator)
	{
		items.clear();
		if (!source.empty())
		{
			size_t pos = 0;
			for (;;)
			{
				const auto nextpos = source.find(separator, pos);
				if (nextpos == std::string::npos) break;
				items.emplace_back(source.substr(pos, nextpos - pos));
				pos = nextpos + 1;
			}
			items.emplace_back(source.substr(pos));
		}
		return items;
	}

	/// <summary>
	/// Splits the source into two items at the separator point.
	/// e.g.
	/// 'ABC:EFG::XYZ:' -> {"ABC","EFG::XYZ:"}
	/// </summary>
	/// <param name="leftItem">The left item.</param>
	/// <param name="rightItem">The right item.</param>
	/// <param name="source">The source.</param>
	/// <param name="separator">The separator.</param>
	static void Split(std::string & leftItem, std::string & rightItem, const std::string & source, char separator)
	{
		leftItem.clear();
		rightItem.clear();
		if (!source.empty())
		{
			const auto nextpos = source.find(separator);
			leftItem = source.substr(0, nextpos);
			if (nextpos != std::string::npos)
				rightItem = source.substr(nextpos + 1);
		}
	}

	/// <summary>
	/// Splits the source into two items at the place where a character appears belong to a set of separators.
	/// e.g.
	/// 'ABC:EFG::XYZ:' -> {"ABC","EFG::XYZ:"}, return ':'
	/// </summary>
	/// <param name="leftItem">The left item.</param>
	/// <param name="rightItem">The right item.</param>
	/// <param name="source">The source.</param>
	/// <param name="separators">Separators.</param>
	/// <returns>A separator/0:any separator doesn't appear</returns>
	static char Split(
		std::string & leftItem,
		std::string & rightItem,
		const std::string & source,
		const char * separators
	)
	{
		leftItem.clear();
		rightItem.clear();
		if (!source.empty())
		{
			const auto nextpos = std::strcspn(source.c_str(), separators);
			leftItem = source.substr(0, nextpos);
			if (nextpos != source.size())
			{
				rightItem = source.substr(nextpos + 1);
				return source.at(nextpos);
			}
		}
		return '\0';
	}


	static std::string GetLangPart(
		const std::string & lang,
		const std::string & source
	)
	{
		static const std::unordered_set<std::string> LANGSET
		{
			"ab",
			"aa",
			"af",
			"ak",
			"sq",
			"am",
			"ar",
			"an",
			"hy",
			"as",
			"av",
			"ae",
			"ay",
			"az",
			"bm",
			"ba",
			"eu",
			"be",
			"bn",
			"bh",
			"bi",
			"bs",
			"br",
			"bg",
			"my",
			"ca",
			"ch",
			"ce",
			"ny",
			"zh",
			"zh-Hans",
			"zh-Hant",
			"cv",
			"kw",
			"co",
			"cr",
			"hr",
			"cs",
			"da",
			"dv",
			"nl",
			"dz",
			"en",
			"eo",
			"et",
			"ee",
			"fo",
			"fj",
			"fi",
			"fr",
			"ff",
			"gl",
			"gd",
			"gv",
			"ka",
			"de",
			"el",
			"kl",
			"gn",
			"gu",
			"ht",
			"ha",
			"he",
			"hz",
			"hi",
			"ho",
			"hu",
			"is",
			"io",
			"ig",
			"id",
			"in",
			"ia",
			"ie",
			"iu",
			"ik",
			"ga",
			"it",
			"ja",
			"jv",
			"kl",
			"kn",
			"kr",
			"ks",
			"kk",
			"km",
			"ki",
			"rw",
			"rn",
			"ky",
			"kv",
			"kg",
			"ko",
			"ku",
			"kj",
			"lo",
			"la",
			"lv",
			"li",
			"ln",
			"lt",
			"lu",
			"lg",
			"lb",
			"gv",
			"mk",
			"mg",
			"ms",
			"ml",
			"mt",
			"mi",
			"mr",
			"mh",
			"mo",
			"mn",
			"na",
			"nv",
			"ng",
			"nd",
			"ne",
			"no",
			"nb",
			"nn",
			"ii",
			"oc",
			"oj",
			"cu",
			"or",
			"om",
			"os",
			"pi",
			"ps",
			"fa",
			"pl",
			"pt",
			"pa",
			"qu",
			"rm",
			"ro",
			"ru",
			"se",
			"sm",
			"sg",
			"sa",
			"sr",
			"sh",
			"st",
			"tn",
			"sn",
			"ii",
			"sd",
			"si",
			"ss",
			"sk",
			"sl",
			"so",
			"nr",
			"es",
			"su",
			"sw",
			"ss",
			"sv",
			"tl",
			"ty",
			"tg",
			"ta",
			"tt",
			"te",
			"th",
			"bo",
			"ti",
			"to",
			"ts",
			"tr",
			"tk",
			"tw",
			"ug",
			"uk",
			"ur",
			"uz",
			"ve",
			"vi",
			"vo",
			"wa",
			"cy",
			"wo",
			"fy",
			"xh",
			"yi",
			"ji",
			"yo",
			"za",
			"zu",
		};

		if (!lang.empty())
		{
			// lang="ja", source="kn;abcde ja;あいうえお en;ABCDE" -> "あいうえお"

			static const std::regex REX(
				R"(\b([A-Za-z\-]+);)"
			);

			const char* str = source.c_str();
			const char* str1 = nullptr;
			const char* str2 = nullptr;
			std::cmatch results;
			while (std::regex_search(str, results, REX))
			{
				auto argument = results[1].str();
				auto pos = results.position();
				auto len = results.length();
				str = str + pos + len;
				if (argument == lang)
				{
					str1 = str;
					break;
				}
			}
			if (str1 != nullptr)
			{
				while (std::regex_search(str, results, REX))
				{
					auto argument = results[1].str();
					auto pos = results.position();
					auto len = results.length();
					if (LANGSET.find(argument) != LANGSET.end())
					{
						str2 = str + pos;
						break;
					}
					str = str + pos + len;
				}
				if (str2 == nullptr)
					str2 = source.c_str() + source.length();

				std::string retstr(str1, str2);
				UtilString::Trim(retstr);
				return retstr;
			}
			else
			{
				return source;
			}
		}
		else
		{
			return source;
		}
	}

};

