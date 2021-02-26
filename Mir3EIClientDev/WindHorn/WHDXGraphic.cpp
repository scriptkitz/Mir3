#include "stdafx.h"
#include "WHEngine.h"

#define _DX_DIP 64.0f

#define COLOR_B16TOB32(w, s) (DWORD)((((w & 0xF800) >> 8) << 0x10) | (((w & 0x07E0) >> 3) << 0x8) | ((w & 0x001F) << 3) | (s*255/100 << 24))

static BYTE						g_bNumDevices = 0;
static DXG_ENUM_DEVICEINFO		g_stDXGEnumDeviceInfo[_MAX_DEVICES];


CWHDXGraphicWindow* CWHDXGraphicWindow::m_pxDXGWnd = NULL;
D2D1_BITMAP_PROPERTIES CWHDXGraphicWindow::s_BitmapProps;

CWHDXGraphicWindow* GetDXGWindowPtr()
{
	return CWHDXGraphicWindow::m_pxDXGWnd;
}

CWHDXGraphicWindow::CWHDXGraphicWindow(WORD wWidth, WORD wHeight, WORD wBpp)
{
	m_pDD				= NULL;
	m_pD3D				= NULL;
	m_hDefGameFont		= NULL;
	m_pd3dDevice		= NULL;
	m_pddsZBuffer		= NULL;
    m_pddsFrontBuffer	= NULL;
    m_pddsBackBuffer	= NULL;
//	?????? ??? ??? ???? ????.
	m_lpcClipper		= NULL;

	m_pD2D1Factory1 = NULL;

	m_stDisplayInfo.wWidth	= wWidth;
	m_stDisplayInfo.wHeight	= wHeight;
	m_stDisplayInfo.wBPP	= wBpp;
	m_bScreenModeFlag		= _DXG_SCREENMODE_WINDOW;
	m_bDeviceModeFlag		= _DXG_DEVICEMODE_PRIMARY;

	SetRect(&m_rcWindow, 0, 0, 0, 0);
	ZeroMemory(&m_stBitsMaskInfo, sizeof(DXG_MASKINFO));

	m_pxDXGWnd = this;

	m_bIsWindowActive = FALSE;
	m_bIsWindowReady = FALSE;

	m_dwTextureTotal	= 0;
	m_dwTextureFree		= 0;
	m_dwVideoTotal		= 0;
	m_dwVideoFree		= 0;

	m_pxDefProcess = NULL;


	s_BitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	s_BitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	s_BitmapProps.dpiX = s_BitmapProps.dpiY = _DX_DIP;
}
VOID CWHDXGraphicWindow::UsedAndFreeMemoryCheck()
{
	DDSCAPS2     ddsCaps2; 

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_TEXTURE;
	m_pDD->GetAvailableVidMem(&ddsCaps2, &m_dwTextureTotal, &m_dwTextureFree); 

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY;
	m_pDD->GetAvailableVidMem(&ddsCaps2, &m_dwVideoTotal, &m_dwVideoFree); 
} 
CWHDXGraphicWindow::~CWHDXGraphicWindow()
{
	FreeDXGEnumModeResources();
    DestroyDXGObjects();

	m_pxDefProcess = NULL;
	m_pxDXGWnd = NULL;
}
BOOL CWHDXGraphicWindow::Create(HINSTANCE hInst, LPTSTR lpCaption, CHAR *pszMenuName, CHAR* pszIconName, BYTE bScreenModeFlag, BYTE bDeviceModeFlag)
{
	DWORD	dwStyle;

	if ( !CWHWindow::Create(hInst, lpCaption, pszMenuName, pszIconName) )
	{
		MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::Create]") TEXT("Window create failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	m_bScreenModeFlag		= bScreenModeFlag;
	m_bDeviceModeFlag		= bDeviceModeFlag;

	if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_POPUP;//WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}
	else
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~dwStyle;
		dwStyle |= WS_POPUP;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}

	if (SUCCEEDED(CreateDXG()))
	{
		m_bIsWindowReady = TRUE;
		CreateDefFont();
		return TRUE;
	}
	else
	{
		MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::Create]") TEXT("DirectGraphic create failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return FALSE;
	}


	return FALSE;
}
LRESULT CWHDXGraphicWindow::MainWndProcDXG(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
		case WM_SYSKEYDOWN:
			return OnSysKeyDown(wParam, lParam);

		case WM_MOVE:
			return OnMove(wParam, lParam);

		case WM_SIZE:
			return OnSize(wParam, lParam);

		case WM_DESTROY:
			return OnDestroy();

        case WM_SETCURSOR:
			return OnSetCursor();   

 //      case WM_GETMINMAXINFO:			
//			return OnGetMinMaxInfo(wParam, lParam);

		default:
			break;
	}

    return CWHWindow::MainWndProc(hWnd, uMsg, wParam, lParam);
}
LRESULT CWHDXGraphicWindow::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
{
	if ( m_hWnd )
	{
		MINMAXINFO* pMinMax = (MINMAXINFO*) lParam;

		DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
		DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
		DWORD dwMenuHeight;
		if( GetMenu( m_hWnd ) )
			dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
		else
			dwMenuHeight    = 0;					

		DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

		pMinMax->ptMinTrackSize.x = m_stDisplayInfo.wWidth  + dwFrameWidth * 2;
		pMinMax->ptMinTrackSize.y = m_stDisplayInfo.wHeight + dwFrameHeight * 2 + 
									dwMenuHeight + dwCaptionHeight;

		pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
		pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
	}

    return 0L;
}
LRESULT CWHDXGraphicWindow::OnSetCursor()
{
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return 0L;
}
LRESULT CWHDXGraphicWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
    if ( SIZE_MAXHIDE == wParam || SIZE_MINIMIZED == wParam )
        m_bIsWindowActive = FALSE;
    else
        m_bIsWindowActive = TRUE;
	UpdateBoundsWnd();
	if (m_pDXGISwapChain4)
	{
		HRESULT hr = S_OK;
		_ReleaseSurface();
		m_pD2D1DeviceContext->EndDraw();
		m_pD3D11DeviceContext->ClearState();
		int w = m_rcWindow.right - m_rcWindow.left;
		int h = m_rcWindow.bottom - m_rcWindow.top;
		//BufferCount 0表示保留交换链存在的缓冲区数量
		//Width 0表示使用目标窗口的客户区宽度
		//Height 0表示使用目标窗口的客户区高度
		//NewFormat DXGI_FORMAT_UNKNOWN表示保留交换链存在的格式
		hr = m_pDXGISwapChain4->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE| DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
#ifdef _DEBUG
			char buff[64] = {};
			sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n",
				(hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pD3D11Device->GetDeviceRemovedReason() : hr);
			OutputDebugStringA(buff);
#endif // _DEBUG
			//如果设备已删除,需要创建新的设备和交换链。
			HandleDeviceLost();
			//现在一切都已设置。不要继续执行此方法，HandleDeviceLost将重新输入此方法并正确设置新设备。

		}
			
		_CreateSurface();
	}

 	return 0L;
}
LRESULT CWHDXGraphicWindow::OnMove(WPARAM wParam, LPARAM lParam)
{
	UpdateBoundsWnd();
	return 0L;
}
LRESULT CWHDXGraphicWindow::OnDestroy()
{
	return CWHWindow::OnDestroy();
}
LRESULT CWHDXGraphicWindow::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_RETURN )
	{
		m_bIsWindowReady = FALSE;

 		D3DTextr_InvalidateAllTextures();

		D3DWILTextr_InvalidateAllTextures();
		D3DWILTextr_DestroyAllTextures();

		if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
		{
			if ( FAILED(ResetDXG(_SCREEN_WIDTH, _SCREEN_HEIGHT, _SCREEN_BPP, _DXG_SCREENMODE_FULLSCREEN, _DXG_DEVICEMODE_PRIMARY|_DXG_DEVICEMODE_D3D)) )
			{
				MessageBox(m_hWnd, TEXT("DirectDraw RESETUP failed!!!") TEXT("The Program will now exit."), TEXT("MirDXG"), MB_ICONERROR | MB_OK);
			}
		}
		else
		{
			if ( FAILED(ResetDXG(_SCREEN_WIDTH, _SCREEN_HEIGHT, _SCREEN_BPP, _DXG_SCREENMODE_WINDOW, _DXG_DEVICEMODE_PRIMARY|_DXG_DEVICEMODE_D3D)) )
			{
				MessageBox(m_hWnd, TEXT("DirectDraw RESETUP failed!!!") TEXT("The Program will now exit."), TEXT("MirDXG"), MB_ICONERROR | MB_OK);
			}
		}

		D3DTextr_RestoreAllTextures(m_pd3dDevice);

		m_bIsWindowReady = TRUE;
	}

	return 0L;
}
HRESULT CWHDXGraphicWindow::DestroyDXGObjects()
{
	//D2D1
	{
		SAFE_RELEASE(m_pD2D1Factory1);
	}

	HRESULT	hr;
	LONG	lDDRefCnt  = 0L;
    LONG	lD3DRefCnt = 0L;

	if ( !(m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW) )
	{
		if ( FAILED(hr = m_pDD->RestoreDisplayMode()) )
			return E_FAIL;
	}
    if ( m_pDD )
	{
		if ( FAILED(hr = m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL)) )
			return E_FAIL;
	}

    if ( m_pd3dDevice )
	{
        if ( 0 < ( lD3DRefCnt = m_pd3dDevice->Release() ) )
			return E_FAIL;
	}
    m_pd3dDevice = NULL;

    SAFE_RELEASE(m_pddsBackBuffer);
    SAFE_RELEASE(m_pddsFrontBuffer);
    SAFE_RELEASE(m_pddsZBuffer);
