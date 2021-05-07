#include "stdafx.h"
#include "UtilString.h"		// For UtilString::Format.
#include "UtilStr.h"		// AppendString
#include "RegBase.h"		// This header.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static std::wstring REG_COMPANY_NAME = L"Dynamic Draw Project";
static std::wstring REG_PACKAGE_NAME = L"Sticktrace";
static std::wstring REG_APPLICATION_NAME = L"Sticktrace";


static constexpr const wchar_t* REG_IS_AUTO_ALT_NAME = L"IsAutoAlt";


static const LOGFONT DEFAULT_LOGFONT[] = {
	{
		-4000,		// LONG lfHeight;
		0,			// LONG lfWidth;
		0,			// LONG lfEscapement;
		0,			// LONG lfOrientation;
		FW_NORMAL,	// LONG lfWeight;
		0,			// BYTE lfItalic;
		0,			// BYTE lfUnderline;
		0,			// BYTE lfStrikeOut;
		DEFAULT_CHARSET,		// BYTE lfCharSet;
		OUT_DEFAULT_PRECIS,		// BYTE lfOutPrecision;
		CLIP_DEFAULT_PRECIS,	// BYTE lfClipPrecision;
		DEFAULT_QUALITY,		// BYTE lfQuality;
		DEFAULT_PITCH,			// BYTE lfPitchAndFamily;
		L"ＭＳ ゴシック"			// WCHAR lfFaceName[LF_FACESIZE];
	}
};

/*************************************************************************
 * <関数>	myGetCompAppSection
 *
 * <機能>	アプリ名、セクション名からキー名を作成。
 *
 * <引数>	wcpAppName	:アプリ名を指定。NULLを指定した場合はデフォルト。
 *			wcpSection	:セクション名。NULLを指定した場合はセクション名無し。
 *
 * <返値>	キー名。
 *
 * <履歴>	09.07.30 Fukushiro M. 作成
 *************************************************************************/
static std::wstring myGetCompAppSection (const wchar_t* wcpSection)
{

	std::wstring name = L"Software";
	if (!REG_COMPANY_NAME.empty())
		name += std::wstring(L"\\") + REG_COMPANY_NAME;
	if (!REG_PACKAGE_NAME.empty())
		name += std::wstring(L"\\") + REG_PACKAGE_NAME;
	if (!REG_APPLICATION_NAME.empty())
		name += std::wstring(L"\\") + REG_APPLICATION_NAME;
	if (wcpSection != nullptr && wcpSection[0] != L'\0')
		name += std::wstring(L"\\") + wcpSection;
	return name;
} // static std::wstring myGetCompAppSection (const wchar_t* wcpSection)

  //********************************************************************************************
  /*!
  * @brief	FcRegBase::SaveEdit等の引数[name]は、IDC_XXXXX、ID_CTL_XXXX といった形式。
  *			これをレジストリ保存用に先頭のIDC_、ID_ を削除。
  * @author	Fukushiro M.
  * @date	2014/08/27(水) 20:15:14
  *
  * @param[in]	const wchar_t*	name	名称。
  *
  * @return	WString	名称
  */
  //********************************************************************************************
