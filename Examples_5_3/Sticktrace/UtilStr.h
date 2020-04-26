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

#if 0

//;----- <#DD SDK EXPORT DEF_H> -----;
typedef std::vector< std::pair<long, long> > longlongVector;
// FFEnumMarkupText関数のコールバックプロシジャ。
typedef BOOL (*FPEnumMarkupTextProc)(BOOL bIsTag, std::wstring& wstr, void* pData);

enum FDRegFontType;

// 文字の種類。
enum FDCharKind
{
	FDCK_ANSI_BEGIN,	// これより下（以上の値）はANSI記号。
	FDCK_ANSI_MARK,		// 記号.20-2f 3a-40 5b-60 7b-7e
	FDCK_ANSI_NUMBER,	// 数字.30-39
	FDCK_ANSI_ALPHABET,	// アルファベット.41-5a 61-7a
	FDCK_ANSI_ETC,		// その他.
	FDCK_ANSI_END,		// これより上（以下の値）はANSI記号。
	FDCK_ZENK_BEGIN,	// これより下（以上の値）は全角記号。
	FDCK_ZENK_ALPHABET,	// 全角アルファベット。
	FDCK_ZENK_END,		// これより上（以下の値）は全角記号。
	FDCK_OTHER_BEGIN,	// これより下はその他のコード。
};

struct FCDiffRecA
{
	enum Cmd
	{
		INS,
		DEL,
	};

	FCDiffRecA (Cmd c, int b, const char* t, int tlen) : cmd(c), begin(b), text(t, tlen) {}
	Cmd cmd;
	int begin;
	astring text;
}; // struct FCDiffRecA

struct FCDiffRecW
{
	enum Cmd
	{
		NONE = 0,
		INS,
		DEL,
	};

	FCDiffRecW () : cmd(NONE), begin(-1) {}
	FCDiffRecW (Cmd c, int b, const wchar_t* t, int tlen) : cmd(c), begin(b), text(t, tlen) {}
	Cmd cmd;
	int begin;
	std::wstring text;
}; // struct FCDiffRecW

//;------ </#DD SDK EXPORT DEF_H> -----;

// 特殊文字。ESCコード。
const wchar_t FDST_CHR_ESC = L'\\';

extern WORD CharSetToCodePage (BYTE byCharSet);
extern wchar_t GetWChar (const _TCHAR*& pStrNext, const _TCHAR* pStr, WORD wCodePage);

// タイ語などで、文字幅が正しく算出されるよう、wCodePageを追加。
extern char* GetAChar (char szBuff[], long& lStrLen, wchar_t wChar, WORD wCodePage);

// タイ語などで、文字幅が正しく算出されるよう、wCodePageを追加。
extern wchar_t CodeToWChar (WORD wCode, WORD wCodePage);
// タイ語などで、文字幅が正しく算出されるよう、wCodePageを追加。
extern BOOL WCharToCode (wchar_t& wChar, WORD wCodePage);

//----- 06.05.21 Fukushiro M. 追加始 ()-----
// 文字コードから文字の種類を返す。
extern FDCharKind FFGetCharKind (wchar_t wChar);
// 英文ワードラップ。
extern BOOL FFIsWrapedWordE (long& lNewCount, const wchar_t* wcpString, long lStringLen, long lCharCount);
// 日本語文ワードラップ。
extern BOOL FFIsWrapedWordJ (long& lNewCount, const wchar_t* wcpString, long lStringLen, long lCharCount);
//----- 06.05.21 Fukushiro M. 追加終 ()-----

// タイ語などで、文字幅が正しく算出されるよう、wCodePageを追加。
extern long FFCalcCharCount (	longVector& vpxTextRight,
								CDC* pDC,
								const wchar_t* wcpStr,
								long pxWidthLimit,
								WORD wCodePage,
								BOOL bIsWordWrapE,
								BOOL bIsWordWrapJ);

