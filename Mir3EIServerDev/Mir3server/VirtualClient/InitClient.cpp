#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>

void InsertLogMsg(LPTSTR lpszMsg);

extern HINSTANCE		g_hInst;
extern HWND				g_hMainWnd;
extern HANDLE			g_hWSAEvent;
extern HANDLE			g_hThread;

DWORD WINAPI OnSockMsg(LPVOID lpParameter);

BOOL ConnectServer(SOCKET &s, SOCKADDR_IN* addr, LPCTSTR lpServerIP, DWORD dwIP, int nPort, long lEvent)
{
	if (s != INVALID_SOCKET)
	{
		closesocket(s);
		s = INVALID_SOCKET;
	}

	s = socket(AF_INET, SOCK_STREAM, 0);

	addr->sin_family	= AF_INET;
	addr->sin_port		= htons(nPort);
	
	if (lpServerIP && strlen(lpServerIP)>0)
		inet_pton(AF_INET, lpServerIP, &addr->sin_addr.s_addr);
	else
		inet_pton(AF_INET, "127.0.0.1", &addr->sin_addr.s_addr);

	if (WSAEventSelect(s, g_hWSAEvent, lEvent) == SOCKET_ERROR)
		return FALSE;


	if (connect(s, (const struct sockaddr FAR*)addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			DWORD	dwThreadID = 0;
			g_hThread = CreateThread(NULL, 0, OnSockMsg, &s, 0, &dwThreadID);
			return TRUE;
		}
	}

	return FALSE;
}

DWORD WINAPI OnSockMsg(LPVOID lpParameter)
{
	SOCKET s = *(SOCKET*)lpParameter;
	while (true)
	{
		DWORD dwIndex = WSAWaitForMultipleEvents(1, &g_hWSAEvent, FALSE, 500, FALSE);
		if (dwIndex == WSA_WAIT_TIMEOUT) continue;
		WSANETWORKEVENTS events;
		if (SOCKET_ERROR != WSAEnumNetworkEvents(s, g_hWSAEvent, &events))
		{
			if (events.lNetworkEvents & FD_CONNECT)
			{
				int errcode = events.iErrorCode[FD_CONNECT_BIT];
				if (errcode)
				{
					TCHAR msg[128];
					HLOCAL errs = NULL;
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
						NULL, errcode, 0, (LPSTR)&errs, 0, NULL);
					_stprintf_s(msg, "连接登录服务器失败!(%d-%s)", errcode, (LPSTR)errs);
					LocalFree(errs);
					InsertLogMsg(msg);
				}
				else
					InsertLogMsg(_T("连接登录服务器成功。"));
			}
			else if (events.lNetworkEvents & FD_READ)
			{
				char	szMsg[4096];
				int nLen = recv(s, szMsg, sizeof(szMsg)-1, 0);
				szMsg[nLen] = '\0';
				InsertLogMsg(szMsg);
			}
			else if (events.lNetworkEvents & FD_CLOSE)
			{
				WSACloseEvent(g_hWSAEvent);
				g_hWSAEvent = NULL;
				break;
			}
		}
	}
	g_hThread = NULL;
	return 0;
}