//	?????? ??? ??? ???? ????.
	SAFE_RELEASE(m_lpcClipper);
    SAFE_RELEASE(m_pD3D);

    if ( m_pDD )
    {
        if ( 0 < ( lDDRefCnt = m_pDD->Release() ) )
			INT nAAAAAA = 0;
//			return E_FAIL;
    }
    m_pDD = NULL;

	if ( m_hDefGameFont != NULL )
	{
		DeleteObject(m_hDefGameFont);
		m_hDefGameFont = NULL; 
	}

	return S_OK;
//    return ( lDDRefCnt==0 && lD3DRefCnt==0 ) ? S_OK : E_FAIL;
}
HRESULT CWHDXGraphicWindow::_ReleaseSurface()
{
	m_pD2D1DeviceContext->SetTarget(nullptr);
	SAFE_RELEASE(m_pDXGISurface);
	SAFE_RELEASE(m_pD2D1Bitmap1);
	return S_OK;
}
HRESULT CWHDXGraphicWindow::_CreateSurface()
{
	HRESULT hr;
	if ((hr=m_pDXGISwapChain4->GetBuffer(0, IID_PPV_ARGS(&m_pDXGISurface)))==S_OK)
	{
		D2D1_BITMAP_PROPERTIES1 prop1;
		prop1.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		prop1.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		prop1.dpiX = prop1.dpiY = _DX_DIP;
		prop1.bitmapOptions = D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE | D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		prop1.colorContext = nullptr;
		hr = m_pD2D1DeviceContext->CreateBitmapFromDxgiSurface(m_pDXGISurface, &prop1, &m_pD2D1Bitmap1);

		m_pD2D1DeviceContext->SetTarget(m_pD2D1Bitmap1);
	}

	return hr;
}
VOID CWHDXGraphicWindow::FreeDXGEnumModeResources()
{	
    for ( BYTE bCnt = 0; bCnt < g_bNumDevices; bCnt++ )
    {
        SAFE_DELETE( g_stDXGEnumDeviceInfo[bCnt].pddsdModes );
    }
}
HRESULT CWHDXGraphicWindow::HandleDeviceLost()
{
	return S_OK;
}
HRESULT CWHDXGraphicWindow::CreateDXG() 
{
	HRESULT hr = S_OK;
	{
		hr = D3D11CreateDevice(nullptr,
#ifdef _DEBUG
			D3D_DRIVER_TYPE_REFERENCE, nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SINGLETHREADED,
#else
			D3D_DRIVER_TYPE_HARDWARE, nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED,
#endif // DEBUG
			nullptr, 0, D3D11_SDK_VERSION, &m_pD3D11Device, &m_FeatureLevel, &m_pD3D11DeviceContext);
		if (!SUCCEEDED(hr)) return hr;


		IDXGIDevice2* _dxgiDv2=nullptr;
		hr = m_pD3D11Device->QueryInterface(IID_PPV_ARGS(&_dxgiDv2));
		if (!SUCCEEDED(hr)) return hr;
		//确保DXGI一次不会排队超过一帧
		hr = _dxgiDv2->SetMaximumFrameLatency(1);
		if (!SUCCEEDED(hr)) return hr;
		UpdateBoundsWnd();

		IDXGIAdapter* _adapter;
		_dxgiDv2->GetAdapter(&_adapter);
		_adapter->GetParent(IID_PPV_ARGS(&m_pDXGIFactory4));
		//SAFE_RELEASE(_adapter);

		int w = m_rcWindow.right - m_rcWindow.left;
		int h = m_rcWindow.bottom - m_rcWindow.top;
		//创建交换链
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		swapChainDesc.Width = 0;
		swapChainDesc.Height = 0;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = FRAME_COUNT;
		swapChainDesc.Scaling = DXGI_SCALING_NONE; // DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc;
		//fsDesc.RefreshRate.Numerator = 0;
		//fsDesc.RefreshRate.Denominator = 0;
		//fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		//fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		//fsDesc.Windowed = TRUE;
		IDXGISwapChain1* dxgiSwapChain;
		hr = m_pDXGIFactory4->CreateSwapChainForHwnd(m_pD3D11Device, m_hWnd, &swapChainDesc, nullptr, nullptr, &dxgiSwapChain);
		if (!SUCCEEDED(hr)) return hr;

		dxgiSwapChain->QueryInterface(IID_PPV_ARGS(&m_pDXGISwapChain4));
		//SAFE_RELEASE(dxgiSwapChain);

		//禁用全屏模式ALT-ENTER切换
		m_pDXGIFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

		//m_pDXGISwapChain4->GetCurrentBackBufferIndex();

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2D1Factory1);
		if (!SUCCEEDED(hr)) return hr;

		hr = m_pD2D1Factory1->CreateDevice(_dxgiDv2, &m_pD2D1Device);
		if (!SUCCEEDED(hr)) return hr;
		hr = m_pD2D1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pD2D1DeviceContext);
		if (!SUCCEEDED(hr)) return hr;

		//SAFE_RELEASE(_dxgiDv2);

		_CreateSurface();

		return hr;
	}
	return hr;
}

