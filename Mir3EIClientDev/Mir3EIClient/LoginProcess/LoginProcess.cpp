// LoginProcess.cpp: implementation of the CLoginProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SETRECT(ret,x1,y1,x2,y2)	{ ret.left = x1;ret.top = y1;ret.right=x2;ret.bottom = y2;}
#define SETBOX(ret,x,y,w,h)			{ ret.left = x;ret.top = y;ret.right = w;ret.bottom.h;}

#define MESSAGE_NUM_LOGIN	0
#define SRVSELBTN_LEFT		110
#define SRVSELBTN_TOP		120

extern CClientSocket g_xLoginSocket;


CLoginProcess::CLoginProcess()
{

	m_hBrush = CreateSolidBrush(RGB(0, 0, 0));
	Init();
}

VOID CLoginProcess::Init()
{
	m_fIsConnected				= CONNECT_FAIL;
	m_Progress					= PRG_PATCH;
	m_UserState					= LGM_INPUT_ID;
	m_fIsLButtonClicked			= FALSE;
	m_nAnimationCounter			= 0;
	ReadIniFileData();
}

VOID CLoginProcess::ReadIniFileData(VOID)
{
	CHAR	szFullPathFileName[MAX_PATH];
	CHAR	szTemp[MAX_PATH];
	INT		nServerCount;
	INT		nLoop;

	GetCurrentDirectory(1024,szFullPathFileName);
	strcat_s(szFullPathFileName+strlen(szFullPathFileName), 3, "\\\0");
 	strcat_s(szFullPathFileName,MIR2EI_INI_FILE_NAME);

	// Get Server IP & PORT
	GetPrivateProfileString( MIR2EI_INI_SECTION, LOGIN_SETVER_NAME, LOGIN_GATE_SERVER_IP, m_szServerIP, 16, szFullPathFileName );
	m_nServerPort	=	GetPrivateProfileInt( MIR2EI_INI_SECTION, LOGIN_SERVER_PORT, LOGIN_SERVER_PORT_DATA, szFullPathFileName );
	// Get ServerCounter
	nServerCount	=	GetPrivateProfileInt( MIR2EI_SRV_SECTION, SERVER_COUNT, 0, szFullPathFileName);
	// Read ServerList From Ini files
	m_SList.ClearAllNodes();
	for( nLoop = 0; nLoop < nServerCount; nLoop++ )
	{
		PServerList	SList = new ServerList;		
		sprintf_s( szTemp, "%s%d%s", "server", nLoop+1, "caption");
		GetPrivateProfileString( MIR2EI_SRV_SECTION,(LPCTSTR) szTemp, NULL, SList->Caption , MAX_PATH, szFullPathFileName );		
		sprintf_s( szTemp, "%s%d%s", "server", nLoop+1, "name");
		GetPrivateProfileString( MIR2EI_SRV_SECTION,(LPCTSTR) szTemp, NULL, SList->Name , MAX_PATH, szFullPathFileName );
		m_SList.AddNode(SList);
	}
}