//----- 04.05.16 Fukushiro M. 追加始 ()-----
// 指定されたテキストを、指定された横幅に収まるように分割する。
extern long FFCalcTextLines (	wstringVector& vText,
								std::vector<longVector>& vvpxTextRight,
								CDC* pDC,
								const wchar_t* wcpText,
								long pxWidthLimit,
								WORD wCodePage,
								BOOL bIsWordWrapE,
								BOOL bIsWordWrapJ);
// 指定されたテキストを、指定された横幅に収まるように分割する。
extern long FFCalcTextLines (	longlongVector& vCountLength,
								std::vector<longVector>& vvpxTextRight,
								CDC* pDC,
								const wchar_t* wcpText,
								long pxWidthLimit,
								WORD wCodePage,
								BOOL bIsWordWrapE,
								BOOL bIsWordWrapJ);

// 指定された幅を上限とする、テキスト全体の幅と高さを計算する。
extern void FFCalcTextFrame (	long& pxWidth,
								long& pxHeight,
								wstringVector& vText,
								std::vector<longVector>& vvpxTextRight,
								long pxWidthLimit,
								CDC* pDC,
								const wchar_t* wcpText,
								long pxLineHeight,
								long pxLineGap,
								WORD wCodePage,
								BOOL bIsWordWrapE,
								BOOL bIsWordWrapJ);

// 指定されたテキストが収まるボックスのサイズを計算する。
extern void FFCalcTextBox (	mmetol& mmBoxWidth,
							mmetol& mmBoxHeight,
							mmetol& mmTextHeight,
							wstringVector& vText,
							std::vector<mmetolVector>& vvmmTextRight,
							double dMaxWidthPerHeight,
							double dStdWidthPerHeight,
							FDRegFontType fontType,
							const wchar_t* wcpText,
							mmetol mmLineGap,
							WORD wCodePage,
							BOOL bIsWordWrapE,
							BOOL bIsWordWrapJ);

//----- 09.04.27 Fukushiro M. 追加始 ()-----
// 指定された幅に収まるテキストの数を計算する。
extern long FFCalcCharCount (	long& pxTextWidth,
								CDC* pDC,
								long pxBoxWidth,
								const wchar_t* wcpText);
//----- 09.04.27 Fukushiro M. 追加終 ()-----

#endif

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

#if 0

// 文字列を先頭に挿入して一つの文字列にする。
extern void FFInsTopString (std::wstring& str, const wchar_t* wcpStr);

// 文字列から指定の文字で区切られる先頭の文字列を取り出す。
extern std::wstring FFExtractUntil (std::wstring& str, wchar_t wc);
extern std::wstring FFExtractUntil (std::wstring& str, const wchar_t* wcp);

// 文字列からcで区切られる先頭の文字列を取り出す。
extern astring FFExtractUntil (astring& str, char c);
extern astring FFExtractUntil (astring& str, const char* cp);
extern astring GetLine (astring& str, long& lLineCount);

//----- 09.05.17 Fukushiro M. 追加始 ()-----
extern std::wstring FFLoadHalfString (DWORD dwId, BOOL bSecond);
//----- 09.05.17 Fukushiro M. 追加終 ()-----

// 文字列左の空白・改行を削除。
extern void TrimLeft (astring& str);
// 文字列右の空白・改行を削除。
extern void TrimRight (astring& str);

// リソースの文字列を取得。
//----------<#DD SDK NOT EXPORT>-----------;
// CString はSDKで使えない。
extern CString FFLoadString (DWORD dwId);
//----------</#DD SDK NOT EXPORT>-----------;
extern std::wstring FFLoadstd::wstring (DWORD dwId);

//----------<#DD VIEWER NOT EXPORT>-----------
extern const wchar_t* GetResstd::wstring (DWORD dwId);
//----------</#DD VIEWER NOT EXPORT>-----------
//----- 08.03.13 Fukushiro M. 削除始 ()-----
//extern std::wstring FFFormatString (const wchar_t* wcpFormat, ...);
//extern std::wstring FormatResString (DWORD dwId, ...);
//----- 08.03.13 Fukushiro M. 削除終 ()-----

