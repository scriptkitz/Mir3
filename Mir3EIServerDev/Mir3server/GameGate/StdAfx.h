// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define UNICODE
#define _UNICODE

#include <WS2tcpip.h>
#include <ws2def.h>
#include <windows.h>
#include <tchar.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include <commctrl.h>
#include <time.h>
#include <process.h>
#include <stdio.h>

#include "resource.h"

#include "..\Def\Queue.h"
#include "..\Def\EnDecode.h"
#include "..\Def\ServerSockHandler.h"
#include "..\Def\Protocol.h"
#include "..\Def\DynamicArray.h"
#include "..\Def\Misc.h"

#include "GameGate.h"
#include "Abusive.h"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "comctl32.lib")

// **************************************************************************************

#define _BMP_CX						16
#define _BMP_CY						16

#define _STATUS_HEIGHT				10
#define _NUMOFMAX_STATUS_PARTS		6

#define _GAMEGATE_SERVER_CLASS		_TEXT("GameGateServerClass")
#define _GAMEGATE_SERVER_TITLE		_TEXT("Legend of Mir II - Game Gate Server")
#define _GAMEGATE_SERVER_REGISTRY	_TEXT("Software\\LegendOfMir\\GameGate")

#define _IDM_MY_WM_USER				WM_USER + 1000
#define _IDM_CLIENTSOCK_MSG			_IDM_MY_WM_USER + 1

#define _IDW_TOOLBAR				_IDM_MY_WM_USER + 2
#define _IDW_STATUSBAR				_IDM_MY_WM_USER + 3

#define _ID_TIMER_KEEPALIVE			_IDM_MY_WM_USER + 4
#define _ID_TIMER_CONNECTSERVER		_IDM_MY_WM_USER + 5

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
