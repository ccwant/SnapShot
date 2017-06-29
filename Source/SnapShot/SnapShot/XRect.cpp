#include "stdafx.h"
#include "XRect.h"


XRect::XRect(void)
{
}


XRect::~XRect(void)
{
}
 BOOL XRect::IsRectEmpty() const throw()
{
	return ::IsRectEmpty(this);
}

 BOOL XRect::IsRectNull() const throw()
{
	return (left == 0 && right == 0 && top == 0 && bottom == 0);
}
void XRect::SetRect(int left,int top,int right,int bottom)
{
	this->left=left;
	this->top=top;
	this->right=right;
	this->bottom=bottom;
}
LONG XRect::Width() const
{
	return abs(left - right);
}
LONG XRect::Height() const
{
	return abs(top - bottom);
}
 void XRect::SetRectEmpty() throw()
{
	::SetRectEmpty(this);
}

 void XRect::CopyRect(_In_ LPCRECT lpSrcRect) throw()
{
	::CopyRect(this, lpSrcRect);
}

 BOOL XRect::EqualRect(_In_ LPCRECT lpRect) const throw()
{
	return ::EqualRect(this, lpRect);
}

void XRect::NormalizeRect() throw()
{

	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}
	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}
void XRect::InflateRect(_In_ LPCRECT lpRect) throw()
{
	left -= lpRect->left;
	top -= lpRect->top;
	right += lpRect->right;
	bottom += lpRect->bottom;
}

void XRect::InflateRect(
	_In_ int l,
	_In_ int t,
	_In_ int r,
	_In_ int b) throw()
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}
 void XRect::InflateRect(
	_In_ int x,
	_In_ int y) throw()
{
	::InflateRect(this, x, y);
}

 void XRect::InflateRect(_In_ SIZE size) throw()
{
	::InflateRect(this, size.cx, size.cy);
}
  BOOL XRect::PtInRect(_In_ POINT point) const throw()
 {
	 return ::PtInRect(this, point);
 }


void XRect::operator=(_In_ const RECT& srXRect) throw()
{
	::CopyRect(this, &srXRect);
}
BOOL XRect::operator==(_In_ const RECT& rect) const throw()
{
	return ::EqualRect(this, &rect);
}

BOOL XRect::operator!=(_In_ const RECT& rect) const throw()
{
	return !::EqualRect(this, &rect);
}

void XRect::operator+=(_In_ POINT point) throw()
{
	::OffsetRect(this, point.x, point.y);
}

void XRect::operator+=(_In_ SIZE size) throw()
{
	::OffsetRect(this, size.cx, size.cy);
}


void XRect::operator-=(_In_ POINT point) throw()
{
	::OffsetRect(this, -point.x, -point.y);
}

void XRect::operator-=(_In_ SIZE size) throw()
{
	::OffsetRect(this, -size.cx, -size.cy);
}


void XRect::operator&=(_In_ const RECT& rect) throw()
{
	::IntersectRect(this, this, &rect);
}

void XRect::operator|=(_In_ const RECT& rect) throw()
{
	::UnionRect(this, this, &rect);
}

XRect XRect::operator+(_In_ POINT pt) const throw()
{
	XRect rect(*this);
	::OffsetRect(&rect, pt.x, pt.y);
	return rect;
}

XRect XRect::operator-(_In_ POINT pt) const throw()
{
	XRect rect(*this);
	::OffsetRect(&rect, -pt.x, -pt.y);
	return rect;
}

XRect XRect::operator+(_In_ SIZE size) const throw()
{
	XRect rect(*this);
	::OffsetRect(&rect, size.cx, size.cy);
	return rect;
}
XRect XRect::operator&(_In_ const RECT& rect2) const throw()
{
	XRect rect;
	::IntersectRect(&rect, this, &rect2);
	return rect;
}

XRect XRect::operator|(_In_ const RECT& rect2) const throw()
{
	XRect rect;
	::UnionRect(&rect, this, &rect2);
	return rect;
}