//----- 08.03.13 Fukushiro M. 削除始 ()-----
//extern std::wstring FFFormatStringV (const wchar_t* wcpFormat, va_list argList);
//extern std::wstring FormatResStringV (DWORD dwId, va_list argList);
//----- 08.03.13 Fukushiro M. 削除終 ()-----

// wchar_tの空白をスキップ。
extern void FFSkipSpace (const wchar_t*& wcpStr);
extern void FFSkipSpace (const char*& cpStr);
// 14.10.20 Fukushiro M. 1行追加 ()
extern BOOL FFSkipSpaceTab (const wchar_t*& wcpStr, BOOL indispensable = FALSE);
//----- 15.01.06 Fukushiro M. 追加始 ()-----
extern BOOL FFSkip1Char (wchar_t wc, const wchar_t*& wcpStr, BOOL indispensable = FALSE);
//----- 15.01.06 Fukushiro M. 追加終 ()-----

extern BOOL FFConvertStr (	std::wstring& strOut,
							const wchar_t*& wcpStr,
							const wchar_t wcaEndCode[],
							int iEndCodeSz);

//extern void FFLoadString (std::wstring& wstr, DWORD dwCtrl);

extern BOOL GetLine (std::wstring& wstrLine, std::wstring& wstrBuff);

// 14.10.23 Fukushiro M. 1行追加 ()
extern wchar_t FFSkipLine (const wchar_t*& tcpNext, long& lLineNumb);


//----- 06.11.17 Fukushiro M. 追加始 ()-----
extern void FFGetCSVTable (	std::vector<wstringVector>& vvTextTable,
							const wchar_t* wcpText);
//----- 06.11.17 Fukushiro M. 追加終 ()-----

extern BOOL FFGetCSVItem (	std::wstring& wstr,
							wchar_t& wLastChar,
							const wchar_t*& wcpNext);

extern void FFCompressLZ77 (BYTEVector& vCompressBuff,
							const BYTE* pRawData,
							DWORD dwRawDataSize);
extern void FFUncompressLZ77 (BYTE* bypRawDataBuff,
							DWORD& dwRawDataSize,
							const BYTE* bypCompressData,
							DWORD dwCompressSize);
extern void FFWriteLZ77DataBlock (BYTEVector& vDataBuff,
									const BYTE* pRawData,
									DWORD dwRawDataSize);
extern void FFReadLZ77DataBlock (BYTEVector& vRawDataBuff,
								const BYTE* pDataBlock,
								DWORD dwDataBlockSize);

extern void FFGuidToString (std::wstring& wstr1, std::wstring& wstr2, std::wstring& wstr3,
							std::wstring& wstr4, std::wstring& wstr5, const GUID& guid);
extern BOOL FFStringToGuid (GUID& guid, const wchar_t* wcp1, const wchar_t* wcp2,
							const wchar_t* wcp3, const wchar_t* wcp4, const wchar_t* wcp5);
// 14.10.20 Fukushiro M. 1行変更 ()
//extern std::wstring FFGuidToString (const GUID& guid);
extern std::wstring FFGuidToString (const GUID& guid, BOOL isParity = FALSE);
extern GUID FFStringToGuid (const wchar_t* wcpGuid, const wchar_t** wcpGuidNext = NULL);
extern std::wstring FFGuidToExString (const GUID& guid);
extern std::wstring FFGetFormedUserId (const GUID& guidUserId, const wchar_t* wcpUserName);

//----- 14.10.20 Fukushiro M. 追加始 ()-----
extern void FFStringToBinary (std::vector<BYTE>& vData, const wchar_t*& tcpData);
extern void FFBinaryToString (std::wstring& strData, long lDataSize, const BYTE* pData);
//----- 14.10.20 Fukushiro M. 追加終 ()-----


//----- 10.02.26 Fukushiro M. 追加始 ()-----
extern std::wstring FFEnumMarkupText (FPEnumMarkupTextProc proc,
								 void* pData,
								 const wchar_t* wcpHeadChar,
								 const wchar_t* wcpTailChar,
								 const wchar_t* wcpText,
								 BOOL bIsBsAsEsc);