static std::wstring MyIdNameToRegName(const wchar_t* name)
{
	// 形式としては以下の２つのみを許可する。
	// IDC_XXX_YYY_ZZZZZZ…  XXX：識別子、YYY：型
	// ID_CTL_XXX_YYY_ZZZZZZ…  XXX：識別子、YYY：型
	// 例：IDC_OPT_CMB_FONTNAME
	// 例：ID_CTL_IMF_EDT_EVENT_1

#ifdef _DEBUG
	std::wstring strName = name;
	if (strName.substr(0, 4) == L"IDC_")
	{
		if (strName.at(4) < L'A' || 'Z' < strName.at(4)) ThrowLeSystemError();
		if (strName.at(5) < L'A' || 'Z' < strName.at(5)) ThrowLeSystemError();
		if (strName.at(6) < L'A' || 'Z' < strName.at(6)) ThrowLeSystemError();

		std::wstring strTmp = strName.substr(7, 5);
		if (strTmp != L"_BTN_" &&
			strTmp != L"_EDT_" &&
			strTmp != L"_CBE_" &&
			strTmp != L"_CMB_" &&
			strTmp != L"_LST_" &&
			strTmp != L"_LSV_" &&
			strTmp != L"_CHK_" &&
			strTmp != L"_RDO_") ThrowLeSystemError();
	}
	else
		if (strName.substr(0, 7) == L"ID_CTL_")
		{
			if (strName.at(7) < L'A' || 'Z' < strName.at(7)) ThrowLeSystemError();
			if (strName.at(8) < L'A' || 'Z' < strName.at(8)) ThrowLeSystemError();
			if (strName.at(9) < L'A' || 'Z' < strName.at(9)) ThrowLeSystemError();

			std::wstring strTmp = strName.substr(10, 5);
			if (strTmp != L"_BTN_" &&
				strTmp != L"_EDT_" &&
				strTmp != L"_CBE_" &&
				strTmp != L"_CMB_" &&
				strTmp != L"_LST_" &&
				strTmp != L"_LSV_" &&
				strTmp != L"_CHK_" &&
				strTmp != L"_RDO_") ThrowLeSystemError();
		}
		else
		{
			ThrowLeSystemError();
		}
#endif//_DEBUG

	if (wcsncmp(name, L"IDC_", _countof(L"IDC_") - 1) == 0)
		return name + _countof(L"IDC_") - 1;
	if (wcsncmp(name, L"ID_CTL_", _countof(L"ID_CTL_") - 1) == 0)
		return name + _countof(L"ID_CTL_") - 1;
	return name;
} // static std::wstring MyIdNameToRegName (const wchar_t* name)

void FCRegBase::SetApplicationInfo(const wchar_t* companyName, const wchar_t* packageName, const wchar_t* applicationName)
{
	// REG_COMPANY_NAME must be filled. If companyName is empty, REG_COMPANY_NAME keeps default value.
	if (companyName != nullptr && companyName[0] != L'\0')
		REG_COMPANY_NAME = companyName;

	// REG_PACKAGE_NAME can be empty.
	if (packageName != nullptr)
		REG_PACKAGE_NAME = packageName;

	// REG_APPLICATION_NAME can be empty.
	if (applicationName != nullptr)
		REG_APPLICATION_NAME = applicationName;
} // FCRegBase::SetApplicationInfo.

/*************************************************************************
* <関数>	FCRegData::InitRegFont
*
* <機能>	レジストリから取得した各種のフォント情報に基づき、FONTを
*			初期化する。
*
* <引数>	font		:フォントを初期化して返す。
*			nType		:フォントの種類を指定。
*			lEscapement	:角度情報を指定。
*			dZoom		:ズーム値を指定。デフォルトはFFG_()->dDisplayPPM。
*
* <履歴>	98.10.30 Fukushiro M. 作成
*			99.03.28 Fukushiro M. レジストリ登録方法を変更。
*************************************************************************/
void FCRegBase::InitRegFont(CFont& font, FDRegFontType nType, long lEscapement, double dZoom)
{
	//----- 06.02.26 Fukushiro M. 追加始 ()-----
	if (dZoom < 0.0)
		dZoom = GetDisplayPPM();
	//----- 06.02.26 Fukushiro M. 追加終 ()-----
	LOGFONT tmp;
	InitRegFont(tmp, nType, lEscapement, dZoom);
	font.CreateFontIndirect(&tmp);
} // FCRegData::InitRegFont.

  /*************************************************************************
  * <関数>	FCRegData::InitRegFont
  *
  * <機能>	レジストリから取得した各種のフォント情報に基づき、FONTを
  *			初期化する。
  *
  * <引数>	logFont		:フォントを初期化して返す。
  *			nType		:フォントの種類を指定。
  *			lEscapement	:角度情報を指定。
  *			dZoom		:ズーム値を指定。デフォルトはFFG_()->dDisplayPPM。
  *
  * <履歴>	99.03.29 Fukushiro M. 作成
  *************************************************************************/
