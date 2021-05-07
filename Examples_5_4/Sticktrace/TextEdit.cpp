// TextEdit.cpp : 実装ファイル
//

#include "stdafx.h"
#include "resource.h"
#include "Astrwstr.h"
#include "UtilStr.h"		// For UtilStr::AppendString.
#include "UtilWin.h"
#include "TextEdit.h"		// This header.

static const CSize EDITOR_BREAKPOINT_SIZE(9, 9);	// BREAKPOINTアイコンサイズ
static const CSize EDITOR_TRACELINE_SIZE(13, 12);	// TRACELINEアイコンサイズ
static const CSize EDITOR_NOTIFY_SIZE(5, 11);		// NOTIFYアイコンサイズ

static const int EDITOR_MARKER_BELT_WIDTH = 15;		// マーカー描画用のベルト領域幅。
static const int EDITOR_TEXT_LEFT = 20;				// テキストの描画領域の左。

// CFCTextEdit

IMPLEMENT_DYNAMIC(CFCTextEdit, CFCDdEdit)

CFCTextEdit::CFCTextEdit ()
			: m_tabSize(4)
			, m_isUseSpacesAsTab(false)
			, m_iCurCharIndex(-1)	// 現在位置。
			, m_iLBtnDownLine(-1)	// マウス左ボタンダウン時の行インデックス。
			, m_bIsBreakpointActivated(FALSE)	// ブレークポイントのアクティブ状態
			, m_hIconBreakpoint(NULL)
			, m_hIconInactiveBP(NULL)
			, m_hIconTraceline(NULL)
			, m_hIconNotify(NULL)
{
} // CFCTextEdit::CFCTextEdit ()

CFCTextEdit::~CFCTextEdit () {}

//********************************************************************************************
/*!
 * @brief	Tabの半角換算の文字数を設定。
 * @author	Fukushiro M.
 * @date	2014/08/16(土) 10:49:07
 *
 * @param[in]	int	tabSize	Tabの半角換算の文字数。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::SetTabSize (int tabSize)
{
	m_tabSize = tabSize;
} // CFCTextEdit::SetTabSize.

void CFCTextEdit::UseSpacesAsTab (bool useSpaces)
{
	m_isUseSpacesAsTab = useSpaces;
} // CFCTextEdit::UseSpacesAsTab.

//********************************************************************************************
/*!
 * @brief	UpdateTextRect 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/14(日) 11:53:36
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::UpdateTextRect ()
{
	// SetRectで設定した矩形は、内部で最適化される。このためGetRect値とは異なる。
	// GetRect->SetRectとすると矩形がどんどん小さくなるのでClientRectを元にする。
	CRect rtClient;
	GetClientRect(rtClient);
	rtClient.left += EDITOR_TEXT_LEFT;
	SetRect(rtClient);
} // CFCTextEdit::UpdateTextRect.

//********************************************************************************************
/*!
 * @brief	文字位置のタブ位置を計算。
 *			まず、半角換算の文字位置を計算。全角は2文字、半角は1文字として計算される。
 *			下の場合、「C」の半角換算位置は2、「う」の版換算位置は7。
 *			---------------------------------------------------
 *			0 1 2 3  4  5  6  <- 文字位置
 *			A B C あ い う え
 *			0 1 2 3  5  7  9  <- 半角換算位置
 *			---------------------------------------------------
 *			タブインデックスは半角換算位置をタブ文字数で割った値。
 *			タブ余り文字数は半角換算位置をタブ文字数で割った余り。
 *			タブ文字数＝４、「う」のタブ位置は
 *			タブインデックス = 7 / 4 = 1
 *			タブ余り文字数 = 7 % 4 = 3
 *			つまり、タブ1文字と半角3文字で「う」の位置となる。
 *			（実際にはタブ文字は使わず、空白４文字を使う）
 *
 * @author	Fukushiro M.
 * @date	2014/08/16(土) 10:44:34
 *
 * @param[out]	int&	tabIndex	タブインデックス。
 * @param[out]	int&	restCount	タブ余り文字数。
 * @param[in]	int		lineTopCharIndex	文字インデックス。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::CharIndexToTabPosition (int& tabIndex, int& restCount, int charIndex) const
{
	// lineTopCharIndex（文字位置）の行インデックス。
	const int lineIndex = LineFromChar(charIndex);
	// lineTopCharIndex（文字位置）の行の先頭文字のインデックス。
	const int charIndex0 = LineIndex(lineIndex);

	int hankakuCount = 0;	// 半角換算の文字数。
	for (int index = charIndex0; index != charIndex; index++)
	{
		if (0xff < GetText()[index])
			hankakuCount += 2;
		else
			hankakuCount += 1;
	}
	tabIndex = hankakuCount / m_tabSize;
	restCount = hankakuCount % m_tabSize;
} // CFCTextEdit::CharIndexToTabPosition.

//********************************************************************************************
/*!
 * @brief	指定された文字位置から調べて、最初の空白（' ','\t'）ではない文字が出現した位置を返す。
 * @author	Fukushiro M.
 * @date	2014/08/16(土) 11:00:38
 *
 * @param[in]	int	lineTopCharIndex	文字位置。
 *
 * @return	int	空白ではない文字位置。
 */
