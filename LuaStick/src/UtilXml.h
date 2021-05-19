#pragma once

#include "UtilString.h"

class UtilXml
{
private:
	static constexpr const char* VALID_TAGS[] = {
		"c",
		"para",
		"see",
		"value",
		"code",
		"param",
		"seealso",
		"example",
		"paramref",
		"summary",
		"exception",
		"permission",
		"typeparam",
		"include",
		"remarks",
		"typeparamref",
		"list",
		"inheritdoc",
		"returns",
		"stick",
		"stickdef",
		"sticktype",
		"stickconv",
	};

private:
	struct TagShot
	{
		std::string name;
		std::map<std::string, std::string> attributes;
		enum class Type
		{
			NIL,
			BEGIN,
			END,
			ONESHOT,
		} type;
	};

//----- 20.03.08  変更前 ()-----
//	static const std::vector<std::regex> & VALID_START_TAGS()
//	{
//		static std::vector<std::regex> _TAGS;
//		if (_TAGS.empty())
//		{
//			for (auto tag : VALID_TAGS)
//			{
//				// if tag="para", std::regex(R"(^<\s*(para)\b)").
//				_TAGS.emplace_back(
//					std::regex(std::string(R"(^<\s*()") + tag + R"()\b)")
//					);
//			}
//		}
//		return _TAGS;
//	}
//
//	static const std::vector<std::regex> & VALID_END_TAGS()
//	{
//		static std::vector<std::regex> _TAGS;
//		if (_TAGS.empty())
//		{
//			for (auto tag : VALID_TAGS)
//			{
//				// if tag="para", std::regex(R"(^<\s*/\s*(para)\b)").
//				_TAGS.emplace_back(
//					std::regex(std::string(R"(^<\s*/\s*()") + tag + R"()\b)")
//				);
//			}
//		}
//		return _TAGS;
//	}
//
//	static bool IsMatchTag(
//		TagShot::Type & type,
//		std::string & name,
//		const char *& xmlNext,
//		const char * xml
//	)
//	{
//		type = TagShot::Type::NIL;
//		name.clear();
//		xmlNext = xml;
//
//		if (*xml != '<') return false;
//
//		std::smatch match;
//		std::string strxml(xml);
//		for (const auto & ex : VALID_START_TAGS())
//		{
//			if (std::regex_search(strxml, match, ex))
//			{
//				type = TagShot::Type::BEGIN;
//				name = match[1].str();
//				xmlNext = xml + match.position() + match.length();
//				return true;
//			}
//		}
//		for (const auto & ex : VALID_END_TAGS())
//		{
//			if (std::regex_search(strxml, match, ex))
//			{
//				type = TagShot::Type::END;
//				name = match[1].str();
//				xmlNext = xml + match.position() + match.length();
//				return true;
//			}
//		}
//		return false;
//	}
//----- 20.03.08  変更後 ()-----
	static const std::unordered_set<std::string> & VALID_TAG_SET()
	{
		static std::unordered_set<std::string> _TAGS;
		if (_TAGS.empty())
		{
			for (auto tag : VALID_TAGS)
				_TAGS.insert(tag);
		}
		return _TAGS;
	}

	static bool IsMatchTag(
		TagShot::Type & type,
		std::string & name,
		const char *& xmlNext,
		const char * xml
	)
	{
		type = TagShot::Type::NIL;
		name.clear();
		xmlNext = xml;

		if (*xml != '<') return false;
		// Skip '<'.
		xml++;
		// Skip spaces.
// 21.05.18 Fukushiro M. 1行変更 ()
//		xml = UtilString::SkipSpace(xml);
		xml = UtilString::SkipSpaceTab(xml);

		bool isStartTag = true;
		if (*xml == '/')
		{
			isStartTag = false;
			// Skip '/'.
			xml++;
			// Skip spaces.
// 21.05.18 Fukushiro M. 1行変更 ()
//			xml = UtilString::SkipSpace(xml);
			xml = UtilString::SkipSpaceTab(xml);
		}

		auto xmlTmp = UtilString::SkipName(xml);
		std::string tmpName(xml, xmlTmp);

		const auto & validTags = VALID_TAG_SET();
		if (validTags.find(tmpName) == validTags.end())
			return false;

		type = isStartTag ? TagShot::Type::BEGIN : TagShot::Type::END;
		name = tmpName;
		xmlNext = xmlTmp;
		return true;
	}
//----- 20.03.08  変更終 ()-----

public:
	struct Tag
	{
		std::string name;
		std::map<std::string, std::string> attributes;
		std::vector<UtilXml::Tag> subTags;
		void Init(const TagShot & tagShot)
		{
			name = tagShot.name;
			attributes = tagShot.attributes;
		}

