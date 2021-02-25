#include "stdafx.h"


#define _LEFT_INTRO				0
#define _TOP_INTRO				0
#define _RIGHT_INTRO			640
#define _BOTTOM_INTRO			480

#define _INTRO_FILE_NAME		".\\Data\\wemade.dat"
#define _LOGO_FILE_NAME			".\\Data\\StartGame.dat"

CLoginAvi::CLoginAvi():CBMWnd()
{
	m_pxImage			= NULL;

	m_nRenderAviState	= _RENDER_AVI_NONE;

	SetRect(&m_rcWnd,0,0,640,480);
	SetRect(&m_rcSrc,_LEFT_INTRO,_TOP_INTRO,_RIGHT_INTRO,_BOTTOM_INTRO);
}

CLoginAvi::~CLoginAvi()
{
	StopAllAvis();
	m_xAvi.ReleaseAvi();
}

VOID CLoginAvi::Create(CWHWilImageData* pxImage)
{
	m_pxImage = pxImage;
	m_xAvi.InitAvi();
	m_xAvi.SetDestRect(m_rcSrc);
}

VOID CLoginAvi::SetRenderAviState(INT nState)
{
	m_xAvi.Stop();
	m_nRenderAviState = nState;
	switch(m_nRenderAviState)
	{
		case _RENDER_AVI_INTRO:
		{
			m_xAvi.SetFileName(_INTRO_FILE_NAME, FALSE);
			m_xAvi.Start();
			break;
		}
		case _RENDER_AVI_LOGO:
		{
			m_xAvi.SetFileName(_LOGO_FILE_NAME, FALSE);
			m_xAvi.Start();
			break;
		}
	}
}

HRESULT CLoginAvi::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

HRESULT CLoginAvi::OnButtonDown(POINT ptMouse)
{
	return 0;
}

HRESULT CLoginAvi::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

HRESULT CLoginAvi::OnButtonUp(POINT ptMouse)
{
	return 0;
}

LRESULT CLoginAvi::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

VOID CLoginAvi::Render(INT	nLoopTime)
{
	switch(m_nRenderAviState)
	{
		case _RENDER_AVI_INTRO:
		{
			if (!m_xAvi.Draw(nLoopTime))
			{
				m_nRenderAviState = _RENDER_AVI_STILL;
			}
			break;
		}
		case _RENDER_AVI_LOGO:
		{
			m_nRenderAviState = _RENDER_AVI_STILL;
			break;
		}
		case _RENDER_AVI_STILL:
		{	

			break;
		}
	}
}


VOID CLoginAvi::StopAllAvis(VOID)
{
	m_xAvi.Stop();
}
