#define  MAXTOOLBARBTNNUM 10
#define  FUNCBTNNUM  MAXTOOLBARBTNNUM

HANDLE hBitmap[MAXTOOLBARBTNNUM];
HPEN	hPen;
BOOL    bDownFlag;
BOOL	bDrawFlag;
int	nDownNum;
HGDIOBJ  hToolBarColor[2];
HWND subhwnd;
DWORD  dwsumwight;

//////////////
BOOL	bDebugStat;				//�Ƿ��ڵ���״̬

UINT	g_uToolBarPos =0;	//������갴��ʱ�����ڵ�ǰ��ť��λ��

int		TOOLBARPOS = 0;//690;			//�Ի��������Ŀ�ʼλ��
int		TOOLBARNUM=MAXTOOLBARBTNNUM;//�Զ��幤������ʵ�ʸ���ť����
int		nCutomToolBarNum=0;	//ʵ�ʹ�������


////////////////��������
int DrawToolBar(HWND hWnd, int npos, HGDIOBJ hgdiobjtmp,HGDIOBJ a3, HGDIOBJ a4,int flag = 1);
int GetButtonPos(int x,int y);
void OpenProcDlg(UINT uid,DLGPROC proc);
void  ShowToolTip(HWND hwnd, char *lpBuf);