void FCRegBase::InitRegFont(LOGFONT& logFont, FDRegFontType nType, long lEscapement, double dZoom)
{
	ASSERT(0 <= nType && nType < FDFT_END);
	ASSERT(GetDisplayPPM() != 0.0);
	if (dZoom < 0.0)
		dZoom = GetDisplayPPM();
	
	logFont = DEFAULT_LOGFONT[nType];
	logFont.lfHeight = (LONG)(((double)logFont.lfHeight / 1000.0) * dZoom);
} // FCRegBase::InitRegFont.



 //********************************************************************************************
  /*!
  * @brief	リストコントロールの内容をレジストリに記録する。
  *
  *			リストコントロールのリスト一覧、アイテムデータ一覧を保存する。
  * @author	Fukushiro M.
  * @date	2014/08/27(水) 20:08:41
  *
  * @param[in]	CWnd*			wnd			ダイアログ。
  * @param[in]	const wchar_t*	name		記録名。
  * @param[in]	int				list		リストコントロール。
  *
  * @return	なし (none)
  */
  //********************************************************************************************
void FCRegBase::SaveListCtrlContents(CWnd* wnd, const wchar_t* name, int list)
{
	std::wstring stream;
	CListCtrl* listCtrl = (CListCtrl*)wnd->GetDlgItem(list);
	for (int iL = 0; iL != listCtrl->GetItemCount(); iL++)
		//----- 行をループ -----
	{
		for (int iC = 0; iC != listCtrl->GetHeaderCtrl()->GetItemCount(); iC++)
			//----- 列をループ -----
		{
			// アイテムのテキストを取得。
			const std::wstring text = (const wchar_t *)listCtrl->GetItemText(iL, iC);
			// 文字列をつないで一つの文字列にする。
			UtilStr::AppendString(stream, text.c_str());
		}
		// アイテム（行）データを文字列に追加。
		UtilStr::AppendString(stream, UtilString::Format(L"%I64d", (__int64)listCtrl->GetItemData(iL)).c_str());
	}
	SaveRegString(NULL, MyIdNameToRegName(name).c_str(), stream.c_str());
} // FCRegBase::SaveListCtrlContents.

  //********************************************************************************************
  /*!
  * @brief	リストコントロールの内容をレジストリに記録された値で初期化する。
  *
  *			リストコントロールのリスト一覧、アイテムデータ一覧を復元する。
  * @author	Fukushiro M.
  * @date	2014/08/27(水) 20:08:41
  *
  * @param[in]	CWnd*			wnd			ダイアログ。
  * @param[in]	const wchar_t*	name		記録名。
  * @param[in]	int				list		リストコントロール。
  * @param[in]	const wchar_t*	defaultItem	デフォルトの項目。
  *
  * @return	なし (none)
  */
  //********************************************************************************************
