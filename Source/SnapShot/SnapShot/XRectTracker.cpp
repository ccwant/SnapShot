#include "stdafx.h"
#include "XRectTracker.h"

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

#define VERIFY(f)          ((void)(f))


int _afxHandleSize = 0;
HCURSOR _afxCursors[10] = { 0, };
HBRUSH _afxHandlerBrush = 0;
HPEN _afxTrackerSolidPen = 0;


struct AFX_HANDLEINFO
{
	size_t nOffsetX;    // offset within RECT for X coordinate
	size_t nOffsetY;    // offset within RECT for Y coordinate
	int nCenterX;       // adjust X by Width()/2 * this number
	int nCenterY;       // adjust Y by Height()/2 * this number
	int nHandleX;       // adjust X by handle size * this number
	int nHandleY;       // adjust Y by handle size * this number
	int nInvertX;       // handle converts to this when X inverted
	int nInvertY;       // handle converts to this when Y inverted
};
const AFX_HANDLEINFO _afxHandleInfo[] =
{
	// corner handles (top-left, top-right, bottom-right, bottom-left
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 0,  0,  0, 1, 3 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 0, -1,  0, 0, 2 },
	{ offsetof(RECT, right), offsetof(RECT, bottom),    0, 0, -1, -1, 3, 1 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     0, 0,  0, -1, 2, 0 },

	// side handles (top, right, bottom, left)
	{ offsetof(RECT, left), offsetof(RECT, top),        1, 0,  0,  0, 4, 6 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 1, -1,  0, 7, 5 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     1, 0,  0, -1, 6, 4 },
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 1,  0,  0, 5, 7 }
};
// the struct below gives us information on the layout of a RECT struct and
//  the relationship between its members
struct AFX_RECTINFO
{
	size_t nOffsetAcross;   // offset of opposite point (ie. left->right)
	int nSignAcross;        // sign relative to that point (ie. add/subtract)
};

// this array is indexed by the offset of the RECT member / sizeof(int)
const AFX_RECTINFO _afxRectInfo[] =
{
	{ offsetof(RECT, right), +1 },
	{ offsetof(RECT, bottom), +1 },
	{ offsetof(RECT, left), -1 },
	{ offsetof(RECT, top), -1 },
};
CXRectTracker::CXRectTracker(void)
{
	Construct();
}


CXRectTracker::~CXRectTracker(void)
{
}
void CXRectTracker::Construct()
{
	static BOOL bInitialized;
	if (!bInitialized)
	{

		if (_afxHandlerBrush == NULL)
		{
			_afxHandlerBrush = CreateSolidBrush(RGB(52,83,145));
		}
		
		// initialize the cursor array
		_afxCursors[0] = LoadCursor(NULL,IDC_SIZENWSE);
		_afxCursors[1] = (LoadCursor(NULL,IDC_SIZENESW));
		_afxCursors[2] = _afxCursors[0];
		_afxCursors[3] = _afxCursors[1];
		_afxCursors[4] = (LoadCursor(NULL,IDC_SIZENS));
		_afxCursors[5] = (LoadCursor(NULL,IDC_SIZEWE));
		_afxCursors[6] = _afxCursors[4];
		_afxCursors[7] = _afxCursors[5];
		_afxCursors[8] = (LoadCursor(NULL,IDC_SIZEALL));
		_afxCursors[9] = (LoadCursor(NULL,IDC_SIZEALL));
		// get default handle size from Windows profile setting
		static const TCHAR szWindows[] = _T("windows");
		static const TCHAR szInplaceBorderWidth[] =
			_T("oleinplaceborderwidth");
		_afxHandleSize = GetProfileInt(szWindows, szInplaceBorderWidth, 4);
		

		bInitialized = TRUE;
	}
	m_nHandleSize = _afxHandleSize;
	m_sizeMin.cy = m_sizeMin.cx = m_nHandleSize*2;
}

