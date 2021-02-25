#ifndef _CAvi_H_
#define _CAvi_H_

#define RELEASE( x )		if( x != NULL ) { x->Release(); x = NULL; }
#define FREE(p)				if( p )			{ free(p); p=NULL; }

class MediaEngineNotify;

class CAvi
{
public:
	CAvi(void);
	~CAvi(void);
	BOOL	SetFileName(const char* Filename, BOOL bLoop = FALSE, BOOL bPlayBack = TRUE);
	void	Start(void);
	void	Stop(void);
	void	NextFrame(void);
	void	ReadFrame(LONG Frame);
	BOOL	GetSound(VOID);
	HRESULT	DrawBuffer(void);
	void	InitAvi();
	void	ReleaseAvi( void );

	void	OnMediaEventNotify(DWORD event, DWORD_PTR param1, DWORD param2);

	__inline void SetDestRect(RECT rcDst)
	{
		m_DstRect = rcDst;
	}
	__inline void SetStopFrame(INT nFrame)
	{
	}
	virtual BOOL ReadDatas(INT nLoopTime);
	virtual BOOL Draw(INT nLoopTime);
public:
	BOOL	m_bPlaying;


	
	IMFDXGIDeviceManager* m_pDXGIManager;
	IMFMediaEngineClassFactory* m_pMFMFactory;
	IMFMediaEngine* m_pMediaEngine;
	IMFMediaEngineEx* m_pMediaEngineEx;
	IMFByteStream* m_pByteStream;

	MediaEngineNotify* m_pNotify;

	RECT					m_SrcRect;
	RECT					m_DstRect;

	CRITICAL_SECTION		m_CriticalSection;
};

class MediaEngineNotify : public IMFMediaEngineNotify
{
	long m_cRef;
	CAvi* m_Avi;
public:
	MediaEngineNotify(CAvi* pAvi):m_cRef(1), m_Avi(pAvi){};

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override{
		if (__uuidof(IMFMediaEngineNotify) == riid)
		{
			*ppv = static_cast<IMFMediaEngineNotify*>(this);
		}
		else
		{
			*ppv = nullptr;
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) AddRef() override{
		return InterlockedIncrement(&m_cRef);
	}

	STDMETHODIMP_(ULONG) Release() override{
		LONG cRef = InterlockedDecrement(&m_cRef);
		if (cRef == 0)
		{
			delete this;
		}
		return cRef;
	}

	STDMETHODIMP EventNotify(DWORD event, DWORD_PTR param1, DWORD param2) override
	{
		if (event == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
		{
			SetEvent(reinterpret_cast<HANDLE>(param1));
		}
		else
		{
			m_Avi->OnMediaEventNotify(event, param1, param2);
		}
		return S_OK;
	}
};

#endif _CAvi_H_