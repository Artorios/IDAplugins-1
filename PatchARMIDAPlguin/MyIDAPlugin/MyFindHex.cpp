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
#define  byte uchar
#define  UINT32 uint

///////////////////////////////////////////////�����޸���ǿ��֧��ģ��ƥ��
///////////֧�� ģ��ƥ��
void getnext_bin(int sub[], int subSize, int next[])
{
	// �õ�next����,��ʵ����������KMPƥ��
	//	printf("sub bin array : ");
	int i,j;
	i = 0;
	j = -1;
	next[0] = -1;
	//printf("%d", next[i]);
	while(i+1 < subSize)
	{
		if(j==-1 || sub[i]==sub[j] || sub[j] == -1)
		{
			++i;
			++j;
#if 1
			if (sub[i] != sub[j])
			{
				next[i] = j;
			}
			else
			{
				next[i] = next[j];
			}

#else
			next[i] = j;
#endif
			//printf(", %d",next[i]);
		}
		else 
		{
			j = next[j];
		}
	}
	//printf("\n");
}


int kmp_bin(byte main[], int mainSize, int sub[], int subSize, int next[])
{
	// ����s��m�еĵ�һ�����ݵ��±�
	int i,j;
	i = 0;
	j = 0;
	int nIndex = -1;
	while(i < mainSize)
	{
		if(j==-1 || ((int)main[i] & 0xFF)==sub[j] || sub[j] == -1)
		{
			++i;
			++j;
			if(j == subSize)
			{
				nIndex = (i-j);
				break;
			}
		}
		else
		{
			j = next[j];
		}
	}
	return nIndex;
}
/////////////////////////////////////
// ��������:kmp
// ��������:������kmp �����㷨
// ��������:matcher:�������ĵ�ַ
// ��������:mlen:�����ռ�
// ��������:pattern: ���ҵ�16�����ַ���
// ��������:plen:�ؼ��ֳ���
// ��������ֵ:�ɹ���������,����-1
//////////////////////////////////////////
int kmp( byte *matcher, UINT32 mlen, char *pattern, UINT32 plen)
{
	int iRet = 0;
	int nLen = plen / 2;
	int *lpNext = (int *) malloc( (nLen + 1) * sizeof(int));
	int *lpSub = (int *) malloc( (nLen + 1) * sizeof(int));
	int i = 0;
	memset(lpNext, 0, (nLen + 1) * sizeof(int));
	memset(lpSub, 0, (nLen + 1) * sizeof(int));
	for(i = 0; i < nLen; i ++)
	{
		if(pattern[i * 2] == '?')
		{
			lpSub[i] = -1;
		}
		else
		{
			sscanf(pattern + i * 2, "%02X", lpSub + i);
		}

	}
	getnext_bin(lpSub, nLen, lpNext);
	iRet = kmp_bin(matcher, mlen, lpSub, nLen, lpNext);
	free(lpNext);
	free(lpSub);
	return iRet;
}
//ͨ��uAddr��ַ���ҵ�����������
int FindSegIndex(uval_t uAddr)
{
	int seg_qty = get_segm_qty();
	int i = 0;
	segment_t *curseg;
	for(i = 0; i < seg_qty; i++)
	{
		curseg = getnseg(i);
		if(uAddr >= curseg->startEA && uAddr< curseg->endEA)
		{
			return i;
		}
	}
	return -1;
}
///////////////////////////////
char *dialog =			//�����ڲ���
	"STARTITEM 0\n"			//�õ�һ���ý���
	"����������ģ��ƥ��ʹ��??����\n\n"	//���ڱ���
	"Please enter the hexadecimal data size less 0x200\n"	//�ı�����
	"<Hex						:A:1024:32::>\n"	//  256��ʾ�����ַ�������ĳ���,  32��ʾ ������ ��ȵ�һ���ַ�������
	"<#���ݵ�ַ 0x#StartAddr:M:9:16::>\n"	//һ��16������
	"<#���ݵ�ַ 0x#EndAddr		:M:9:16::>\n"	//һ��16������
	"<#�г�����ƥ���ַ:C>>\n"
	"<#��ת����һ�����:C>>\n"
	"<#StartAddr��Ϊ���׵�ַ:C>>\n";



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
	char szValue[MAXSTR + 1] = "";
	char szInValue[MAXSTR + 1] = "";
	//	char szTmp[100] = {0};
	uval_t nStartAddres = get_screen_ea();
	uval_t nEndAddres = nStartAddres + 0x100;
	int FindAddr = 0;
	int i = 0;
	int j = 0;
	UINT32 size = 0;
	int nCover = 0;
	USHORT checkmask = 1;
	USHORT checkjumpto = 1;
	USHORT checkStrartEA = 0;
	//	qstrncpy(szInValue,"",sizeof(szInValue));
	////������������ ��ֱ�ӵ���getseg��������
	i = FindSegIndex(nStartAddres);
	if(i >= 0)
	{
		segment_t *curseg = getnseg(i);
		nEndAddres = curseg->endEA;
	}
	while(!isLoaded(nEndAddres - 1))
	{
		nEndAddres -= 0x100;
	}
	if(AskUsingForm_c(dialog, szInValue, &nStartAddres, &nEndAddres, &checkmask, &checkjumpto, &checkStrartEA) == 1)
	{
		if(checkmask & 1)
		{
			//	nAddres = get_screen_ea();
			segment_t *curseg;
			if( curseg = getseg(nStartAddres))
			{
				nStartAddres = curseg->startEA;;
			}
		}

		int len = strlen(szInValue);
		for(i = 0; i < len; i++)
		{
			if(szInValue[i] != ' ')
			{
				szValue[j++] = szInValue[i];
			}
		}
		len = strlen(szValue);
		int nHexLen = len / 2;
		if(!len)
		{
			warning("����������");
			return;
		}
		if(len % 2)
		{
			warning("���˿ո�����ݳ��Ȳ���2�ı���");
			return;
		}
		for(i = 0;i < len;i++)
		{
			if(szValue[i] != '?'&& !isxdigit(szValue[i]))
			{
				warning("�����к��Ƿ��ַ�");
				return;
			}
		}
		msg("==============��ʼ����==============\n");
		msg("StartAddres:0x%08X  EndAddres:0x%08X\n", nStartAddres, nEndAddres);
		msg("����������Ϊ:%s\n�������:\n", szValue);
		if(isLoaded(nStartAddres) && isLoaded(nEndAddres - 1))
		{
			size = nEndAddres - nStartAddres;
			if(size > 0)
			{
				uchar *mem = (uchar *)malloc(size + 1);
				uchar *lpTmp = mem;
				get_many_bytes(nStartAddres,mem, size);
				do
				{
					FindAddr = kmp(lpTmp, size, szValue, len);
					if(FindAddr < 0)
					{
						break;
					}
					else
					{
						msg("0x%08X\n", FindAddr + nStartAddres + lpTmp - mem);
						if(checkjumpto)
						{
							checkjumpto = 0;
							ea_t addr = FindAddr + nStartAddres + lpTmp - mem;
							if(!jumpto(addr))
							{
								msg("jumpto Error!");
							}
						}
					}
					FindAddr += nHexLen;
					lpTmp += FindAddr;
					size -= FindAddr;
				}while(checkmask == 1);
				free(mem);
				msg("==============��������==============\n");
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
char IDAP_comment[] = "Comment of my first ida plugin MyFind. By �յ�";
char IDAP_help[] = "http://bbs.chinapyg.com/";
// ��Edit->Plugins �˵��У��������ʵ���ƣ�
// ���ܱ��û���plugins.cfg�ļ���д
char IDAP_name[] = "MyFindHex";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "Alt-f";
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