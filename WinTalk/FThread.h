#pragma once


#include "resource.h"
#include "afxdialogex.h"

// FThread

class FThread : public CWinThread
{
	DECLARE_DYNCREATE(FThread)

protected:
	virtual ~FThread();

public:
	FThread();           // 动态创建所使用的受保护的构造函数
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	BOOL CreateServer(CString FileName);
	BOOL CreateClient(CString FileName);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedFcancel();
};

/*
#pragma once


// FDlg 对话框

public class FDlg : public CDialog
{
	DECLARE_DYNAMIC(FDlg)

public:
	FDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~FDlg();

// 对话框数据
	enum { IDD = IDD_FDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
*/