//********************************************************************************************
int CFCTextEdit::FindNoSpaceCharIndex (int charIndex) const
{
	int index = charIndex;
	for (; index != GetText().length(); index++)
	{
		if (GetText()[index] != L' ' && GetText()[index] != L'\t') break;
	}
	return index;
} // CFCTextEdit::FindNoSpaceCharIndex.

//********************************************************************************************
/*!
 * @brief	指定された行の最初の位置へカレットを移動する。行が表示されるようスクロールする。
 * @author	Fukushiro M.
 * @date	2014/09/11(木) 15:59:56
 *
 * @param[in]	int	lineIndex	0から始まる行インデックス。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::SetCurLineIndex (int lineIndex, bool selectLine)
{
	// Gets the starting char index of the specified line:"lineIndex".
	const auto lineTopCharIndex = LineIndex(lineIndex);
	if (!selectLine)
	{
		// 位置を選択。自動的に表示範囲にスクロールする。
		SetSel(lineTopCharIndex, lineTopCharIndex);
	}
	else
	{
		SetSel(lineTopCharIndex, lineTopCharIndex + LineLength(lineTopCharIndex));
	}
} // CFCTextEdit::SetCurLineIndex.

//********************************************************************************************
/*!
 * @brief	CurLineIndex の取得。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/16(火) 22:13:50
 *
 * @return	int	
 */
//********************************************************************************************
int CFCTextEdit::GetCurLineIndex () const
{
	// 選択範囲を取得。
	int startCharIndex;
	int endCharIndex;
	GetSel(startCharIndex, endCharIndex);
	// 選択範囲の行を取得。
	return LineFromChar(startCharIndex);
} // CFCTextEdit::GetCurLineIndex.

