#pragma once

/*************************************************************************
 * <クラス>	FCEditDraw
 *
 * <解説>	CDCを修正線用に属性設定＆属性復帰する。
 *
 * <履歴>	00.01.12 Fukushiro M. 作成
 *************************************************************************/
class FCEditDraw
{
public:
	FCEditDraw (CDC* pDC,
				long lgPenW,
				COLORREF col,
				long lPenStyle,
				long lgOffsetX,
				long lgOffsetY);

protected:
	void	Initialize (CDC* pDC,
						long lgPenW,
						COLORREF col,
						long lPenStyle,
						long lgOffsetX,
						long lgOffsetY);

public:

	~FCEditDraw ();
public:
	void	Polyline (const CPoint* lpPoints, int nCount);
	void	Line (	const CPoint* pPoint0, const CPoint* pPoint1,
					const CPoint* pPoint2 = NULL, const CPoint* pPoint3 = NULL,
					const CPoint* pPoint4 = NULL, const CPoint* pPoint5 = NULL,
					const CPoint* pPoint6 = NULL, const CPoint* pPoint7 = NULL,
					const CPoint* pPoint8 = NULL, const CPoint* pPoint9 = NULL);
	void	MoveTo (const CPoint& lgpt);
	void	LineTo (const CPoint& lgpt);

protected:
	CDC* 	m_pDC;
	long	m_lgOffsetX;	// スクロールオフセット値。
	long	m_lgOffsetY;	// スクロールオフセット値。
	CPen	m_pen;
	CBrush	m_brush;
	int		m_rop2Old;
	CPen*	m_pPenOld;
	CBrush*	m_pBrushOld;

	CPoint	m_lgptLastMoveTo;
}; // class FCEditDraw.