//----- 10.02.26 Fukushiro M. 追加終 ()-----

//----- 10.03.07 Fukushiro M. 追加始 ()-----
extern void FFExtractLinkTag (std::vector<std::wstringPair>& vLinkText, const wchar_t* wcpData);
//----- 10.03.07 Fukushiro M. 追加終 ()-----

//----- 10.03.09 Fukushiro M. 追加始 ()-----
extern std::wstring FFConvToHtmlSpecText (const wchar_t* wcpText);
extern std::wstring FFConvFromHtmlSpecText (const wchar_t* wcpText);
//----- 10.03.09 Fukushiro M. 追加終 ()-----

//----- 14.08.29 Fukushiro M. 追加始 ()-----
extern const wchar_t* FFTextEncodeTypeToString (FTTextEncodeType encodeType);
extern FTTextEncodeType FFStringToTextEncodeType (std::wstring wstr);
//----- 14.08.29 Fukushiro M. 追加終 ()-----

//----- 14.09.21 Fukushiro M. 追加始 ()-----
extern void FFCompText (std::vector<FCDiffRecA>& vDiff, const char* textA, int lengthA, const char* textB, int lengthB);
extern void FFCompText (std::vector<FCDiffRecW>& vDiff, const wchar_t* textA, int lengthA, const wchar_t* textB, int lengthB);
//----- 14.09.21 Fukushiro M. 追加終 ()-----


//;----- <#DD DDTOOL EXPORT DEF_H> -----;

//;----- <#DD DOCCONV EXPORT DEF_H> -----;

//----- 08.02.11 Fukushiro M. 追加始 ()-----

//;----- <#DD SERVER EXPORT DEF_H> -----;

inline _TCHAR* FFTcsncpy_s (
							_TCHAR* strDest,
							size_t numberOfElements,
							const _TCHAR* strSource,
							size_t count
							)
{
	_tcsncpy_s(	strDest,
				numberOfElements,
				strSource,
				count);
	return strDest;
}

inline wchar_t* FFWcsncpy_s (
							wchar_t* strDest,
							size_t numberOfElements,
							const wchar_t* strSource,
							size_t count
							)
{
	wcsncpy_s(	strDest,
				numberOfElements,
				strSource,
				count);
	return strDest;
}

//;----- </#DD SERVER EXPORT DEF_H> -----;

inline char* FFStrncpy_s (
							char* strDest,
							size_t numberOfElements,
							const char* strSource,
							size_t count
							)
{
	strncpy_s(	strDest,
				numberOfElements,
				strSource,
				count);
	return strDest;
}

//;----- <#DD SERVER EXPORT DEF_H> -----;

inline _TCHAR* FFTcscpy_s (
							_TCHAR* strDestination,
							size_t numberOfElements,
							const _TCHAR* strSource
							)
{
	_tcscpy_s(	strDestination,
				numberOfElements,
				strSource);
	return strDestination;
}

inline wchar_t* FFWcscpy_s (
							wchar_t* strDestination,
							size_t numberOfElements,
							const wchar_t* strSource
							)
{
	wcscpy_s(	strDestination,
				numberOfElements,
				strSource);
	return strDestination;
}

//;----- </#DD SERVER EXPORT DEF_H> -----;

inline char* FFStrcpy_s (
							char* strDestination,
							size_t numberOfElements,
							const char* strSource
							)
{
	strcpy_s(	strDestination,
				numberOfElements,
				strSource);
	return strDestination;
}

//;----- <#DD SERVER EXPORT DEF_H> -----;

inline wchar_t* FFWcsupr_s (
							wchar_t* str,
							size_t numberOfElements
							)
{
	_wcsupr_s(	str,
				numberOfElements);
	return str;
}

//;----- </#DD SERVER EXPORT DEF_H> -----;

inline char* FFStrupr_s (
							char* str,
							size_t numberOfElements
							)
{
	_strupr_s(	str,
				numberOfElements);
	return str;
}