//********************************************************************************************
/*!
 * @brief	AddMarker 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/16(火) 21:13:06
 *
 * @param[in]	MarkerType	markerType	タイプ。
 * @param[in]	int			lineIndex	番号。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::AddMarker (MarkerType markerType, const std::string & contentName, int lineIndex)
{
	// 更新行に追加。
	if (m_contentName == contentName)
		m_vUpdatedLine.push_back(lineIndex);
	std::wstring wstr;
	GetLineText(wstr, lineIndex);

	std::map<std::pair<std::string, int>, std::wstring>* mpPoint = nullptr;
	switch (markerType)
	{
	case MARKER_BREAKPOINT:	// ブレークポイント
		mpPoint = &m_mpBreakpoint;
		break;
	case MARKER_TRACELINE:	// トレース行
		mpPoint = &m_mpTraceline;
		break;
	case MARKER_NOTIFY:		// 行の通知
		mpPoint = &m_mpNotify;
		break;
	}
	(*mpPoint)[std::make_pair(contentName, lineIndex)] = wstr;
	if (markerType == MARKER_BREAKPOINT)	// ブレークポイント
		GetParent()->SendMessage(WM_USER_BREAKPOINT_UPDATED);
} // CFCTextEdit::AddMarker.

//********************************************************************************************
/*!
 * @brief	RemoveMarker 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/16(火) 21:13:03
 *
 * @param[in]	MarkerType	markerType	タイプ。
 * @param[in]	int			lineIndex	番号。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::RemoveMarker (MarkerType markerType, const std::string & contentName, int lineIndex)
{
	// 更新行に追加。
	if (m_contentName == contentName)
		m_vUpdatedLine.push_back(lineIndex);
	std::map<std::pair<std::string, int>, std::wstring>* mpPoint = nullptr;
	switch (markerType)
	{
	case MARKER_BREAKPOINT:	// ブレークポイント
		mpPoint = &m_mpBreakpoint;
		break;
	case MARKER_TRACELINE:	// トレース行
		mpPoint = &m_mpTraceline;
		break;
	case MARKER_NOTIFY:		// 行の通知
		mpPoint = &m_mpNotify;
		break;
	}
	mpPoint->erase(std::make_pair(contentName, lineIndex));
	if (markerType == MARKER_BREAKPOINT)	// ブレークポイント
		GetParent()->SendMessage(WM_USER_BREAKPOINT_UPDATED);
} // CFCTextEdit::RemoveMarker.

//********************************************************************************************
/*!
 * @brief	Marker のクリア。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/16(火) 21:12:59
 *
 * @param[in]	MarkerType	markerType	タイプ。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::ClearMarker(MarkerType markerType, const std::string & contentName)
{
	std::map<std::pair<std::string, int>, std::wstring>* mpPoint = nullptr;
	switch (markerType)
	{
	case MARKER_BREAKPOINT:	// ブレークポイント
		mpPoint = &m_mpBreakpoint;
		break;
	case MARKER_TRACELINE:	// トレース行
		mpPoint = &m_mpTraceline;
		break;
	case MARKER_NOTIFY:		// 行の通知
		mpPoint = &m_mpNotify;
		break;
	}
	auto ibegin = mpPoint->lower_bound(std::make_pair(contentName, 0));
	auto iend = mpPoint->lower_bound(std::make_pair(contentName, INT_MAX));
	// 更新行に追加。
	if (m_contentName == contentName)
	{
		for (auto i = ibegin; i != iend; i++)
			m_vUpdatedLine.push_back(i->first.second);
	}
	mpPoint->erase(ibegin, iend);
	if (markerType == MARKER_BREAKPOINT)	// ブレークポイント
		GetParent()->SendMessage(WM_USER_BREAKPOINT_UPDATED);
} // CFCTextEdit::ClearMarker.

void CFCTextEdit::ClearMarker(MarkerType markerType)
{
	std::map<std::pair<std::string, int>, std::wstring>* mpPoint = nullptr;
	switch (markerType)
	{
	case MARKER_BREAKPOINT:	// ブレークポイント
		mpPoint = &m_mpBreakpoint;
		break;
	case MARKER_TRACELINE:	// トレース行
		mpPoint = &m_mpTraceline;
		break;
	case MARKER_NOTIFY:		// 行の通知
		mpPoint = &m_mpNotify;
		break;
	}
	for (auto & i : *mpPoint)
	{
		// 更新行に追加。
		if (m_contentName == i.first.first)
			m_vUpdatedLine.push_back(i.first.second);
	}
	mpPoint->clear();
	if (markerType == MARKER_BREAKPOINT)	// ブレークポイント
		GetParent()->SendMessage(WM_USER_BREAKPOINT_UPDATED);
} // CFCTextEdit::ClearMarker.

//********************************************************************************************
/*!
 * @brief	AllMarker のクリア。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 16:26:51
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::ClearAllMarker ()
{
	std::map<std::pair<std::string, int>, std::wstring>* mpPointArray[] = { &m_mpBreakpoint, &m_mpTraceline, &m_mpNotify };
	for (auto * mpPoint : mpPointArray)
		mpPoint->clear();
	GetParent()->SendMessage(WM_USER_BREAKPOINT_UPDATED);
} // CFCTextEdit::ClearAllMarker.

//********************************************************************************************
/*!
 * @brief	IsThereMarker 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/16(火) 21:13:22
 *
 * @param[in]	MarkerType	markerType	タイプ。
 * @param[in]	int			lineIndex	番号。
 *
 * @return	BOOL	TRUE:成 / FALSE:否
 */
//********************************************************************************************
BOOL CFCTextEdit::IsThereMarker (MarkerType markerType, const std::string & contentName, int lineIndex) const
{
	switch (markerType)
	{
	case MARKER_BREAKPOINT:	// ブレークポイント
		return (m_mpBreakpoint.find(std::make_pair(contentName, lineIndex)) != m_mpBreakpoint.end());
	case MARKER_TRACELINE:	// トレース行
		return (m_mpTraceline.find(std::make_pair(contentName, lineIndex)) != m_mpTraceline.end());
	case MARKER_NOTIFY:		// 行の通知
		return (m_mpNotify.find(std::make_pair(contentName, lineIndex)) != m_mpNotify.end());
	}
	return FALSE;
} // CFCTextEdit::IsThereMarker.

bool CFCTextEdit::IsThereMarkerRegion() const
{
	return !m_vUpdatedLine.empty();
}

