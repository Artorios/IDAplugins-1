#ifndef __U_MAINCALE_HEADER_
#define  __U_MAINCALE_HEADER_
#include <windows.h>  
#include <commctrl.h>  
#include "resource.h"
#define  RETLEN 20
#define  MAX_BUF 512
#define BUFSIZE 1024

LRESULT CALLBACK CalcDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam); 
LRESULT CALLBACK NewEditProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LJNewEditProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK JZNewEditProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK JZNewEditProc2(HWND, UINT, WPARAM, LPARAM);

void Dlg_Init(HWND hnwd);
BOOL IsHex(TCHAR buf);
BOOL LjIsHex(TCHAR buf);
unsigned long HexToDec(char *s);
void HexCharUpper(char *s);
BOOL JZIsDec(TCHAR buf);
BOOL JZIsHex(TCHAR buf);
//��������
//+
void Ssys_Add(HWND hwnd);
//-
void Ssys_Sub(HWND hwnd);
//*
void Ssys_Mul(HWND hwnd);
//  /
void Ssys_Div(HWND hwnd);
//ȡ��
void Ssys_Mod(HWND hwnd);

//�߼�����
//XOR
void Ljys_XOR(HWND hwnd);
//AND
void Ljys_AND(HWND hwnd);
//OR
void Ljys_OR(HWND hwnd);
//NOT
void Ljys_NOT(HWND hwnd);
//SHL
void Ljys_SHL(HWND hwnd);
//SHR
void Ljys_SHR(HWND hwnd);

//����ת��
void JZZH_TextZH(HWND hwnd);
void JZZH_DecToHex(HWND hwnd);
void JZZH_HexToDec(HWND hwnd);
//
DWORD GetDirverInfo(LPSTR szDrive);

#endif