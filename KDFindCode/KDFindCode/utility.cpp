#include "utility.h"
void get_nextval(const char *T, int next[])
{
	// ��ģʽ��T��next����ֵ���������� next��
	int j = 0, k = -1;
	next[0] = -1;
	while ( T[j/*+1*/] != '\0' )
	{
		if (k == -1 || T[j] == T[k])
		{
			++j; ++k;
			if (T[j]!=T[k])
				next[j] = k;
			else
				next[j] = next[k];
		}// if
		else
			k = next[k];
	}// while
	////�������Ҽӵ���ʾ����
	// for(int  i=0;i<j;i++)
	//{
	//     cout<<next[i];
	//}
	//cout<<endl;
}// get_nextval��
unsigned int KMP(const char *Text,const char* Pattern, unsigned int startIndex) //const ��ʾ�����ڲ�����ı����������ֵ��
{
	if( !Text||!Pattern||  Pattern[0]=='\0'  ||  Text[0]=='\0' )//
		return -1;//��ָ���մ�������-1��
	unsigned int len=0;
	const char * c=Pattern;
	while(*c++!='\0')//�ƶ�ָ����ƶ��±�졣
	{    
		++len;//�ַ������ȡ�
	}
	int *next=new int[len+1];
	get_nextval(Pattern,next);//��Pattern��next����ֵ

	unsigned int index=startIndex,i=0,j=0;
	while(Text[i]!='\0'  && Pattern[j]!='\0' )
	{
		if(Text[i]== Pattern[j])
		{
			++i;// �����ȽϺ���ַ�
			++j;
		}
		else
		{
			index += j-next[j];
			if(next[j]!=-1)
				j=next[j];// ģʽ�������ƶ�
			else
			{
				j=0;
				++i;
			}
		}
	}//while

	delete []next;
	if(Pattern[j]=='\0')
		return index + startIndex;// ƥ��ɹ�
	else
		return -1;      
}
void FilterSpacing(char *pbuf)
{
	int i = 0, j = 0;
	int len = 0;
	if(pbuf == NULL || 0 == strlen(pbuf))
	{
		return ;
	}
	len = strlen(pbuf);
	for(i = 0; i < len; i++)
	{
		if(pbuf[i] == ' ' || pbuf[i] == '\t' )
		{
			if(j && pbuf[j - 1] != ' ')
			{
				pbuf[j] = ' ';
				j++;
			}
			continue;
		}
		else
		{
			pbuf[j] = pbuf[i];
			j++;
		}
	}
	pbuf[j] = 0;
}
char*  GetCurrentDate(char *pOutbuf, int nOutSize)
{
	if(pOutbuf && nOutSize)
	{
		SYSTEMTIME stLocal;  
		::GetLocalTime(&stLocal);  
		//��ʾʱ��ļ����  
		_snprintf(pOutbuf,nOutSize,"%u/%u/%u %u:%u:%u",                   
			stLocal.wYear, stLocal.wMonth, stLocal.wDay,  
			stLocal.wHour, stLocal.wMinute, stLocal.wSecond);  
	}
	return pOutbuf;
}
/* ���ֲ���
 * �㷨˼�룺1������������(��С����)��2��ÿ�θ��м����mid�Ƚϣ������ȿ���ֱ�ӷ��أ�
 * �����mid����������Ҵ��һ�ߣ������������С��һ�ߡ�

  ���룺����õ����� - sSource[]�������С - array_size�����ҵ�ֵ - key
  ���أ��ҵ�������Ӧ��λ�ã����򷵻�-1
*/
unsigned int DichotomySearch(unsigned int *sSource, unsigned int array_size, unsigned int key, unsigned int startindex)
{	
	unsigned int low = 0, high = array_size - 1, mid;
	low= startindex;
	while (low <= high)
	{		
		mid = (low + high) / 2;//��ȡ�м��λ��
		
		if (sSource[mid] == key)			
			return mid ;	//�ҵ��򷵻���Ӧ��λ��
		if (sSource[mid] > key)			
			high = mid - 1;	//�����key�������͵�λ�ò���
		else
			low = mid + 1;	//�����keyС�������ߵ�λ�ò���
	}	
	return -1;	
}
void* MALLOC(unsigned int nSize)
{
	void *pret = malloc(nSize);
	if(pret)
	{
		memset(pret, 0, nSize);
	}
	return pret;
}
void FREE(void *p)
{
	if(p)
	{
		free(p);
	}
}
vector<string> tokenize(const string& src, string tok,            
	bool trim, string null_subst)  
{  
	if( src.empty() || tok.empty() ) 
		throw "tokenize: empty string\0";  

	vector<string> v;  
	int pre_index = 0, index = 0, len = 0;  
	while ((index = src.find_first_of(tok, pre_index)) > 0) 
	{  
		if( (len = index-pre_index) > 0 )  
		{
			string s = src.substr(pre_index, len);
			s.erase(0,s.find_first_not_of(" "));	//ɾ�� ��β�ո�
			s.erase(s.find_last_not_of(" ") + 1); 
			v.push_back(s);  
		}
		else if(trim)  
			v.push_back(null_subst);  
		pre_index = index+1;  
	}  
	string endstr = src.substr(pre_index); 
	endstr.erase(0,endstr.find_first_not_of(" "));  
	endstr.erase(endstr.find_last_not_of(" ") + 1); 
	if( trim) v.push_back( endstr.empty()?null_subst:endstr );  
	else if( !endstr.empty() ) 
		v.push_back(endstr);  
	return v;  
}
//��  �ܣ���lpszSour�в����ַ���lpszFind��lpszFind�п��԰���ͨ���ַ���?��
//��  ����nStartΪ��lpszSour�е���ʼ����λ��
//����ֵ���ɹ�����ƥ��λ�ã����򷵻�-1
//ע  �⣺Called by ��bool MatchingString()��
int FindingString(const char* lpszSour, const char* lpszFind, int nStart /* = 0 */)
{
	//	ASSERT(lpszSour && lpszFind && nStart >= 0);
	if(lpszSour == NULL || lpszFind == NULL || nStart < 0)
		return -1;

	int m = strlen(lpszSour);
	int n = strlen(lpszFind);

	if( nStart+n > m )
		return -1;

	if(n == 0)
		return nStart;

	//KMP�㷨
	int* next = new int[n];
	//�õ������ַ�����next����
	{	n--;

	int j, k;
	j = 0;
	k = -1;
	next[0] = -1;

	while(j < n)
	{	if(k == -1 || lpszFind[k] == '?' || lpszFind[j] == lpszFind[k])
	{	j++;
	k++;
	next[j] = k;
	}
	else
		k = next[k];
	}

	n++;
	}

	int i = nStart, j = 0;
	while(i < m && j < n)
	{
		if(j == -1 || lpszFind[j] == '?' || lpszSour[i] == lpszFind[j])
		{	i++;
		j++;
		}
		else
			j = next[j];
	}

	delete []next;

	if(j >= n)
		return i-n;
	else
		return -1;
}