//********************************************************************************************
/*!
 * @brief	RedrawMarker 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/14(日) 12:03:59
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::RedrawMarker()
{
	CClientDC dc(this);
	const CRect rtMarker = GetMarkerRect();

	// A:１行目テキストの開始Y座標。
	const CPoint pt0 = PosFromChar(LineIndex(0));
	const int textStartY = pt0.y;

	if (m_vUpdatedLine.empty())
	{
		CRgn rgn;
		rgn.CreateRectRgn(rtMarker.left, rtMarker.top, rtMarker.right, rtMarker.bottom);
		dc.SelectClipRgn(&rgn);
	}
	else
	{
		CRgn rgn0;
		rgn0.CreateRectRgn(0, 0, 0, 0);
		dc.SelectClipRgn(&rgn0);
		for (auto i : m_vUpdatedLine)
		{
			const int lineTop = textStartY + i * GetlineHeight();
			const int lineBottom = lineTop + GetlineHeight();
			const CRect rtUpdate = rtMarker & CRect(rtMarker.left, lineTop, rtMarker.right, lineBottom);
			CRgn rgn;
			rgn.CreateRectRgn(rtUpdate.left, rtUpdate.top, rtUpdate.right, rtUpdate.bottom);
			dc.SelectClipRgn(&rgn, RGN_OR);
		}
	}
	m_vUpdatedLine.clear();

	CBrush brushBack(FD_COLREF_ULTLA_LIGHT_GRAY);

	// マーカーベルトを描画。
	dc.FillRect(CRect(rtMarker.left, rtMarker.top, rtMarker.right, rtMarker.bottom), &brushBack);

	const std::map<std::pair<std::string, int>, std::wstring>* mpPointArray[] = { &m_mpBreakpoint, &m_mpTraceline, &m_mpNotify };
	const CSize iconSizeArray[] = { EDITOR_BREAKPOINT_SIZE, EDITOR_TRACELINE_SIZE, EDITOR_NOTIFY_SIZE };
	HICON hiconArray[] = { m_bIsBreakpointActivated ? m_hIconBreakpoint : m_hIconInactiveBP, m_hIconTraceline, m_hIconNotify };
	for (int j = 0; j != _countof(mpPointArray); j++)
	{
		const auto * mpPoint = mpPointArray[j];
		const auto iconSize = iconSizeArray[j];
		auto hicon = hiconArray[j];

		auto ibegin = mpPoint->lower_bound(std::make_pair(m_contentName, 0));
		auto iend = mpPoint->lower_bound(std::make_pair(m_contentName, INT_MAX));

		for (auto i = ibegin; i != iend; i++)
		{
			const int lineTop = textStartY + i->first.second * GetlineHeight();
			const int lineBottom = lineTop + GetlineHeight();
			if (lineBottom < rtMarker.top) continue;
			if (rtMarker.bottom < lineTop) break;
			const int lineMiddle = (lineTop + lineBottom) / 2;
			::DrawIconEx(dc.m_hDC,
				rtMarker.CenterPoint().x - iconSize.cx / 2,
				lineMiddle - iconSize.cy / 2,
				hicon,
				iconSize.cx,
				iconSize.cy,
				0, NULL, DI_NORMAL);
		}
	}
} // CFCTextEdit::RedrawMarker.

//********************************************************************************************
/*!
 * @brief	CorrectMarker 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 16:09:53
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::CorrectMarker ()
{
	BOOL isMarkerMoved = FALSE;
	const int lineCount = GetLineCount();
	std::map<std::pair<std::string, int>, std::wstring>* mpPointArray[] = { &m_mpBreakpoint, &m_mpTraceline, &m_mpNotify };
	for (auto * mpPoint : mpPointArray)
	{
		auto ibegin = mpPoint->lower_bound(std::make_pair(m_contentName, 0));
		auto iend = mpPoint->lower_bound(std::make_pair(m_contentName, INT_MAX));
		int diff = 0;
		std::map<std::pair<std::string, int>, std::wstring> mpNewPoint;
		for (auto i = ibegin; i != iend; i++)
		{
			int newLineIndex = -1;
			// Search the same text line like the following.
			// odr|
			//----|-----------------------------------------
			//	7 |	const int lineTop = textStartY + i->first.second * GetlineHeight();
			//	5 |	const int lineBottom = lineTop + GetlineHeight();
			//	3 |	if (lineBottom < rtMarker.top) continue;
			//	1 |	if (rtMarker.bottom < lineTop) break;
			//	2 |	const int lineMiddle = (lineTop + lineBottom) / 2;
			//	4 |	if (rtMarker.bottom < lineTop) break;
			//	6 |	const int lineMiddle = (lineTop + lineBottom) / 2;
			//----------------------------------------------

			// 0,1,-1,2,-2,3,-3… のように+-で広がるようにしたい。
			// 整数I(1,2,3,4…)をループし、符号をI%2=0(正)/1(負)、数値をI/2とすると
			// 上記の+-で広がる数列が得られる。
			for (int j = 1; j != 40; j++)
			{
				const int k = (j % 2 == 0) ? j / 2 : -j / 2;
				const int tmpIndex = i->first.second + diff + k;		// (A)マーカー検索の初期値として差分(diff)を加算。
				if (0 <= tmpIndex && tmpIndex < lineCount)
				{
					// Search the same text line.
					std::wstring lineText;
					GetLineText(lineText, tmpIndex);
					if (i->second == lineText)
					{
						newLineIndex = tmpIndex;
						break;
					}
				}
			}
			if (newLineIndex == -1)
			{	//----- if the same text line was not found ------
				isMarkerMoved = TRUE;
			} else
			{	//----- if the same text line was found ------
				// 移動後行と移動前行の差分。差分が＋１の場合、次のマーカーも＋１の行に移動している可能性が高い。
				// このため、差分を次のマーカーの検索の初期値(A)に使う。
				mpNewPoint[std::make_pair(m_contentName, newLineIndex)] = i->second;
				diff = newLineIndex - i->first.second;
				//----- if the same text line was at the different line number ------
				if (diff != 0)
					isMarkerMoved = TRUE;
			}
		}
		mpPoint->erase(ibegin, iend);
		mpPoint->insert(mpNewPoint.begin(), mpNewPoint.end());
	}
	if (isMarkerMoved)
		RedrawMarker();
	GetParent()->SendMessage(WM_USER_BREAKPOINT_UPDATED);
} // CFCTextEdit::CorrectMarker.

//********************************************************************************************
/*!
 * @brief	ActivateBreakpoint 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/23(火) 08:29:26
 *
 * @param[in]	BOOL	isActivate	。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::ActivateBreakpoint (BOOL isActivate)
{
	m_bIsBreakpointActivated = isActivate;
} // CFCTextEdit::ActivateBreakpoint.

//********************************************************************************************
/*!
 * @brief	MarkerStream の出力。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/17(水) 22:55:52
 *
 * @param[in,out]	std::wstring&	wstrStream	文字列。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::OutMarkerStream (std::wstring& wstrStream) const
{
	const std::map<std::pair<std::string, int>, std::wstring>* mpMarkerArray[] = { &m_mpBreakpoint, &m_mpTraceline, &m_mpNotify };
	for (const auto * mpMarker : mpMarkerArray)
	{
		// 文字列をつないで一つの文字列にする。
		UtilStr::AppendDWORD(wstrStream, (DWORD)mpMarker->size());
		for (const auto & i : *mpMarker)
		{
			UtilStr::AppendString(wstrStream, Astrwstr::astr_to_wstr((std::wstring&)std::wstring{},  i.first.first).c_str());
			UtilStr::AppendDWORD(wstrStream, (DWORD)i.first.second);
			UtilStr::AppendString(wstrStream, i.second.c_str());
		}
	}
} // CFCTextEdit::OutMarkerStream.

//********************************************************************************************
/*!
 * @brief	InMarkerStream 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/17(水) 22:55:50
 *
 * @param[in,out]	std::wstring&	wstrStream	文字列。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::InMarkerStream (std::wstring& wstrStream)
{
	std::map<std::pair<std::string, int>, std::wstring>* mpMarkerArray[] = { &m_mpBreakpoint, &m_mpTraceline, &m_mpNotify };
	for (auto * mpMarker : mpMarkerArray)
	{
		mpMarker->clear();
		const int markersCount = (int)UtilStr::ExtractDWORD(wstrStream);
		for (int i = 0; i != markersCount; i++)
		{
			const std::string contentName = Astrwstr::wstr_to_astr((std::string&)std::string{}, UtilStr::ExtractString(wstrStream));
			const int lineIndex = (int)UtilStr::ExtractDWORD(wstrStream);
			const std::wstring lineText = UtilStr::ExtractString(wstrStream);
			(*mpMarker)[std::make_pair(contentName, lineIndex)] = lineText;
		}
	}
	GetParent()->SendMessage(WM_USER_BREAKPOINT_UPDATED);
} // CFCTextEdit::InMarkerStream.

void CFCTextEdit::SetContentName(const std::string & name)
{
	m_contentName = name;
}

const std::string & CFCTextEdit::GetContentName() const
{
	return m_contentName;
}

const std::map<std::pair<std::string, int>, std::wstring>& CFCTextEdit::GetBreakpoint() const
{
	return m_mpBreakpoint;
}

BOOL CFCTextEdit::SearchForward (const std::wstring & keyword)
{
	std::wstring text;
	UtilWin::GetWindowText(this, text);
	// 選択範囲を記録。
	int startCharIndex;
	int endCharIndex;
	GetSel(startCharIndex, endCharIndex);
	auto pos = std::wstring::npos;
	for (int i = 0; i != 2; i++)
	{
		pos = text.find(keyword, endCharIndex);
		if (pos != std::wstring::npos)
			break;
		endCharIndex = 0;
	}
	if (pos != std::wstring::npos)
	{
		SetSel((int)pos, (int)(pos + keyword.length()));
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // CFCTextEdit::SearchForward.

BOOL CFCTextEdit::SearchBackward (const std::wstring & keyword)
{
	std::wstring text;
	UtilWin::GetWindowText(this, text);
	// 選択範囲を記録。
	int startCharIndex;
	int endCharIndex;
	GetSel(startCharIndex, endCharIndex);
	startCharIndex--;
	if (startCharIndex == -1)
		startCharIndex = text.length();
	auto pos = std::wstring::npos;
	for (int i = 0; i != 2; i++)
	{
		pos = text.rfind(keyword, startCharIndex);
		if (pos != std::wstring::npos)
			break;
		startCharIndex = text.length();
	}
	if (pos != std::wstring::npos)
	{
		SetSel((int)pos, (int)(pos + keyword.length()));
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // CFCTextEdit::SearchBackward.

//********************************************************************************************
/*!
 * @brief	LineText の取得。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/17(水) 22:55:41
 *
 * @param[in,out]	std::wstring&	text		。
 * @param[in]		int			lineIndex	番号。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::GetLineText (std::wstring& text, int lineIndex) const
{
	// 選択範囲の行を取得。
	const int len = LineLength(LineIndex(lineIndex));
	std::vector<wchar_t> vBuff(len + 1, L'\0');
	GetLine(lineIndex, vBuff.data(), vBuff.size());
	text = vBuff.data();
} // CFCTextEdit::GetLineText.

//********************************************************************************************
/*!
 * @brief	MarkerRect の取得。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/15(月) 14:19:12
 *
 * @return	CRect	
 */