void CXRectTracker::Draw(HDC pDC) const
{
	// set initial DC state
	ASSERT(SaveDC(pDC) != 0);
	SetMapMode(pDC,MM_TEXT);

	// get normalized rectangle
	XRect rect = m_rect;
	rect.NormalizeRect();

	HPEN pOldPen = NULL;
	HBRUSH pOldBrush = NULL;
	HBRUSH pTemp;
	int nOldROP;

	//注释1
	if (_afxTrackerSolidPen == NULL)
	{
		_afxTrackerSolidPen = CreatePen(PS_SOLID, 0,RGB(52,83,145));
	}
	// draw lines
	pOldPen = (HPEN)SelectObject(pDC,_afxTrackerSolidPen);
	pOldBrush =(HBRUSH) SelectObject(pDC,GetStockObject(NULL_BRUSH));
	nOldROP = SetROP2(pDC,R2_COPYPEN);
	rect.InflateRect(+1, +1);   // borders are one pixel outside
	Rectangle(pDC,rect.left, rect.top, rect.right, rect.bottom);
	SetROP2(pDC,nOldROP);

	// draw resize handles
	if (true)
	{
		UINT mask = GetHandleMask();
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				GetHandleRect((TrackerHit)i, &rect);
				FillRect(pDC,&rect,_afxHandlerBrush);
			}
		}
	}

	// cleanup pDC state
	if (pOldPen != NULL)
		SelectObject(pDC,pOldPen);
	if (pOldBrush != NULL)
		SelectObject(pDC,pOldBrush);
	ASSERT(RestoreDC(pDC,-1));
}
BOOL CXRectTracker::SetCursor(HWND pWnd, UINT nHitTest) const
{
	// trackers should only be in client area
	if (nHitTest != HTCLIENT)
		return FALSE;

	// convert cursor position to client co-ordinates
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(pWnd,&point);

	// do hittest and normalize hit
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
		return FALSE;

	// need to normalize the hittest such that we get proper cursors
	nHandle = NormalizeHit(nHandle);

	// handle special case of hitting area between handles
	//  (logically the same -- handled as a move -- but different cursor)
	if (nHandle == hitMiddle && !m_rect.PtInRect(point))
	{
		// only for trackers with hatchedBorder (ie. in-place resizing)
		//if (m_nStyle & hatchedBorder)
			nHandle = (TrackerHit)9;
	}

	ASSERT(nHandle < _countof(_afxCursors));
	::SetCursor(_afxCursors[nHandle]);
	return TRUE;
}

