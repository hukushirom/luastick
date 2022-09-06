#pragma once

class StringBuffer : public std::vector<std::string>
{
public:
	StringBuffer & operator << (const std::string & v)
	{
		emplace_back(v);
		return *this;
	}
	StringBuffer & operator << (__int32 v)
	{
		emplace_back(std::to_string(v));
		return *this;
	}
	StringBuffer & operator << (__int64 v)
	{
		emplace_back(std::to_string(v));
		return *this;
	}

//----- 22.09.06 Fukushiro M. 追加始 ()-----
	StringBuffer & operator << (unsigned __int32 v)
	{
		emplace_back(std::to_string(v));
		return *this;
	}
//----- 22.09.06 Fukushiro M. 追加終 ()-----

	StringBuffer & operator << (unsigned __int64 v)
	{
		emplace_back(std::to_string(v));
		return *this;
	}

//----- 22.09.06 Fukushiro M. 削除始 (追加)-----
//	StringBuffer & operator << (size_t v)
//	{
//		emplace_back(std::to_string(v));
//		return *this;
//	}
//----- 22.09.06 Fukushiro M. 削除終 (追加)-----

	std::string & get(std::string & str) const
	{
		str.clear();
		for (const auto & s : *this)
			str += s;
		return str;
	}
};