//********************************************************************************************
CRect CFCTextEdit::GetMarkerRect () const
{
	CRect rtClient;
	GetClientRect(rtClient);
	return CRect(rtClient.left, rtClient.top, rtClient.left + EDITOR_MARKER_BELT_WIDTH, rtClient.bottom);
} // CFCTextEdit::GetMarkerRect

//********************************************************************************************
/*!
 * @brief	YToLineIndex 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/15(月) 14:38:48
 *
 * @param[in]	int	y	。★★★ Caution!! 可読性を高めるために１文字の変数名は避けて下さい ★★★
 *
 * @return	int	
 */
//********************************************************************************************
int CFCTextEdit::YToLineIndex (int y)
{
	const CPoint pt0 = PosFromChar(LineIndex(0));
	// マウス左ボタンダウン時の行インデックス。
	return (y - pt0.y) / GetlineHeight();
} // CFCTextEdit::YToLineIndex

BEGIN_MESSAGE_MAP(CFCTextEdit, BASE_CLASS)
	ON_MESSAGE(WM_USER_TEXT_EDIT_INPUT_TAB, &CFCTextEdit::OnUserTextEditInputTab)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_CONTROL_REFLECT_EX(EN_UPDATE, &CFCTextEdit::OnUpdate)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CFCTextEdit メッセージ ハンドラー

