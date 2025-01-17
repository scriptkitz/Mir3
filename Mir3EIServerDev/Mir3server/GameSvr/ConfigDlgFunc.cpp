#include "stdafx.h"

BOOL jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

BOOL CALLBACK ConfigDlgFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			RECT rcMainWnd, rcDlg;

			GetWindowRect(g_hMainWnd, &rcMainWnd);
			GetWindowRect(hWndDlg, &rcDlg);

			MoveWindow(hWndDlg, rcMainWnd.left + (((rcMainWnd.right - rcMainWnd.left) - (rcDlg.right - rcDlg.left)) / 2), 
						rcMainWnd.top + (((rcMainWnd.bottom - rcMainWnd.top) - (rcDlg.bottom - rcDlg.top)) / 2), 
						(rcDlg.right - rcDlg.left), (rcDlg.bottom - rcDlg.top), FALSE);

			SendMessage(GetDlgItem(hWndDlg, IDC_DBSRV_PORT), EM_LIMITTEXT, (WPARAM)5, (LPARAM)0L);

			DWORD	dwIP = 0;
			TCHAR	szIP[20] = { 0 };
			TCHAR	szPort[24] = { 0 };
			TCHAR	szServer[24] = { 0 };
			TCHAR	szPath[256] = { 0 };
			TCHAR	szDatabase[256] = { 0 };

			int		nPort = 0, nServerNum = 0;
			int		i1=0, i2=0, i3=0, i4=0;

			jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerIP"), (LPBYTE)&szIP);
			_stscanf_s(szIP, _T("%ld.%ld.%ld.%ld"), &i1, &i2, &i3, &i4);
			dwIP = MAKEIPADDRESS(i1,i2,i3,i4);
			SendMessage(GetDlgItem(hWndDlg, IDC_DBSVR_IP), IPM_SETADDRESS, (WPARAM)0, (LPARAM)(DWORD)dwIP);

			jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerPort"), (LPBYTE)&nPort);
			_itow_s(nPort, szPort, 10);
			SetWindowText(GetDlgItem(hWndDlg, IDC_DBSRV_PORT), szPort);
			
			jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("ServerNumber"), (LPBYTE)&nServerNum);
			_itow_s(nServerNum, szServer, 10);
			SetWindowText(GetDlgItem(hWndDlg, IDC_SRVNUMBER), szServer);

			jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("MapFileLoc"), (LPBYTE)szPath);
			SetWindowText(GetDlgItem(hWndDlg, IDC_MAPFILE_LOC), szPath);

			jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("Device"), (LPBYTE)szDatabase);
			SetWindowText(GetDlgItem(hWndDlg, IDC_DBMS_DEVICE), szDatabase);

			break;
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				{
					DWORD	dwIP = 0;
					TCHAR	szIP[20];
					TCHAR	szPort[24];
					TCHAR	szServer[24];
					TCHAR	szPath[256];
					TCHAR	szDatabase[256];

					int		nRemotePort = 0, nServerNum = 0;
					BYTE	btInstalled = 1;

					jRegSetKey(_GAME_SERVER_REGISTRY, _TEXT("Installed"), REG_BINARY, (LPBYTE)&btInstalled, sizeof(BYTE));

					GetWindowText(GetDlgItem(hWndDlg, IDC_SRVNUMBER), szServer, sizeof(szServer));
					nServerNum = _wtoi(szServer);
					jRegSetKey(_GAME_SERVER_REGISTRY, _TEXT("ServerNumber"), REG_DWORD, (LPBYTE)&nServerNum, sizeof(DWORD));

					SendMessage(GetDlgItem(hWndDlg, IDC_DBSVR_IP), IPM_GETADDRESS, (WPARAM)0, (LPARAM)(LPDWORD)&dwIP);
					_stprintf_s(szIP, _T("%ld.%ld.%ld.%ld"),
						FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP),
						THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP));
					jRegSetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerIP"), REG_SZ, (LPBYTE)szIP, _tcsclen(szIP)*sizeof(TCHAR));

					GetWindowText(GetDlgItem(hWndDlg, IDC_DBSRV_PORT), szPort, sizeof(szPort));
					nRemotePort = _wtoi(szPort);
					jRegSetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerPort"), REG_DWORD, (LPBYTE)&nRemotePort, sizeof(DWORD));

					GetWindowText(GetDlgItem(hWndDlg, IDC_MAPFILE_LOC), szPath, sizeof(szPath));
					jRegSetKey(_GAME_SERVER_REGISTRY, _TEXT("MapFileLoc"), REG_SZ, (LPBYTE)szPath, sizeof(szPath)/sizeof(TCHAR));

					GetWindowText(GetDlgItem(hWndDlg, IDC_DBMS_DEVICE), szDatabase, sizeof(szDatabase));
					jRegSetKey(_GAME_SERVER_REGISTRY, _TEXT("Device"), REG_SZ, (LPBYTE)szDatabase, sizeof(szDatabase)/sizeof(TCHAR));

				}
				case IDCANCEL:
					return EndDialog(hWndDlg, IDCANCEL);
			}

			break;
		}
	}

	return FALSE;
}
