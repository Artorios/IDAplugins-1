#include <windows.h>
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <expr.hpp>
#include <bytes.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <diskio.hpp>



#pragma comment(lib,"ida.lib")
#define  MSG msg
#define  USHORT ushort




char *dialog =			//�����ڲ���
	"STARTITEM 1\n"			//�õ�һ���ý���
	"RebaseAddr\n\n"	//���ڱ���
	"Please Input Addr\n"	//�ı�����
#ifdef __EA64__
	"<#���ݵ�ַ 0x#Old(Hex):N:19:16::>\n"	//һ��16������
	"<#���ݵ�ַ 0x#New(Hex):N:19:16::>\n";	//һ��16������
#else
	"<#���ݵ�ַ 0x#Old(Hex):N:11:16::>\n"	//һ��16������
	"<#���ݵ�ַ 0x#New(Hex):N:11:16::>\n";	//һ��16������
#endif


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
// ������Դ�plugins.cfg�ļ��У�������һ�����Ͳ�����
// �����²�ͬ���ȼ����߲˵�ʱ������Ҫһ���������ͬ
// ������ʱ����ǳ����á�
void __stdcall IDAP_run(int arg)
{
	// �����ʵ��
	//��LOG����ʾһ���ַ���
	char *lpSavePath;
	char szFileName[256] = {0};
	//	char szTmp[100] = {0};
	uval_t nOldAddr = get_screen_ea();
	uval_t nNewAddr = nOldAddr + 0x100;
	uval_t nSub = 0;
	int j = 0;
	int i = 0;
	FILE *handle;
	FILE *f;
	int size = 0;
	int nCover = 0;
	//	qstrncpy(szInValue,"",sizeof(szInValue));

	if(AskUsingForm_c(dialog, &nOldAddr, &nNewAddr) == 1)
	{
		nSub = nNewAddr - nOldAddr;
		rebase_program(nSub, MSF_FIXONCE);
	}

	return;
}
// �������Щ�ַ����ǿ��������趨��
char IDAP_comment[] = "Comment of my first ida plugin. By �յ�";
char IDAP_help[] = "www.chinapy.com";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "KDReBaseProgram";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "Shift-Alt-R";
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