// FThread.cpp : 实现文件
//

#include "stdafx.h"
#include "FThread.h"



// FThread

IMPLEMENT_DYNCREATE(FThread, CWinThread)

CString fNmae;

FThread::FThread()
{
}

FThread::~FThread()
{
}

BOOL FThread::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	// 弹出主窗口对话框
	CDialog dlg;
	dlg.SubclassDlgItem(IDD_FDIALOG,this->m_pMainWnd);
	m_pMainWnd = &dlg;
	dlg.DoModal();
	//if(isSend==TRUE)
		CreateServer("");
	//else
		CreateClient("");
	return TRUE;
}

int FThread::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(FThread, CWinThread)
	ON_BN_CLICKED(IDC_FCANCEL, &FThread::OnClickedFcancel)
END_MESSAGE_MAP()


BOOL CreateServer(CString FileName){
	/*sockaddr_in server_addr; 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY; 
	server_addr.sin_port = htons(FILEPORT);
	::memset(server_addr.sin_zero,0,8);
	// 创建socket 
  SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (SOCKET_ERROR == m_Socket) 
    { 
		::AfxMessageBox("Create Socket Error!"); 
    } 
  
  //绑定socket和服务端(本地)地址 
  if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
  { 
    ::AfxMessageBox("Server Bind Failed: "+ WSAGetLastError()); 
  } 
  
  //监听 
  if (SOCKET_ERROR == listen(m_Socket, 10)) 
  { 
    ::AfxMessageBox("Server Listen Failed: "+WSAGetLastError()); 
  }*/
	return 1;
}
BOOL CreateClient(CString FileName){
	return 1;
}

// FThread 消息处理程序


void FThread::OnClickedFcancel()
{
	// TODO:  在此添加控件通知处理程序代码
}
// C:\Users\Jim\Desktop\WinTalk03\WinTalk\FThread.cpp : 实现文件
//


/*

// FDlg 对话框

IMPLEMENT_DYNAMIC(FDlg, CDialog)

FDlg::FDlg(CWnd* pParent /*=NULL*//*)
	: CDialog(FDlg::IDD, pParent)
{

}

FDlg::~FDlg()
{
}

void FDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(FDlg, CDialog)
END_MESSAGE_MAP()


// FDlg 消息处理程序
*/