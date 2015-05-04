
// WinTalk.h : WinTalk 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "afxwin.h"
#include <afxcmn.h>
#include <winsock2.h> 
#include <commctrl.h>
#include <afxtempl.h>

#define BUFFER_SIZE 512
//using std::queue;

// 告诉连接器与WS2_32库连接
#pragma comment(lib,"WS2_32.lib")
// 链接到comctl32.lib库
#pragma comment(lib,"comctl32.lib")


// CWinTalkApp:
// 有关此类的实现，请参阅 WinTalk.cpp
//


class CWinTalkApp : public CWinApp
{
public:
	CWinTalkApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

public:
	//afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWinTalkApp theApp;

// MyDlg dialog

class MyDlg : public CDialog
{
	DECLARE_DYNAMIC(MyDlg)

public:
	MyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MyDlg();
	void AddList(sockaddr_in);
	BOOL UDPMulticast();
	BOOL CreateServer();
	BOOL SendMC(char);	//发送组播
	BOOL CreateFServer(char *);
	BOOL CreateFClient(char *,sockaddr_in);
// Dialog Data
	enum { IDD = IDD_MAINDIALOG };

protected:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// 套节字通知事件
	afx_msg long OnSocketMC(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSocketMSG(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl mListHost;
	CEdit mEditInput;
	CEdit mEditLog;
	CArray<sockaddr_in, sockaddr_in&> s_List;
	SOCKET socketMC;
	SOCKET socketMsg;
	sockaddr_in localaddr;
	sockaddr_in remote;
	char hostname[128];
	struct hostent*pHost;

	CString mLogStr ;
	CString mInputStr ;
	afx_msg void OnClickedButtonMsg();
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnClickedButtonFlush();
};
