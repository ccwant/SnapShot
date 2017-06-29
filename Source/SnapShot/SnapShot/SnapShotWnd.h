#pragma once

#include "stdafx.h"
#include "XRectTracker.h"
#include "XEnumAllWindowsRect.h"

class CSnapShotWnd
{
public:
	CSnapShotWnd(void);
	~CSnapShotWnd(void);

	void InitWindow(HWND hWnd);
	void OnPaint(HDC pDc);
	void InitScreenDC();
	void OnLButtonDblClk(POINT point);
	void OnLButtonDown(POINT point);
	void OnLButtonUp(POINT point);
	BOOL OnSetCursor(HWND pWnd, UINT nHitTest);
	void OnMouseMove(POINT point);
private:

	HWND m_hWnd;

	BOOL m_IsLButtonDown;
	BOOL IsMove;
	BOOL m_IsCreateWindow;
	BOOL IsWindowShot;//判断是从window选择的截图区域
	POINT m_LastPoint;
	//创建过程
	//1.选择window
	//2.判断是否移动，如果移动了，则放弃window，重新创建截图区域
	//3.在鼠标抬起事件中判断，若window没有被放弃，则创建window截图区域


	int m_ScreenWidth;
	int m_ScreenHeight;
	//放大镜大小
	int m_MagnifierSize;

	//桌面句柄
	HDC m_DesktopDC;
	//桌面Bitmap
	HBITMAP  m_DesktopBitmap;
	HBITMAP m_pGray;

	//屏幕区域
	RECT m_ScreenRect;
	//快速选择的window矩形区域
	XRect m_WindowRect;
	//放大镜矩形区域
	XRect m_MagnifierRect;


	//鼠标位置
	POINT m_MousePoint;

	CXRectTracker m_RectTracker;
	CEnumAllWindowsRect m_AllWindowsRect;
	//画遮罩层
	void DrawMask(HDC dc,XRect rect);
	void DrawAutoWindow(HDC dc,POINT point);
	//画放大镜层
	void DrawMagnifier(HDC dc);
	void DrawMagnifierBg(HDC dc);
	void DrawText(HDC dc,int x,int y,LPCWSTR lpString,int size);

	void InitGrayBitMap();
};