//;----- <#DD SERVER EXPORT DEF_H> -----;

inline wchar_t* FFWcslwr_s (
							wchar_t* str,
							size_t numberOfElements
							)
{
	_wcslwr_s(	str,
				numberOfElements);
	return str;
}

//;----- </#DD SERVER EXPORT DEF_H> -----;

inline char* FFStrlwr_s (
							char* str,
							size_t numberOfElements
							)
{
	_strlwr_s(	str,
				numberOfElements);
	return str;
}

inline _TCHAR* FFTcscat_s (
							_TCHAR* strDestination,
							size_t numberOfElements,
							const _TCHAR* strSource
							)
{
	_tcscat_s(	strDestination,
				numberOfElements,
				strSource);
	return strDestination;
}

inline wchar_t* FFWcscat_s (
							wchar_t* strDestination,
							size_t numberOfElements,
							const wchar_t* strSource
							)
{
	wcscat_s(	strDestination,
				numberOfElements,
				strSource);
	return strDestination;
}

inline wchar_t* FFWcsncat_s (
							wchar_t* strDest,
							size_t numberOfElements,
							const wchar_t* strSource,
							size_t count
							)
{
	wcsncat_s(	strDest,
				numberOfElements,
				strSource,
				count);
	return strDest;
}
//----- 08.02.11 Fukushiro M. 追加終 ()-----

//;----- </#DD DOCCONV EXPORT DEF_H> -----;

//;----- </#DD DDTOOL EXPORT DEF_H> -----;

/*************************************************************************
 * <関数>	FFStrNew
 *
 * <機能>	文字列を新しい領域へコピーする。
 *
 * <引数>	pStr	:文字列。
 *
 * <返値>	新しい領域。
 *
 * <注意>	_tcsdupは領域確保にmallocを使うので、newを使う本関数を用意。
 *
 * <履歴>	99.01.05 Fukushiro M. 作成
 *************************************************************************/
inline char* FFStrNew (const char* pStr)
{
//----- 08.01.23 Fukushiro M. 変更前 ()-----
//	char* pNew = new char[strlen(pStr) + 1];
//	strcpy(pNew, pStr);
//----- 08.01.23 Fukushiro M. 変更後 ()-----
	const size_t szBuff = strlen(pStr) + 1;
	char* pNew = new char[szBuff];
	FFStrcpy_s(pNew, szBuff, pStr);
//----- 08.01.23 Fukushiro M. 変更終 ()-----
	return pNew;
} // FFStrNew.

//;----- <#DD SERVER EXPORT DEF_H> -----;

/*************************************************************************
 * <関数>	FFStrNew
 *
 * <機能>	文字列を新しい領域へコピーする。
 *
 * <引数>	pStr	:文字列。
 *
 * <返値>	新しい領域。
 *
 * <注意>	wcsdupは領域確保にmallocを使うので、newを使う本関数を用意。
 *
 * <履歴>	02.02.03 Fukushiro M. 作成
 *************************************************************************/
inline wchar_t* FFStrNew (const wchar_t* pStr)
{
//----- 08.01.23 Fukushiro M. 変更前 ()-----
//	wchar_t* pNew = new wchar_t[wcslen(pStr) + 1];
//	wcscpy(pNew, pStr);
//----- 08.01.23 Fukushiro M. 変更後 ()-----
	const size_t szBuff = wcslen(pStr) + 1;
	wchar_t* pNew = new wchar_t[szBuff];
	FFWcscpy_s(pNew, szBuff, pStr);
//----- 08.01.23 Fukushiro M. 変更終 ()-----
	return pNew;
} // FFStrNew.

//********************************************************************************************
/*!
 * @brief	文字列を新しい領域へコピーする。
 *			コピー先に既に領域が設定されている場合、それを削除して作り直す。
 *			コピー元がNULLの場合にも対応する。
 *
 * @author	Fukushiro M.
 * @date	2014/08/19(火) 14:05:37
 *
 * @param[in,out]	wchar_t*		pStrTo		コピー先。
 * @param[in]		const wchar_t*	pStrFrom	コピー元。
 *
 * @return	なし (none)
 */