void FCRegBase::LoadListCtrlContents(CWnd* wnd, const wchar_t* name, int list)
{
	CListCtrl* listCtrl = (CListCtrl*)wnd->GetDlgItem(list);
	std::wstring stream = L"";
	LoadRegString(NULL, MyIdNameToRegName(name).c_str(), stream);
	int iL = 0;
	while (!stream.empty())
	{
		// 行を追加。
		listCtrl->InsertItem(iL, L"");
		for (int iC = 0; iC != listCtrl->GetHeaderCtrl()->GetItemCount(); iC++)
			//----- 列をループ -----
		{
			// つないだ文字列から一つの文字列を取り出す。
			const std::wstring text = UtilStr::ExtractString(stream);
			listCtrl->SetItemText(iL, iC, text.c_str());
			if (stream.empty()) break;
		}
		if (stream.empty()) break;
		// アイテム（行）データを取り出す。
		const __int64 itemData = _wcstoi64(UtilStr::ExtractString(stream).c_str(), NULL, 10);
		listCtrl->SetItemData(iL, (DWORD_PTR)itemData);
		iL++;
	}
} // FCRegBase::LoadListCtrlContents.

  //********************************************************************************************
  /*!
  * @brief	リストコントロールのカラム幅をレジストリに記録する。
  *
  *			リストコントロールのリスト一覧、アイテムデータ一覧を保存する。
  * @author	Fukushiro M.
  * @date	2014/08/27(水) 20:08:41
  *
  * @param[in]	CWnd*			wnd			ダイアログ。
  * @param[in]	const wchar_t*	name		記録名。
  * @param[in]	int				list		リストコントロール。
  *
  * @return	なし (none)
  */
  //********************************************************************************************
void FCRegBase::SaveListCtrlColumnWidth(CWnd* wnd, const wchar_t* name, int list)
{
	std::wstring stream;
	CListCtrl* listCtrl = (CListCtrl*)wnd->GetDlgItem(list);
	for (int iC = 0; iC != listCtrl->GetHeaderCtrl()->GetItemCount(); iC++)
	{
		const int columnWidth = listCtrl->GetColumnWidth(iC);
		// アイテム（行）データを文字列に追加。
		UtilStr::AppendString(stream, UtilString::Format(L"%ld", columnWidth).c_str());
	}
	SaveRegString(NULL, MyIdNameToRegName(name).c_str(), stream.c_str());
} // FCRegBase::SaveListCtrlColumnWidth.

  //********************************************************************************************
  /*!
  * @brief	リストコントロールのカラム幅をレジストリに記録された値で初期化する。
  *
  *			リストコントロールのリスト一覧、アイテムデータ一覧を復元する。
  * @author	Fukushiro M.
  * @date	2014/08/27(水) 20:08:41
  *
  * @param[in]	CWnd*			wnd			ダイアログ。
  * @param[in]	const wchar_t*	name		記録名。
  * @param[in]	int				list		リストコントロール。
  * @param[in]	const wchar_t*	defaultItem	デフォルトの項目。
  *
  * @return	なし (none)
  */
  //********************************************************************************************
void FCRegBase::LoadListCtrlColumnWidth(CWnd* wnd, const wchar_t* name, int list)
{
	CListCtrl* listCtrl = (CListCtrl*)wnd->GetDlgItem(list);
	std::wstring stream = L"";
	LoadRegString(NULL, MyIdNameToRegName(name).c_str(), stream);
	std::vector<int> vColumnWidth;
	while (!stream.empty())
	{
		// アイテム（行）データを取り出す。
		const __int64 columnWidth = _wcstoi64(UtilStr::ExtractString(stream).c_str(), NULL, 10);
		vColumnWidth.push_back((int)columnWidth);
	}
	if (!vColumnWidth.empty())
	{
		while ((int)vColumnWidth.size() < listCtrl->GetHeaderCtrl()->GetItemCount())
			vColumnWidth.push_back(100);
		for (int iC = 0; iC != listCtrl->GetHeaderCtrl()->GetItemCount(); iC++)
			listCtrl->SetColumnWidth(iC, vColumnWidth[iC]);
	}
} // FCRegBase::LoadListCtrlColumnWidth.

/*************************************************************************
 * <関数>	FCRegBase::SaveRegBinary
 *
 * <機能>	レジストリの所定のキーにバイナリデータを書き出す。
 *
 * <引数>	wcpSection	:レジストリセクションを指定。
 *			wcpName	:レジストリ名を指定。
 *			pData	:書き出すデータを指定。
 *			dwDataSz:データサイズを指定。
 *			wcpAppName	:アプリケーション名を指定。nullptr を指定した場合は
 *						 FFGetLocalAppName関数の値を使う。
 *
 * <解説>	Software/Dynamic Draw Project/ThoughtTickler5/[AppName]/[wcpSection]/[wcpName]に
 *			pDataで指定されたデータを書き出す。
 *
 * <履歴>	00.02.07 Fukushiro M. 作成
 *************************************************************************/
