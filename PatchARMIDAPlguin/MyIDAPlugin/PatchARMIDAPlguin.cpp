#include "DlgMain.h"


HINSTANCE g_hInstance;
char g_szReleasePath[256] = {0};

BOOL ReleaseRes(char* strFileName, WORD wResID, char* strFileType)
{
	DWORD   dwWrite=0;          

	// �����ļ�    
	HANDLE  hFile = CreateFile(strFileName, GENERIC_WRITE,FILE_SHARE_WRITE,NULL,    
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);    
	if ( hFile == INVALID_HANDLE_VALUE )    
	{    
		return FALSE;    
	}    

	// ������Դ�ļ��С�������Դ���ڴ桢�õ���Դ��С    
	HRSRC   hrsc =  FindResource(g_hInstance, MAKEINTRESOURCE(wResID), strFileType);    
	HGLOBAL hG = LoadResource(g_hInstance, hrsc);    
	DWORD   dwSize = SizeofResource( g_hInstance,  hrsc);    

	// д���ļ�    
	WriteFile(hFile,hG,dwSize,&dwWrite,NULL);       
	//GlobalFree(hG);
	CloseHandle( hFile );    
	 return TRUE;
}
int __stdcall IDAP_init(void)
{
	//��������һЩУ�飬��ȷ�����Ĳ���Ǳ����ں��ʵĻ����
	//����PLUGIN_SKIP ��PLUGIN_OK����PLUGIN_KEEP�����庬�������
	char szTmpBuf[256] = {0};
	if(PLFM_ARM != ph.id)
	{
		return PLUGIN_SKIP;
	}
	GetModuleFileName(g_hInstance, szTmpBuf, sizeof(szTmpBuf));
	GetShortPathName(szTmpBuf, g_szReleasePath, sizeof(g_szReleasePath));
	*strrchr(g_szReleasePath, '\\') = 0;
#ifdef __EA64__
	strcat(g_szReleasePath, "\\as64.exe");
#else
	strcat(g_szReleasePath, "\\as.exe");
#endif
	if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(g_szReleasePath))
	{
#ifdef __EA64__
		ReleaseRes(g_szReleasePath, IDR_AS64_EXE, "EXE");
#else
		ReleaseRes(g_szReleasePath, IDR_AS_EXE, "EXE");
#endif
	}
//	MessageBox(NULL, g_szReleasePath, NULL, 0);
	return PLUGIN_OK;
}
void __stdcall IDAP_term(void)
{
	//���������ʱ��һ���������ڴ����һ����������Ĵ��롣
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
void __stdcall IDAP_run(int arg)
{
	MSG msg;
	HWND hMainDlg = NULL;
	if( (hMainDlg = FindWindow("#32770","ARM_Code��")) == NULL)
	{
		EnumWindows(EnumWindowsProc,0);
		hMainDlg = CreateDialog(g_hInstance, (LPCTSTR)IDD_DIALOG1, NULL,(DLGPROC)Main_Proc);

		ShowWindow(hMainDlg, SW_SHOWNA);
	}
	else
	{
		SetForegroundWindow(hMainDlg);
	}

// 	while (GetMessage(&msg, NULL, 0, 0))
// 	{
// 		TranslateMessage(&msg);
// 		if(msg.message == WM_CLOSE || msg.message == WM_DESTROY)
// 		{
// 			Main_OnClose(hMainDlg);
// 			break;
// 		}
// 		
// 		DispatchMessage(&msg);
// 		
// 	}
	return;
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(g_hInstance = hModule);
	}
	return (TRUE);
}
// �������Щ�ַ����ǿ��������趨��
char IDAP_comment[] = "Comment of my first ida plugin MyFind. By �յ�";
char IDAP_help[] = "http://bbs.chinapyg.com/";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "MyPatchARMCode";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "shift-SPACE";
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