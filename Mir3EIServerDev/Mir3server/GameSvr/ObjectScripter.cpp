#include "stdafx.h"

void CScripterObject::Create(int nX, int nY, CMirMap* pMap)
{
	m_dwRunTime			= GetTickCount();
	m_dwRunNextTick		= 5000;

	strcpy_s(m_szName, "小助手");

	m_nCurrX			= nX;
	m_nCurrY			= nY;
	m_nDirection		= 4;
	m_pMap				= pMap;

	m_tFeature.btGender	= 0;
	m_tFeature.btWear	= 4;
	m_tFeature.btHair	= 1;
	m_tFeature.btWeapon	= 5;

//	m_fScriptFile = fopen("capture.txt", "r+");
	
	pMap->AddNewObject(nX, nY, OS_MOVINGOBJECT, this);

	AddRefMsg(RM_TURN, m_nDirection, nX, nY, 0, m_szName);
}

void CScripterObject::Operate()
{
	int n = rand() % 7;


	if (rand() % 10  == 0)
	{
		char szText[128];
		char szEncodeText[128];

		if (rand() % 2 == 0)
			strcpy_s(szText, "你好，我是EI运营小助手。");
		else
			strcpy_s(szText, "@移动 鹿1");

		int nPos = fnEncode6BitBufA((unsigned char *)szText, szEncodeText, memlen(szText) - 1, sizeof(szEncodeText));
		szEncodeText[nPos] = '\0';

		ProcessForUserSaid(szEncodeText);
	}
	else
		WalkTo(n);
}