//********************************************************************************************
inline void FFStrNew (wchar_t*& pStrTo, const wchar_t* pStrFrom)
{
	if (pStrTo != NULL)
	{
		delete pStrTo;
		pStrTo = NULL;
	}
	if (pStrFrom != NULL)
	{
		const size_t szBuff = wcslen(pStrFrom) + 1;
		pStrTo = new wchar_t[szBuff];
		FFWcscpy_s(pStrTo, szBuff, pStrFrom);
	}
} // FFStrNew.

//;----- </#DD SERVER EXPORT DEF_H> -----;

/*************************************************************************
 * <関数>	StrNewNCpy
 *
 * <機能>	指定された文字数の文字列を新しい領域へコピーする。
 *
 * <引数>	pStr	:文字列。
 *			iLen	:文字数。
 *
 * <返値>	新しい領域。
 *
 * <注意>	_tcsdupは領域確保にmallocを使うので、newを使う本関数を用意。
 *
 * <履歴>	99.04.17 Fukushiro M. 作成
 *************************************************************************/
inline wchar_t* StrNewNCpy (const wchar_t* pStr, int iLen)
{
	wchar_t* pNew = new wchar_t[iLen + 1];
// 08.01.23 Fukushiro M. 1行変更 ()
//	wcsncpy(pNew, pStr, iLen);
	FFWcsncpy_s(pNew, iLen + 1, pStr, iLen);
	pNew[iLen] = L'\0';
	return pNew;
} // StrNewNCpy.

/*************************************************************************
 * <関数>	StrNewNCpy
 *
 * <機能>	指定された文字数の文字列を新しい領域へコピーする。
 *
 * <引数>	pStr	:文字列。
 *			iLen	:文字数。
 *
 * <返値>	新しい領域。
 *
 * <注意>	_tcsdupは領域確保にmallocを使うので、newを使う本関数を用意。
 *
 * <履歴>	99.04.17 Fukushiro M. 作成
 *************************************************************************/
inline char* StrNewNCpy (const char* pStr, int iLen)
{
	char* pNew = new char[iLen + 1];
// 08.01.23 Fukushiro M. 1行変更 ()
//	strncpy(pNew, pStr, iLen);
	FFStrncpy_s(pNew, iLen + 1, pStr, iLen);
	pNew[iLen] = '\0';
	return pNew;
} // StrNewNCpy.

//********************************************************************************************
/*!
 * @brief	指定された2つの文字列が等しいか調べる。
 *			部品属性の比較用。
 *
 * @author	Fukushiro M.
 * @date	2014/08/19(火) 10:44:51
 *
 * @param[in]	const wchar_t*	a	文字列。NULLの場合もある。
 * @param[in]	const wchar_t*	b	文字列。NULLの場合もある。
 *
 * @return	BOOL	TRUE:等しい / FALSE:等しくない
 */
//********************************************************************************************
inline BOOL almosteq (const wchar_t* a, const wchar_t* b)
{
	if (a == b) return TRUE;
	// a != b なので、a、bのいずれかがNULLならば等しくない。
	if (a == NULL || b == NULL) return FALSE;
	return (wcscmp(a, b) == 0);
} // almosteq (const wchar_t* a, const wchar_t* b)

//********************************************************************************************
/*!
 * @brief	文字列がNULLまたは空文字の場合TRUE。
 * @author	Fukushiro M.
 * @date	2014/10/20(月) 00:07:44
 *
 * @param[in]	const wchar_t*	tcp	文字列。
 *
 * @return	BOOL	TRUE:NULLまたは空文字 / FALSE:通常文字列
 */
//********************************************************************************************
inline BOOL FFIsStrEmpty (const wchar_t* tcp)
{
	return (tcp == NULL) || (tcp[0] == L'\0');
} // FFIsStrEmpty

#endif

};