void FCRegBase::SaveRegBinary (	const wchar_t* wcpSection,
								const wchar_t* wcpName,
								const BYTE* pData,
								DWORD dwDataSz)
{
	FCRegBase::SaveBinary(pData, dwDataSz, HKEY_CURRENT_USER, myGetCompAppSection(wcpSection).c_str(), wcpName);
} // FCRegBase::SaveRegBinary.

/*************************************************************************
 * <関数>	FCRegBase::LoadRegBinary
 *
 * <機能>	レジストリの所定のキーからバイナリデータを読み込む。
 *
 * <引数>	wcpSection	:レジストリセクションを指定。
 *			wcpName		:レジストリ名を指定。
 *			pData		:バッファを指定。読み込んだデータを返す。
 *						 データサイズを調べる場合は nullptr を指定。
 *			dwDataSz	:バッファサイズを指定。
 *			wcpAppName	:アプリケーション名を指定。nullptr を指定した場合は
 *						 FFGetLocalAppName関数の値を使う。
 *
 * <返値>	取得されたデータのサイズを返す。
 *			dwDataSz がデータサイズを下回る場合は、データは1バイトたりとも
 *			取得されない。
 *			pData が nullptr の場合は、記録されているデータのサイズを返す。
 *
 * <解説>	Software/Dynamic Draw Project/ThoughtTickler5/[AppName]/[wcpSection]/[wcpName]から
 *			データを読み込む。
 *
 * <履歴>	00.02.07 Fukushiro M. 作成
 *************************************************************************/
DWORD FCRegBase::LoadRegBinary (const wchar_t* wcpSection,
								const wchar_t* wcpName,
								BYTE* pData,
								DWORD dwDataSz)
{
	return FCRegBase::LoadBinary(pData, dwDataSz, HKEY_CURRENT_USER, myGetCompAppSection(wcpSection).c_str(), wcpName);
} // FCRegBase::LoadRegBinary.

  /*************************************************************************
  * <関数>	FCRegBase::SaveRegString
  *
  * <機能>	レジストリの所定のキーに文字データを書き出す。
  *
  * <引数>	wcpSection	:レジストリセクションを指定。
  *			wcpName		:レジストリ名を指定。
  *			wcpData		:書き出すデータを指定。
  *
  * <解説>	Software/Dynamic Draw Project/ThoughtTickler5/[AppName]/[wcpName]に
  *			wcpDataで指定されたデータを書き出す。
  *
  * <履歴>	00.02.07 Fukushiro M. 作成
  *************************************************************************/
void FCRegBase::SaveRegString(
	const wchar_t* wcpSection,
	const wchar_t* wcpName,
	const wchar_t* wcpData
)
{
	// レジストリキー名を作成。
	const std::wstring strRegVarKey(myGetCompAppSection(wcpSection));
	SaveBinary(
		(const BYTE*)wcpData,
		(wcslen(wcpData) + 1) * sizeof(wchar_t),
		HKEY_CURRENT_USER,
		strRegVarKey.c_str(),
		wcpName
	);
} // FCRegBase::SaveRegString.

  /*************************************************************************
  * <関数>	FCRegBase::LoadRegString
  *
  * <機能>	レジストリの所定のキーから文字データを読み込む。
  *
  * <引数>	wcpSection	:レジストリセクションを指定。
  *			wcpName		:レジストリ名を指定。
  *			strData		:バッファを指定。読み込んだデータを返す。
  *
  * <返値>	データが取得できた場合 TRUE、できなかった場合 FALSE。
  *
  * <解説>	Software/Dynamic Draw Project/ThoughtTickler5/[AppName]/[wcpName]から
  *			データを読み込む。
  *
  * <履歴>	00.02.07 Fukushiro M. 作成
  *************************************************************************/
