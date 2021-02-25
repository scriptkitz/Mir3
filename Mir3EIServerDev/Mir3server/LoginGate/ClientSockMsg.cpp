#include "stdafx.h"

extern HANDLE g_hWSAEvent;

DWORD WINAPI	ThreadFuncForMsg(LPVOID lpParameter);

BOOL			CheckSocketError(LPARAM lParam);

VOID WINAPI		OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

void			OnCommand(WPARAM wParam, LPARAM lParam);
BOOL			InitThread(LPTHREAD_START_ROUTINE lpRoutine);

extern SOCKET					g_csock;

extern HWND						g_hMainWnd;
extern HWND						g_hStatusBar;
extern HWND						g_hToolBar;

HANDLE							g_hMsgThread = INVALID_HANDLE_VALUE;

CWHQueue						g_xMsgQueue;

static char	WorkBuff[8192];
static int	nWorkBuffLen;

DWORD WINAPI OnClientSockMsg(LPVOID lpParam)
{

	while (true)
	{
		DWORD dwIndex = WSAWaitForMultipleEvents(1, &g_hWSAEvent, FALSE, 500, FALSE);
		if (dwIndex == WSA_WAIT_TIMEOUT) continue;
		WSANETWORKEVENTS events;
		if (SOCKET_ERROR != WSAEnumNetworkEvents(g_csock, g_hWSAEvent, &events))
		{
			if (events.lNetworkEvents & FD_CONNECT)
			{
				int errcode = events.iErrorCode[FD_CONNECT_BIT];
				if (errcode)
				{
					InsertLogMsg(IDS_CANT_CONNECT);
					closesocket(g_csock);
					g_csock = INVALID_SOCKET;

					SetTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER, 10000, (TIMERPROC)OnTimerProc);
					return 0;
				}
				else
				{
					if (InitThread(ThreadFuncForMsg))
					{
						KillTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER);

						SetTimer(g_hMainWnd, _ID_TIMER_KEEPALIVE, 5000, (TIMERPROC)OnTimerProc);

						InsertLogMsg(IDS_CONNECT_LOGINSERVER);
						SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)_TEXT("Connected"));
					}
				}
			}
			else if (events.lNetworkEvents & FD_READ)
			{
				int		nSocket = 0;
				char* pszFirst = NULL, * pszEnd = NULL;

				UINT nRecv = 0;

				ioctlsocket(g_csock, FIONREAD, (u_long*)&nRecv);

				if (nRecv)
				{
					char* pszPacket = new char[nRecv + 1];

					nRecv = recv(g_csock, pszPacket, nRecv, 0);

					pszPacket[nRecv] = '\0';

					if (!(g_xMsgQueue.PushQ((BYTE*)pszPacket)))
						InsertLogMsg(_TEXT("[INFO] Not enough queue(g_xMsgQueue) buffer."));

					continue;
				}

				break;
			}
			else if (events.lNetworkEvents & FD_CLOSE)
			{
				closesocket(g_csock);
				g_csock = INVALID_SOCKET;

				OnCommand(IDM_STOPSERVICE, 0);
				break;
			}
		}
	}

	return 0L;
}

void SendExToServer(char *pszPacket)
{
	DWORD	dwSendBytes;
	WSABUF	buf;

	buf.len = memlen(pszPacket) - 1;
	buf.buf = pszPacket;

	if ( WSASend(g_csock, &buf, 1, &dwSendBytes, 0, NULL, NULL) == SOCKET_ERROR )
	{
		int nErr = WSAGetLastError();
	}
}
