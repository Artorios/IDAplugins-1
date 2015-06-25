#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <expr.hpp>
#include <bytes.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <diskio.hpp>
#include <dbg.hpp>


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

bool kmpcmp(unsigned char by, int sub)
{
	if( (by & (~(sub >> 24))) == (sub & 0xFF) )
	{
		return true;
	}
	return false;
}
int kmp_bin(unsigned char main[], int mainSize, int sub[], int subSize, int next[])
{
	// ����s��m�еĵ�һ�����ݵ��±�
	int i,j;
	i = 0;
	j = 0;
	int nIndex = -1;
	while(i < mainSize)
	{
		if(j==-1 || sub[j] == -1 || kmpcmp(main[i], sub[j]))
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
int CharToHex(char ch)
{
	const char szTable[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	int i = 0;
	for(i = 0; i < sizeof(szTable); i++)
	{
		if(ch == szTable[i])
		{
			return i;
		}
	}
	return -1;
}
/////////////////////////////////////
// ��������:kmp
// ��������:������kmp �����㷨
// ��������:matcher:�������ĵ�ַ
// ��������:mlen:�����ռ�
// ��������:pattern: ���ҵ�16�����ַ���
// ��������:plen:�ؼ��ֳ���
// ��������:nStartPost: ����������ʼλ��
// ��������ֵ:�ɹ���������,����-1
//////////////////////////////////////////
int kmp( unsigned char *matcher, unsigned int mlen, char *pattern, unsigned int plen, int nStartPost)
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
		if(pattern[i * 2] == '?' && pattern[i * 2 + 1] == '?')
		{
			lpSub[i] = -1;
		}
		else if(pattern[i * 2] == '?')
		{
			lpSub[i] = 0xF0000000 + CharToHex(pattern[i * 2 + 1]);
		}
		else if(pattern[i * 2 + 1] == '?')
		{
			lpSub[i] = 0x0F000000 + CharToHex(pattern[i * 2]) * 0x10;
		}
		else
		{
			sscanf(pattern + i * 2, "%02X", lpSub + i);
		}
	}
	getnext_bin(lpSub, nLen, lpNext);
	iRet = kmp_bin(matcher + nStartPost, mlen - nStartPost, lpSub, nLen, lpNext);
	free(lpNext);
	free(lpSub);
	if(iRet < 0)
	{
		return iRet;
	}
	else
	{
		return iRet + nStartPost;
	}
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
	"����������ģ��ƥ��ʹ��?����\n\n"	//���ڱ���
	"Please enter the hexadecimal data size less 0x200\n"	//�ı�����
	"<Hex						:A:1024:32::>\n"	//  256��ʾ�����ַ�������ĳ���,  32��ʾ ������ ��ȵ�һ���ַ�������
#ifdef __EA64__
	"<#���ݵ�ַ 0x#StartAddr:M:17:16::>\n"	//һ��16������
	"<#���ݵ�ַ 0x#EndAddr		:M:17:16::>\n"	//һ��16������
#else
	"<#���ݵ�ַ 0x#StartAddr:M:9:16::>\n"	//һ��16������
	"<#���ݵ�ַ 0x#EndAddr		:M:9:16::>\n"	//һ��16������
#endif
	"<#�г�����ƥ���ַ:C>>\n"
	"<#��ת����һ�����:C>>\n"
	"<#StartAddr��Ϊ���׵�ַ:C>>\n"
	"<#�������ж�:C>>\n";



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
	ea_t nStartAddres = get_screen_ea();
	ea_t nEndAddres = nStartAddres + 0x100;
	int FindAddr = 0;
	int i = 0;
	int j = 0;
	UINT32 size = 0;
	int nCover = 0;
	segment_t *curseg;
	USHORT checkmask = 1;
	USHORT checkjumpto = 1;
	USHORT checkStrartEA = 1;
	USHORT checkAllSeg = 0;
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
	if(PLFM_ARM == ph.id)
	{
		regval_t  val;
		get_reg_val("pc", &val);
		if(val.ival == -1)
		{
			checkAllSeg = 1;
			checkStrartEA = 0;
		}
	}
	else
	{
		regval_t  val;
		get_reg_val("eip", &val);
		if(val.ival == -1)
		{
			checkAllSeg = 1;
			checkStrartEA = 0;
		}
	}
	if(AskUsingForm_c(dialog, szInValue, &nStartAddres, &nEndAddres, &checkmask, &checkjumpto, &checkStrartEA, &checkAllSeg) == 1)
	{
		if(checkAllSeg & 1)
		{
			int seg_qty = get_segm_qty();
			if(seg_qty > 0)
			{
				curseg = getnseg(0);
				nStartAddres = curseg->startEA;
				nEndAddres = curseg->endEA - 1;
				for(i = 1; i < seg_qty; i++)
				{
					curseg = getnseg(i);
					if(isLoaded(curseg->endEA - 1) )// && SEG_CODE == curseg->type)
					{
						nEndAddres = curseg->endEA - 1;
					}
				}

			}
		}
		else if(checkmask & 1)
		{
			//	nAddres = get_screen_ea();
			
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
#ifdef __EA64__
		msg("StartAddres:0x%llX  EndAddres:0x%llX\n", nStartAddres, nEndAddres);
#else
		msg("StartAddres:0x%08X  EndAddres:0x%08X\n", nStartAddres, nEndAddres);
#endif
		msg("����������Ϊ:%s\n�������:\n", szValue);
		if(isLoaded(nStartAddres) && isLoaded(nEndAddres - 1))
		{
			size = nEndAddres - nStartAddres;
			if(size > 0)
			{
				uchar *mem = (uchar *)malloc(size + 1);
				get_many_bytes(nStartAddres,mem, size);
				do
				{
					FindAddr = kmp(mem, size, szValue, len, FindAddr);
					if(FindAddr < 0)
					{
						break;
					}
					else
					{
					#ifdef __EA64__
						msg("0x%llX\n", FindAddr + nStartAddres);
					#else
						msg("0x%08X\n", FindAddr + nStartAddres);
					#endif
						if(checkjumpto)
						{
							checkjumpto = 0;
							ea_t addr = FindAddr + nStartAddres;
							if(!jumpto(addr))
							{
								msg("jumpto Error!");
							}
						}
					}
					FindAddr += nHexLen;
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
char IDAP_name[] = "KDFindHex";
// ����������ȼ������ַ��Ķ��壬�Ƚ��׶�
char IDAP_hotkey[] = "shift-Alt-f";
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