int CXRectTracker::HitTest(POINT point) const
{
	TrackerHit hitResult = hitNothing;

	XRect rectTrue;
	GetTrueRect(&rectTrue);
	ASSERT(rectTrue.left <= rectTrue.right);
	ASSERT(rectTrue.top <= rectTrue.bottom);
	if (rectTrue.PtInRect(point))
	{
		if (true)
			hitResult = (TrackerHit)HitTestHandles(point);
		else
			hitResult = hitMiddle;
	}
	return hitResult;
}
void CXRectTracker::GetTrueRect(LPRECT lpTrueRect) const
{
	XRect rect = m_rect;
	rect.NormalizeRect();
	int nInflateBy = 0;
	//if ((m_nStyle & (resizeOutside|hatchedBorder)) != 0)
		nInflateBy += GetHandleSize() - 1;
	//if ((m_nStyle & (solidLine|dottedLine)) != 0)
		++nInflateBy;
	rect.InflateRect(nInflateBy, nInflateBy);
	*lpTrueRect = rect;
}
int CXRectTracker::GetHandleSize(LPCRECT lpRect) const
{
	if (lpRect == NULL)
		lpRect = &m_rect;
	int size = m_nHandleSize;
	if (true)
	{
		// make sure size is small enough for the size of the rect
		int sizeMax = min(abs(lpRect->right - lpRect->left),
			abs(lpRect->bottom - lpRect->top));
		if (size * 2 > sizeMax)
			size = sizeMax / 2;
	}
	return size;
}
//************************************
// 方法名称: TrackRubberBand
// 创建日期: 2017/03/31
// 创 建 人: admin
// 函数说明: 创建
// 函数参数: HWND pWnd
// 函数参数: POINT point
// 返 回 值: BOOL
//************************************
BOOL CXRectTracker::TrackRubberBand(HWND pWnd, POINT point)
{
	m_rect.SetRect(point.x, point.y, point.x, point.y);
	return TrackHandle(hitBottomRight,pWnd,point);
}
//************************************
// 方法名称: Track
// 创建日期: 2017/03/31
// 创 建 人: admin
// 函数说明: 移动
// 函数参数: HWND pWnd
// 函数参数: POINT point
// 返 回 值: BOOL
//************************************
BOOL CXRectTracker::Track(HWND pWnd, POINT point)
{
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
	{
		// didn't hit a handle, so just return FALSE
		return FALSE;
	}
	return TrackHandle(nHandle,pWnd,point);
}
int CXRectTracker::HitTestHandles(POINT point) const
{
	XRect rect;
	UINT mask = GetHandleMask();
	// see if hit anywhere inside the tracker
	GetTrueRect(&rect);
	if (!rect.PtInRect(point))
		return hitNothing;  // totally missed

	// see if we hit a handle
	for (int i = 0; i < 8; ++i)
	{
		if (mask & (1<<i))
		{
			GetHandleRect((TrackerHit)i, &rect);
			if (rect.PtInRect(point))
				return (TrackerHit)i;
		}
	}

	// last of all, check for non-hit outside of object, between resize handles
	if (true)
	{
		XRect rect = m_rect;
		rect.NormalizeRect();
		if (true)
			rect.InflateRect(+1, +1);
		if (!rect.PtInRect(point))
			return hitNothing;  // must have been between resize handles
	}

	return hitMiddle;   // no handle hit, but hit object (or object border)
}
void CXRectTracker::GetHandleRect(int nHandle, XRect* pHandleRect) const
{
	ASSERT(nHandle < 8);

	// get normalized rectangle of the tracker
	XRect rectT = m_rect;
	rectT.NormalizeRect();
	rectT.InflateRect(+1, +1);
	// since the rectangle itself was normalized, we also have to invert the
	//  resize handles.
	nHandle = NormalizeHit(nHandle);
	// handle case of resize handles outside the tracker
	int size = GetHandleSize();
	rectT.InflateRect(size-1, size-1);
	// calculate position of the resize handle
	int nWidth = rectT.Width();
	int nHeight = rectT.Height();
	XRect rect;
	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
	rect.left = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetX);
	rect.top = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetY);
	rect.left += size * pHandleInfo->nHandleX;
	rect.top += size * pHandleInfo->nHandleY;
	rect.left += pHandleInfo->nCenterX * (nWidth - size) / 2;
	rect.top += pHandleInfo->nCenterY * (nHeight - size) / 2;
	rect.right = rect.left + size;
	rect.bottom = rect.top + size;

	*pHandleRect = rect;
}
int CXRectTracker::NormalizeHit(int nHandle) const
{
	ASSERT(nHandle <= 8 && nHandle >= -1);
	if (nHandle == hitMiddle || nHandle == hitNothing)
		return nHandle;
	ASSERT(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
	if (m_rect.Width() < 0)
	{
		nHandle = (TrackerHit)pHandleInfo->nInvertX;
		ASSERT(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
		pHandleInfo = &_afxHandleInfo[nHandle];
	}
	if (m_rect.Height() < 0)
		nHandle = (TrackerHit)pHandleInfo->nInvertY;
	return nHandle;
}

UINT CXRectTracker::GetHandleMask() const
{
	UINT mask = 0x0F;   // always have 4 corner handles
	int size = m_nHandleSize*3;
	if (abs(m_rect.Width()) - size > 4)
		mask |= 0x50;
	if (abs(m_rect.Height()) - size > 4)
		mask |= 0xA0;
	return mask;
}
void CXRectTracker::GetModifyPointers(
	int nHandle, int** ppx, int** ppy, int* px, int* py)
{
	ASSERT(nHandle >= 0);
	ASSERT(nHandle <= 8);

	if (nHandle == hitMiddle)
		nHandle = hitTopLeft;   // same as hitting top-left

	*ppx = NULL;
	*ppy = NULL;

	// fill in the part of the rect that this handle modifies
	//  (Note: handles that map to themselves along a given axis when that
	//   axis is inverted don't modify the value on that axis)

	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
	if (pHandleInfo->nInvertX != nHandle)
	{
		*ppx = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetX);
		if (px != NULL)
			*px = **ppx;
	}
	else
	{
		// middle handle on X axis
		if (px != NULL)
			*px = m_rect.left + abs(m_rect.Width()) / 2;
	}
	if (pHandleInfo->nInvertY != nHandle)
	{
		*ppy = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetY);
		if (py != NULL)
			*py = **ppy;
	}
	else
	{
		// middle handle on Y axis
		if (py != NULL)
			*py = m_rect.top + abs(m_rect.Height()) / 2;
	}
}

void CXRectTracker::AdjustRect(int nHandle, LPRECT)
{
	if (nHandle == hitMiddle)
		return;

	// convert the handle into locations within m_rect
	int *px, *py;
	GetModifyPointers(nHandle, &px, &py, NULL, NULL);

	// enforce minimum width
	int nNewWidth = m_rect.Width();
	int nAbsWidth =  abs(nNewWidth);
	if (px != NULL && nAbsWidth < m_sizeMin.cx)
	{
		nNewWidth = nAbsWidth != 0 ? nNewWidth / nAbsWidth : 1;
		ptrdiff_t iRectInfo = (int*)px - (int*)&m_rect;
		ASSERT(0 <= iRectInfo && iRectInfo < _countof(_afxRectInfo));
		const AFX_RECTINFO* pRectInfo = &_afxRectInfo[iRectInfo];
		*px = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) +
			nNewWidth * m_sizeMin.cx * -pRectInfo->nSignAcross;
	}

	// enforce minimum height
	int nNewHeight = m_rect.Height();
	int nAbsHeight = abs(nNewHeight);
	if (py != NULL && nAbsHeight < m_sizeMin.cy)
	{
		nNewHeight = nAbsHeight != 0 ? nNewHeight / nAbsHeight : 1;
		ptrdiff_t iRectInfo = (int*)py - (int*)&m_rect;
		ASSERT(0 <= iRectInfo && iRectInfo < _countof(_afxRectInfo));
		const AFX_RECTINFO* pRectInfo = &_afxRectInfo[iRectInfo];
		*py = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) +
			nNewHeight * m_sizeMin.cy * -pRectInfo->nSignAcross;
	}
}

