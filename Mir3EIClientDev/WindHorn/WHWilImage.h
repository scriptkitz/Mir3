/******************************************************************************************************************
                                                                                                                   
	模块名称:																											
																													
	作者:																											
	创建日期:																											
																													
	[日期] [修订]：修订内容 																						
                                                                                                                   
*******************************************************************************************************************/



#ifndef _WILIMAGEHANDLER_
#define	_WILIMAGEHANDLER_



/******************************************************************************************************************

	CWilImageData Class Declaration

   将图像加载到内存中或以文件内存映射的方式设置在所需的偏移位置。

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
	BOOL				m_bIsMemMapped;				// 是否使用内存映射。
	BOOL				m_bIsCompressed;			// 是否压缩。 

	LPBYTE				m_pbStartData;				// 数据起始地址（整个数据区域与文件大小相同。） 

	INT					m_nCurrImageIdx;			// 当前图形的索引。 


public:
	NEWWIXIMAGEINFO		m_stNewWixImgaeInfo;		//wix文件头

	LPWILIMAGEINFO		m_lpstCurrWilImageInfo;		// 指向当前信息的指针。 
	LPNEWWILIMAGEINFO	m_lpstNewCurrWilImageInfo;	

	BYTE*				m_pbCurrImage;				// 指向当前图像的指针将被归档。 
	TCHAR				m_szWilFileName[MAX_PATH];

	CWHWilImageData();
	~CWHWilImageData();

	BOOL NewLoad(CHAR* szWilFile, BOOL bComp = TRUE);
	BOOL NewSetIndex(DWORD dwIndex);				// 用从外部接收到的索引设置表示当前图像信息的地址。 


	BOOL Load(CHAR* szWilFile, BOOL bIsMemMapped = TRUE, BOOL bComp = TRUE);
	VOID Init();
	VOID Destroy();
	VOID SetIndex(INT nIndex);						// 用从外部接收到的索引设置表示当前图像信息的地址。 
};


#endif //_WILIMAGEHANDLER_
