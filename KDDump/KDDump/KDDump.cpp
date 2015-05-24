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
"STARTITEM 0\n"			//�õ�һ���ý���
"Dump Data \n\n"	//���ڱ���
"Please Input Addr\n"	//�ı�����
#ifdef __EA64__
"<#���ݵ�ַ 0x#StartAddr  :M:17:16::>\n"	//һ��16������
"<#���ݵ�ַ 0x#EndAddr/Len:M:17:16::>\n"	//һ��16������
#else
"<#���ݵ�ַ 0x#StartAddr  :M:9:16::>\n"	//һ��16������
"<#���ݵ�ַ 0x#EndAddr/Len:M:9:16::>\n"	//һ��16������
#endif
"<##Option##EndAddr:R>\n"	//����ѡ���ṩ��
"<Len:R>>\n"		//���ڵĵڶ���
"<##Check Boxes##StartAddr��Ϊ���׵�ַ:C>>\n";

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
	segment_t *curseg;
	USHORT nRadio = 0;
	USHORT checkmask = 0;
	ea_t nStartAddres = get_screen_ea();
	ea_t nEndAddres = nStartAddres + 0x100;
	int j = 0;
	int i = 0;
	FILE *handle;
	FILE *f;
	asize_t size = 0;
	int nCover = 0;
	//	qstrncpy(szInValue,"",sizeof(szInValue));
	if( curseg = getseg(nStartAddres))
	{
		nEndAddres = curseg->endEA;;
	}
	while(!isLoaded(nEndAddres - 1))
	{
		nEndAddres -= 0x100;
	}
	if(AskUsingForm_c(dialog, &nStartAddres, &nEndAddres, &nRadio, &checkmask) == 1)
	{
		if(checkmask)
		{
			nStartAddres = curseg->startEA;
		}
		if(nRadio)
		{
			nEndAddres += nStartAddres;
		}
		msg("==============��ʼDump����==============\n");
#ifdef __EA64__
		msg("StartAddres:0x%llX  EndAddres:0x%llX\n", nStartAddres, nEndAddres);
#else
		msg("StartAddres:0x%08X  EndAddres:0x%08X\n", nStartAddres, nEndAddres);
#endif
		if(isLoaded(nStartAddres) && isLoaded(nEndAddres - 1))
		{
			size = nEndAddres - nStartAddres;
			if(size > 0)
			{
#ifdef __EA64__
				sprintf(szFileName, "%llX-%llX.Dump", nStartAddres, nEndAddres);
#else
				sprintf(szFileName, "%08X-%08X.Dump", nStartAddres, nEndAddres);
#endif
				lpSavePath = askfile_cv(1, szFileName, "SavePath", 0);
				if(lpSavePath == NULL)
				{
					msg("ȡ����Dump\n");
					return;
				}
				if( (f = fopen(lpSavePath, "r")) != NULL)
				{
					fclose(f);
					if( askyn_c(1, "�Ƿ񸲸��ļ�") <= 0)
					{
						warning("Dump ʧ��!\n");
						return;
					}
				}
				handle = fopenWB(lpSavePath);
				if(handle == NULL)
				{
					warning("���ļ�ʧ�� Error!\n");
					return;
				}
				
				uchar * mem = (uchar *)qalloc(size + 1);
				get_many_bytes(nStartAddres,mem, size);
				ewrite(handle, mem, size);
				eclose(handle);
				qfree(mem);
				msg("SavePath:%s\n", lpSavePath);
				msg("==============Dump���ݳɹ�==============\n");
			}
			else
			{
				warning("Error !Size С����!\n");
			}
		}
		else
		{
			if(!isLoaded(nStartAddres))
			{
				warning("Error!  StartAddres��ַ������!\n");
			}
			else
			{
				warning("Error!  EndAddres��ַ������!\n");
			}

		}

	}
	return;
}
// �������Щ�ַ����ǿ��������趨��
char IDAP_comment[] = "Comment of my first ida plugin. By: �յ�.";
char IDAP_help[] = "http://bbs.chinapyg.com/";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "KDDump";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "Alt-d";
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