BOOL FCRegBase::LoadRegString(
	const wchar_t* wcpSection,
	const wchar_t* wcpName,
	std::wstring& strData
)
{
	// レジストリキー名を作成。
	const std::wstring strRegVarKey(myGetCompAppSection(wcpSection));
	std::vector<BYTE> vData;
	if (!LoadBinary(vData, HKEY_CURRENT_USER, strRegVarKey.c_str(), wcpName))
		return FALSE;
	strData = (const wchar_t*)vData.data();
	return TRUE;
} // FCRegBase::LoadRegString.

  /*************************************************************************
  * <関数>	FCRegBase::SaveRegDWORD
  *
  * <機能>	指定のレジストリキーに32ビット値を記録する。
  *
  * <引数>	wcpSection	:レジストリセクションを指定。
  *			wcpName		:レジストリ名を指定。
  *			dwValue		:記録する32ビット値を指定。
  *			wcpAppName	:アプリケーション名を指定。NULL を指定した場合は
  *						 FFGetLocalAppName関数の値を使う。
  *
  * <解説>	Software/Dynamic Draw Project/ThoughtTickler5/[AppName]/[wcpName]に
  *			dwValueで指定されたデータを書き出す。
  *
  * <履歴>	09.07.30 Fukushiro M. 作成
  *************************************************************************/
void FCRegBase::SaveRegDWORD(
	const wchar_t* wcpSection,
	const wchar_t* wcpName,
	DWORD dwValue)
{
	SaveDWORD(dwValue,
		HKEY_CURRENT_USER,
		myGetCompAppSection(wcpSection).c_str(),
		wcpName);
} // FCRegBase::SaveRegDWORD.

  /*************************************************************************
  * <関数>	FCRegBase::LoadRegDWORD
  *
  * <機能>	指定のレジストリキーから32ビット値を読み出す。
  *
  * <引数>	wcpSection	:レジストリセクションを指定。
  *			wcpName		:レジストリ名を指定。
  *			dwValue		:読み込んだ32ビット値を返す。
  *			wcpAppName	:アプリケーション名を指定。NULL を指定した場合は
  *						 FFGetLocalAppName関数の値を使う。
  *
  * <返値>	データが取得できた場合 TRUE、できなかった場合 FALSE。
  *
  * <解説>	Software/Dynamic Draw Project/ThoughtTickler5/[AppName]/[wcpName]の
  *			値を読み出す。
  *
  * <履歴>	09.07.30 Fukushiro M. 作成
  *************************************************************************/
bool FCRegBase::LoadRegDWORD(
	const wchar_t* wcpSection,
	const wchar_t* wcpName,
	DWORD& dwValue)
{
	return LoadDWORD(dwValue, HKEY_CURRENT_USER, myGetCompAppSection(wcpSection).c_str(), wcpName);
} // FCRegBase::LoadRegDWORD.

BOOL FCRegBase::GetIsAutoAlt (int isAutoAlt)
{
	static int IS_AUTO_ALT = -1;
	if (isAutoAlt == -1)
	{
		if (IS_AUTO_ALT == -1)
		{
			// Set 'false' as the default value.
			IS_AUTO_ALT = 0;
			LoadRegBinary(NULL, REG_IS_AUTO_ALT_NAME, (BYTE*)&IS_AUTO_ALT, sizeof(IS_AUTO_ALT));
		}
	}
	else
	{
		IS_AUTO_ALT = isAutoAlt;
		SaveRegBinary(NULL, REG_IS_AUTO_ALT_NAME, (const BYTE*)&IS_AUTO_ALT, sizeof(IS_AUTO_ALT));
	}
	return (BOOL)IS_AUTO_ALT;
} // FCRegBase::GetIsAutoAlt.