CLoginProcess::~CLoginProcess()
{
	DeleteObject(m_hBrush);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Load
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CLoginProcess::Load(void)
{	
	int nSrvCount;

	m_Image.NewLoad(IMAGE_INTERFACE_1, TRUE);

	ZeroMemory(g_xChatEditBox.m_szInputMsg,sizeof(g_xChatEditBox.m_szInputMsg));

	pPatch = new CPatch;
	pPatch->InitPatch();
	ShowWindow(g_xMainWnd.GetSafehWnd(), SW_SHOW);

	SendMessage(g_xChatEditBox.GetSafehWnd(),EM_SETPASSWORDCHAR,NULL,0);

	// Create Server Select Btn
	nSrvCount = m_SList.GetCounter();
	m_xAvi.Create(&m_Image);
	m_xLogin.Create(&m_Image);
	m_xSelectSrv.Create(&m_Image,&m_SList);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Message
//////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CLoginProcess::DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_CTLCOLOREDIT:
		{
			if ((HWND)lParam == g_xChatEditBox.GetSafehWnd())
			{
				SetBkColor((HDC)wParam, RGB(0, 0, 0));
				SetTextColor((HDC)wParam, RGB(255, 255, 255));

				UnrealizeObject(m_hBrush);
				
				POINT pt;

				SetBrushOrgEx((HDC)wParam, 0, 0, &pt);
				return (LRESULT)m_hBrush;
			}
			break;
		}
		case WM_LBUTTONDOWN:
			OnLButtonDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(wParam, lParam);
			break;
		case ID_SOCKCLIENT_EVENT_MSG:
			g_xLoginSocket.OnSocketMessage(wParam, lParam);
			break;
		case WM_KEYDOWN:
			UpdateWindow(g_xMainWnd.GetSafehWnd());
			OnKeyDown(wParam, lParam);
			break;
		case WM_SYSKEYDOWN:
		{
			if ( wParam == VK_RETURN )
			{
				//g_xMainWnd.OnSysKeyDown(wParam, lParam);
			}
			return 0L;
		}
		case WM_MOVE:
		{
			OnWindowMove(wParam,lParam);
			return 0L;
		}
		case WM_MOUSEMOVE:
			OnMouseMove(wParam,lParam);
			break;
		case _WM_USER_MSG_INPUTTED:
		{
			switch(m_Progress)
			{
				case PRG_LOGIN:
				case PRG_NEW_ACCOUNT:
					OnKeyDown((WPARAM)VK_RETURN,NULL);
					break;
				case PRG_CHANGE_PASS:
					break;
			}
		}
	}
	return CWHDefProcess::DefMainWndProc(hWnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Event
//////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CLoginProcess::OnKeyDown(WPARAM wParam,LPARAM lParam)
{
	if(!m_xMsgBox.IsActive())
	{
		switch(m_Progress)
		{
			case PRG_LOGIN:// LOGIN 일경우
			{
				m_xLogin.OnKeyDown(wParam,lParam);
				break;
			}
			case PRG_NEW_ACCOUNT:
			{
				if (wParam == VK_RETURN || wParam == VK_TAB)
				{
					SetFocusBefore();
					m_UserState = (m_UserState+1) > 16 ? 0 : m_UserState + 1;
					SetFocusAfter();
				}
				break;
			}
		}
	}
	return 0L;
}

LRESULT CLoginProcess::OnLButtonDown( WPARAM wParam, LPARAM lParam )
{	
//	RECT	tRect;
//	int i;
	if( m_xMsgBox.IsActive() )
	{
		POINT ptMouse;
		ptMouse.x = LOWORD( lParam );
		ptMouse.y = HIWORD( lParam );		
		m_xMsgBox.OnButtonDown( ptMouse );
	}
	else
	{
		switch( m_Progress )
		{
			case PRG_LOGIN:
			{
				// Input Area Click
				m_xLogin.OnButtonDown( wParam, lParam );
				break;
			}
			case PRG_SERVER_SELE:
			{
				m_xSelectSrv.OnButtonDown( wParam, lParam );
				break;
			}
			case PRG_NEW_ACCOUNT:
			{
				break;
			}
		}
	}
	return 0L;
}

LRESULT CLoginProcess::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{	
//	int i;
//	INT MsgNum;
	POINT	ptMouse;

	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);

	if(m_fIsConnected && !m_xMsgBox.IsActive())
	{
		m_fIsLButtonClicked = FALSE;
		switch(m_Progress)
		{
			case PRG_LOGIN:
			{
				m_xLogin.OnButtonUp(wParam, lParam);
				break;
			}// case
			case PRG_SERVER_SELE:
			{
				m_xSelectSrv.OnButtonUp(wParam, lParam);
				break;
			}
			case PRG_NEW_ACCOUNT:
			{
/*				for( i = BTN_NEW_OK_ID ; i <= BTN_NEW_QUIT_ID ; i++ )
				{
					if ( m_pBtn[i].CheckMouseOn( LOWORD( lParam ), HIWORD( lParam ) ) )
					{
						switch( m_pBtn[i].m_nButtonID )
						{
						case BTN_NEW_OK:
							{
								int Count;
								BOOL IsNotFilled = FALSE;

								Count = (sizeof(g_LoginInputState) / sizeof(INPUTSTATE));

								SetFocusBefore();

								for(int i = LGM_INPUT_USER_ID ; i <= LGM_INPUT_ANSWER_2 ; i++)
								{
									if(strlen(g_LoginInputState[i-8].szData) == 0 )
									{
										IsNotFilled= TRUE;
										break;
									}
								}
								
								if(IsNotFilled)
								{
									MsgNum = ERROR_STR_NEW_ACCOUNT_5;
								}
								else
								{
									if(strcmp( g_LoginInputState[3].szData, g_LoginInputState[4].szData) != 0 )
									{	
										MsgNum=ERROR_STR_NEW_ACCOUNT_2;							
									}
									else
									{
										if(!CheckSSNO(g_LoginInputState[6].szData))
										{
											MsgNum=ERROR_STR_NEW_ACCOUNT_3;
										}
										else
										{
											
											if(!CheckEMail(g_LoginInputState[14].szData))
											{
												MsgNum=ERROR_STR_NEW_ACCOUNT_4;
											}
											else
											{
												//OnNewAccount();
												break;
											}
										}
									}
								}
								SetErrorMessage(MsgNum,m_szErrorMsg);
								m_xMsgBox.ShowMessageBox(m_szErrorMsg, 1, _WNDIMGINDX_MSGBOX1);
								break;
							}
						case BTN_NEW_CANCLE:
							{
								m_Progress = PRG_LOGIN;
								m_UserState = LGM_INPUT_ID;
								ZeroMemory(g_xChatEditBox.m_szInputMsg,sizeof(g_xChatEditBox.m_szInputMsg));
								SetWindowText(g_xChatEditBox.GetSafehWnd(),NULL);
								SetFocusAfter();
								break;
							}
						case BTN_NEW_QUIT:
							{
								m_Progress = PRG_LOGIN;
								m_UserState = LGM_INPUT_ID;
								ZeroMemory(g_xChatEditBox.m_szInputMsg,sizeof(g_xChatEditBox.m_szInputMsg));
								SetWindowText(g_xChatEditBox.GetSafehWnd(),NULL);
								SetFocusAfter();
								break;
							}
						}
					}
					m_pBtn[i].m_nState = BUTTON_STATE_UP;
				}*/
			break;
			}
		}
	}
	else if(m_xMsgBox.IsActive())
			if(m_xMsgBox.OnButtonUp(ptMouse)==1)
				m_xMsgBox.HideMessageBox();

	return 0L;
}

LRESULT CLoginProcess::OnMouseMove(WPARAM wParam, LPARAM lParam)
{	
//	int i;
	m_MousePos.x = LOWORD(lParam);
	m_MousePos.y = HIWORD(lParam);
//	g_xMainWnd.OnSetCursor(IDC_ARROW);
	if(m_xMsgBox.IsActive())
	{
		m_xMsgBox.MoveWnd(m_MousePos);	// Moon
		return 0l;
	}

	switch (m_Progress)
	{
	case PRG_CONNECT:
		break;
	case PRG_INTRO:
		m_xAvi.OnMouseMove( wParam, lParam);
		break;
	case PRG_LOGIN:					
		m_xLogin.OnMouseMove( wParam, lParam);
		break;
	case PRG_SERVER_SELE:
		m_xSelectSrv.OnMouseMove( wParam, lParam);
		break;
	}

/*
	if(m_fIsConnected && !m_fIsLButtonClicked)
	{
		for( i = BTN_NEW_ID ; i <= BTN_NEW_QUIT_ID ; i ++)
		{
			if (m_pBtn[i].CheckMouseOn(LOWORD(lParam), HIWORD(lParam)))
				m_pBtn[i].m_nState = BUTTON_STATE_ON;
			else
				m_pBtn[i].m_nState = BUTTON_STATE_UP;
		}
	}*/
	return 0L;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Socket
//////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CLoginProcess::OnConnectToServer()
{
	SetFocus(g_xChatEditBox.GetSafehWnd());

	m_fIsConnected	= CONNECT_SUCCESS;			// Connection Success
}

char* CLoginProcess::OnMessageReceive(CHAR* pszMessage)
{
	char	*pszFirst = pszMessage;
	char	*pszEnd;

	unsigned char sb[1024];
	if(*pszFirst != '#')
	{
		char* KEYS = "SoftSmile";
		unsigned char lastchar = 0;
		for (size_t i = 0; i < strlen(pszMessage)/2; i++)
		{
			unsigned int b = 0;
			sscanf_s(pszMessage + i * 2, "%02X", &b);
			if (i == 0)
			{
				lastchar = b;
				continue;
			}
			unsigned char k = KEYS[(i-1) % strlen(KEYS)];
			short t = k ^ b;
			if (t < lastchar) t += 0xFF;
			t -= lastchar;
			t = t & 0xFF;
			sb[i - 1] = t;
			sb[i] = 0;
			lastchar = b;
		}
		pszFirst = (char*)sb;
	}

	while (pszEnd = strchr(pszFirst, '!'))
	{
		*pszEnd = '\0';

		OnSocketMessageRecieve(pszFirst + 1);

		if (*(pszEnd + 1) == '#')
			pszFirst = pszEnd + 1;
		else
			return NULL;
	}
	return pszFirst;
}

void CLoginProcess::OnSocketMessageRecieve(char *pszMsg)
{
	INT	nMsgNum;
	_TDEFAULTMESSAGE	tdm;

	fnDecodeMessage(&tdm, pszMsg);

	switch(tdm.wIdent)
	{
		// ID Not Found
		case SM_ID_NOTFOUND:
		{
			m_Progress	= PRG_LOGIN;
			SetErrorMessage(23,m_szErrorMsg);
			m_xMsgBox.ShowMessageBox(m_szErrorMsg, 1, _WNDIMGINDX_MSGBOX1);
			break;
		}
		// Incorrect Password
		case SM_PASSWD_FAIL:
		{
			m_Progress	= PRG_LOGIN;
			
			if(tdm.nRecog==(-2))
				nMsgNum = ERROR_STR_LOGIN_3;
			else
				nMsgNum = ERROR_STR_LOGIN_2;
			SetErrorMessage(nMsgNum,m_szErrorMsg);
			m_xMsgBox.ShowMessageBox(m_szErrorMsg, 1, _WNDIMGINDX_MSGBOX1);
			break;
		}

		case SM_CERTIFICATION_FAIL:
		{
			SetErrorMessage(FATAL_ERROR_STR,m_szErrorMsg);
			m_xMsgBox.ShowMessageBox(m_szErrorMsg, 1, _WNDIMGINDX_MSGBOX1);			// 용기
			break;
		}

		case SM_PASSOK_SELECTSERVER:
		{
			m_xSelectSrv.OpenWnd();
			char version[32] = { 0 };
			fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), version, sizeof(version));

			m_Progress = PRG_SERVER_SELE;
			Clear(RGB(0,0,0));
//			g_xLoginSocket.OnSelectServer(NULL);

			break;
		}

		case SM_NEWID_SUCCESS:	
		{
			m_Progress = PRG_LOGIN;
			m_UserState = LGM_INPUT_ID;
			ZeroMemory(g_xChatEditBox.m_szInputMsg,sizeof(g_xChatEditBox.m_szInputMsg));
			SetFocusAfter();

			SetErrorMessage(NEW_ACCOUNT_WELL_DONE,m_szErrorMsg);
			m_xMsgBox.ShowMessageBox(m_szErrorMsg, 1, _WNDIMGINDX_MSGBOX1);
			break;
		}
		// New ID Create Fail
		case SM_NEWID_FAIL:
		{
			m_Progress	= PRG_NEW_ACCOUNT;

			switch(tdm.nRecog)
			{
			case 1:
				nMsgNum=ERROR_STR_NEW_ACCOUNT_1;
				break;
			case 2:
				nMsgNum=ERROR_STR_NEW_ACCOUNT_6;
				break;
			case 3:
				nMsgNum=ERROR_STR_NEW_ACCOUNT_7;
				break;
			}
			SetErrorMessage(nMsgNum,m_szErrorMsg);
			m_xMsgBox.ShowMessageBox(m_szErrorMsg, 1, _WNDIMGINDX_MSGBOX1);
			break;
		}
		// Change Password Success
		case SM_CHGPASSWD_SUCCESS:
		{
			m_Progress	= PRG_LOGIN;
			break;
		}
		// Change Passsword Fail
		case SM_CHGPASSWD_FAIL:
		{
			m_Progress	= PRG_CHANGE_PASS;	
			break;	
		}

		case SM_SELECTSERVER_OK:
		{
			char szDecodeMsg[256];
			char *pszIP = szDecodeMsg, *pszPort, *pszIdenty;

			ZeroMemory(szDecodeMsg, sizeof(szDecodeMsg));

			fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));

			if (pszPort = strchr(szDecodeMsg, '/')){
				*pszPort = '\0';
				pszPort++;

				if (pszIdenty = strchr(pszPort, '/')){
					*pszIdenty = '\0';
					pszIdenty++;

					strcpy_s(g_szServerIP, pszIP);
					g_nServerPort = atoi(pszPort);

					m_nAnimationCounter = 0;
					
					g_nCertifyCode = tdm.nRecog;
					g_xLoginSocket.DisconnectToServer();

					Clear(RGB(0,0,0));
					m_Progress	= PRG_TO_SELECT_CHR;
				}
			}
			break;
		}
		// Connection Lost(close Connection)
		case	SM_OUTOFCONNECTION:
		{
			m_Progress	= PRG_QUIT;									// Quit for Some Reason		프로그램 종료
			SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
			break;
		}
	}
