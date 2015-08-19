#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <stdio.h>


#pragma comment(lib,"ida.lib")
#define  MSG msg
#define  USHORT ushort

char *dialog =			//�����ڲ���
	"STARTITEM 0\n"			//�õ�һ���ý���
	"Patch Datan\n\n"	//���ڱ���
	"Please enter the hexadecimal data size less 0x200\n"	//�ı�����
	"<String:A:32:32::>\n"	//��һ���ַ�������
	"<#���ݵ�ַ 0x#Addres:M:9:16::>\n"	//һ��16������
	"<ѭ������  0x#nCount:M:9:16::>\n"	//һ��16��������
	// "<ѭ������:D:256:10::>\n"	//һ��10��������	//ע�������256 �����������ֵķ�Χ  �����15��ʾ�༭��ĳ���
	"<##����д��������д��##Write:R>\n"	//����ѡ���ṩ��
	"<XorWrite:R>>\n"		//���ڵĵڶ���
	"<##Check Boxes##ȡ����ַ ����Addresֵ:C>>\n";	//����ѡ���ṩ��


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
	char *lpInBuf = NULL;
	char *lpTmpBuf = NULL;
	char *lpFilePath = NULL;
	char szValue[MAXSTR + 1] = "";
	char szInValue[MAXSTR + 1] = "";
	//	char szTmp[100] = {0};
	int nPatchLogFlags = 1;
	uval_t nAddres = get_screen_ea();
	asize_t nCount = 1;
	USHORT nRadio = 0;		//��ѡ��ť
	USHORT checkmask = 0;
	uint32 j = 0;
	uint32 i = 0;
	//	qstrncpy(szInValue,"",sizeof(szInValue));

	if(AskUsingForm_c(dialog, szInValue, &nAddres, &nCount, &nRadio, &checkmask) == 1)
	{
		if(checkmask & 1)
		{
			nAddres = get_screen_ea();
		}
		uint32 len = strlen(szInValue);
		for(i = 0; i < len; i++)
		{
			if(szInValue[i] != ' ')
			{
				szValue[j++] = szInValue[i];
			}
		}
		len = strlen(szValue);
		uint32 nHexLen = len / 2;
		if(!len)
		{
			if( lpFilePath = askfile_cv(0, "*.*", "OpenPath", 0))
			{
				FILE* handle = fopen(lpFilePath, "rb");
				if(handle == NULL)
				{
					warning("���ļ�ʧ�� Error!\n");
					return;
				}
				fseek(handle, 0, SEEK_END);
				nHexLen = ftell(handle);
				fseek(handle, 0, SEEK_SET);
				lpTmpBuf = (char *)malloc(nHexLen + 1);
				memset(lpTmpBuf, 0, nHexLen + 1);
				fread(lpTmpBuf, 1, nHexLen, handle);
				fclose(handle);
				strcpy(szValue, lpFilePath);
			}
			else
			{
				warning("����������");
				return;
			}

		}
		else if(len % 2)
		{
			warning("���ݳ��Ȳ���2�ı���");
			return;
		}
		else
		{
			for(i = 0;i < len;i++)
			{
				if(!isxdigit(szValue[i]))
				{
					warning("�����к��з�16����ֵ");
					return;
				}
			}
			lpTmpBuf = (char *) malloc(nHexLen + 1);
			memset(lpTmpBuf, 0, nHexLen + 1);
			for(i = 0; i < nHexLen; i++)
			{
				sscanf(&szValue[i * 2],"%02x",lpTmpBuf + i);
			}
		}
		lpInBuf = (char*)malloc(nHexLen * nCount + 1);
		memset(lpInBuf, 0, nHexLen * nCount + 1);
		for(i = 0; i < nCount; i++)
		{
			memcpy(lpInBuf + i * nHexLen, lpTmpBuf, nHexLen);
		}
		free(lpTmpBuf);

		msg("==============�޲����ݿ�����==============\n");
		msg("String Value = %s\n",szValue);
		msg("Addres:0x%08X  ValueSize = 0x%08X  Index:%d check = %d\n",nAddres, nHexLen, nCount,checkmask);
		if(!nRadio)
		{
			MSG("WriteData \n",nRadio);
		}
		else
		{
			MSG("XorWrite Data \n",nRadio);
		}
		if( nHexLen * nCount > 0x100)
		{
			nPatchLogFlags = askyn_c(0, "%s", "�Ƿ��ӡ��־!(Ĭ�ϲ���ӡ)\n��Ϊ�����������Ӱ��IDA�����!\nNo��Cancel��ʾ����ӡ��־!\n");
		}
		////////////////////	
		uint32 nSum = 0;
		uchar *mem = NULL;
		if(isLoaded(nAddres) && isLoaded(nAddres + nCount *  nHexLen - 1))
		{

			mem = (uchar *)malloc(nCount *  nHexLen + 1);
			if(mem == NULL)
			{
				warning("malloc ����ִ��ʧ��!");
				return;
			}
			get_many_bytes(nAddres,mem, nCount *  nHexLen);
			if(nPatchLogFlags > 0)
			{
				msg("ԭʼ����:\n");
				for(i = 0; i < nCount *  nHexLen; i++)
				{	
					msg("%02X", mem[i]);
				}
				msg("\n");
			}

		}
		else
		{
			if(!isLoaded(nAddres))
			{
				warning("���ݵ�ַ����: 0x%p",nAddres);
			}
			else
			{
				warning("д�����ݳ��ȹ���");
			}
			return;
		}
		///////////////////////////
		//		long nValue = 0;
		for(j = 0;j < nCount;j++)
		{
			for(i = 0;i < nHexLen; i++)
			{
				//				memcpy(szTmp,&szInValue[j * len + i * 2],2);
				//sscanf(&szValue[i * 2],"%02x",&nValue);
				if(nRadio == 1)
				{
					lpInBuf[nSum] ^=   mem[nSum];
				}
				mem[nSum++] = lpInBuf[nSum];

			}
		}
		put_many_bytes(nAddres, mem, nSum);
		if(nPatchLogFlags > 0)
		{
			msg("��������:\n");
			for(i = 0; i < nSum; i++)
			{	
				msg("%02X", mem[i]);
			}
			msg("\n");
		}
		refresh_idaview_anyway();		//ˢ�·���ര��
		free(mem);
		free(lpInBuf);

	}
	return;
}
// �������Щ�ַ����ǿ��������趨��
char IDAP_comment[] = "Comment of my first ida plugin. By �յ�";
char IDAP_help[] = "www.chinapy.com";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "Patch Addres Data";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "Alt-P";
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