//********************************************************************************************
/*!
 * @brief	PreTranslateMessage 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/17(水) 19:59:37
 *
 * @param[in,out]	MSG*	pMsg	。
 *
 * @return	BOOL	TRUE:成 / FALSE:否
 */
//********************************************************************************************
BOOL CFCTextEdit::PreTranslateMessage (MSG* pMsg)
{
	// TRACE(L"CFCTextEdit::PreTranslateMessage %x %x %x\n", pMsg->message, pMsg->wParam, pMsg->lParam);

	// 選択範囲を取得。
	int startCharIndex;
	int endCharIndex;
	GetSel(startCharIndex, endCharIndex);
	if (m_iCurCharIndex != startCharIndex)
	{
		// 現在位置。
		m_iCurCharIndex = startCharIndex;
		// CFCTextEditの現在行の移動。wParam=ControlId, lParam=lineIndex。
		GetParent()->PostMessage(WM_USER_TEXT_EDIT_CURLINE_CHANGED, (WPARAM)GetDlgCtrlID(), (LPARAM)LineFromChar(startCharIndex));
	}

	switch (pMsg->message)
	{
	case WM_KEYDOWN:	// キーを押す。
		switch (pMsg->wParam)
		{
		case VK_TAB:	// Tab
			if (!IsReadOnly())
			{
				PostMessage(WM_USER_TEXT_EDIT_INPUT_TAB);
				return TRUE;
			}
			break;
		}
		break;
	}

	return BASE_CLASS::PreTranslateMessage(pMsg);
} // BOOL CFCTextEdit::PreTranslateMessage (MSG* pMsg)

//********************************************************************************************
/*!
 * @brief	WM_USER_TEXT_EDIT_INPUT_TABのメッセージハンドラー。タブキーが押されたときに実行される。
 * @author	Fukushiro M.
 * @date	2014/08/16(土) 11:02:03
 *
 * @param[in]		WPARAM		未使用
 * @param[in,out]	LPARAM		未使用
 *
 * @return	LRESULT	1
 */
