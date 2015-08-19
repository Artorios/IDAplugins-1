#include <windows.h>
#include <Shlwapi.h>
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <expr.hpp>
#include <bytes.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <diskio.hpp>
#include "ChineseCode.h"


#pragma comment(lib,"ida.lib")
#pragma comment(lib, "Shlwapi.lib")
#define  MSG msg
#define  USHORT ushort
#define  byte uchar
#define  UINT32 uint

static HINSTANCE g_hinstPlugin = NULL;
static char g_szIniPath[MAX_PATH] = { 0 };
static char g_szCopuStringSection[] = "KDCopyString";
static char g_szOptionsKey[] = "Options";
static int g_CheckClip = 1;
static int g_Index = 0;
static const char g_CodeTable[][256] = {"UTF-8", "GB2132", "Unicode"}; 
///////////////////////////////////////////////�����޸���ǿ��֧��ģ��ƥ��
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
///////////////////////////////
char *dialog =			//�����ڲ���
"STARTITEM 0\n"			//�õ�һ���ý���
"ѡ�ж�Ӧ�ı���\n\n"	//���ڱ���
#ifdef __EA64__
"<#���ݵ�ַ 0x#Addres:M:17:16::>\n"
#else
"<#���ݵ�ַ 0x#Addres:M:9:16::>\n"
#endif
"<##Options##UTF-8:R>\n"	//����ѡ���ṩ��
"<GB2132:R>\n"		//���ڵĵڶ���
"<Unicode:R>>\n"
"<##Check Boxes##�Ƿ񽫽�����͵����а�:C>>\n";

//��д�����ļ�
int WritePluginCfg(char *lpFilePath, char *lpPluginName)
{
	char szBuf[256] = {0};
	sprintf(szBuf, "\tCopyStringOptions\t%s\t\"\"\t1", lpPluginName);
	FILE *lpf = fopen(lpFilePath, "rb");
	fseek(lpf, 0, SEEK_END);
	int nFileSize = ftell(lpf);
	fseek(lpf, 0, SEEK_SET);
	char *lpFileBuf = (char*)malloc(nFileSize + 1);
	memset(lpFileBuf, 0, nFileSize + 1);
	fread(lpFileBuf, 1, nFileSize, lpf);
	fclose(lpf);
	//	warning("%s\n", lpFileBuf);
	if(strstr(lpFileBuf, szBuf) == NULL)
	{
		lpf = fopen(lpFilePath, "a+");
		fwrite("\n", 1, 1, lpf);
		fwrite(szBuf, 1, strlen(szBuf), lpf);
		fclose(lpf);
	}
	free(lpFileBuf);
	return 0;
}
int __stdcall IDAP_init(void)
{
	//��������һЩУ�飬��ȷ�����Ĳ���Ǳ����ں��ʵĻ����
	//����PLUGIN_SKIP ��PLUGIN_OK����PLUGIN_KEEP�����庬�������
	GetModuleFileName(g_hinstPlugin, g_szIniPath, sizeof(g_szIniPath));
	g_szIniPath[sizeof(g_szIniPath) - 1] = '\0';

	/* Change the extension of plugin to '.ini'. */
	PathRenameExtension(g_szIniPath, ".ini");
	if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(g_szIniPath))
	{
		char szPluginCfgPath[256] = {0};
		char szModuleName[256] = {0};

		strcpy( szPluginCfgPath, g_szIniPath);
		strcpy(szModuleName, strrchr(szPluginCfgPath, '\\') + 1);
//		*strrchr(szPluginCfgPath, '\\') = 0;
		*strrchr(szModuleName, '.') = 0;
//		strcat(szPluginCfgPath, "\\plugins.cfg");
//		WritePluginCfg(szPluginCfgPath, szModuleName);
		//	warning("cfgPath:%s\n ModuleName:%s\n", szPluginCfgPath, szModuleName);
	}
	/* Get options saved in ini file */
	g_Index = GetPrivateProfileInt(g_szCopuStringSection, g_szOptionsKey, 0, g_szIniPath);
	g_CheckClip = GetPrivateProfileInt(g_szCopuStringSection, "CheckClip", 1, g_szIniPath);

	return PLUGIN_KEEP;

}
void WriteIniFile()
{
	char szBuf[] = "��ʹ�� ACSII, UTF-8, UNICODE ���ֱ����Ӧ����Ϊ0,1,2     ��ͨ����סshift Ȼ����Edit->Plugins->CopyStringOptionsȥ����ini�ļ�";
	char szIndex[10] = {0};
	itoa(g_Index, szIndex, 10);
	WritePrivateProfileString(g_szCopuStringSection, g_szOptionsKey, szIndex, g_szIniPath);
	itoa(g_CheckClip, szIndex, 10);
	WritePrivateProfileString(g_szCopuStringSection, "CheckClip", szIndex, g_szIniPath);
	WritePrivateProfileString(g_szCopuStringSection, "˵��", szBuf, g_szIniPath);
}
void __stdcall IDAP_term(void)
{
	//���������ʱ��һ���������ڴ����һ����������Ĵ��롣
	WriteIniFile();
	return;
}

