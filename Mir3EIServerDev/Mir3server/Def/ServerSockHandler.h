
#ifndef _DEFAULT_SOCKET_HANDLER
#define _DEFAULT_SOCKET_HANDLER

#define DATA_BUFSIZE 8192

enum _OVERLAPPED_FLAG
{
	OVERLAPPED_RECV,
	OVERLAPPED_SEND,
} OVERLAPPED_FLAG;

typedef struct tag_TCOMPLETIONPORT
{
   OVERLAPPED		Overlapped;
   WSABUF			DataBuf;
   CHAR				Buffer[DATA_BUFSIZE];
   INT				nOvFlag;
} _TCOMPLETIONPORT, * _LPTCOMPLETIONPORT;

typedef struct tag_TOVERLAPPEDEX
{
   CHAR				Buffer[DATA_BUFSIZE];
   WSABUF			DataBuf;
   SOCKET			Socket;
   WSAOVERLAPPED	Overlapped;
   DWORD			BytesSEND;
   DWORD			BytesRECV;
   INT				nOvFlag;
} _TOVERLAPPEDEX, * _LPTOVERLAPPEDEX;

BOOL InitServerSocket(SOCKET &s, SOCKADDR_IN* addr, int nPort);
BOOL ConnectToServer(SOCKET &s, SOCKADDR_IN* addr, LPCTSTR lpServerIP, int nPort, long lEvent,
	LPTHREAD_START_ROUTINE pCallback, LPVOID lpParam);
BOOL ClearSocket(SOCKET &s);

BOOL InitThread(LPTHREAD_START_ROUTINE lpRoutine);

BOOL CheckSocketError(LPARAM lParam);

#endif
