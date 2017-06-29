#pragma once
class CXRectTracker
{
public:
	CXRectTracker(void);
	~CXRectTracker(void);

	enum TrackerHit
	{
		hitNothing = -1,
		hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
		hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
	};
	POINT* m_mousePoint;
	POINT m_fristDown;
	XRect m_rect;
	XRect m_rectMax;
	SIZE m_sizeMin;    // minimum X and Y size during track operation
	
	BOOL m_bFinalErase;     // TRUE if DragTrackerRect called for final erase

	int m_nHandleSize;  // size of resize handles (default from WIN.INI)
	void Construct();
	void CXRectTracker::Draw(HDC pDC) const;
	BOOL SetCursor(HWND pWnd, UINT nHitTest) const;
	int HitTest(POINT point) const;
	void GetTrueRect(LPRECT lpTrueRect) const;
	virtual int GetHandleSize(LPCRECT lpRect = NULL) const;

	BOOL TrackRubberBand(HWND pWnd, POINT point);
	BOOL Track(HWND pWnd, POINT point);

	void SetMousePoint(POINT* point);


	virtual UINT GetHandleMask() const;
	 int HitTestHandles(POINT point) const;
	 void GetHandleRect(int nHandle, XRect* pHandleRect) const;
	 int NormalizeHit(int nHandle) const;

	 void GetModifyPointers(
		 int nHandle, int** ppx, int** ppy, int* px, int* py);
	 virtual void AdjustRect(int nHandle, LPRECT lpRect);
	BOOL TrackHandle(int nHandle, HWND pWnd, POINT point);
};

