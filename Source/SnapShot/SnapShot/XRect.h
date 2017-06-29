#pragma once


class XRect :public RECT
{
public:
	XRect(void);
	~XRect(void);

	/*LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;*/
	 BOOL IsRectEmpty() const throw();
	  BOOL IsRectNull() const throw();
	LONG Width() const;
	LONG Height() const;
	void SetRect(int left,int top,int right,int bottom);
	void NormalizeRect() throw();
	void InflateRect(_In_ LPCRECT lpRect) throw();
	void InflateRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw();
	void InflateRect(
		_In_ int x,
		_In_ int y) throw();
	void InflateRect(_In_ SIZE size) throw();
	BOOL PtInRect(_In_ POINT point) const throw();
	BOOL EqualRect(_In_ LPCRECT lpRect) const throw();
	void CopyRect(_In_ LPCRECT lpSrcRect) throw();
	void SetRectEmpty() throw();
	void operator=(_In_ const RECT& srXRect) throw();
	BOOL operator==(_In_ const RECT& rect) const throw();
	BOOL operator!=(_In_ const RECT& rect) const throw();
	void operator+=(_In_ POINT point) throw();
	void operator+=(_In_ SIZE size) throw();
	void operator-=(_In_ POINT point) throw();
	void operator-=(_In_ SIZE size) throw();
	void operator&=(_In_ const RECT& rect) throw();
	void operator|=(_In_ const RECT& rect) throw();
	XRect operator+(_In_ POINT pt) const throw();
	XRect operator-(_In_ POINT pt) const throw();
	XRect operator+(_In_ SIZE size) const throw();
	XRect operator&(_In_ const RECT& rect2) const throw();
	XRect operator|(_In_ const RECT& rect2) const throw();
};
