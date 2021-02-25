#ifndef _WINDHORN_DXGRAPHIC
#define	_WINDHORN_DXGRAPHIC

/******************************************************************************************************************

	CWHDXGraphicWindow Class Declaration

*******************************************************************************************************************/

const static UINT		FRAME_COUNT = 2;

typedef struct tagDXG_DISPLAYINFO 
{
	WORD			wWidth;
	WORD			wHeight;
	WORD			wBPP;
}DXG_DISPLAYINFO, *LPDXG_DISPLAYINFO;

typedef struct DXG_ENUM_DEVICEINFO
{
    // 3D 叼官捞胶 沥焊.
    TCHAR			szDeviceDesc[256];
    GUID*			pDeviceGUID;
    D3DDEVICEDESC7	ddDeviceDesc;
    BOOL			f3DHardware;
    // 靛肺快 靛扼捞滚 沥焊.
	TCHAR			szDriverDesc[256];
    GUID*			pDriverGUID;
    DDCAPS			ddDriverCaps;
    DDCAPS			ddHELCaps;

    GUID            guidDevice;
    GUID            guidDriver;
    DDSURFACEDESC2* pddsdModes;
    DWORD           dwNumModes;
}DXG_ENUM_DEVICEINFO, *LPDXG_ENUM_DEVICEINFO;

typedef struct tagDXG_MASKINFO
{
	BYTE			bRCnt;
	BYTE			bGCnt;
	BYTE			bBCnt;
	BYTE			bRShift;
	BYTE			bGShift;
	BYTE			bBShift;
	DWORD			dwRMask;
	DWORD			dwGMask;
	DWORD			dwBMask;
}DXG_MASKINFO, *LPDXG_MASKINFO;

class CWHDXGraphicWindow : public CWHWindow
{
//1: Constuctor/Destructor
public:
	CWHDXGraphicWindow(WORD wWidth = _SCREEN_WIDTH, WORD wHeight = _SCREEN_HEIGHT, WORD wBpp = _SCREEN_BPP);
	~CWHDXGraphicWindow();

//2: Variables
protected:
	LPDIRECT3D7				m_pD3D;
    LPDIRECTDRAW7			m_pDD;
	LPDIRECT3DDEVICE7		m_pd3dDevice;
    LPDIRECTDRAWSURFACE7	m_pddsFrontBuffer;
    LPDIRECTDRAWSURFACE7	m_pddsBackBuffer;
	LPDIRECTDRAWSURFACE7	m_pddsZBuffer;
	LPDIRECTDRAWCLIPPER		m_lpcClipper;

public:
	static D2D1_BITMAP_PROPERTIES  s_BitmapProps;
	//DirectWrite
	IDWriteFactory*			m_pDWriteFactory;
	IDWriteTextFormat*		m_pDWriteTextFormat;

	// D3D11
	ID3D11Device*			m_pD3D11Device;				//D3D11设备
	ID3D11DeviceContext*	m_pD3D11DeviceContext;		//D3D11设备上下文
	D3D_FEATURE_LEVEL		m_FeatureLevel;				//功能等级

	//DXGI
	IDXGIFactory4*			m_pDXGIFactory4;			//DXGI工厂类4
	IDXGISwapChain4*		m_pDXGISwapChain4;			//DXGI交换链1
	IDXGISurface*			m_pDXGISurface;				//DXGI平面
	//D2D1
	ID2D1Factory1*			m_pD2D1Factory1;			//D2D1工厂类1
	ID2D1Device*			m_pD2D1Device;				//D2D1设备
	ID2D1DeviceContext*		m_pD2D1DeviceContext;		//D2D1设备上下文
	ID2D1Bitmap1*			m_pD2D1Bitmap1;				//D2D1位图

///////<---------------------------------------------------------------------------------------------------------------------------
	HFONT					m_hDefGameFont;
///////<---------------------------------------------------------------------------------------------------------------------------

	BYTE					m_bScreenModeFlag;
    BYTE					m_bDeviceModeFlag;

public:
	DWORD					m_dwTextureTotal;	// 傈眉 荤侩且荐 乐绰 皋葛府.
	DWORD					m_dwTextureFree;    // 泅犁 巢篮 皋葛府.
	DWORD					m_dwVideoTotal; 	// vram 傈眉 皋葛府.
	DWORD					m_dwVideoFree;      // 泅犁 巢篮 vram 皋葛府 .

