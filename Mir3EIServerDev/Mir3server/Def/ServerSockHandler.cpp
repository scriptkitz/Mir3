#include "stdafx.h"

extern HWND		g_hMainWnd;
extern HANDLE	g_hWSAEvent;
extern HANDLE	g_hWSAThread;

HANDLE			g_hIOCP = NULL;

//unsigned long	g_lAcceptThread = 0;
HANDLE			g_hAcceptThread = INVALID_HANDLE_VALUE;

//UINT WINAPI AcceptThread(LPVOID lpParameter);
DWORD WINAPI	AcceptThread(LPVOID lpParameter);
DWORD WINAPI	ServerWorkerThread(LPVOID lpParameter);

INT				CreateIOCPWorkerThread(int nThread);
BOOL			InitServerThreadForMsg();

typedef unsigned (__stdcall *ThreadStartRoutine) (
    LPVOID lpThreadParameter
	);

BOOL InitThread(LPTHREAD_START_ROUTINE lpRoutine)
{
	DWORD	dwThreadIDForMsg = 0;

	HANDLE hThreadForMsg	= CreateThread(NULL, 0, lpRoutine,	NULL, 0, &dwThreadIDForMsg);

	if (hThreadForMsg)
	{
		CloseHandle(hThreadForMsg);
	
		return TRUE;
	}

	return FALSE;
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

BOOL InitServerSocket(SOCKET &s, SOCKADDR_IN* addr, int nPort)
{
	if (s == INVALID_SOCKET)
	{
		s = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		addr->sin_family		= AF_INET;
		addr->sin_port			= htons(nPort);
		addr->sin_addr.s_addr	= htonl(INADDR_ANY);

		if ((bind(s, (const struct sockaddr FAR*)addr, sizeof(SOCKADDR_IN))) == SOCKET_ERROR)
			return FALSE;

		if ((listen(s, 5)) == SOCKET_ERROR)
			return FALSE;

		CreateIOCPWorkerThread(1);

		if (!InitThread(AcceptThread))
			return FALSE;
	}
	else 
		return FALSE;

	return TRUE;
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

BOOL ClearSocket(SOCKET &s)
{
	if (s != INVALID_SOCKET)
	{
		LINGER lingerStruct;

		lingerStruct.l_onoff	= 1;
		lingerStruct.l_linger	= 0;

		setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));

		closesocket(s);
		s = INVALID_SOCKET;

		return TRUE;
	}

	return FALSE;
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

BOOL ConnectToServer(SOCKET& s, SOCKADDR_IN* addr, LPCTSTR lpServerIP, int nPort, long lEvent,
	LPTHREAD_START_ROUTINE pCallback, LPVOID lpParam)
{
	if (s != INVALID_SOCKET)
	{
		closesocket(s);
		s = INVALID_SOCKET;
	}
	if (g_hWSAEvent == NULL)
	{
		g_hWSAEvent = WSACreateEvent();
	}
	s = socket(AF_INET, SOCK_STREAM, 0);

	addr->sin_family	= AF_INET;
	addr->sin_port		= htons(nPort);
	
#ifdef UNICODE
	InetPtonW(AF_INET, lpServerIP, &addr->sin_addr.s_addr);
#else
	inet_pton(AF_INET, lpServerIP, &addr->sin_addr.s_addr);
#endif

	if (WSAEventSelect(s, g_hWSAEvent, lEvent) == SOCKET_ERROR)
		return FALSE;

	if (connect(s, (const struct sockaddr FAR*)addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			DWORD dwThreadID;
			g_hWSAThread = CreateThread(NULL, 0, pCallback, lpParam, 0, &dwThreadID);
			return TRUE;
		}
	}

	return FALSE;
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

BOOL CheckSocketError(LPARAM lParam)
{
	switch (WSAGETSELECTERROR(lParam))
	{
		case WSANOTINITIALISED:
			InsertLogMsg(_T("A successful WSAStartup must occur before using this function."));
			return FALSE;
		case WSAENETDOWN:
			InsertLogMsg(_T("The network subsystem has failed."));
			return FALSE;
		case WSAEADDRINUSE:
			InsertLogMsg(_T("The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function."));
			return FALSE;
		case WSAEINTR:
			InsertLogMsg(_T("The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall."));
			return FALSE;
		case WSAEINPROGRESS:
			InsertLogMsg(_T("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."));
			return FALSE;
		case WSAEALREADY:
			InsertLogMsg(_T("A nonblocking connect call is in progress on the specified socket. Note In order to preserve backward compatibility, this error is reported as WSAEINVAL to Windows Sockets 1.1 applications that link to either WINSOCK.DLL or WSOCK32.DLL."));
			return FALSE;
		case WSAEADDRNOTAVAIL:
			InsertLogMsg(_T("The remote address is not a valid address (such as ADDR_ANY)."));
			return FALSE;
		case WSAEAFNOSUPPORT:
			InsertLogMsg(_T("Addresses in the specified family cannot be used with this socket."));
			return FALSE;
		case WSAECONNREFUSED:			// Can't Connect Server...
		case WSAETIMEDOUT:				// Time Out
			InsertLogMsg(IDS_CANT_CONNECT);
			return FALSE;
		case WSAEFAULT:
			InsertLogMsg(_T("The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family."));
			return FALSE;
		case WSAEINVAL:
			InsertLogMsg(_T("The parameter s is a listening socket, or the destination address specified is not consistent with that of the constrained group the socket belongs to."));
			return FALSE;
		case WSAEISCONN:
			InsertLogMsg(_T("The socket is already connected (connection-oriented sockets only)."));
			return FALSE;
		case WSAENETUNREACH:
			InsertLogMsg(_T("The network cannot be reached from this host at this time."));
			return FALSE;
		case WSAENOBUFS:
			InsertLogMsg(_T("No buffer space is available. The socket cannot be connected."));
			return FALSE;
		case WSAENOTSOCK:
			InsertLogMsg(_T("The descriptor is not a socket."));
			return FALSE;
		case WSAEWOULDBLOCK:
			InsertLogMsg(_T("The socket is marked as nonblocking and the connection cannot be completed immediately."));
			return FALSE;
		case WSAEACCES:
			InsertLogMsg(_T("Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled."));
			return FALSE;
	}

	return TRUE;
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

// **************************************************************************************
//
//			
//
// **************************************************************************************

INT CreateIOCPWorkerThread(int nThread)
{
	DWORD	dwThreadID;

	if (g_hIOCP != INVALID_HANDLE_VALUE)
	{
		SYSTEM_INFO SystemInfo;

		if ((g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
			return -1;

		GetSystemInfo(&SystemInfo);

		for (UINT i = 0; i < SystemInfo.dwNumberOfProcessors * nThread; i++)
		{
			HANDLE ThreadHandle;

			if ((ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, g_hIOCP, 0, &dwThreadID)) == NULL)
				return -1;

			CloseHandle(ThreadHandle);
		}

		return SystemInfo.dwNumberOfProcessors * nThread;
	}

	return -1;
}
