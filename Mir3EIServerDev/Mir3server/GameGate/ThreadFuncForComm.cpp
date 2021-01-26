#include "stdafx.h"

#define PACKET_KEEPALIVE		"%--$"

extern HWND				g_hToolBar;
extern HWND				g_hStatusBar;

extern SOCKET			g_csock;
extern SOCKADDR_IN		g_caddr;

DWORD WINAPI OnClientSockMsg(LPVOID lpThreadParameter);
BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

VOID WINAPI OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	switch (idEvent)
	{
		case _ID_TIMER_KEEPALIVE:
		{
			if (g_csock != INVALID_SOCKET)
			{
				SendSocketMsgS(GM_CHECKCLIENT, 0, 0, 0, 0, NULL);
				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)_TEXT("Check Activity"));
			}

			break;
		}
		case _ID_TIMER_CONNECTSERVER:
		{
			if (g_csock == INVALID_SOCKET)
			{
				TCHAR	tsIP[20] = { 0 };
				int		nPort = 0;

				InsertLogMsg(IDS_APPLY_RECONNECT);

				jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("RemoteIP"), (LPBYTE)&tsIP);

				if (!jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("RemotePort"), (LPBYTE)&nPort))
					nPort = 5000;
				//_IDM_CLIENTSOCK_MSG
				ConnectToServer(g_csock, &g_caddr, tsIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE, OnClientSockMsg, NULL);
			}

			break;
		}
	}
}