	BOOL					m_bIsWindowActive;
	BOOL					m_bIsWindowReady;

    RECT					m_rcWindow;
	DXG_DISPLAYINFO			m_stDisplayInfo;
	DXG_MASKINFO			m_stBitsMaskInfo;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 橇肺技胶甫 唱穿扁困茄 函荐急攫.
	static CWHDXGraphicWindow*	m_pxDXGWnd;
	CWHDefProcess*				m_pxDefProcess;

	VOID					(*RenderProcess)(int);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//3: Methods
protected:
	VOID	FreeDXGEnumModeResources();

	HRESULT CreateDXG();
	HRESULT DestroyDXGObjects();
	HRESULT HandleDeviceLost();


	HRESULT _ReleaseSurface();
	HRESULT _CreateSurface();
public:

    __inline LPDIRECTDRAW7			GetDirectDraw()     { return m_pDD;				}
    __inline LPDIRECT3D7			GetDirect3D()		{ return m_pD3D;			}
	__inline LPDIRECT3DDEVICE7		Get3DDevice()		{ return m_pd3dDevice;		}
    __inline LPDIRECTDRAWSURFACE7	GetFrontBuffer()    { return m_pddsFrontBuffer; }
    __inline LPDIRECTDRAWSURFACE7	GetBackBuffer()     { return m_pddsBackBuffer;	}
    __inline BYTE					GetDeviceMode()     { return m_bDeviceModeFlag; }
	__inline BYTE					GetScreenMode()		{ return m_bScreenModeFlag; }

	virtual VOID			UpdateBoundsWnd();

	HRESULT					ResetDXG(WORD wWidth, WORD wHeight, WORD wBPP, BYTE bScreenModeFlag, BYTE bDeviceModeFlag);
	BOOL					Create(HINSTANCE hInst, LPTSTR lpCaption = NULL, CHAR *pszMenuName = NULL, CHAR* pszIconName = NULL, BYTE bScreenModeFlag = _DXG_SCREENMODE_WINDOW, BYTE bDeviceModeFlag = _DXG_DEVICEMODE_PRIMARY);

	RECT					CenterRect(RECT rect)
	{
		RECT rc;
		int ww = (m_rcWindow.right - m_rcWindow.left);
		int wh = (m_rcWindow.bottom - m_rcWindow.top);
		int tw = (rect.right - rect.left);
		int th = (rect.bottom - rect.top);

		rc.left = rect.left + (ww - tw) / 2;
		rc.top = rect.top + (wh - th) / 2;
		rc.right = rc.left + tw;
		rc.bottom = rc.top + th;
		
		return rc;
	}

///////<---------------------------------------------------------------------------------------------------------------------------
	HFONT					CreateGameFont(LPCSTR szFontName, INT nHeight, INT nWidth = 0, INT nWeight = FW_NORMAL, BOOL bItalic = FALSE, BOOL bULine = FALSE, BOOL bStrikeOut = FALSE, DWORD dwCharSet = DEFAULT_CHARSET);
	VOID					CreateDefFont();

	VOID					PutsHan(LPDIRECTDRAWSURFACE7 pSurface, INT nX, INT nY, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont = NULL);
	VOID					PutsHan(LPDIRECTDRAWSURFACE7 pSurface, RECT rc, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont = NULL);

	SIZE					GetStrLength(LPDIRECTDRAWSURFACE7 pSurface, HFONT hFont, CHAR* szFormat, ...);
///////<---------------------------------------------------------------------------------------------------------------------------

	BOOL 					StringDivide(INT nDivideWidth, INT& nDividedLine, CHAR* szSrc, CHAR* szResult);
//	BOOL					StringDivideLen(INT nDivideLen, INT& nDividedLine, CHAR* szSrc, CDLList<CHAR*>* m_pxpStr);

	VOID 					StringPlus(CHAR* szResult, CHAR* szSrc, ...);
	CHAR*					IntToStr(INT nNum);

	VOID					UsedAndFreeMemoryCheck();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BOOL DecodeWilImageData(int nWidth, int nHeight, const WORD* pwSrc, WORD* pwDst, WORD wChooseColor1=0xFFFF, WORD wChooseColor2=0xFFFF, BYTE bOpa=100);
	BOOL DrawImage(D2D1_RECT_F drawRect, D2D1_SIZE_U bitmapSize, const void* pBitmapData, UINT32 pitch);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	WORD					ConvertColor24To16(COLORREF dwColor);
	// 渲染功能函数
	// 1. GDI的绘制功能（绘制线条矩形等）。
	HRESULT					DrawWithGDI(RECT rc, LPDIRECTDRAWSURFACE7 pSurface, DWORD dwColor, BYTE bKind);
	// 3. WORD类型数据绘制功能（仅绘制裁剪的区域wClipWidth*wClipHeight)
	BOOL					DrawWithImagePerLineClipRgn(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight);

