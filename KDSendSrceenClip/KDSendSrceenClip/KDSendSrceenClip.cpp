#include <windows.h>
#include <Shlwapi.h>
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <stdio.h>



#pragma comment(lib,"ida.lib")
#define  MSG msg
#define  USHORT ushort


/////////////////////////////////////////////
int SnedClipData(char *lpBuf, int nlen)
{
	if( 0 == nlen || lpBuf == NULL)
	{
		return 0;
	}
	if(OpenClipboard(NULL)) //OpenClipboard()�Ǵ򿪼��а�ɹ�����TRUE ʧ�ܷ���FALSE
	{
		HANDLE handle;
		char* pBuf;
		EmptyClipboard(); //��ռ��а��ü��а�Ȩ��
		handle=GlobalAlloc(GMEM_MOVEABLE,nlen + 1); //�����ڴ�ռ�                                                  
		pBuf=(char*)GlobalLock(handle); //�����ռ�
		memcpy(pBuf,lpBuf, nlen); //���ռ�д������
		GlobalUnlock(handle); //�����ռ�
		SetClipboardData(CF_TEXT,handle);  //���ü��а�������
	}
	CloseClipboard();  //�رռ��а� һ��Ҫ�ر� �������������޷�ʹ�ü��а�
	return 0;
}

int __stdcall IDAP_init(void)
{
	//��������һЩУ�飬��ȷ�����Ĳ���Ǳ����ں��ʵĻ����
	//����PLUGIN_SKIP ��PLUGIN_OK����PLUGIN_KEEP�����庬�������
	return PLUGIN_OK;
}
void __stdcall IDAP_term(void)
{
	//���������ʱ��һ���������ڴ����һ����������Ĵ��롣
	return;
}

// ������Դ�plugins.cfg�ļ��У�������һ�����Ͳ�����
// �����²�ͬ���ȼ����߲˵�ʱ������Ҫһ���������ͬ
// ������ʱ����ǳ����á�
void __stdcall IDAP_run(int arg)
{
	char szBuf[512] = {0};
#ifdef __EA64__
	sprintf(szBuf, "0x%llX", get_screen_ea());
#else
	sprintf(szBuf, "0x%08X", get_screen_ea());
#endif 
	SnedClipData(szBuf, strlen(szBuf));
	msg("SendScreenAddrClip: %s\n", szBuf);
	return;
}
// �������Щ�ַ����ǿ��������趨��
char IDAP_comment[] = "Comment of my first ida plugin. By: �յ�.";
char IDAP_help[] = "http://bbs.chinapyg.com/";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "KDSendScreenAddrClip";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "Shift-Alt-x";
// ����PLUGIN���󵼳�����Ҫ���ԡ�
//�����һ��Ҫ����ģ�ԭ��ο�����
plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION, // IDA version plug-in is written for
	0, // Flags (see below)
	IDAP_init, // Initialisation function
	IDAP_term, // Clean-up function
	IDAP_run, // Main plug-in body
	IDAP_comment, // Comment �Cunused
	IDAP_help, // As above �Cunused
	IDAP_name, // Plug-in name shown in
	// Edit->Plugins menu
	IDAP_hotkey // Hot key to run the plug-in
};