HRESULT CWHDXGraphicWindow::ResetSize(WORD wWidth, WORD wHeight)
{
	DXGI_MODE_DESC desc = { 0 };
	desc.Width = wWidth;
	desc.Height = wHeight;
	return m_pDXGISwapChain4->ResizeTarget(&desc);
}

HRESULT CWHDXGraphicWindow::ResetDXG(WORD wWidth, WORD wHeight, WORD wBPP, BYTE bScreenModeFlag, BYTE bDeviceModeFlag)
{
    HRESULT hr;
    DWORD	dwStyle;

	if ( m_bDeviceModeFlag == bDeviceModeFlag && m_bScreenModeFlag == bScreenModeFlag && 
		 m_stDisplayInfo.wWidth == wWidth && m_stDisplayInfo.wHeight == wHeight && m_stDisplayInfo.wBPP == wBPP )
		return E_FAIL;													    

    if ( FAILED(DestroyDXGObjects()) )
	{
		MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::ResetDXG]") TEXT("DirectGraphic reset failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	m_stDisplayInfo.wWidth	= wWidth;
	m_stDisplayInfo.wHeight	= wHeight;
	m_stDisplayInfo.wBPP	= wBPP;
	m_bScreenModeFlag		= bScreenModeFlag;
	m_bDeviceModeFlag		= bDeviceModeFlag;

	if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}
	else
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~dwStyle;
		dwStyle |= WS_POPUP;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}

	if ( hr = FAILED(CreateDXG()) )
	{
		MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::ResetDXG]") TEXT("DirectGraphic create failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	CreateDefFont();

	return S_OK;
}
VOID CWHDXGraphicWindow::UpdateBoundsWnd()
{
	if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
	{
        GetClientRect(m_hWnd, &m_rcWindow);
        ClientToScreen(m_hWnd, (POINT*)&m_rcWindow);
        ClientToScreen(m_hWnd, (POINT*)&m_rcWindow+1);
    }
    else
    {
        SetRect(&m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }
}
HFONT CWHDXGraphicWindow::CreateGameFont(LPCSTR szFontName, INT nHeight, INT nWidth, INT nWeight, BOOL bItalic, BOOL bULine, BOOL bStrikeOut, DWORD dwCharSet)
{
	INT nYsize = 0;

	// ???? ???.
	if ( m_pddsBackBuffer )
	{
		HDC hDC;
		m_pddsBackBuffer->GetDC(&hDC);
		nYsize = -MulDiv(nHeight, GetDeviceCaps(hDC, LOGPIXELSY), 72); 
		m_pddsBackBuffer->ReleaseDC(hDC);
	}

	return CreateFont(nYsize, nWidth, 0, 0, nWeight, bItalic, bULine, bStrikeOut, dwCharSet, 
					  OUT_DEFAULT_PRECIS,	CLIP_DEFAULT_PRECIS, 
					  DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, szFontName);

}
VOID CWHDXGraphicWindow::CreateDefFont()
{
	{
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
		m_pDWriteFactory->CreateTextFormat(L"宋体", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL, 12, L"zh-cn", &m_pDWriteTextFormat);

	}
	// ?? ????? ???? ??? ?? ??? ??? Clear.
	if ( m_hDefGameFont != NULL )
	{
		DeleteObject(m_hDefGameFont); 
		m_hDefGameFont = NULL;
	}

	m_hDefGameFont	= CreateGameFont("宋体", 9);
}
VOID CWHDXGraphicWindow::StringPlus(CHAR* szResult, CHAR* szSrc, ...)
{
	INT nCnt = 0;

	va_list vaMarker;
	CHAR*	pszString;


//	ZeroMemory(szResult, strlen(szResult));
	va_start(vaMarker, szSrc);
	pszString = szSrc;
	
	while ( TRUE )
	{		
		strcpy_s(&szResult[strlen(szResult)], _tcslen(pszString)+1, pszString);
		if ( !strcmp(pszString,  "") )		break;
		pszString = (CHAR*)va_arg(vaMarker, CHAR*);
	}

	va_end(vaMarker);
}
CHAR* CWHDXGraphicWindow::IntToStr(INT nNum)
{
	static CHAR szResult[MAX_PATH];
	ZeroMemory(szResult, MAX_PATH);
	_itoa_s(nNum, szResult, 10);
	return szResult;
}
VOID CWHDXGraphicWindow::PutsHan(LPDIRECTDRAWSURFACE7 pSurface, INT nX, INT nY, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont)
{
	RECT rc;
	rc.left = nX;
	rc.top = nY;
	rc.right = m_pD2D1DeviceContext->GetSize().width;
	rc.bottom = m_pD2D1DeviceContext->GetSize().height;
	PutsHan(NULL, rc, foreColor, backColor, szText, nullptr);
}
VOID CWHDXGraphicWindow::PutsHan(LPDIRECTDRAWSURFACE7 pSurface, RECT rc, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont)
{
	wchar_t* temp = nullptr;
	static ID2D1SolidColorBrush* brsh = nullptr;
	if (!brsh)
	{
		m_pD2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(foreColor, 255), &brsh);
	}
	brsh->SetColor(D2D1::ColorF(foreColor, 255));
	int needlen = MultiByteToWideChar(CP_ACP, 0, szText, -1, nullptr, 0);
	temp = new wchar_t[needlen];
	needlen = MultiByteToWideChar(CP_ACP, 0, szText, -1, temp, needlen);
	m_pD2D1DeviceContext->DrawText(temp, needlen-1, m_pDWriteTextFormat, D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom), brsh);
	m_pD2D1DeviceContext->Flush();
	SAFE_DELETE(temp);
}
SIZE CWHDXGraphicWindow::GetStrLength(LPDIRECTDRAWSURFACE7 pSurface, HFONT hFont, CHAR* szFormat, ...)
{
	va_list arg;
    CHAR	szBuf[1024];
	SIZE	sSize;
	IDWriteTextLayout* pLayout = nullptr;

	va_start(arg, szFormat);
    vsprintf_s(szBuf, szFormat, arg);
    va_end(arg);

	sSize.cx = 0;
	sSize.cy = 0;

	wchar_t* temp = nullptr;
	int needlen = MultiByteToWideChar(CP_ACP, 0, szBuf, -1, nullptr, 0);
	temp = new wchar_t[needlen];
	needlen = MultiByteToWideChar(CP_ACP, 0, szBuf, -1, temp, needlen);
	if (SUCCEEDED(m_pDWriteFactory->CreateTextLayout(temp, needlen - 1, m_pDWriteTextFormat, 0, 0, &pLayout)))
	{
		DWRITE_TEXT_METRICS metrics;
		pLayout->GetMetrics(&metrics);
		sSize.cx = metrics.widthIncludingTrailingWhitespace;
		sSize.cy = metrics.height;
		SAFE_RELEASE(pLayout);
	}
	SAFE_DELETE(temp);
	return sSize;
}
BOOL CWHDXGraphicWindow::StringDivide(INT nDivideWidth, INT& nDividedLine, CHAR* szSrc, CHAR* szResult)
{
	CHAR szTmpCheck[MAX_PATH];
	
	nDividedLine = 1;

	INT nStartLen	= 0;
	INT nEndLen		= 0;
	INT nResultLen	= 0;

	INT nWordCheck	= 0;

	if ( szSrc[0] != NULL )
	{
//		ZeroMemory(szResult, MAX_PATH);

		for ( INT nCnt = 0; nCnt < (INT)strlen(szSrc); nCnt++)
		{
			ZeroMemory(szTmpCheck, MAX_PATH);

			nEndLen = nCnt+1;

			// ??? ????? ?? ????? ???? ???? ????.
			if ( nEndLen >= MAX_PATH * 2 )
			{
				szResult[MAX_PATH-1] = NULL;
				break;
			}

			// ??????? ???? ???.
			memcpy(&szTmpCheck, &szSrc[nStartLen], nEndLen-nStartLen);

			SIZE sizeLen;
			sizeLen = GetStrLength(NULL, NULL, szTmpCheck);

			// ???? ??? ??? ????? ?????.
			if ( sizeLen.cx > nDivideWidth )
			{
				// ?? 2??? ????. 
				if ( szSrc[nEndLen-1] < 0 )
				{
					// ?? ????? 2??? ??? ????
					if ( !(nWordCheck%2) )
					{
						nStartLen += strlen(szTmpCheck)-1;
						nCnt--;
						szResult[nResultLen] = '`';
						nResultLen++;
						nDividedLine++;
					}
					else
					{
						nStartLen += strlen(szTmpCheck)-2;
						nCnt -= 2;
						szResult[nResultLen-1] = '`';
						nDividedLine++;
						nWordCheck--;
					}
				}
				// 1??? ??. 
				// ???? ?????????? ????? ??.
				else
				{
					nStartLen += strlen(szTmpCheck)-1;
					nCnt--;
					szResult[nResultLen] = '`';
					nResultLen++;
					nDividedLine++;
				}
			}
			else
			{
				if ( szSrc[nEndLen-1] < 0 )
					nWordCheck++;

				szResult[nResultLen] = szSrc[nEndLen-1];
				nResultLen++;
			}
		}
		return TRUE;
	}
	return FALSE;

}
BOOL CWHDXGraphicWindow::DecodeWilImageData(int nWidth, int nHeight, const WORD* pwSrc, WORD* pwDst, WORD wChooseColor1, WORD wChooseColor2, BYTE bOpa)
{
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = nWidth;
	rc.bottom = nHeight;

	int nWidthStart = 0, nWidthEnd = 0;
	int nCntCopyWord = 0, nCurrWidth = 0, nLastWidth = 0;
	if (*(DWORD*)pwSrc == 0) pwSrc += 2; //如果前4个字节为0，则跳过前4个字节。注意pwSrc是WORD所以+2.
	for (INT nYCnt = rc.top; nYCnt < rc.bottom; nYCnt++)
	{
		// 获取一行的长度（单位是字） 
		nWidthEnd += pwSrc[nWidthStart];
		nWidthStart++;

		// 在屏幕上传播的长度与行长一样。 
		for (INT x = nWidthStart; x < nWidthEnd; )
		{
			if (pwSrc[x] == 0xC0)
			{
				x++;
				nCntCopyWord = pwSrc[x];
				x++;
				nCurrWidth += nCntCopyWord;
			}
			else if (pwSrc[x] == 0xC1)
			{
				x++;
				nCntCopyWord = pwSrc[x];
				x++;

				nLastWidth = nCurrWidth;
				nCurrWidth += nCntCopyWord;

				for (size_t i = 0; i < nCntCopyWord; i++)
				{
					DWORD* a = (DWORD*)pwDst;
					a[(nYCnt * nWidth) + nLastWidth + i] = COLOR_B16TOB32(pwSrc[x + i], bOpa);
				}

				x += nCntCopyWord;
			}
			else if (pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3)
			{
				WORD wDyingKind, wChooseColor;
				wDyingKind = pwSrc[x];
				switch (wDyingKind)
				{
				case 0xC2:
					wChooseColor = wChooseColor1;
					break;
				case 0xC3:
					wChooseColor = wChooseColor2;
					break;
				}
				x++;
				nCntCopyWord = pwSrc[x];
				x++;

				nLastWidth = nCurrWidth;
				nCurrWidth += nCntCopyWord;

				WORD wPixel;
				WORD bBlueWantedColor, bGreenWantedColor, bRedWantedColor;
				BYTE bBlueSrc, bGreenSrc, bRedSrc;
				FLOAT rBlueRate, rGreenRate, bRedRate;

				for (INT nCheck = 0; nCheck < nCntCopyWord; nCheck++)
				{
					wPixel = wChooseColor;
					bBlueWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
					bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
					bRedWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

					wPixel = pwSrc[x + nCheck];
					bBlueSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
					bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
					bRedSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

					rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask >> m_stBitsMaskInfo.bBShift));
					rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask >> m_stBitsMaskInfo.bGShift));
					bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask >> m_stBitsMaskInfo.bRShift));

					bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor * rBlueRate));
					bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor * rGreenRate));
					bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor * bRedRate));

					if (bBlueWantedColor > (m_stBitsMaskInfo.dwBMask >> m_stBitsMaskInfo.bBShift))
						bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask >> m_stBitsMaskInfo.bBShift);
					if (bGreenWantedColor > (m_stBitsMaskInfo.dwGMask >> m_stBitsMaskInfo.bGShift))
						bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask >> m_stBitsMaskInfo.bGShift);
					if (bRedWantedColor > (m_stBitsMaskInfo.dwRMask >> m_stBitsMaskInfo.bRShift))
						bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask >> m_stBitsMaskInfo.bRShift);

					pwDst[(nYCnt * nWidth) + (nLastWidth + nCheck)] = ((bRedWantedColor << m_stBitsMaskInfo.bRShift) |
						(bGreenWantedColor << m_stBitsMaskInfo.bGShift) |
						(bBlueWantedColor << m_stBitsMaskInfo.bBShift));
				}
				x += nCntCopyWord;
			}
		}
		// 将行尾移至下一行的开头。 
		nWidthEnd++;

		nWidthStart = nWidthEnd;
		nCurrWidth = 0;
	}

	return TRUE;
}
BOOL CWHDXGraphicWindow::DrawImage(D2D1_RECT_F drawRect, D2D1_SIZE_U bitmapSize, const void* pBitmapData, UINT32 pitch)
{
	BOOL r = FALSE;
	ID2D1Bitmap* pBitmap;
	if (SUCCEEDED(m_pD2D1DeviceContext->CreateBitmap(bitmapSize, pBitmapData, pitch, s_BitmapProps, &pBitmap)))
	{
		m_pD2D1DeviceContext->DrawBitmap(pBitmap, &drawRect);
		r = TRUE;
	}
	SAFE_RELEASE(pBitmap);

	return r;
}
WORD CWHDXGraphicWindow::ConvertColor24To16(COLORREF dwColor)
{
	WORD wRet;

	INT nRed  = RGB_GETRED(dwColor);
	INT nGreen = RGB_GETGREEN(dwColor);
	INT nBlue   = RGB_GETBLUE(dwColor);
	
	nRed = (nRed*(1<<m_stBitsMaskInfo.bRCnt))/(1<<8);
	nGreen = (nGreen*(1<<m_stBitsMaskInfo.bGCnt))/(1<<8);
	nBlue = (nBlue*(1<<m_stBitsMaskInfo.bBCnt))/(1<<8);

	wRet = (nRed << m_stBitsMaskInfo.bRShift) | (nGreen << m_stBitsMaskInfo.bGShift) | (nBlue << m_stBitsMaskInfo.bBShift); 
	return wRet;
}
HRESULT CWHDXGraphicWindow::DrawWithGDI(RECT rc, LPDIRECTDRAWSURFACE7 pSurface, DWORD dwColor, BYTE bKind)
{
	static ID2D1SolidColorBrush* brsh = nullptr;
	if (!brsh)
	{
		m_pD2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(dwColor), &brsh);
	}
	brsh->SetColor(D2D1::ColorF(dwColor));

	switch ( bKind )
	{
	case 0:
		{
			m_pD2D1DeviceContext->DrawLine(D2D1::Point2F(rc.left, rc.top), D2D1::Point2F(rc.right, rc.bottom), brsh);
		}
		break;
	case 1:
		{
			m_pD2D1DeviceContext->DrawRectangle(D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom), brsh);
		}
		break;
	case 2:
		{
			m_pD2D1DeviceContext->FillRectangle(D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom), brsh);
		}
		break;
	default:
		break;
	}

	return S_OK;
}