		/// <summary>
		/// Get tag contents as text.
		/// The text is converted using UtilString::ReplaceToHtml to output into HTML help finally.
		/// So, GetText replaces some special characters for UtilString::ReplaceToHtml, like following.
		/// 1. para end-tag is replaced \r\n.
		/// 2. Every \r\n under code-tag is replaced \f.
		/// 3. The text under code-tag is enclosed with pre tag and code tag.
		///    Then greater-than and less-than sign are replaced with \a and \b.
		/// </summary>
		/// <returns></returns>
		std::string GetText() const
		{
			auto text = GetTextSub();
			return UtilString::TrimRight(text);
		}

		std::string GetTextSub() const
		{
			if (name.empty())	// if plane xml.
				return attributes.at("");
			if (name == "para")
			{
				std::string text;
				for (auto & subTag : subTags)
					text += subTag.GetTextSub();
				text += "\r\n";
				return text;
			}
			else if (name == "code")
			{
				std::string text;
				for (auto & subTag : subTags)
					text += subTag.GetTextSub();
				if (!text.empty() && text.front() == '\r')
					text.erase(text.begin());
				if (!text.empty() && text.front() == '\n')
					text.erase(text.begin());
				// '<' -> '\a'  , '>' -> '\b', "\r\n" -> '\f'
				text = UtilString::Replace(text, "\r\n", "\f");
				text = std::string("\apre class=\"element-lua-form\"\b\acode\b") + text + "\a/code\b\a/pre\b";
				return text;
			}
			std::string text;
			for (auto & subTag : subTags)
				text += subTag.GetTextSub();
			return text;
		}
	};

	/// <summary>
	/// Parse start tag to end tag.
	/// </summary>
	/// <param name="tag"></param>
	/// <param name="xml"></param>
	/// <returns>true:tag exists/false:tag doesn't exist</returns>
	static bool Parse(Tag & tag, const char *& xml)
	{
		// e.g. "abc <xyz ...>efg <para>xxxx</para> </xyz> ..." -> tag={name="xyz",subtags={{name="" data="efg"}{name="para" data="xxxx"}}
		// Skip xml until first tag position.
		for (;;)
		{
			// "  < abc ..."  -> "< abc ..."
			auto xmlNext = ::strchr(xml, '<');
			if (xmlNext == nullptr || *xmlNext != '<')
			{	//----- Do not hit the char '<' -----
				xml += strlen(xml);
				return false;
			}
			xml = xmlNext;
			TagShot::Type type;
			std::string name;
			if (IsMatchTag(type, name, xmlNext, xml)) break;
		}
		TagShot tagShot;
		if (!ParseShot(tagShot, xml, false, false))
			ThrowLeSystemError();
		if (tagShot.type == TagShot::Type::END)
			ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "End tag appears without matched start tag", tagShot.name);

		tag.Init(tagShot);
		if (tagShot.type == TagShot::Type::BEGIN)
		{
			const bool isPreserveSpace = (tag.name == "code" || tag.name == "para");
			const bool isPreserveCR = (tag.name == "code");
			ParseSub(tag, xml, isPreserveSpace, isPreserveCR);
		}
		return true;
	}

