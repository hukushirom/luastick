#pragma once

#include "DdEdit.h"	// Baseclass.

#ifdef SUPER_CLASS
#undef SUPER_CLASS
#endif
#define SUPER_CLASS CFCDdEdit

// CFCTextEdit

//********************************************************************************************
/*!
 * @class	CFCTextEdit
 * @brief	CEditの拡張。簡易テキストエディタとして使えるよう、TABキーでインデント等が設定できる。
 * @author	Fukushiro M.
 * @date	2014/08/16(土) 10:38:44
 */
//********************************************************************************************
class CFCTextEdit : public CFCDdEdit
{
protected:
	using BASE_CLASS = CFCDdEdit;

	DECLARE_DYNAMIC(CFCTextEdit)
public:
	enum MarkerType
	{
		MARKER_NONE = 0,
		MARKER_BREAKPOINT,	// ブレークポイント
		MARKER_TRACELINE,	// トレース行
		MARKER_NOTIFY,		// 行の通知
	};

public:
	CFCTextEdit ();
	virtual ~CFCTextEdit ();
	virtual void SetTabSize (int tabSize);
	virtual void UseSpacesAsTab (bool useSpaces);

	virtual void UpdateTextRect ();

	virtual void CharIndexToTabPosition (int& tabIndex, int& restCount, int charIndex) const;
	virtual int FindNoSpaceCharIndex (int charIndex) const;
	virtual void SetCurLineIndex (int lineIndex, bool selectLine);
	virtual int GetCurLineIndex () const;

	virtual void AddMarker (MarkerType markerType, const std::string & name, int lineIndex);
	virtual void RemoveMarker (MarkerType markerType, const std::string & name, int lineIndex);
	virtual void ClearMarker (MarkerType markerType, const std::string & contentName);
	virtual void ClearMarker (MarkerType markerType);
	virtual void ClearAllMarker ();
	virtual BOOL IsThereMarker (MarkerType markerType, const std::string & name, int lineIndex) const;
	virtual bool IsThereMarkerRegion() const;
	virtual void RedrawMarker ();
	virtual void CorrectMarker ();
	virtual void ActivateBreakpoint (BOOL isActivate);
	virtual void OutMarkerStream (std::wstring& wstrStream) const;
	virtual void InMarkerStream (std::wstring& wstrStream);
	virtual void SetContentName(const std::string& name);
	virtual const std::string& GetContentName() const;
	virtual const std::map<std::pair<std::string, int>, std::wstring>& GetBreakpoint () const;
	virtual BOOL SearchForward (const std::wstring & keyword);
	virtual BOOL SearchBackward (const std::wstring & keyword);

protected:
	virtual void GetLineText (std::wstring& text, int lineIndex) const;
	virtual CRect GetMarkerRect () const;
	virtual int YToLineIndex (int y);

public:
	virtual BOOL PreTranslateMessage (MSG* pMsg);

protected:
	int		m_tabSize;			// Tabの半角換算の文字数。
	bool	m_isUseSpacesAsTab;	// Use space chars instead of tab char.
	int		m_iCurCharIndex;	// 現在位置。
	int		m_iLBtnDownLine;	// マウス左ボタンダウン時の行インデックス。

	std::string m_contentName;
	std::map<std::pair<std::string, int>, std::wstring>	m_mpBreakpoint;		// ブレークポイント（行インデックス->行テキスト）
	std::map<std::pair<std::string, int>, std::wstring>	m_mpTraceline;		// トレースポイント（行インデックス->行テキスト）
	std::map<std::pair<std::string, int>, std::wstring>	m_mpNotify;			// 通知ポイント（行インデックス->行テキスト）

	BOOL		m_bIsBreakpointActivated;		// ブレークポイントのアクティブ状態
	HICON		m_hIconBreakpoint;
	HICON		m_hIconInactiveBP;
	HICON		m_hIconTraceline;
	HICON		m_hIconNotify;

	std::vector<int>	m_vUpdatedLine;			// マーカー変更時の再描画対象行。

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUserTextEditInputTab (WPARAM, LPARAM);
	virtual void PreSubclassWindow ();
	afx_msg int OnCreate (LPCREATESTRUCT cs);
	afx_msg void OnPaint ();
	afx_msg void OnSize (UINT nType, int cx, int cy);
	afx_msg BOOL OnUpdate ();
	afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
	afx_msg void OnMouseMove (UINT nFlags, CPoint point);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
}; // class CFCTextEdit : public SUPER_CLASS


