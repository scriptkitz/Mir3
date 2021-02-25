/******************************************************************************************************************
                                                                                                                   
	ģ������:																											
																													
	����:																											
	��������:																											
																													
	[����] [�޶�]���޶����� 																						
                                                                                                                   
*******************************************************************************************************************/



#ifndef _WILIMAGEHANDLER_
#define	_WILIMAGEHANDLER_



/******************************************************************************************************************

	CWilImageData Class Declaration

   ��ͼ����ص��ڴ��л����ļ��ڴ�ӳ��ķ�ʽ�����������ƫ��λ�á�

*******************************************************************************************************************/


#pragma pack(1)

typedef struct tagWILFILEHEADER
{
	CHAR	szTmp[40];
	INT		nImageCount;
}WILFILEHEADER, *LPWILFILEHEADER;

typedef struct tagWILFILEIMAGEINFO
{
	SHORT	shWidth;
	SHORT	shHeight;
	SHORT	shPX;
	SHORT	shPY;
	DWORD	dwImageLength;
}WILIMAGEINFO, *LPWILIMAGEINFO;

typedef struct tagWIXFILEIMAGEINFO
{
	CHAR	szTmp[40];
	INT		nIndexCount;
	INT*	pnPosition;
}WIXIMAGEINFO, *LPWIXIMAGEINFO;


typedef struct tagNEWWILFILEHEADER
{
	SHORT	shComp;
	CHAR	szTitle[20];
	SHORT	shVer;
	INT		nImageCount;
}NEWWILFILEHEADER, *LPNEWWILFILEHEADER;

typedef struct tagNEWWILFILEIMAGEINFO
{
	SHORT	shWidth;
	SHORT	shHeight;
	SHORT	shPX;
	SHORT	shPY;
	CHAR	bShadow;					
	SHORT	shShadowPX;
	SHORT	shShadowPY;
	DWORD	dwImageLength;
}NEWWILIMAGEINFO, *LPNEWWILIMAGEINFO;
typedef struct tagNEWWIXFILEIMAGEINFO
{
	CHAR	szTitle[20];
	INT		nIndexCount;
	INT*	pnPosition;
}NEWWIXIMAGEINFO, *LPNEWWIXIMAGEINFO;

#pragma pack(8)


class CWHWilImageData
{
private:
protected:
	BOOL				m_bIsMemMapped;				// �Ƿ�ʹ���ڴ�ӳ�䡣
	BOOL				m_bIsCompressed;			// �Ƿ�ѹ���� 

	LPBYTE				m_pbStartData;				// ������ʼ��ַ�����������������ļ���С��ͬ���� 

	INT					m_nCurrImageIdx;			// ��ǰͼ�ε������� 


public:
	NEWWIXIMAGEINFO		m_stNewWixImgaeInfo;		//wix�ļ�ͷ

	LPWILIMAGEINFO		m_lpstCurrWilImageInfo;		// ָ��ǰ��Ϣ��ָ�롣 
	LPNEWWILIMAGEINFO	m_lpstNewCurrWilImageInfo;	

	BYTE*				m_pbCurrImage;				// ָ��ǰͼ���ָ�뽫���鵵�� 
	TCHAR				m_szWilFileName[MAX_PATH];

	CWHWilImageData();
	~CWHWilImageData();

	BOOL NewLoad(CHAR* szWilFile, BOOL bComp = TRUE);
	BOOL NewSetIndex(DWORD dwIndex);				// �ô��ⲿ���յ����������ñ�ʾ��ǰͼ����Ϣ�ĵ�ַ�� 


	BOOL Load(CHAR* szWilFile, BOOL bIsMemMapped = TRUE, BOOL bComp = TRUE);
	VOID Init();
	VOID Destroy();
	VOID SetIndex(INT nIndex);						// �ô��ⲿ���յ����������ñ�ʾ��ǰͼ����Ϣ�ĵ�ַ�� 
};


#endif //_WILIMAGEHANDLER_