BOOL CWHDXGraphicWindow::DrawWithImagePerLineClipRgn(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth  - 1;
	INT	nEndY		= wClipHeight - 1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset = nStartX - nX;
			nWidth	 = nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth	 = nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset + nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset + nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			for ( INT y=rc.top ; y < rc.bottom ; y++ )
			{
				memcpy(&pwdDst[((y+nY) * (ddsd.lPitch >> 1)) + (nX + rc.left)], &pwSrc[y * nXSize + rc.left], (rc.right-rc.left)*2);
			}

			m_pddsBackBuffer->Unlock(NULL);
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CWHDXGraphicWindow::DrawWithImageForCompMemToMem(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, INT nDstXSize, INT nDstYSize, WORD* pwDst, WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= nDstXSize - 1;
	INT	nEndY		= nDstYSize - 1;

	if ( pwDst != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y? ???.
			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y?? ??? ??? ???? Count?? ???.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// ???? ??? ????.(??? ??)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// ??? ???? ??? ????.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left?? ???? ??? ??? ?? ??.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								memcpy(&pwDst[((nYCnt+nY) * nDstXSize) + (rc.left+nX)], &pwSrc[x], sizeof(WORD)*(nCurrWidth-rc.left));
								x += (nCurrWidth-rc.left);
							}
							// rc.right?? ???? ??? ??? ?? ??.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								memcpy(&pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								memcpy(&pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*nCntCopyWord);
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left?? ???? ??? ??? ?? ??.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																							    (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																								(bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right?? ???? ??? ??? ?? ??.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																								(bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																								(bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																								(bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																								(bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// ??? ?? ?? ??? ???? ????.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CWHDXGraphicWindow::DrawWithImageForComp(INT nX, INT nY, RECT rcSrc, WORD* pwSrc, WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= rcSrc.right-rcSrc.left;
	INT	nHeight		= rcSrc.bottom-rcSrc.top;
	INT	nXOffset	= rcSrc.left;
	INT	nYOffset	= rcSrc.top;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= m_stDisplayInfo.wWidth -1;
	INT	nEndY		= m_stDisplayInfo.wHeight-1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset += nStartX - nX;
			nWidth	 = rcSrc.right - nXOffset;
		}
		if ( (nX+rcSrc.right-rcSrc.left-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset += nStartY - nY;
			nHeight	 = rcSrc.bottom - nYOffset;
		}		
		if ( (nY+rcSrc.bottom-rcSrc.top-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;


		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y? ???.
			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y?? ??? ??? ???? Count?? ???.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// ???? ??? ????.(??? ??)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// ??? ???? ??? ????.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left?? ???? ??? ??? ?? ??.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX)], &pwSrc[x], sizeof(WORD)*(nCurrWidth-rc.left));
								x += (nCurrWidth-rc.left);
							}
							// rc.right?? ???? ??? ??? ?? ??.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*nCntCopyWord);
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left?? ???? ??? ??? ?? ??.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right?? ???? ??? ??? ?? ??.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// ??? ?? ?? ??? ???? ????.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}
BOOL CWHDXGraphicWindow::DrawWithImageForComp(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wChooseColor1, WORD wChooseColor2)
{
	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));
	D2D1_RECT_F rect = D2D1::RectF(nX, nY, nXSize + nX, nYSize + nY);
	D2D1_SIZE_U size = D2D1::SizeU(nXSize, nYSize);
	BOOL Result = FALSE;

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst, wChooseColor1, wChooseColor2))
	{
		Result = DrawImage(rect, size, pwdDst, nXSize*sizeof(DWORD));
	}
	SAFE_DELETE_ARR(pwdDst);
	return Result;
}
BOOL CWHDXGraphicWindow::DrawWithImageForCompClipRgnBase(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1, WORD wChooseColor2)
{
	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));
	D2D1_RECT_F rect = D2D1::RectF(nX, nY,
		nX + nXSize < wClipWidth ? nXSize + nX : wClipWidth,
		nY + nYSize < wClipHeight ? nYSize + nY : wClipHeight);
	D2D1_SIZE_U size = D2D1::SizeU(nXSize, nYSize);
	BOOL Result = FALSE;

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst, wChooseColor1, wChooseColor2))
	{
		Result = DrawImage(rect, size, pwdDst, nXSize * sizeof(DWORD));
	}
	SAFE_DELETE_ARR(pwdDst);
	return Result;
}
BOOL CWHDXGraphicWindow::DrawWithABlendCompDataWithBackBuffer(INT nX, INT nY, 
															  INT nXSize, INT nYSize, WORD* pwSrc,
															  WORD wClipWidth, WORD wClipHeight,
															  WORD wChooseColor1, WORD wChooseColor2, BYTE bOpa)
{
	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));
	D2D1_RECT_F rect = D2D1::RectF(nX, nY, nXSize + nX, nYSize + nY);
	D2D1_SIZE_U size = D2D1::SizeU(nXSize, nYSize);
	BOOL Result = FALSE;

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst, wChooseColor1, wChooseColor2, bOpa))
	{
		//m_pD2D1DeviceContext->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
		Result = DrawImage(rect, size, pwdDst, nXSize * sizeof(DWORD));
	}
	SAFE_DELETE_ARR(pwdDst);
	return Result;
}
BOOL CWHDXGraphicWindow::DrawWithShadowABlend(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight,
											  WORD* pwShadowClrSrc, BOOL bBlend, BYTE bShadowType, BYTE bOpa)

