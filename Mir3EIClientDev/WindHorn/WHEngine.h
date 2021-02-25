#ifndef _WINDHORN_
#define	_WINDHORN_

//////////////////////////////////////////////////////////////////////////////////////
//#define STRICT
#define D3D_OVERLOADS
//////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddraw.h>
#include "../Include/d3d.h"
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <dsound.h>
#include <dshow.h>
#include <crtdbg.h>

//WRL
#include <wrl.h>

//DirectWrite
#include <dwrite.h>

//D3D11
#include <d3d11.h>
//D2D
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
//DXGI
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dwrite.lib")


#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "ddraw.lib")
//#pragma comment (lib, "d3dim.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "wininet.lib")
#pragma comment (lib, "dsound.lib")

#define _WINDHORN_CLASS				_T("WH GEngine")

#define _SCREEN_WIDTH				800
#define _SCREEN_HEIGHT				600
#define _PATCH_SCREEN_WIDTH			640
#define _PATCH_SCREEN_HEIGHT		480
#define _SCREEN_BPP					16

#define _DXG_SCREENMODE_WINDOW		1
#define _DXG_SCREENMODE_FULLSCREEN	2
#define _DXG_DEVICEMODE_PRIMARY		1
#define _DXG_DEVICEMODE_SECONDARY	2
#define _DXG_DEVICEMODE_D3D			4
#define _DXG_DEVICEMODE_ZBUFFER		8
#define _DXG_FONT_NORMAL			0
#define _DXG_FONT_ITALIC			1
#define _DXG_FONT_UNDERLINE			2
#define _DXG_FONT_BOLD				3

#define _MAX_DEVICES			    10

#define  SAFE_DELETE(p)				{ if ( p ) { delete ( p );     ( p ) = NULL; } }
#define  SAFE_DELETE_ARR(p)				{ if ( p ) { delete[] ( p );     ( p ) = NULL; } }
#define  SAFE_RELEASE(p)			{ if ( p ) { ( p )->Release(); ( p ) = NULL; } }

#include "WHImage.h"
#include "WHWindow.h"
#include "WHDefProcess.h"
#include "WHDXGraphic.h"
#include "WHWilImage.h"
#include "WH3DInit.h"
#include "WHSurface.h"
#include "WHWilTexture.h"

#include "RegHandler.h"

#include ".\D3DCommon\D3DUtil.h"
#include ".\D3DCommon\D3DTextr.h"
#include ".\D3DCommon\D3DMath.h"
#include ".\D3DCommon\DDUtil.h"

#endif //_WINDHORN_