void FCRegBase::SetIsAutoAlt (BOOL isAutoAlt)
{
	GetIsAutoAlt((int)isAutoAlt);
} // FCRegBase::SetIsAutoAlt.

/*************************************************************************
 * <関数>	FCRegBase::SaveBinary
 *
 * <機能>	指定のレジストリキーにバイナリデータを記録する。
 *
 * <引数>	pData		:記録するバイナリデータを指定。
 *			dwDataSz	:pDataのサイズを指定。
 *			hKey		:メインキーを指定。CLASS_ROOT等。
 *			wcpSubKey	:サブキーを指定。
 *			wcpName		:レジストリの名前を指定。
 *
 * <履歴>	04.09.03 Fukushiro M. 作成
 *************************************************************************/
void FCRegBase::SaveBinary (const BYTE* pData,
							DWORD dwDataSz,
							HKEY hKey,
							const wchar_t* wcpSubKey,
							const wchar_t* wcpName)
{
	// レジストリキーの生成・オープンをトライ。
	HKEY hSubKey;
	DWORD dwDisposition;
	if (::RegCreateKeyEx(hKey, wcpSubKey, 0L, (wchar_t *)L"", REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS, nullptr,
						&hSubKey, &dwDisposition) != ERROR_SUCCESS) return;
	// バイナリデータ書き出し。
	(void)::RegSetValueEx(hSubKey, wcpName, 0, REG_BINARY, pData, dwDataSz);
	// レジストリをクローズ。
	::RegCloseKey(hSubKey);
} // FCRegBase::SaveBinary.

/*************************************************************************
 * <関数>	FCRegBase::LoadBinary
 *
 * <機能>	指定のレジストリキーからバイナリデータを取り出す。
 *
 * <引数>	pData		:記録するバイナリデータを返す。
 *			dwDataSz	:pDataのサイズを指定。
 *			hKey		:メインキーを指定。CLASS_ROOT等。
 *			wcpSubKey	:サブキーを指定。
 *			wcpName		:レジストリの名前を指定。
 *
 * <返値>	取得されたデータのサイズを返す。
 *			dwDataSz がデータサイズを下回る場合は、データは1バイトたりとも
 *			取得されない。
 *			pData が nullptr の場合は、記録されているデータのサイズを返す。
 *
 * <解説>	pData には、あらかじめ領域を用意しておく必要がある。
 *
 * <履歴>	04.09.03 Fukushiro M. 作成
 *************************************************************************/
DWORD FCRegBase::LoadBinary (	BYTE* pData,
								DWORD dwDataSz,
								HKEY hKey,
								const wchar_t* wcpSubKey,
								const wchar_t* wcpName)
{
	// レジストリキーのオープンをトライ。
	HKEY hSubKey;
	if (::RegOpenKeyEx(hKey, wcpSubKey, 0L, KEY_QUERY_VALUE, &hSubKey) != ERROR_SUCCESS)
		return 0;
  do {
	DWORD dwRegSz = 0;
	//----- バイナリデータのサイズチェック -----
	if (::RegQueryValueEx(hSubKey, wcpName, nullptr, nullptr, nullptr, &dwRegSz) != ERROR_SUCCESS) break;
	if (pData != nullptr)
	{
		// 用意されているバッファサイズよりデータが大きい場合は終了。
		if (dwDataSz < dwRegSz) break;
		// バイナリデータ読み込み。
		if (::RegQueryValueEx(hSubKey, wcpName, nullptr, nullptr, pData, &dwRegSz) != ERROR_SUCCESS) break;
	}
	// レジストリをクローズ。
	::RegCloseKey(hSubKey);
	// データサイズを返す。
	return dwRegSz;
  } while (FALSE);
	// レジストリをクローズ。
	::RegCloseKey(hSubKey);
	return 0;
} // FCRegBase::LoadBinary.

  /*************************************************************************
  * <関数>	FCRegBase::LoadBinary
  *
  * <機能>	指定のレジストリキーからバイナリデータを取り出す。
  *
  * <引数>	vData		:記録するバイナリデータを返す。
  *			hKey		:メインキーを指定。CLASS_ROOT等。
  *			wcpSubKey	:サブキーを指定。
  *			wcpName		:レジストリの名前を指定。
  *
  * <返値>	成功した場合はTRUE。
  *
  * <履歴>	09.06.16 Fukushiro M. 作成
  *************************************************************************/