{
	// ?? ?? ???.
	if ( bShadowType == 48 )
		return FALSE;

	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));
	D2D1_RECT_F rect = D2D1::RectF(nX, nY, nXSize + nX, nYSize + nY);
	D2D1_SIZE_U size = D2D1::SizeU(nXSize, nYSize);
	BOOL Result = FALSE;

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst, 0xFFFF, 0xFFFF, bOpa))
	{
		Result = DrawImage(rect, size, pwdDst, nXSize * sizeof(DWORD));
	}
	SAFE_DELETE_ARR(pwdDst);
	return Result;
}
VOID CWHDXGraphicWindow::DrawWithShadowABlend(INT nX, INT nY, INT nXSize, INT nYSize, INT nPX, INT nPY, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight,
											  WORD* pwShadowClrSrc, BOOL bBlend, BYTE bOpa)
{
	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));
	D2D1_RECT_F rect = D2D1::RectF(nX, nY, wClipWidth + nX, wClipHeight + nY);
	D2D1_SIZE_U size = D2D1::SizeU(nXSize, nYSize);

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst, 0xFFFF, 0xFFFF, bOpa))
	{
		DrawImage(rect, size, pwdDst, nXSize * sizeof(DWORD));
	}
	SAFE_DELETE_ARR(pwdDst);
}
BOOL CWHDXGraphicWindow::DrawWithABlendCompDataWithLightedColor(INT nX, INT nY, 
															    INT nXSize, INT nYSize, WORD* pwSrc,
															    WORD wClipWidth, WORD wClipHeight,
															    WORD wChooseColor1, WORD wChooseColor2)
{
	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));
	D2D1_RECT_F rect = D2D1::RectF(nX, nY, wClipWidth + nX, wClipHeight + nY);
	D2D1_SIZE_U size = D2D1::SizeU(wClipWidth, wClipHeight);
	BOOL Result = FALSE;

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst, wChooseColor1, wChooseColor2))
	{
		Result = DrawImage(rect, size, pwdDst, nXSize * sizeof(DWORD));
	}
	SAFE_DELETE_ARR(pwdDst);
	return Result;
}
BOOL CWHDXGraphicWindow::DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wColor, BOOL bFocused, BOOL bBlend)
{
	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst))
	{
		D2D1_RECT_U rect = D2D1::RectU(nX, nY, wClipWidth + nX, wClipHeight + nY);
		m_pD2D1Bitmap1->CopyFromMemory(&rect, (const void*)pwdDst, nXSize * sizeof(DWORD));
		m_pD2D1DeviceContext->Flush();
		SAFE_DELETE(pwdDst);
		return TRUE;
	}
	SAFE_DELETE(pwdDst);
	return FALSE;
}
BOOL CWHDXGraphicWindow::DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight,WORD wColor , WORD wChooseColor1, WORD wChooseColor2)
{
	WORD* pwdDst = (WORD*)new DWORD[nXSize * nYSize];
	ZeroMemory(pwdDst, nXSize * nYSize * sizeof(DWORD));

	if (DecodeWilImageData(nXSize, nYSize, pwSrc, pwdDst))
	{
		D2D1_RECT_U rect = D2D1::RectU(nX, nY, wClipWidth + nX, wClipHeight + nY);
		m_pD2D1Bitmap1->CopyFromMemory(&rect, (const void*)pwdDst, nXSize * sizeof(DWORD));
		m_pD2D1DeviceContext->Flush();
		SAFE_DELETE(pwdDst);
		return TRUE;
	}
	SAFE_DELETE(pwdDst);
	return FALSE;
}

