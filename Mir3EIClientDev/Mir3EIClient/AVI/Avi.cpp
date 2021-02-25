#include "StdAfx.h"
#include "Avi.h"

//////////////////////////////////////////////////////////////////////////////////
// CAvi Constructor
//////////////////////////////////////////////////////////////////////////////////
CAvi::CAvi(void)
{
	m_pNotify = new MediaEngineNotify(this);
	m_pMFMFactory = nullptr;
	m_pMediaEngine = nullptr;
	m_pByteStream = nullptr;
	m_bPlaying = FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi Destructor
//////////////////////////////////////////////////////////////////////////////////
CAvi::~CAvi(void)
{

}

void CAvi::InitAvi()
{
	MFStartup(MF_VERSION);
	InitializeCriticalSection(&m_CriticalSection);

	UINT resetToken;
	MFCreateDXGIDeviceManager(&resetToken, &m_pDXGIManager);
	m_pDXGIManager->ResetDevice(g_xMainWnd.m_pD3D11Device, resetToken);

	IMFAttributes* pMFAttrs = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pMFMFactory));
	MFCreateAttributes(&pMFAttrs, 1);
	hr = pMFAttrs->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, m_pDXGIManager);
	hr = pMFAttrs->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, m_pNotify);
	hr = pMFAttrs->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM);
	hr = m_pMFMFactory->CreateInstance(MF_MEDIA_ENGINE_WAITFORSTABLE_STATE, pMFAttrs, &m_pMediaEngine);
	hr = m_pMediaEngine->QueryInterface(IID_PPV_ARGS(&m_pMediaEngineEx));
}

void CAvi::ReleaseAvi( void )
{
	CAvi::Stop( );
	DeleteCriticalSection(&m_CriticalSection);

	SAFE_RELEASE(m_pDXGIManager);
	SAFE_RELEASE(m_pMFMFactory);
	SAFE_RELEASE(m_pMediaEngine);
	SAFE_RELEASE(m_pMediaEngineEx);
	SAFE_RELEASE(m_pByteStream);

	SAFE_DELETE(m_pNotify);

	MFShutdown();
}

void CAvi::OnMediaEventNotify(DWORD event, DWORD_PTR param1, DWORD param2)
{
	switch (event)
	{
		case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
		{
			//m_fEOS = FALSE;
		}
		break;
		case MF_MEDIA_ENGINE_EVENT_CANPLAY:
			break;
		case MF_MEDIA_ENGINE_EVENT_PLAY:
			m_bPlaying = TRUE;
			break;
		case MF_MEDIA_ENGINE_EVENT_PAUSE:
			m_bPlaying = FALSE;
			break;
		case MF_MEDIA_ENGINE_EVENT_ENDED:
			{
				Stop();
			}
			break;
		case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
			break;
		case MF_MEDIA_ENGINE_EVENT_ERROR:
		{
			switch (param1)
			{
			case MF_MEDIA_ENGINE_ERR_SRC_NOT_SUPPORTED:
				//不支持的文件.
				MessageBox(NULL, "不支持的视频格式！", "错误", 0);
				break;
			default:
				break;
			}
			Stop();
			break;
		}
		case MF_MEDIA_ENGINE_EVENT_LOADSTART:
		case MF_MEDIA_ENGINE_EVENT_PURGEQUEUEDEVENTS:
		{
		}
			break;
		default:
		{
			break;
		}
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////
// CAvi Create
//////////////////////////////////////////////////////////////////////////////////
BOOL CAvi::SetFileName(const char* Filename, BOOL bLoop, BOOL bPlayBack)
{
	if(Filename == NULL) return FALSE;

	SAFE_RELEASE(m_pByteStream);

	wchar_t wfilename[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, Filename, -1, wfilename, MAX_PATH);
	HRESULT hr = MFCreateFile(MF_ACCESSMODE_READ, MF_OPENMODE_FAIL_IF_NOT_EXIST, 
		MF_FILEFLAGS_NONE, wfilename, &m_pByteStream);
	hr = m_pMediaEngineEx->SetSourceFromByteStream(m_pByteStream, wfilename);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////
// CAvi Start
//////////////////////////////////////////////////////////////////////////////////
void CAvi::Start(void)
{
	if (m_pMediaEngine->HasVideo() && !m_bPlaying)
	{
		m_pMediaEngine->Play();

		m_bPlaying = true;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi Stop
//////////////////////////////////////////////////////////////////////////////////
void CAvi::Stop(void)
{
	m_bPlaying = FALSE;
	if (m_pMediaEngine)
		m_pMediaEngine->Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi NextFrame
//////////////////////////////////////////////////////////////////////////////////
void CAvi::NextFrame(void)
{
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi ReadFrame
//////////////////////////////////////////////////////////////////////////////////
void CAvi::ReadFrame(LONG Frame)
{
}

//////////////////////////////////////////////////////////////////////////////////
// CDXAvi GetSound
//////////////////////////////////////////////////////////////////////////////////
BOOL CAvi::GetSound(VOID)
{	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi DrawBuffer
//////////////////////////////////////////////////////////////////////////////////
HRESULT CAvi::DrawBuffer(void)
{
	const MFARGB rgb = { 0 };
	HRESULT rt = S_OK;

	EnterCriticalSection(&m_CriticalSection);

	if (m_pMediaEngine)
	{
		LONGLONG pts;
		if ((rt = m_pMediaEngine->OnVideoStreamTick(&pts)) == S_OK)
		{
			IDXGISurface* surface = nullptr;

			if (SUCCEEDED(g_xMainWnd.m_pDXGISwapChain4->GetBuffer(0, IID_PPV_ARGS(&surface))))
			{
				DXGI_SURFACE_DESC desc;
				surface->GetDesc(&desc);
				RECT rect;
				rect.left = rect.top = 0;
				rect.right = desc.Width;
				rect.bottom = desc.Height;
				rt = m_pMediaEngine->TransferVideoFrame(surface, nullptr, &rect, &rgb);
				SAFE_RELEASE(surface);
			}
		}
	}

	LeaveCriticalSection(&m_CriticalSection);

	return rt;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi DrawBuffer
//////////////////////////////////////////////////////////////////////////////////
BOOL CAvi::Draw(INT	nLoopTime)
{
	HRESULT rval = 0;

	ReadDatas(nLoopTime);

	rval = DrawBuffer();

	if(!SUCCEEDED(rval)) return FALSE;

	return m_bPlaying;
}

BOOL CAvi::ReadDatas(INT nLoopTime)
{
	return TRUE;
}
