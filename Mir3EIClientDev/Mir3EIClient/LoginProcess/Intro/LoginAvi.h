// LoginProcess.h: interface for the CLoginProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LOGIN_AVI_H
#define _LOGIN_AVI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _RENDER_AVI_NONE		0
#define _RENDER_AVI_INTRO		1
#define _RENDER_AVI_LOGO		2
#define _RENDER_AVI_STILL		9

class CLoginAvi : public CBMWnd 
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Functions
public:
	CLoginAvi();
	~CLoginAvi();

	VOID	SetRenderAviState(INT nState);

	virtual VOID		Create(CWHWilImageData* pxImage);

	BOOL				IsPlaying() { return m_xAvi.m_bPlaying; };
	VOID				StopAllAvis(VOID);

	virtual HRESULT		OnButtonDown(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonDown(POINT ptMouse);
	virtual HRESULT		OnButtonUp(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonUp(POINT ptMouse);
	virtual LRESULT		OnMouseMove(WPARAM wParam, LPARAM lParam);

	virtual	VOID		Render(INT	nLoopTime);
private:
protected:
	VOID				SetFocusBefore(VOID);
	VOID				SetFocusAfter(VOID);
	/////////////////////////////////////////////////////////////////////////////////////////
	// Variable
public:
private:
	INT					m_nRenderAviState;
protected:
	CWHWilImageData		*m_pxImage;
	CAvi				m_xAvi;
	RECT				m_rcSrc;
};
#endif //_LOGIN_AVI_H
