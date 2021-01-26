#include "stdafx.h"

extern HANDLE g_hWSAThread;

DWORD WINAPI OnClientSockMsg(LPVOID lpThreadParameter);
BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

VOID WINAPI OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	switch (idEvent)
	{
		case _ID_TIMER_CONNECTSERVER:
		{
			if (g_csock == INVALID_SOCKET)
			{
				if (g_hWSAThread)
				{
					WaitForSingleObject(g_hWSAThread, INFINITE);
					g_hWSAThread = NULL;
				}
				TCHAR	tsIP[20] = { 0 };
				int		nPort = 0;
				TCHAR	szPort[24];

				InsertLogMsg(IDS_APPLY_RECONNECT);

				jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerIP"), (LPBYTE)&tsIP);
				jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerPort"), (LPBYTE)&nPort);
				_itow_s(nPort, szPort, 10);
				ConnectToServer(g_csock, &g_caddr, tsIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE, OnClientSockMsg, NULL);
			}

			break;
		}
	}
}