/*	
	switch(tdm.wIdent)
	{
		case	SM_NEEDUPDATE_ACCOUNT:	// Need to account update
			m_Progress	= PRG_NEED_ACCOUNT_CHANGE;	// Need Account change
			break;
		case	SM_UPDATEID_SUCCESS:	// account update success
			m_Progress	= PRG_LOGIN;		// Login
			break;
		case	SM_UPDATEID_FAIL:		// account update Fail
			m_Progress	= PRG_NEED_ACCOUNT_CHANGE;	// Login
			break;
	}
*/
}

VOID CLoginProcess::SetFocusBefore(VOID)
{
	INT nTemp;

	nTemp = (( m_Progress == PRG_LOGIN) ? m_UserState - 1 : m_UserState + 2 );

	ZeroMemory(g_xChatEditBox.m_szInputMsg,sizeof(g_xChatEditBox.m_szInputMsg));
	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
	g_xChatEditBox.SetSelectAll();
}

VOID CLoginProcess::SetFocusAfter(VOID)
{
	CHAR	cChr;
	INT		nTemp;

	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_SHOW);

	nTemp = (( m_Progress == PRG_LOGIN) ? m_UserState - 1 : m_UserState + 2 );


	if( ( m_UserState != LGM_INPUT_PASSWORD ) &&
		( m_UserState != LGM_INPUT_USER_PASSWORD ) &&
		( m_UserState != LGM_INPUT_REPASS ) )
		cChr = NULL;
	else
		cChr = '*';

	SendMessage(g_xChatEditBox.GetSafehWnd(),EM_SETPASSWORDCHAR,(WPARAM)cChr,0);

	SetFocus(g_xChatEditBox.GetSafehWnd());
	g_xChatEditBox.SetSelectAll();
}