//********************************************************************************************
LRESULT CFCTextEdit::OnUserTextEditInputTab (WPARAM, LPARAM)
{
	// 選択範囲を取得。
	int startCharIndex;
	int endCharIndex;
	GetSel(startCharIndex, endCharIndex);
	// 選択範囲の行を取得。
	const int startLineIndex = LineFromChar(startCharIndex);
	const int lastLineIndex = LineFromChar(endCharIndex);

	if (startLineIndex == lastLineIndex)
	{	//----- 選択範囲が１行に収まる場合 -----
		// 選択範囲が１行に収まる場合は、タブ文字を挿入する。

		if (m_isUseSpacesAsTab)
		{
			// startCharIndex のタブ位置を求める。
			int tabIndex;	// タブインデックス
			int restCount;	// タブ余り文字数
			CharIndexToTabPosition(tabIndex, restCount, startCharIndex);
			// 空白数を計算。
			int spaceCount = m_tabSize - restCount;
			// 空白を挿入。
			ReplaceSel(CString(L' ', spaceCount), TRUE);
		}
		else
		{
			ReplaceSel(L"\t", TRUE);
		}
	}
	else
	{	//----- 選択範囲が複数行に渡る場合 -----
		// 選択範囲が複数行に渡る場合は、範囲の先頭にインデントを入れる。SHIFTキーを押している場合はインデントを減らす。

		// 選択範囲の最後が行の先頭だった場合、その行はインデントに含めない。
		// 下図の場合、インデント対象は5行目～6行目。7行目は対象とはならない。
		//
		// ※ __ は選択範囲
		//      ______________________
		// 5行   あ い う え お か \n
		//       10 11 12 13 14 15 16
		//      ______________________
		// 6行   く け こ さ し す \n
		//       17 18 19 20 21 22 23
		//      _
		// 7行   そ あ い う え お \n
		//       24 25 26 27 28 29 30
		//

		// 選択範囲の最後が行の先頭だった場合、その行はインデントに含めない。
		const BOOL isEndSelectionTop = (LineIndex(lastLineIndex) == endCharIndex);
		const int endLineIndex = isEndSelectionTop ? lastLineIndex : lastLineIndex + 1;
		for (int lineIndex = startLineIndex; lineIndex != endLineIndex; lineIndex++)
		{
			// 行 lineIndex の最初の文字のインデックスを求める。
			const int iCharIndex0 = LineIndex(lineIndex);

			// シフトキーの場合はインデントを減らす。
			if (GetKeyState(VK_SHIFT) < 0)
			{
				int spaceCount = 0;

				if (GetText()[iCharIndex0] == L'\t')
				{
					spaceCount = 1;
				}
				else if (GetText()[iCharIndex0] == L' ')
				{
					for (spaceCount = 1; spaceCount != m_tabSize && GetText()[iCharIndex0 + spaceCount] == L' '; spaceCount++);
				}
				// 行の最初の空白を削除。
				SetSel(iCharIndex0, iCharIndex0 + spaceCount);
				ReplaceSel(L"", TRUE);
			}
			else
			{
				// 行の最初に空白を挿入。
				SetSel(iCharIndex0, iCharIndex0);
				if (m_isUseSpacesAsTab)
					ReplaceSel(CString(L' ', m_tabSize), TRUE);
				else
					ReplaceSel(L"\t", TRUE);
			}
		}

		// 新しい選択範囲の最初を計算。
		const int startSelection = LineIndex(startLineIndex);

		// 新しい選択範囲の最後を計算。
		// 注意！ LineLengthの引数は、CEdit::LineLengthのマニュアルでは行インデックスだが、実際は文字インデックス。
		const int endSelection = isEndSelectionTop ? LineIndex(lastLineIndex) : LineIndex(lastLineIndex) + LineLength(LineIndex(lastLineIndex));
		SetSel(startSelection, endSelection);
	}
	return 1;
} // CFCTextEdit::OnUserTextEditInputTab.


//----- 14.09.10 Fukushiro M. 追加始 ()-----

//********************************************************************************************
/*!
 * @brief	SubclassWindow関数の処理直前に実行される。SubclassWindow関数は
 *			DDX_Controlによって実行されるため、ダイアログでの動的割り当て
 *			のときにOnCreateの代わりに初期化処理を行う。
 *
 * @author	Fukushiro M.
 * @date	2014/09/10(水) 15:24:36
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::PreSubclassWindow ()
{
	// アイコンロード。
	if (m_hIconNotify == NULL)
	{
		m_hIconBreakpoint = AfxGetApp()->LoadIcon(IDI_EDITOR_BREAKPOINT);
		m_hIconInactiveBP = AfxGetApp()->LoadIcon(IDI_EDITOR_INACTIVE_BREAKPOINT);
		m_hIconTraceline = AfxGetApp()->LoadIcon(IDI_EDITOR_TRACELINE);
		m_hIconNotify = AfxGetApp()->LoadIcon(IDI_EDITOR_NOTIFY);
	}
	// Baseclass関数実行。
	BASE_CLASS::PreSubclassWindow();
} // CFCTextEdit::PreSubclassWindow

//********************************************************************************************
/*!
 * @brief	OnCreate 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/22(月) 16:46:01
 *
 * @param[in,out]	LPCREATESTRUCT	cs	。
 *
 * @return	int	
 */