//��ť�¼�����Ӧ����
void idaapi button_func(TView *fields[], int code)
{
	msg("The button was pressed!\n");
}
// ������Դ�plugins.cfg�ļ��У�������һ�����Ͳ�����
// �����²�ͬ���ȼ����߲˵�ʱ������Ҫһ���������ͬ
// ������ʱ����ǳ����á�
int ReadString(uval_t nAddres, char *lpOutBuf)
{
	int i = 0;
	int iRet = 0;
	switch (g_Index)
	{
	case 0:
		{
			char chTmp = 0;
			char *lpTmpBuf = (char *)malloc(0x1000 * sizeof(char));
			memset(lpTmpBuf, 0, 0x1000 * sizeof(char) );
			while( (chTmp = get_byte(nAddres + i)) && (i < 0x1000))
			{
				lpTmpBuf[i++] = chTmp;
			}
			iRet = Utf8ToGB2312(lpTmpBuf, strlen(lpTmpBuf), lpOutBuf);
			free(lpTmpBuf);
		}
		break;
	case 1:
		{
			char chTmp = 0;
			while( (chTmp = get_byte(nAddres + i)) && (i < 0x1000))
			{
				lpOutBuf[i++] = chTmp;
			}
			iRet = i;
		}
		break;
	case 2:
		{
			ushort usTmp = 0;
			ushort *lpTmpBuf = (ushort *)malloc(0x1000 * sizeof(ushort));
			memset(lpTmpBuf, 0, 0x1000 * sizeof(ushort) );
			while( (usTmp = get_word(nAddres + i * 2)) && (i < 0x1000))
			{
				lpTmpBuf[i++] = usTmp;
			}
			iRet = UnicodeToGB2132((wchar_t *)lpTmpBuf, wcslen((wchar_t*)lpTmpBuf), lpOutBuf);
			free(lpTmpBuf);
		}
		break;
	default:
		break;
	}
	return iRet;
}
void __stdcall IDAP_run(int arg)
{
	//msg("IDAP_run arg: %d \ng_Index:%d \ng_Path :%s\n", arg, g_Index, g_szIniPath);
	// �����ʵ��
	//��LOG����ʾһ���ַ���
	msg("============��ʼ�����ַ���==============\n");
	uval_t nAddres = get_screen_ea();
	int i = 0;
	int j = 0;
	UINT32 size = 0;
	int nCover = 0;
	USHORT checkmask = 0;
	char *lpBuf = (char *)malloc(0x1000);
	memset(lpBuf, 0, 0x1000);
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if(AskUsingForm_c(dialog, &nAddres, &g_Index, &g_CheckClip) == 1)
		{
			WriteIniFile();
			if(!isLoaded(nAddres))
			{
				msg("��ַ��Ч!��ȡ����ʧ��!\n");
				return;
			}
			else
			{
				size = ReadString(nAddres, lpBuf);
			}
		}
	}
	else
	{
		size = ReadString(get_screen_ea(), lpBuf);
	}
	if(g_CheckClip && size)
	{
		msg("�ѽ�����������ͼ��а�!\n");
		SnedClipData(lpBuf, size);
	}
	msg("ʹ��%s�����ӡ�ַ���:\n%s\n", g_CodeTable[g_Index], lpBuf);
	free(lpBuf);
	return;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID)
{
	if (DLL_PROCESS_ATTACH == fdwReason)
	{
		DisableThreadLibraryCalls(g_hinstPlugin = hinstDLL);
	}

	return TRUE;
}
// �������Щ�ַ����ǿ��������趨��
char IDAP_comment[] = "Comment of my first ida plugin MyFind. By �յ�";
char IDAP_help[] = "http://bbs.chinapyg.com/";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "KDCopyString";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "Alt-i";
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