VOID CLoginProcess::SetNextProc()
{
	DeleteProc();
	g_xLoginSocket.m_pxDefProc = g_xMainWnd.m_pxDefProcess = &g_xChrSelProc;
	g_xChrSelProc.Load();
	g_bProcState = _CHAR_SEL_PROC;
}

VOID CLoginProcess::DeleteProc()
{
	m_xAvi.StopAllAvis();
//	m_Image2.Destroy();
	m_Image.Destroy();
}

LRESULT CLoginProcess::OnWindowMove(WPARAM wParam,LPARAM lParam)
{
	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
	g_xMainWnd.OnMove(wParam, lParam);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Rander
//////////////////////////////////////////////////////////////////////////////////////////////////////

VOID CLoginProcess::RenderScene(INT nLoopTime)
{
	static	DWORD	dwLastTick = 0;
	switch (m_Progress)
	{
		case PRG_QUIT:
		{
			SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
			return;
		}
		case PRG_PATCH:
		{
			RenderPatch(0);
			pPatch->DrawProgressImage();
			break;
		}
		default:
			break;
	}

	if(m_fIsConnected==CONNECT_SUCCESS)
	{
		if (m_Progress == PRG_INTRO)
		{
			m_xAvi.Render(nLoopTime);
		}
		else
		{
			m_Image.NewSetIndex(IMG_IDX_LOGIN_BACK);
			g_xMainWnd.DrawWithImageForComp(0, 0,
				m_Image.m_lpstNewCurrWilImageInfo->shWidth,
				m_Image.m_lpstNewCurrWilImageInfo->shHeight,
				(WORD*)(m_Image.m_pbCurrImage));
		}

		switch (m_Progress)
		{
			case PRG_CONNECT:
			{
				ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
				m_nAnimationCounter = 0;
				break;
			}
			case PRG_INTRO:
			{
				ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
				RenderIntro(nLoopTime);
				break;
			}
			// Render For Login
			case PRG_LOGIN:					
			{
				m_xLogin.Render(nLoopTime);
				break;
			}
			// Render 
			case PRG_TO_SELECT_CHR:
			{
				ShowWindow(g_xChatEditBox.GetSafehWnd(),SW_HIDE);
				RenderScroll(nLoopTime);
				break;
			}
			// Render For Server Selection
			case PRG_SERVER_SELE:
			{
				ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
				m_Image.NewSetIndex(IMG_IDX_LOGIN_BACK);					// 임시
				m_xSelectSrv.Render(nLoopTime);
				break;
			}
			// Render For New Account
			case PRG_NEW_ACCOUNT:
			{
				ShowWindow(g_xChatEditBox.GetSafehWnd(),( m_xMsgBox.IsActive() ? SW_HIDE : SW_SHOW ));
//				RenderNewAccount(nLoopTime);
				break;
			}
			// Render For Change Password
			case PRG_CHANGE_PASS:
			{
				ShowWindow(g_xChatEditBox.GetSafehWnd(),( m_xMsgBox.IsActive() ? SW_HIDE : SW_SHOW ));
				RenderPassword(nLoopTime);
				break;
			}
		}

	}
}


VOID CLoginProcess::RenderIntro(INT nLoopTime)
{
	static DWORD dwLastTick = 0;
	dwLastTick += nLoopTime;
	if(dwLastTick>100)
	{
		m_nAnimationCounter++;
		if(m_nAnimationCounter==50)
			m_xAvi.SetRenderAviState(_RENDER_AVI_INTRO);
		if(m_nAnimationCounter>50 && !m_xAvi.IsPlaying())
		{
			m_Progress = PRG_LOGIN;
			m_xLogin.OpenWnd();
			m_nAnimationCounter = -999;
		}

	}

}



VOID CLoginProcess::RenderNewAccount(int nLoopTime)
{
	/*char Pass[10];

	MoveWindow(g_xChatEditBox.GetSafehWnd(),
						g_xMainWnd.m_rcWindow.left + g_LoginInputState[m_UserState+2].Left,
						g_xMainWnd.m_rcWindow.left +  g_LoginInputState[m_UserState+2].Top,
						g_LoginInputState[m_UserState+2].Width,
						g_LoginInputState[m_UserState+2].Height, TRUE);
	// Draw Back Image
	g_xMainWnd.DrawWithImageForComp(0, 0,
					m_Image2.m_lpstNewCurrWilImageInfo->shWidth,
					m_Image2.m_lpstNewCurrWilImageInfo->shHeight,(WORD*)(m_Image2.m_pbCurrImage));

	// Draw Image
	m_Image2.NewSetIndex(IMAGE_INDEX_NEWACCOUNT);
	g_xMainWnd.DrawWithImageForComp(ACCOUNT_WIN_POS_X, ACCOUNT_WIN_POS_Y,
						m_Image2.m_lpstNewCurrWilImageInfo->shWidth,
						m_Image2.m_lpstNewCurrWilImageInfo->shHeight,(WORD*)(m_Image2.m_pbCurrImage));

	for (int i = BTN_NEW_OK_ID; i <= BTN_NEW_QUIT_ID; i++){

		if (m_pBtn[i].m_nState != BUTTON_STATE_UP) {

			m_Image2.NewSetIndex(m_pBtn[i].m_nButtonID + (m_pBtn[i].m_nState - 1 ) / 2 );
			g_xMainWnd.DrawWithImageForComp(m_pBtn[i].m_Rect.left, m_pBtn[i].m_Rect.top, 
										m_pBtn[i].m_Rect.right - m_pBtn[i].m_Rect.left, 
										m_pBtn[i].m_Rect.bottom - m_pBtn[i].m_Rect.top, (WORD*)(m_Image2.m_pbCurrImage));
			break;
		}
	}

	RenderNewAccountHelpTxt();

	for(i =LGM_INPUT_USER_ID ; i <= LGM_INPUT_ANSWER_2 ; i++){

		if( ( i != LGM_INPUT_USER_PASSWORD  ) && ( i!= LGM_INPUT_REPASS ) ){
			g_xMainWnd.PutsHan(NULL, g_LoginInputState[i].Left +2, g_LoginInputState[i].Top +2, RGB(255,255,255), RGB(0,0,0),g_LoginInputState[i].szData);
		}
		else{
			ZeroMemory(Pass,sizeof(Pass));
			memset(Pass,'*',strlen(g_LoginInputState[i].szData));
			g_xMainWnd.PutsHan(NULL, g_LoginInputState[i].Left +2, g_LoginInputState[i].Top +2, RGB(255,255,255), RGB(0,0,0), Pass);
		}
	}*/
}


VOID CLoginProcess::RenderPatch(int nLoopTime)
{	
	RECT tRect;

	if(pPatch->m_bEndPatch == TRUE)//GetFtpFiles()==0l)	// Patch가 종료 되었으면
	{
		
		if(pPatch->m_bPatched == TRUE)	// 唐뫘劤깻뫘劤供냥，폘痰뫘劤뇹잿넋埼。
		{
			char PatchName[1024];
			ZeroMemory(PatchName,1024);
			GetCurrentDirectory(1024,PatchName);
			strcat_s(PatchName,MIR2_PATCH_FILE_NAME);
			m_Progress = PRG_QUIT;
			ShellExecute(NULL,"open",PatchName,NULL,NULL,SW_SHOWNORMAL);
		}
		else
		{
			g_xLoginSocket.ConnectToServer(g_xMainWnd.GetSafehWnd(), m_szServerIP, m_nServerPort, ID_SOCKCLIENT_EVENT_MSG);
			m_Progress = PRG_INTRO;
			strcpy_s(g_szLoginServerIP, m_szServerIP);
			g_nLoginServerPort = m_nServerPort;

			g_xMainWnd.ResetSize(_PATCH_SCREEN_WIDTH, _PATCH_SCREEN_HEIGHT);
		}
		SAFE_DELETE(pPatch);
	}
	else
	{
		// 계속 패치 해야하면
		pPatch->GetFtpFiles();
	}

	if(m_xMsgBox.IsActive())
	{
		HINSTANCE hLib;
		hLib = LoadLibrary(MESSAGE_DLL_1);
		SetErrorMessage(300,m_szErrorMsg);		// 헤당 Error 메시지 찾기
		FreeLibrary(hLib);
		MessageBox(g_xMainWnd.GetSafehWnd(),m_szErrorMsg,"Error",MB_OK);
	}
}

VOID CLoginProcess::RenderPassword(int nLoopTime)
{
	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
}

VOID CLoginProcess::RenderNewAccountHelpTxt(VOID)
{

}


VOID CLoginProcess::RenderScroll(INT nLoopTime)
{
	static INT	nScroll = 0;
	static INT	nDelay = 0;
	nDelay += nLoopTime;


	if(nDelay > 40)
	{	nScroll += 5;
		if(nScroll >= 180)
		{	
			SetNextProc();												// 케릭터 셀렉트 서버로 이동 (임시)
			return;
		}
	}

//	m_Image.NewSetIndex(IMG_IDX_LOGIN_BACK);
//	g_xMainWnd.DrawWithImageForCompClipRgn(SCR_LEFT, SCR_TOP + nScroll, SCR_RIGHT, SCR_BOTTOM, (WORD*)(m_Image.m_pbCurrImage),SCR_LEFT,SCR_BOTTOM-nScroll);
}


BOOL CLoginProcess::CheckSSNO(char* szSSNo)
{	// 미완
	int		nLen = 0, nYear = 0;
	long	nSum = 0, nCheckSum = 0;
	char	szDate[7], szYear[5], szSex[5] = "1234";
	int		nTemp;
	int		Counter=1;
	int		i;
	CHAR*	pszLowCode = NULL;
	CHAR	sztSSNo[15];

	strcpy_s(sztSSNo,szSSNo);

	pszLowCode = strchr(sztSSNo,'-');
	if(pszLowCode==NULL)
		return FALSE;								// 주민등록번호 상에 -이 없으면 오류처리
	pszLowCode ++;

	nLen = strlen( sztSSNo );
	if(nLen != 14)
		return FALSE;
	
	// 성별 코드 확인
	if((*pszLowCode - '0') < 0 || (*pszLowCode - '0') > 4 ) 
		return FALSE;								// 성별 코드 맞지 않음

	strncpy_s(szDate,sztSSNo,6);
	strncpy_s(szYear,sztSSNo,2);
	nYear= atoi(szYear);
	nCheckSum = *( sztSSNo + nLen - 1 ) - '0';

	memcpy(pszLowCode-1,pszLowCode,strlen(pszLowCode));

	for( i = 0 ; i < nLen - 1 ; i++ )
	{
		if(szSSNo[i]=='-')
			return FALSE;
		nTemp = (sztSSNo[i]-'0')*(Counter>=9? Counter=2:++Counter);
		nSum += nTemp;
	}

	nSum = nSum % 11;
	nSum = 11 - nSum;
	if( nCheckSum != nSum )
		return FALSE;
	return TRUE;
}

BOOL CLoginProcess::DateCehck( char* szDate )
{
	char szYear[5],szMonth[3],szDay[3];
	int nYear,nMonth,nDay;
	int tDay;

	if ( strlen( szDate ) != 8)
		return FALSE;
	
	strncpy_s( szYear, szDate, 4 );
	strncpy_s( szMonth, szDate + 4, 2 );
	strncpy_s( szDay, szDate + 6, 2 );

	nYear	= atoi( szYear );
	nMonth	= atoi( szMonth );
	nDay	= atoi( szDay );

	// Check Month's days
	switch( nMonth )
	{
		case 1:	case 3:	case 5:	case 7: case 8: case 10:case 12:
			tDay = 31;
			break;
		case 4: case 6: case 9: case 11:
			tDay = 30;
			break;
		case 2:
			tDay = IsLeapYear( nYear ) ? 29 : 28;
			break;
		default :
			return FALSE;
	}

	if( nDay < 1 || nDay > tDay ) return FALSE;

	return TRUE;
}

BOOL CLoginProcess::IsLeapYear( int nYear )
{
	if ( ( nYear % 4 ) == 0){

		if ( ( nYear % 100 ) == 0){

			if ( ( nYear % 400 ) == 0)
				return TRUE;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CLoginProcess::CheckEMail( char* szEMail )	// E Mail Check
{
	CHAR* pszTemp = NULL;

	// '@'는 맨앞에 올수 없다. '@'는 꼭 포함 하고 있어야 한다.
	pszTemp = strchr( szEMail , '@');
	if( ( ( pszTemp - szEMail ) == 0 ) ||
		( pszTemp == NULL ) )
		return FALSE; 

	// '@'는 하나이상 가질수 없다.
	pszTemp = strchr( pszTemp + 1 ,'@');
	if( pszTemp != NULL ) return FALSE; 

	// '.'는 맨앞에 올수 없다. '.'는 하나이상 꼭 포함 하고 있어야 한다.
	pszTemp = strchr(szEMail,'.');
	if( ( ( pszTemp - szEMail ) == 0 ) ||
		( pszTemp == NULL ) )
		return FALSE; 
	
	return TRUE;
}

BOOL CLoginProcess::StringDivideLen( INT nDivideLen, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr )
{
	CHAR	szTmpCheck[MAX_PATH];
	CHAR	szResult[MAX_PATH];
	INT		nCnt; 
	
	nDividedLine = 1;

	INT nStartLen	= 0;
	INT nEndLen		= 0;
	INT nResultLen	= 0;
	INT nWordCheck	= 0;

	if ( szSrc[0] != NULL ){
		ZeroMemory(szResult,MAX_PATH);

		for ( nCnt = 0; nCnt < (INT) strlen( szSrc ); nCnt++){
			ZeroMemory(szTmpCheck, MAX_PATH);
			nEndLen = nCnt+1;

			// 현재부분까지의 문자열을 읽는다.
			memcpy(&szTmpCheck, &szSrc[nStartLen], nEndLen-nStartLen);
			INT nsLen;
			nsLen = strlen(szTmpCheck);

			// 현재까지 읽어온 부분이 허용넓이를 넘어섰을때.
			if (nsLen > nDivideLen ){
				// 현재 2바이트 문자라면. 
				if ( szSrc[nEndLen-1] < 0 ){
					// 현재 앞에문자가 2바이트 문자가 아니라면
					if ( !(nWordCheck%2) ){
						CHAR* pszNewLine;
						nStartLen += strlen(szTmpCheck)-1;
						nCnt--;
						pszNewLine = new CHAR[nResultLen+1];
						memcpy(pszNewLine,szResult,nResultLen);
						pszNewLine[nResultLen]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
					}
					else{
						CHAR* pszNewLine;
						nStartLen += strlen(szTmpCheck)-2;
						nCnt -= 2;
						pszNewLine = new CHAR[nResultLen];
						memcpy(pszNewLine,szResult,nResultLen-1);
						pszNewLine[nResultLen-1]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
						nWordCheck--;
					}
				}
				// 1바이트 문자. 
				// 현재보다 한바이트앞부분까지만 고려해주면 된다.
				else{
					CHAR* pszNewLine;
					nStartLen += strlen(szTmpCheck)-1;
					nCnt--;
					pszNewLine = new CHAR[nResultLen+1];
					memcpy(pszNewLine,szResult,nResultLen);
					pszNewLine[nResultLen]=NULL;
					m_pxpStr->AddNode(pszNewLine);
					nResultLen=0;
					nDividedLine++;
				}
			}
			// 강제개행     \n으로하면  Error ????
			else if(szSrc[nEndLen-1]=='\n'){
				CHAR* pszNewLine;
				nStartLen += strlen(szTmpCheck)-1;
				pszNewLine = new CHAR[nResultLen+1];
				memcpy(pszNewLine,szResult,nResultLen);
				pszNewLine[nResultLen]=NULL;
				m_pxpStr->AddNode(pszNewLine);
				nResultLen=0;
				nDividedLine++;
			}
			else{
				if ( szSrc[nEndLen-1] < 0 )
					nWordCheck++;

				szResult[nResultLen] = szSrc[nEndLen-1];
				nResultLen++;
			}
		}
		if( nResultLen != 0 ){
			CHAR* pszNewLine;
			pszNewLine = new CHAR[nResultLen+1];
			memcpy(pszNewLine,szResult,nResultLen);
			pszNewLine[nResultLen]=NULL;
			m_pxpStr->AddNode(pszNewLine);
			nDividedLine++;
			nResultLen=0;
		}
		return TRUE;
	}
	return FALSE;
}

VOID CLoginProcess::SetErrorMessage(INT nMsg, CHAR* szMgs)
{
	HINSTANCE hLib;

	hLib = LoadLibrary(MESSAGE_DLL_1); // Load Dll
	
	if ( nMsg!=99 && nMsg!=0 )
		LoadString(hLib, nMsg+MESSAGE_NUM_LOGIN, m_szErrorMsg, 512);	
	else if( nMsg!=0 )	
		LoadString(hLib, 9999, m_szErrorMsg, 512);
	else
		szMgs[0]=NULL;

	FreeLibrary(hLib);
}

