
#include "DlgMain.h"


HINSTANCE g_hInstance;
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

//��ť�¼�����Ӧ����
void idaapi button_func(TView *fields[], int code)
{
	msg("The button was pressed!\n");
}


void __stdcall IDAP_run(int arg)
{

	HWND hMainDlg = NULL;
	if(NULL == (hMainDlg=FindWindow("#32770", "Find Code��")) )
	{
		EnumWindows(EnumWindowsProc,0);
		hMainDlg = CreateDialog(g_hInstance, (LPCTSTR)IDD_DIALOG_FINDCODE, NULL,(DLGPROC)Main_Proc);
		if(NULL == hMainDlg)
		{
			msg("hMainDlg NULL 0x%X\n",GetLastError());
		}
		else
		{
			ShowWindow(hMainDlg, SW_SHOWNA);
		}
	}
	else
	{
		POINT p;
		GetCursorPos(&p);
		RECT rect;
		GetWindowRect(hMainDlg,&rect);   //rect���洰�ڴ�С
		MoveWindow(hMainDlg, p.x, p.y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		SwitchToThisWindow(hMainDlg, TRUE);
	}
	
	

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
char IDAP_comment[] = "Comment of my first ida plugin. By: �յ�.";
char IDAP_help[] = "http://bbs.chinapyg.com/";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "KDFindCode";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "Shift-Alt-t";
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