HRESULT CWHDXGraphicWindow::Present()
{
	HRESULT hr;

	{
		DXGI_PRESENT_PARAMETERS params;
		params.DirtyRectsCount = 0;
		params.pDirtyRects = NULL;
		params.pScrollOffset = NULL;
		params.pScrollRect = NULL;
		hr = m_pDXGISwapChain4->Present1(1, 0, &params);

		return hr;
	}

	if ( NULL == m_pddsFrontBuffer || NULL == m_pddsBackBuffer )
	{
		return E_POINTER;
	}

	while ( TRUE )
	{
		if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
		{
		 //?????? ??? ??? ???? ????.
			m_pddsFrontBuffer->SetClipper(m_lpcClipper);
			hr = m_pddsFrontBuffer->Blt(&m_rcWindow, m_pddsBackBuffer, NULL, DDBLT_WAIT, NULL);
		}
		else
		{
			hr = m_pddsFrontBuffer->Flip(NULL, DDFLIP_WAIT );
		}

		if(hr == DDERR_SURFACELOST)
		{
			m_pddsFrontBuffer->Restore();
			m_pddsBackBuffer->Restore();
		}

		if(hr != DDERR_WASSTILLDRAWING)
			return hr;
	} 
}
HRESULT CWHDXGraphicWindow::RestoreSurfaces()
{
    HRESULT hr;

	if ( !m_pDD )
		return E_FAIL;

    if ( FAILED(hr = m_pDD->RestoreAllSurfaces()) )
        return hr;

    return S_OK;
}
