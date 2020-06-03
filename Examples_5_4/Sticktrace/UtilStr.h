#pragma once

#include "mba/suba.h"
#include "mba/diff.h"
#include "mba/msgno.h"
#include "UtilString.h"		// For UtilString::Format.
#include "LeException.h"

struct FCDiffRecW
{
	enum Cmd
	{
		NONE = 0,
		INS,
		DEL,
	};

	FCDiffRecW() : cmd(NONE), begin(-1) {}
	FCDiffRecW(Cmd c, int b, const wchar_t* t, int tlen) : cmd(c), begin(b), text(t, tlen) {}
	Cmd cmd;
	int begin;
	std::wstring text;
}; // struct FCDiffRecW

class UtilStr
{
public:
//----- 17.10.20 Fukushiro M. 削除始 ()-----
//	template<typename ... Args>
//	static std::wstring Format(const std::wstring & format, Args ... args)
//	{
//		std::vector<wchar_t> buffer(std::swprintf(nullptr, 0, format.c_str(), args ...) + 1);
//		std::swprintf(buffer.data(), buffer.size(), format.c_str(), args ...);
//		return std::wstring(buffer.data(), buffer.size() - 1);
//	}
//----- 17.10.20 Fukushiro M. 削除終 ()-----

	static void CompText(std::vector<FCDiffRecW>& vDiff, const wchar_t* textA, int lengthA, const wchar_t* textB, int lengthB)
	{
		struct My
		{
			//********************************************************************************************
			/*!
			* @brief	Cmpに引数(object)を渡すための処理関数。Cmp(idx(s,idx1,ctx),idx(s,idx2,ctx))として使われる。
			* @author	Fukushiro M.
			* @date	2014/09/21(日) 21:20:10
			*
			* @param[in]		const void*	s		データの先頭ポインタ。diff関数のtextA,textB引数。
			* @param[in]		int			idx		データのインデックス。0～lengthA/0～lengthB
			* @param[in,out]	void*		context	diff関数に渡したユーザーデータ。
			*
			* @return	const void*	データの比較対象となるポインタ。
			*/
			//********************************************************************************************
			static const void* Idx(const void* s, int idx, void* context)
			{
				return (const void*)((const wchar_t*)s + idx);
			}

			//********************************************************************************************
			/*!
			* @brief	diff関数中でデータの比較を行う。
			* @author	Fukushiro M.
			* @date	2014/09/21(日) 21:26:46
			*
			* @param[in]	const void*	object1	比較対象のデータ。diff関数のtextA[idx]の要素。
			* @param[in]	const void*	object2	比較対象のデータ。diff関数のtextB[idx]の要素。
			* @param[in]	void*		context	diff関数に渡したユーザーデータ。
			*
			* @return	int	<0:object1<object2/=0:object1=object2/>0:object1>object2
			*/
			//********************************************************************************************
			static int Cmp(const void* object1, const void* object2, void* context)
			{
				if (*(const wchar_t*)object1 == *(const wchar_t*)object2) return 0;
				return (*(const wchar_t*)object1 < *(const wchar_t*)object2) ? -1 : 1;
			}
		};

		struct varray* ses = varray_new(sizeof(struct diff_edit), NULL);
		int sn;
		int d = diff(textA, 0, lengthA, textB, 0, lengthB, My::Idx, My::Cmp, NULL, 0, ses, &sn, NULL);

		if (d == -1)
		{
			varray_del(ses);
			MMNO(errno);
			ThrowLeSystemError();
		}

		int pos = 0;
		for (int i = 0; i != sn; i++)
		{
			struct diff_edit* e = (struct diff_edit*)varray_get(ses, i);
			switch (e->op)
			{
			case DIFF_MATCH:
				pos += e->len;
				break;
			case DIFF_DELETE:
				vDiff.push_back(FCDiffRecW(FCDiffRecW::DEL, pos, textA + e->off, e->len));
				break;
			case DIFF_INSERT:
				vDiff.push_back(FCDiffRecW(FCDiffRecW::INS, pos, textB + e->off, e->len));
				pos += e->len;
				break;
			}
		}
		varray_del(ses);
	}

	static CString& LoadString(CString& str, UINT nID)
	{
		str.LoadString(nID);
		return str;
	}

	// 文字列をつないで一つの文字列にする。
	static void AppendString (std::wstring& str, const wchar_t* wcpStr)
	{
		str += UtilString::Format(L"%x ", ::wcslen(wcpStr)) + wcpStr;
	} // FFAppendString.

	static void AppendDWORD (std::wstring& str, DWORD dw)
	{
		str += UtilString::Format(L"%x ", dw);
	} // FFAppendDWORD

	// つないだ文字列から一つの文字列を取り出す。
	static std::wstring ExtractString (std::wstring& str)
	{
		if (str.empty()) return L"";
		const wchar_t* wcpStr = str.c_str();
		wchar_t* wcpNext;
		DWORD dwLen = std::wcstoul(wcpStr, &wcpNext, 16);
		int iNumLen = (int)(wcpNext - wcpStr) + 1;
		const std::wstring strHead = str.substr(iNumLen, dwLen);
		str.erase(0, iNumLen + dwLen);
		return strHead;
	} // FFExtractString.
	
	static DWORD ExtractDWORD (std::wstring& str)
	{
		if (str.empty()) return DWORD(-1);
		const wchar_t* wcpStr = str.c_str();
		wchar_t* wcpNext;
		DWORD dwVal = std::wcstoul(wcpStr, &wcpNext, 16);
		const int iNumLen = (int)(wcpNext - wcpStr) + 1;
		str.erase(0, iNumLen);
		return dwVal;
	} // FFExtractDWORD
};