void CXRectTracker::SetMousePoint(POINT* point)
{
	m_mousePoint = point;
}
BOOL CXRectTracker::TrackHandle(int nHandle, HWND pWnd, POINT point)
{
	ASSERT(nHandle >= 0);
	ASSERT(nHandle <= 8);   // handle 8 is inside the rect
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	// save original width & height in pixels
	int nWidth = m_rect.Width();
	int nHeight = m_rect.Height();


	// set capture to the window which received this message
	SetCapture(pWnd);
	ASSERT(pWnd == GetCapture());
	XRect rectSave = m_rect;

	// find out what x/y coords we are supposed to modify
	int *px, *py;
	int xDiff, yDiff;
	GetModifyPointers(nHandle, &px, &py, &xDiff, &yDiff);
	xDiff = point.x - xDiff;
	yDiff = point.y - yDiff;
	XRect rectOld;
	BOOL bMoved = FALSE;
	int x = 0;
	int y = 0;
	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));
		if (GetCapture() != pWnd)
			break;
		switch (msg.message)
		{
			case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			x = GET_X_LPARAM(msg.lParam);
			y = GET_Y_LPARAM(msg.lParam);
			if(m_mousePoint != NULL){
				m_mousePoint->x = x;
				m_mousePoint->y = y;
			}
			
			rectOld = m_rect;
			// handle resize cases (and part of move)
			if (px != NULL)
				*px = x - xDiff;
			if (py != NULL)
				*py = y - yDiff;
			// handle move case
			if (nHandle == hitMiddle)
			{
				m_rect.right = m_rect.left + nWidth;
				m_rect.bottom = m_rect.top + nHeight;
			}
			// allow caller to adjust the rectangle if necessary
			AdjustRect(nHandle, &m_rect);
		
			m_bFinalErase = (msg.message == WM_LBUTTONUP);
			if (!rectOld.EqualRect(&m_rect) || m_bFinalErase)
			{
				if (bMoved)
				{
					//m_bErase = TRUE;
					//注释1
					//DrawTrackerRect(&rectOld, pWndClipTo, pDrawDC, pWnd);
				}
				if(!m_rectMax.IsRectNull())
				{

					if(m_rectMax.right <= m_rect.right)
					{
						int offset=m_rectMax.right -  m_rect.right;
						m_rect.right = m_rectMax.right;
						m_rect.left +=offset;
					}
					if(m_rectMax.top >= m_rect.top)
					{
						int offset=m_rectMax.top -  m_rect.top;
						m_rect.top = m_rectMax.top;
						m_rect.bottom +=offset;
					}
					if(m_rectMax.left >= m_rect.left)
					{
						int offset=m_rectMax.left -  m_rect.left;
						m_rect.left = m_rectMax.left;
						m_rect.right +=offset;
					}
					if(m_rectMax.bottom <= m_rect.bottom)
					{
						int offset=m_rectMax.bottom -  m_rect.bottom;
						m_rect.bottom = m_rectMax.bottom;
						m_rect.top +=offset;
					}
				}
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;


			}
			InvalidateRect(pWnd,NULL,false) ; 
			//UpdateWindow(pWnd);
			if(m_bFinalErase)
				goto ExitLoop;
			if (!rectOld.EqualRect(&m_rect))
			{
				//m_bErase = FALSE;
				//注释2
				//DrawTrackerRect(&m_rect, pWndClipTo, pDrawDC, pWnd);
			}
			break;
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				//m_bErase = m_bFinalErase = TRUE;
				 m_bFinalErase = TRUE;
				//注释3
				//DrawTrackerRect(&m_rect, pWndClipTo, pDrawDC, pWnd);
			}
			m_rect = rectSave;
			goto ExitLoop;
		default:
			DispatchMessage(&msg);
			break;
		}
	}
ExitLoop:
	ReleaseCapture();
	// restore rect in case bMoved is still FALSE
	if (!bMoved)
		m_rect = rectSave;

	m_bFinalErase = FALSE;
	//m_bErase = FALSE;

	// return TRUE only if rect has changed
	return !rectSave.EqualRect(&m_rect);

}