	// wChooseColor1用于对衣服(0xC2)染色, wChooseColor2(0xC3)用于对头部背部（刀，等其他）染色。
	// 5. 压缩（编号0）和染色的WORD数据绘制功能
	BOOL					DrawWithImageForComp(INT nX, INT nY, RECT rcSrc, WORD* pwSrc, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	BOOL					DrawWithImageForComp(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);



	// 6. 压缩（编号0）和染色的WORD类型数据绘制功能（仅绘制裁剪部分区域）
	BOOL					DrawWithImageForCompClipRgnBase(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	__inline VOID			DrawWithImageForCompClipRgn(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF, BOOL bLighted = FALSE)
	{
		if ( bLighted )		DrawWithABlendCompDataWithLightedColor(nX, nY, nXSize, nYSize, pwSrc, wClipWidth, wClipHeight, wChooseColor1, wChooseColor2);
		else				DrawWithImageForCompClipRgnBase(nX, nY, nXSize, nYSize, pwSrc, wClipWidth, wClipHeight, wChooseColor1, wChooseColor2);
	}


	// 7. 压缩（编号0）和染色的WORD类型数据绘制功能（从源内存到目的内存）
	BOOL					DrawWithImageForCompMemToMem(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, INT nDstXSize, INT nDstYSize, WORD* pwDst, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	// 8. 压缩（编号0）和染色的WORD类型数据绘制功能
	// 9. 压缩（编号0）和染色的WORD类型数据绘制功能

	// 10. 压缩（编号0）和染色的WORD类型数据绘制功能（Alpha将压缩数据的源区域与背景混合）
	BOOL					DrawWithABlendCompDataWithBackBuffer(INT nX, INT nY, 
																 INT nXSize, INT nYSize, WORD* pwSrc,
																 WORD wClipWidth, WORD wClipHeight,
																 WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF, BYTE bOpa = 50);
	// 11. 压缩（编号0）和染色的WORD类型数据绘制功能（在白色表面上以所需的颜色照明（明亮）绘制压缩的数据源区域）
	BOOL					DrawWithABlendCompDataWithLightedColor(INT nX, INT nY, 
																   INT nXSize, INT nYSize, WORD* pwSrc,
																   WORD wClipWidth, WORD wClipHeight,
																   WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	// 阴影绘制功能.
	// 12. 压缩（编号0）和染色的WORD类型数据绘制功能（Alpha混合了背面压缩数据源区域的阴影，阴影减少一半的高度、宽度增加通过YSize倾斜到背面）
	BOOL					DrawWithShadowABlend(INT nX, INT nY, 
												 INT nXSize, INT nYSize, WORD* pwSrc,
												 WORD wClipWidth, WORD wClipHeight, WORD* pwShadowClrSrc,
												 BOOL bBlend = FALSE, BYTE bShadowType = 48, BYTE bOpa = 50);
	VOID					DrawWithShadowABlend(INT nX, INT nY, INT nXSize, INT nYSize, 
		                                         INT nPX, INT nPY, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD* pwShadowClrSrc,
												 BOOL bBlend = FALSE, BYTE bOpa = 50);
	// 色彩处理.
	// 13.
	BOOL					DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wColor, BOOL bFocused = FALSE, BOOL bBlend = FALSE);
	BOOL					DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wColor, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	HRESULT					Present();
	HRESULT					RestoreSurfaces();

//4: Message Map
public:
	virtual LRESULT MainWndProcDXG(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam);


protected:

	__inline virtual LRESULT MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{ 
		if ( m_pxDefProcess )	return m_pxDefProcess->DefMainWndProc(hWnd, uMsg, wParam, lParam);
		else					return MainWndProcDXG(hWnd, uMsg, wParam, lParam); 
	}

	virtual LRESULT	OnDestroy() override;
	virtual LRESULT	OnSetCursor();
};

CWHDXGraphicWindow*		GetDXGWindowPtr();

#endif //_WINDHORN_DXGRAPHIC