//��	  �ܣ���ͨ������ַ���ƥ��
//��	  ����lpszSour��һ����ͨ�ַ�����
//			  lpszMatch��һ���԰���ͨ������ַ�����
//			  bMatchCaseΪ0�������ִ�Сд���������ִ�Сд��
//��  ��  ֵ��ƥ�䣬����1�����򷵻�0��
//ͨ������壺
//		��*��	���������ַ������������ַ�����
//		��?��	��������һ���ַ�������Ϊ�գ�
//ʱ	  �䣺	2001.11.02	13:00
bool MatchingString(const char* lpszSour, const char* lpszMatch, bool bMatchCase /*  = true */)
{
	//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	if(lpszMatch[0] == 0)//Is a empty string
	{
		if(lpszSour[0] == 0)
			return true;
		else
			return false;
	}

	int i = 0, j = 0;

	//���ɱȽ�����ʱԴ�ַ���'szSource'
	char* szSource =
		new char[ (j = strlen(lpszSour)+1) ];

	if( bMatchCase )
	{	//memcpy(szSource, lpszSour, j);
		while( *(szSource+i) = *(lpszSour+i++) );
	}
	else
	{	//Lowercase 'lpszSour' to 'szSource'
		i = 0;
		while(lpszSour[i])
		{	if(lpszSour[i] >= 'A' && lpszSour[i] <= 'Z')
		szSource[i] = lpszSour[i] - 'A' + 'a';
		else
			szSource[i] = lpszSour[i];

		i++;
		}
		szSource[i] = 0;
	}

	//���ɱȽ�����ʱƥ���ַ���'szMatcher'
	char* szMatcher = new char[strlen(lpszMatch)+1];

	//��lpszMatch���������ġ�*������һ����*�����Ƶ�szMatcher��
	i = j = 0;
	while(lpszMatch[i])
	{
		szMatcher[j++] = (!bMatchCase) ?
			( (lpszMatch[i] >= 'A' && lpszMatch[i] <= 'Z') ?//Lowercase lpszMatch[i] to szMatcher[j]
			lpszMatch[i] - 'A' + 'a' :
		lpszMatch[i]
		) :
		lpszMatch[i];		 //Copy lpszMatch[i] to szMatcher[j]
		//Merge '*'
		if(lpszMatch[i] == '*')
			while(lpszMatch[++i] == '*');
		else
			i++;
	}
	szMatcher[j] = 0;

	//��ʼ����ƥ����

	int nMatchOffset, nSourOffset;

	bool bIsMatched = true;
	nMatchOffset = nSourOffset = 0;
	while(szMatcher[nMatchOffset])
	{
		if(szMatcher[nMatchOffset] == '*')
		{
			if(szMatcher[nMatchOffset+1] == 0)
			{	//szMatcher[nMatchOffset]�����һ���ַ�

				bIsMatched = true;
				break;
			}
			else
			{	//szMatcher[nMatchOffset+1]ֻ����'?'����ͨ�ַ�

				int nSubOffset = nMatchOffset+1;

				while(szMatcher[nSubOffset])
				{	if(szMatcher[nSubOffset] == '*')
				break;
				nSubOffset++;
				}

				if( strlen(szSource+nSourOffset) <
					size_t(nSubOffset-nMatchOffset-1) )
				{	//Դ�ַ���ʣ�µĳ���С��ƥ�䴮ʣ��Ҫ�󳤶�
					bIsMatched = false; //�ж���ƥ��
					break;			//�˳�
				}

				if(!szMatcher[nSubOffset])//nSubOffset is point to ender of 'szMatcher'
				{	//���ʣ�²����ַ��Ƿ�һһƥ��

					nSubOffset--;
					int nTempSourOffset = strlen(szSource)-1;
					//�Ӻ���ǰ����ƥ��
					while(szMatcher[nSubOffset] != '*')
					{
						if(szMatcher[nSubOffset] == '?')
							;
						else
						{	if(szMatcher[nSubOffset] != szSource[nTempSourOffset])
						{	bIsMatched = false;
						break;
						}
						}
						nSubOffset--;
						nTempSourOffset--;
					}
					break;
				}
				else//szMatcher[nSubOffset] == '*'
				{	nSubOffset -= nMatchOffset;

				char* szTempFinder = new char[nSubOffset];
				nSubOffset--;
				memcpy(szTempFinder, szMatcher+nMatchOffset+1, nSubOffset);
				szTempFinder[nSubOffset] = 0;

				int nPos = ::FindingString(szSource+nSourOffset, szTempFinder, 0);
				delete []szTempFinder;

				if(nPos != -1)//��'szSource+nSourOffset'���ҵ�szTempFinder
				{	nMatchOffset += nSubOffset;
				nSourOffset += (nPos+nSubOffset-1);
				}
				else
				{	bIsMatched = false;
				break;
				}
				}
			}
		}		//end of "if(szMatcher[nMatchOffset] == '*')"
		else if(szMatcher[nMatchOffset] == '?')
		{
			if(!szSource[nSourOffset])
			{	bIsMatched = false;
			break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	//���szMatcher[nMatchOffset]�����һ���ַ���
				//��szSource[nSourOffset]�������һ���ַ�
				bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
		else//szMatcher[nMatchOffset]Ϊ�����ַ�
		{
			if(szSource[nSourOffset] != szMatcher[nMatchOffset])
			{	bIsMatched = false;
			break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	bIsMatched = false;
			break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
	}

	delete []szSource;
	delete []szMatcher;
	return bIsMatched;
}

//��  �ܣ�����ƥ�䣬��ͬƥ���ַ���֮���á�,������
//			�磺��*.h,*.cpp��������ƥ�䡰*.h���͡�*.cpp��
//��  ����nMatchLogic = 0, ��ͬƥ�����else���룻bMatchCase, �Ƿ��С����
//����ֵ�����bRetReversed = 0, ƥ�䷵��true������ƥ�䷵��true
//ʱ  �䣺2001.11.02  17:00
bool MultiMatching(const char* lpszSour, const char* lpszMatch, int nMatchLogic /* = 0 */, bool bRetReversed /* = 0 */, bool bMatchCase /* = true */)
{
	//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	char* szSubMatch = new char[strlen(lpszMatch)+1];
	bool bIsMatch;

	if(nMatchLogic == 0)//���
	{	bIsMatch = 0;
	int i = 0;
	int j = 0;
	while(1)
	{	if(lpszMatch[i] != 0 && lpszMatch[i] != ',')
	szSubMatch[j++] = lpszMatch[i];
	else
	{	szSubMatch[j] = 0;
	if(j != 0)
	{
		bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
		if(bIsMatch)
			break;
	}
	j = 0;
	}

	if(lpszMatch[i] == 0)
		break;
	i++;
	}
	}
	else//����
	{	bIsMatch = 1;
	int i = 0;
	int j = 0;
	while(1)
	{	if(lpszMatch[i] != 0 && lpszMatch[i] != ',')
	szSubMatch[j++] = lpszMatch[i];
	else
	{	szSubMatch[j] = 0;

	bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
	if(!bIsMatch)
		break;

	j = 0;
	}

	if(lpszMatch[i] == 0)
		break;
	i++;
	}
	}

	delete []szSubMatch;

	if(bRetReversed)
		return !bIsMatch;
	else
		return bIsMatch;
}
char *trim(char *str)
{
	int i = 0;
	int j = 0;
	int len = strlen(str);

	if(str !=NULL && len>0)
	{
		for (i = 0; i<len; i++)
		{
			if (*(str+i) != ' ' && *(str+i) != '\t')
			{
				break;
			}
		}
		for (j = len-1; j>=0; j--)
		{
			if (*(str+j) !=' ' && *(str+j) != '\t')
			{
				break;
			}
		}
		*(str+j+1) = '\0';
	}
	return str+i;//�ȼ���return memmove(str,str+i,j-i+2);�˴���ʵ�������memcpy���ο���ʵ�֣�
}