//********************************************************************************************
int CFCTextEdit::OnCreate (LPCREATESTRUCT cs)
{
	// Baseclass関数実行。
	if (BASE_CLASS::OnCreate(cs) == -1)
		return -1;
	// アイコンロード。
	if (m_hIconNotify == NULL)
	{
		m_hIconBreakpoint = AfxGetApp()->LoadIcon(IDI_EDITOR_BREAKPOINT);
		m_hIconInactiveBP = AfxGetApp()->LoadIcon(IDI_EDITOR_INACTIVE_BREAKPOINT);
		m_hIconTraceline = AfxGetApp()->LoadIcon(IDI_EDITOR_TRACELINE);
		m_hIconNotify = AfxGetApp()->LoadIcon(IDI_EDITOR_NOTIFY);
	}
	return 0;
} // CFCTextEdit::OnCreate

//********************************************************************************************
/*!
 * @brief	OnPaint 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/14(日) 09:44:16
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::OnPaint ()
{
	// Baseclass関数実行。
	BASE_CLASS::OnPaint();
	// マーカーを描画。
	RedrawMarker();
} // CFCTextEdit::OnPaint

//********************************************************************************************
/*!
 * @brief	OnSize 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/14(日) 22:28:56
 *
 * @param[in]	UINT	nType	タイプ。
 * @param[in]	int		cx		。
 * @param[in]	int		cy		。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::OnSize (UINT nType, int cx, int cy)
{
	// Superclass関数実行。
	BASE_CLASS::OnSize(nType, cx, cy);
	// テキスト領域を更新。
	UpdateTextRect();
} // CFCTextEdit::OnSize.

//********************************************************************************************
/*!
 * @brief	OnUpdate 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/14(日) 22:28:58
 *
 * @return	BOOL	TRUE:成 / FALSE:否
 */
//********************************************************************************************
BOOL CFCTextEdit::OnUpdate ()
{
	// マーカー位置を補正。補正の場合は再描画。
	CorrectMarker();
	// Baseclass関数実行。
	return BASE_CLASS::OnUpdate();
} // CFCTextEdit::OnUpdate.

//********************************************************************************************
/*!
 * @brief	OnLButtonDown 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/15(月) 14:12:25
 *
 * @param[in]	UINT	nFlags	。
 * @param[in]	CPoint	point	。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::OnLButtonDown (UINT nFlags, CPoint point)
{
	const CRect rtMarker = GetMarkerRect();
	if (rtMarker.PtInRect(point))
	{
		// マウス左ボタンダウン時の行インデックス。
		m_iLBtnDownLine = YToLineIndex(point.y);

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	} else
	{
		// マウス左ボタンダウン時の行インデックス。
		m_iLBtnDownLine = -1;
		BASE_CLASS::OnLButtonDown(nFlags, point);	// Baseclass関数実行。
	}
} // CFCTextEdit::OnLButtonDown

//********************************************************************************************
/*!
 * @brief	OnLButtonUp 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/15(月) 14:24:03
 *
 * @param[in]	UINT	nFlags	。
 * @param[in]	CPoint	point	。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::OnLButtonUp (UINT nFlags, CPoint point)
{
	const CRect rtMarker = GetMarkerRect();
	// マウス左ボタンダウン時の行インデックス。
	if (m_iLBtnDownLine != -1)
	{
		if (rtMarker.PtInRect(point))
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		if (m_iLBtnDownLine < GetLineCount() && m_iLBtnDownLine == YToLineIndex(point.y))
		{
			// CFCTextEditのマーカー領域クリック。wParam=lineIndex。
			GetParent()->PostMessage(WM_USER_TEXT_EDIT_MARKER_CLICKED, (WPARAM)GetDlgCtrlID(), (LPARAM)m_iLBtnDownLine);
		}
		m_iLBtnDownLine = -1;
	} else
	{
		if (rtMarker.PtInRect(point))
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		BASE_CLASS::OnLButtonUp(nFlags, point);	// Baseclass関数実行。
	}
} // CFCTextEdit::OnLButtonUp

//********************************************************************************************
/*!
 * @brief	OnMouseMove 関数。
 *
 *
 * @author	Fukushiro M.
 * @date	2014/09/15(月) 14:15:38
 *
 * @param[in]	UINT	nFlags	。
 * @param[in]	CPoint	dvPos	位置。
 *
 * @return	なし (none)
 */
//********************************************************************************************
void CFCTextEdit::OnMouseMove (UINT nFlags, CPoint point)
{
	// Baseclass関数実行。
	BASE_CLASS::OnMouseMove(nFlags, point);

	const CRect rtMarker = GetMarkerRect();
	if (rtMarker.PtInRect(point))
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
} // CFCTextEdit::OnMouseMove

LRESULT CFCTextEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	auto result = BASE_CLASS::WindowProc(message, wParam, lParam);
	if (message == WM_SETFONT)
		UpdateTextRect();
	return result;
}