private:
	/// <summary>
	/// Parse xml until end tag [tag.name] appears.
	/// </summary>
	/// <param name="tag">Parent tag.</param>
	/// <param name="xml">Text.</param>
	/// <param name="isPreserveSpace">true:Parse space as valid code/false:Remove top space</param>
	/// <param name="isPreserveCR">true:Parse CR as valid code/false:Remove top CR</param>
	static void ParseSub(Tag & tag, const char *& xml, bool isPreserveSpace, bool isPreserveCR)
	{
		for (;;)
		{
			TagShot tagShot;
			if (!ParseShot(tagShot, xml, isPreserveSpace, isPreserveCR))
				continue;
			if (tagShot.type == TagShot::Type::END)
			{
				if (tagShot.name == tag.name)
					return;
				ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "Start tag and end tag was not unmatched", tag.name, tagShot.name);
			}
			else if (tagShot.type == TagShot::Type::ONESHOT)
			{
				Tag subTag;
				subTag.Init(tagShot);
				tag.subTags.emplace_back(subTag);
			}
			else	// tagShot.type == TagShot::BEGIN
			{
				Tag subTag;
				subTag.Init(tagShot);
				bool isPreserveSpaceTmp = isPreserveSpace;
				bool isPreserveCRTmp = isPreserveCR;
				if (subTag.name == "code")
				{
					isPreserveSpaceTmp = true;
					isPreserveCRTmp = true;
				}
				else if (subTag.name == "para")
				{
					isPreserveSpaceTmp = true;
				}
				ParseSub(subTag, xml, isPreserveSpaceTmp, isPreserveCRTmp);
				tag.subTags.emplace_back(subTag);
			}
		}
	}

	// "  < abc aaa = "hello" bbb = "world" / > " -> Tag{name:"abc", attributes:{aaa:"hello", bbb:"world"}, type:ONESHOT}
	// "  < abc aaa = "hello" bbb = "world" >xyz</abc> " -> Tag{name:"abc", attributes:{aaa:"hello", bbb:"world"}, type:BEGIN}
	// "  xyz</abc> " -> Tag{name:"", attributes:{:"xyz"}, type:ONESHOT}
	// "  </abc> " -> "</abc>"
	/// <param name="isPreserveSpace">true:Parse space as valid code/false:Remove top space</param>
	/// <param name="isPreserveCR">true:Parse CR as valid code/false:Remove top CR</param>
	static bool ParseShot(TagShot & tagShot, const char *& xml, bool isPreserveSpace, bool isPreserveCR)
	{
		tagShot.type = TagShot::Type::NIL;
//----- 20.08.25 Fukushiro M. 変更前 ()-----
//		if (*xml == '\0')
//			return false;
//----- 20.08.25 Fukushiro M. 変更後 ()-----
		if (*xml == '\0')
			ThrowLeException(LeError::UNEXPECTED_EOF);
//----- 20.08.25 Fukushiro M. 変更終 ()-----
		if (*xml == '<')
		{
			TagShot::Type type;
			std::string name;
			const char * xmlNext;
			if (IsMatchTag(type, name, xmlNext, xml))
			{
				tagShot.type = type;
				tagShot.name = name;
				xml = xmlNext;
			}
		}
		if (tagShot.type != TagShot::Type::NIL)
		{
			for (;;)
			{
				// " aaa = ..."  -> "aaa = ..."
// 21.05.18 Fukushiro M. 1行変更 ()
//				xml = UtilString::SkipSpace(xml);
				xml = UtilString::SkipSpaceTab(xml);

				if (*xml == '/')
				{
					// "/ > "  -> " > "
					xml++;
					// " > "  -> "> "
// 21.05.18 Fukushiro M. 1行変更 ()
//					xml = UtilString::SkipSpace(xml);
					xml = UtilString::SkipSpaceTab(xml);
					if (*xml != '>')
						ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "Unexpected character was appeared", *xml);
					// "> "  -> " "
					xml++;
					tagShot.type = TagShot::Type::ONESHOT;
					break;
				}
				if (*xml == '>')
				{
					// "> "  -> " "
					xml++;
					break;
				}
				if (*xml == '\0')
					ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "'>' does not exist");

				// "aaa = ..."  -> " = ..."
				auto xmlNext = UtilString::SkipName(xml);
				if (xml == xmlNext)
					ThrowLeException(LeError::TAG_OCCURED_UNEXPECTED_PLACE, "Unexpected character was appeared", *xml);
				// get "aaa".
				const std::string attrName(xml, xmlNext);
				xml = xmlNext;
				// " = ..."  -> "= ..."
// 21.05.18 Fukushiro M. 1行変更 ()
//				xml = UtilString::SkipSpace(xml);
				xml = UtilString::SkipSpaceTab(xml);
				if (*xml != '=')
					ThrowLeSystemError();
				xml++;	// Skip '='
				// " "hello" "  -> ""hello" "
// 21.05.18 Fukushiro M. 1行変更 ()
//				xml = UtilString::SkipSpace(xml);
				xml = UtilString::SkipSpaceTab(xml);
				// ""hello" bbb = "world" " -> " bbb = "world" "
				xmlNext = UtilString::SkipDqSentence(xml);
				// get dq content.
				const std::string attrValue(xml + 1, xmlNext - 1);
				xml = xmlNext;
				// set attribute.
				tagShot.attributes[attrName] = attrValue;
			}
		}
		else
		{
			auto xmlTop = xml;
			const char * xmlNext;
			for (;;)
			{
				xmlNext = ::strchr(xml, '<');
				if (xmlNext == nullptr || *xmlNext != '<')
				{	//----- Do not hit the char '<' -----
					xmlNext = xml + strlen(xml);
					break;
				}
				TagShot::Type type;
				std::string name;
				const char * xmlTmp;
				if (IsMatchTag(type, name, xmlTmp, xmlNext))
					break;
				xml = xmlNext + 1;
			}

			xml = xmlNext;
			std::string text(xmlTop, xmlNext);
			if (!isPreserveCR)
				text = UtilString::Replace(text, "\n", "", "\r", "");
			if (!isPreserveSpace)
				text = UtilString::TrimLeftSpace(text);
			if (text.empty())
				return false;
			tagShot.type = TagShot::Type::ONESHOT;
			// set attribute.
			UtilString::TrimRight(text);
			tagShot.attributes[""] = text;
		}
		return true;
	}

};

