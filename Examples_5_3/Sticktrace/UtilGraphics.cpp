#include "stdafx.h"

#include "UtilGraphics.h"	// This header.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*************************************************************************
 * <関数>	FCEditDraw::FCEditDraw
 *
 * <機能>	コンストラクタ。
 *
 * <引数>	pDC		:描画用デバイスコンテキストを指定。
 *			lgPenW	:ペンの幅を指定。
 *			col		:表示させる色を指定。背景の反転色になることを考慮して指定。
 *			lPenStyle:ペンのスタイルを指定。
 *					 PS_SOLID/PS_DASH/PS_DOT/PS_DASHDOT/PS_DASHDOTDOT から指定。
 *			lgOffsetX, lgOFfsetY :描画時の座標のオフセット値を指定。
 *
 * <履歴>	05.02.02 Fukushiro M. 作成
 *************************************************************************/
FCEditDraw::FCEditDraw (CDC* pDC, long lgPenW, COLORREF col, long lPenStyle,
						long lgOffsetX, long lgOffsetY)
{
	// オブジェクトを初期化。
	Initialize(pDC, lgPenW, col, lPenStyle, lgOffsetX, lgOffsetY);
} // FCEditDraw::FCEditDraw.

/*************************************************************************
 * <関数>	FCEditDraw::Initialize
 *
 * <機能>	初期化する。コンストラクタで実行。
 *
 * <引数>	pDC		:描画用デバイスコンテキストを指定。
 *			lgPenW	:ペンの幅を指定。
 *			col		:表示させる色を指定。背景の反転色になることを考慮して指定。
 *			lPenStyle:ペンのスタイルを指定。
 *					 PS_SOLID/PS_DASH/PS_DOT/PS_DASHDOT/PS_DASHDOTDOT から指定。
 *			lgOffsetX, lgOFfsetY :描画時の座標のオフセット値を指定。
 *
 * <履歴>	05.02.02 Fukushiro M. 作成
 *************************************************************************/
void FCEditDraw::Initialize (	CDC* pDC,
								long lgPenW,
								COLORREF col,
								long lPenStyle,
								long lgOffsetX,
								long lgOffsetY)
{
	if (col == COLORREF(-1))
		col = RGB(255, 0, 0) ^ RGB(255, 255, 255);
	if (lPenStyle == -1)
		lPenStyle = PS_SOLID;
	m_pDC = pDC;
	m_brush.CreateSolidBrush(col);
	m_lgOffsetX = lgOffsetX;
	m_lgOffsetY = lgOffsetY;
	// Penを作成。(Style,Width,Color)の3引数を取るCreatePenでは、
	// NVIDIAのビデオカードなどでゴミが残る場合がある。
	LOGBRUSH logbPen = { BS_SOLID, col, 0 };
	//----- ペンの設定 -----
	m_pen.CreatePen(PS_GEOMETRIC | lPenStyle | PS_ENDCAP_FLAT | PS_JOIN_BEVEL, lgPenW, &logbPen);
	//----- pDCの描画属性設定 -----
	m_rop2Old = m_pDC->SetROP2(R2_XORPEN);
	m_pPenOld = m_pDC->SelectObject(&m_pen);
	m_pBrushOld = m_pDC->SelectObject(&m_brush);
} // FCEditDraw::Initialize.

/*************************************************************************
 * <関数>	FCEditDraw::~FCEditDraw
 *
 * <機能>	デストラクタ。
 *
 * <履歴>	00.01.12 Fukushiro M. 作成
 *************************************************************************/
FCEditDraw::~FCEditDraw ()
{
	//----- pDCの描画属性を戻す -----
	m_pDC->SelectObject(m_pBrushOld);
	m_pDC->SelectObject(m_pPenOld);
	m_pDC->SetROP2(m_rop2Old);
} // FCEditDraw::~FCEditDraw.

/*************************************************************************
 * <関数>	FCEditDraw::Polyline
 *
 * <機能>	多角線を引く。
 *
 * <引数>	lpPoints	:多角線の頂点座標配列を指定。LONG_MAXで区切られた
 *						 複数の多角線でも描画できる。
 *			nCount		:lpPointsの配列サイズを指定。
 *
 * <解説>	WinNT/Win2000では、幅0/1の線を引くときに、BeginPath/EndPath
 *			を指定しないと、XORで２回 直線を引いた時微妙に異なる線を引く。
 *
 * <履歴>	00.04.09 Fukushiro M. 作成
 *************************************************************************/
void FCEditDraw::Polyline (const CPoint* lpPoints, int nCount)
{
	m_pDC->BeginPath();
	for (long lC = 0; lC < nCount; lC++)
	{
		if (lpPoints[lC].x != LONG_MAX)
		{
			// A3横長,余白0に設定し、用紙いっぱいに大きい四角で表示が乱れるのを防ぐため、
			// mylgx2dvx、mylgy2dvy中でFFLimitCoord で値を制限する。
			MoveTo(lpPoints[lC]);
			for (lC++; lC != nCount && lpPoints[lC].x != LONG_MAX; lC++)
				LineTo(lpPoints[lC]);
		}
	}
	m_pDC->EndPath();
	m_pDC->StrokePath();
} // FCEditDraw::Polyline.

/*************************************************************************
 * <関数>	FCEditDraw::Line
 *
 * <機能>	直線を引く。
 *
 * <引数>	pPoint0	:直線の第１座標を指定。
 *			pPoint1	:直線の第２座標を指定。
 *			pPoint3	:直線の第３座標を指定。・・・・
 *
 * <履歴>	00.04.09 Fukushiro M. 作成
 *************************************************************************/
void FCEditDraw::Line (	const CPoint* pPoint0, const CPoint* pPoint1,
						const CPoint* pPoint2, const CPoint* pPoint3,
						const CPoint* pPoint4, const CPoint* pPoint5,
						const CPoint* pPoint6, const CPoint* pPoint7,
						const CPoint* pPoint8, const CPoint* pPoint9)
{
	std::vector<CPoint> vlgptPoly;
do {
	vlgptPoly.push_back(*pPoint0);
	vlgptPoly.push_back(*pPoint1);
	if (pPoint2 == NULL) break;
	vlgptPoly.push_back(*pPoint2);
	if (pPoint3 == NULL) break;
	vlgptPoly.push_back(*pPoint3);
	if (pPoint4 == NULL) break;
	vlgptPoly.push_back(*pPoint4);
	if (pPoint5 == NULL) break;
	vlgptPoly.push_back(*pPoint5);
	if (pPoint6 == NULL) break;
	vlgptPoly.push_back(*pPoint6);
	if (pPoint7 == NULL) break;
	vlgptPoly.push_back(*pPoint7);
	if (pPoint8 == NULL) break;
	vlgptPoly.push_back(*pPoint8);
	if (pPoint9 == NULL) break;
	vlgptPoly.push_back(*pPoint9);
} while (FALSE);
	Polyline(vlgptPoly.data(), (int)vlgptPoly.size());
} // FCEditDraw::Line.

void FCEditDraw::MoveTo(const CPoint & lgpt)
{
	const auto x = lgpt.x - m_lgOffsetX;
	const auto y = lgpt.y - m_lgOffsetY;
	m_pDC->MoveTo(x, y);
}

void FCEditDraw::LineTo(const CPoint & lgpt)
{
	const auto x = lgpt.x - m_lgOffsetX;
	const auto y = lgpt.y - m_lgOffsetY;
	m_pDC->LineTo(x, y);
}