BOOL FCRegBase::LoadBinary(
	std::vector<BYTE>& vData,
	HKEY hKey,
	const wchar_t* wcpSubKey,
	const wchar_t* wcpName
)
{
	// レジストリキーのオープンをトライ。
	HKEY hSubKey;
	if (::RegOpenKeyEx(hKey, wcpSubKey, 0L, KEY_QUERY_VALUE, &hSubKey) != ERROR_SUCCESS)
		return FALSE;
	for (;;)
	{
		DWORD dwRegSz = 0;
		//----- バイナリデータのサイズチェック -----
		if (::RegQueryValueEx(hSubKey, wcpName, NULL, NULL, NULL, &dwRegSz) != ERROR_SUCCESS)
			break;
		vData.resize(dwRegSz);
		// バイナリデータ読み込み。
		if (::RegQueryValueEx(hSubKey, wcpName, NULL, NULL, vData.data(), &dwRegSz) != ERROR_SUCCESS)
			break;
		// レジストリをクローズ。
		::RegCloseKey(hSubKey);
		return TRUE;
	}
	// レジストリをクローズ。
	::RegCloseKey(hSubKey);
	// データ領域を削除。
	vData.clear();
	return FALSE;
} // FCRegBase::LoadBinary.

  /*************************************************************************
  * <関数>	FCRegBase::SaveDWORD
  *
  * <機能>	指定のレジストリキーに32ビット値を記録する。
  *
  * <引数>	dwValue		:記録する32ビット値を指定。
  *			hKey		:メインキーを指定。CLASS_ROOT等。
  *			wcpSubKey	:サブキーを指定。
  *			wcpName		:レジストリの名前を指定。
  *
  * <履歴>	09.07.30 Fukushiro M. 作成
  *************************************************************************/
void FCRegBase::SaveDWORD(DWORD dwValue,
	HKEY hKey,
	const wchar_t* wcpSubKey,
	const wchar_t* wcpName)
{
	// レジストリキーの生成・オープンをトライ。
	HKEY hSubKey;
	DWORD dwDisposition;
	if (::RegCreateKeyEx(hKey, wcpSubKey, 0L, (wchar_t *)_T(""), REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, nullptr,
		&hSubKey, &dwDisposition) != ERROR_SUCCESS) return;
	// 32ビット値書き出し。
	(void)::RegSetValueEx(hSubKey, wcpName, 0, REG_DWORD,
		(LPBYTE)&dwValue, (DWORD)sizeof(dwValue));
	// レジストリをクローズ。
	::RegCloseKey(hSubKey);
} // FCRegBase::SaveDWORD.

bool FCRegBase::LoadDWORD(
	DWORD & dwValue, HKEY hKey, const wchar_t * wcpSubKey, const wchar_t * wcpName)
{
	// レジストリキーのオープンをトライ。
	HKEY hSubKey;
	if (::RegOpenKeyEx(hKey, wcpSubKey, 0L, KEY_QUERY_VALUE, &hSubKey) != ERROR_SUCCESS)
		return false;
	for (;;)
	{
		DWORD dwRegSz = sizeof(dwValue);
		// データ読み込み。
		if (::RegQueryValueEx(hSubKey, wcpName, nullptr, nullptr,
			(LPBYTE)&dwValue, &dwRegSz) != ERROR_SUCCESS)
			break;
		// レジストリをクローズ。
		::RegCloseKey(hSubKey);
		return true;
	}
	// レジストリをクローズ。
	::RegCloseKey(hSubKey);
	return false;
}

