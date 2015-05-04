
// WinTalk.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "WinTalk.h"
#include "MainFrm.h"
#include "FDialog.h"

// 定义网络事件通知消息
#define WM_SOCKET_MC WM_USER + 101
#define WM_SOCKET_MSG WM_USER + 102

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinTalkApp

BEGIN_MESSAGE_MAP(CWinTalkApp, CWinApp)
	//ON_COMMAND(ID_APP_ABOUT, &CWinTalkApp::OnAppAbout)
END_MESSAGE_MAP()


// CWinTalkApp 构造

CWinTalkApp::CWinTalkApp()
{
	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	//SetAppID(_T("WinTalk.AppID.NoVersion"));
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CWinTalkApp 对象

CWinTalkApp theApp;


// CWinTalkApp 初始化

BOOL CWinTalkApp::InitInstance()
{
	CWinApp::InitInstance();

	// 初始化Winsock库
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);
	// 弹出主窗口对话框
	MyDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

int CWinTalkApp::ExitInstance()
{
	//TODO:  处理可能已添加的附加资源
	return CWinApp::ExitInstance();
}

// CWinTalkApp 消息处理程序

// MyDlg dialog

IMPLEMENT_DYNAMIC(MyDlg, CDialog)

MyDlg::MyDlg(CWnd* pParent /*=NULL*/)
: CDialog(MyDlg::IDD, pParent)
, mInputStr(_T(""))
, mLogStr(_T(""))
{
}

MyDlg::~MyDlg()
{
}

void MyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, mInputStr);
	DDV_MaxChars(pDX, mInputStr, 500);
	DDX_Text(pDX, IDC_EDIT_LOG, mLogStr);
	DDV_MaxChars(pDX, mLogStr, 10000);
}


BEGIN_MESSAGE_MAP(MyDlg, CDialog)
	ON_MESSAGE(WM_SOCKET_MC,OnSocketMC)
	ON_MESSAGE(WM_SOCKET_MSG, OnSocketMSG)
	ON_BN_CLICKED(IDC_BUTTON_MSG, &MyDlg::OnClickedButtonMsg)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &MyDlg::OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_FLUSH, &MyDlg::OnClickedButtonFlush)
END_MESSAGE_MAP()


// MyDlg message handlers
void MyDlg::OnCancel()
{
	//TODO 
	SendMC('Q');
	if(socketMC != INVALID_SOCKET)
	::closesocket(socketMC);
	if(socketMsg != INVALID_SOCKET)
	::closesocket(socketMsg);
	::WSACleanup();
	CDialog::OnCancel();
}

BOOL MyDlg::OnInitDialog()
{
	mListHost.SubclassDlgItem(IDC_LIST_HOST,this);
	LONG lStyle;
	lStyle = GetWindowLong(mListHost.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(mListHost.m_hWnd, GWL_STYLE, lStyle);//设置style
	mListHost.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES/*|LVS_EX_CHECKBOXES*/); //设置扩展风格
	mListHost.InsertColumn( 0, "IP", LVCFMT_LEFT, 175 );//插入列

	
	if(gethostname(hostname,128)!=0){
		::AfxMessageBox("获取用户名失败");
	}
	pHost = gethostbyname(hostname);

	socketMC = ::socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	UDPMulticast();	//初始化组播网络发现
	SendMC('J');	//发送组播消息,J取"JOIN"
	if(!CreateServer()){
		::AfxMessageBox("初始化失败");
	}
	return 1;
}

void MyDlg::AddList(sockaddr_in sa){
	for (int i = 0; i < s_List.GetCount(); i++){	//已经有该IP则return
		if (s_List.GetAt(i).sin_addr.S_un.S_addr == sa.sin_addr.S_un.S_addr)
			return;
	}
	s_List.Add(sa);
	int index = mListHost.GetItemCount();
	int nRow = mListHost.InsertItem(index, inet_ntoa(sa.sin_addr));//插入行
	//mListHost.SetItemText(nRow, 1, "jack");//设置数据
}
//UDP实现组播网络发现
BOOL MyDlg::UDPMulticast(){
	if(socketMC == INVALID_SOCKET){
		return 0;
	}
	int ret;
	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(MCPORT);
	local.sin_addr.S_un.S_addr = INADDR_ANY;
	memset(local.sin_zero, 0, 8);
	ret = bind(socketMC, (struct sockaddr *)&local, sizeof(local));	//绑定地址
	if(ret==SOCKET_ERROR) return FALSE;

	ret = ::WSAAsyncSelect(socketMC,m_hWnd,WM_SOCKET_MC,FD_READ|FD_CLOSE);	//设置异步模式
	if(ret==SOCKET_ERROR) return FALSE;
	//加入组播
	ip_mreq mreq;
	memset(&mreq, 0, sizeof(struct ip_mreq));
	mreq.imr_multiaddr.S_un.S_addr = inet_addr("234.0.12.34");    //组播源地址
	mreq.imr_interface.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)pHost->h_addr_list[0]));       //本地地址
	int m = setsockopt(socketMC, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(struct ip_mreq));
	if (m == SOCKET_ERROR) return FALSE;
	//在 IP 头中设置出局多点广播数据报的“有效时间”（TTL）
	int optval = 8;
	setsockopt(socketMC, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&optval, sizeof(int));
	//指定当发送主机是多点广播组的成员时，是否将出局多点广播数据报的副本传送至发送主机
	int loop = 0;
	setsockopt(socketMC, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loop, sizeof(int));

	remote.sin_addr.S_un.S_addr = inet_addr("234.0.12.34");  //组播地址
	remote.sin_family = AF_INET;  
	remote.sin_port = htons(MCPORT);  
	memset(remote.sin_zero,0,8);

	//setsockopt(sock,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcast,sizeof(mcast));	//!!!关闭时调用，离开组播
	//closesocket(sock);	//!!!
	return 1;
}

//发送组播
BOOL MyDlg::SendMC(char c){
	char buff[2] = { c, '\0' };		//组播网络发现标识
	return sendto(socketMC, buff, 2, 0, (struct sockaddr*)&remote, sizeof(remote))==2?TRUE:FALSE;
}

BOOL MyDlg::CreateServer(){
	socketMsg = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketMsg == INVALID_SOCKET) return FALSE;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(MSGPORT);
	localaddr.sin_addr.S_un.S_addr = INADDR_ANY;
	memset(localaddr.sin_zero, 0, 8);
	//绑定地址
	if (bind(socketMsg, (struct sockaddr *)&localaddr, sizeof(localaddr)) == SOCKET_ERROR) return FALSE;
	//设置异步模式
	if (::WSAAsyncSelect(socketMsg, m_hWnd, WM_SOCKET_MSG, FD_READ | FD_CLOSE) == SOCKET_ERROR) return FALSE;
	return TRUE;
}

long MyDlg::OnSocketMC(WPARAM wParam,LPARAM lParam){
	SOCKET s = wParam;
	if (WSAGETSELECTERROR(lParam)){
		::closesocket(s);
		return 0;//TODO
	}
	switch (WSAGETSELECTEVENT(lParam)){
	case FD_READ:
		{
			//TODO
			sockaddr_in from;
			int fromlen = sizeof(from);
			char recv[2];
			memset(recv, 0, sizeof(char) * 2);
			if (::recvfrom(s, recv, sizeof(recv), 0, (sockaddr*)&from, &fromlen) == -1){
				::closesocket(s);
				break;
			}
			if (recv[0] == 'R' || recv[0] == 'J'){
				AddList(from);	//将IP加入列表中
			}
			if (recv[0] == 'J'){
				//JOIN,发送回应RESPONSE
				char buffT[2] = { 'R', '\0' };
				::sendto(s, buffT, strlen(buffT), 0, (struct sockaddr*)&from, sizeof(from));
			}
			else if (recv[0] == 'Q'){	//Quit退出，删除对应IP
				for (int i = 0; i < s_List.GetCount(); i++){	
					if (s_List.GetAt(i).sin_addr.S_un.S_addr == from.sin_addr.S_un.S_addr){
						s_List.RemoveAt(i);
						mListHost.DeleteItem(i);
						break;
					}
				}
			}
		}
		break;
	case FD_CLOSE:
		::closesocket(s);
		break;
	}

	return 0;
}

long MyDlg::OnSocketMSG(WPARAM wParam, LPARAM lParam){
	SOCKET s = wParam;
	if (WSAGETSELECTERROR(lParam)){
		::closesocket(s);
		return 0;//TODO
	}
	switch (WSAGETSELECTEVENT(lParam)){
	case FD_READ:
		{
			//TODO
			UpdateData();
			sockaddr_in from;
			int fromlen = sizeof(from);
			char recv[MAX_INPUT];
			memset(recv, 0, sizeof(char) * MAX_INPUT);
			if (::recvfrom(s, recv, sizeof(recv), 0, (sockaddr*)&from, &fromlen) == -1){
				::closesocket(s);
				break;
			}
			CString s = recv;
			int n = s.Find(':');
			CString s1 = s.Left(n);
			CString s2 = s.Right(s.GetLength()-n-1);
			if(s1 == "FILESEND"){			//接收文件消息
				//TODO
				//FDialog dlg;
				//if(dlg.DoModal()==IDOK){
					//TODO接收文件
				::AfxMessageBox(s2);
					char fileName[100] = "";
					OPENFILENAME file={0};
					file.lStructSize=sizeof(file);
					file.lpstrFile=fileName;
					file.nMaxFile = 100;
					file.lpstrFilter = "All Files(*.*)\0*.*\0\0";
					file.nFilterIndex = 1;
					if(!::GetSaveFileName(&file))
						return 0;
					CreateFClient(fileName,from);
				//}else return 0;
				
				//return 0;
			}
			if (mLogStr.GetLength() + 300 > 10000)
				mLogStr.Delete(0, 300);
			mLogStr += "[";					//接收消息并且更新Log
			mLogStr += inet_ntoa(from.sin_addr);
			mLogStr += "]说：\r\n	";
			mLogStr += recv; mLogStr += "\r\n\r\n";
			UpdateData(FALSE);
		}

		break;
	case FD_CLOSE:
		::closesocket(s);
		break;
	}

	return 0;
}

void MyDlg::OnClickedButtonMsg()
{
	if (mListHost.GetFirstSelectedItemPosition() == NULL) return;
	//获取输入框文本
	UpdateData();
	CString s = "你对";
	char* buff = (LPSTR)(LPCTSTR)mInputStr;
	int len = mInputStr.GetLength();
	// 查找选中的IP

	for (int i = 0; i<mListHost.GetItemCount(); i++)
	{
		int ii;
		if ( (ii= mListHost.GetItemState(i, LVIS_SELECTED))
			== LVIS_SELECTED /*|| mListHost.GetCheck(i)*/)
		{
			sockaddr_in remoteT /*= s_List.GetAt(i)*/;
			remoteT.sin_addr.S_un.S_addr = (s_List.GetAt(i)).sin_addr.S_un.S_addr;
			remoteT.sin_family = AF_INET;
			remoteT.sin_port = htons(MSGPORT);
			memset(remoteT.sin_zero, 0, 8);
			int ret = sendto(socketMsg, buff, len, 0, (sockaddr*)&remoteT, sizeof(remoteT));	//发消息
			s += "["; s += inet_ntoa(s_List.GetAt(i).sin_addr); s += "]";
		}
	}
	s += "说：\r\n	";
	if (mLogStr.GetLength() + s.GetLength() > 10000)
		mLogStr.Delete(0, s.GetLength());
	mLogStr += (s + mInputStr+"\r\n\r\n");
	mInputStr = "";
	UpdateData(FALSE);
}


void MyDlg::OnBnClickedButtonFile()
{
	//TODO
	char fileName[100] = "";
	OPENFILENAME file={0};
	file.lStructSize=sizeof(file);
	file.lpstrFile=fileName;
	file.nMaxFile = 100;
	file.lpstrFilter = "All Files(*.*)\0*.*\0\0";
	file.nFilterIndex = 1;
	if(!::GetOpenFileName(&file)){
		return;
	}
	
	CString buff = "FILESEND";buff += ":";
	buff += inet_ntoa(*(struct in_addr*)pHost->h_addr_list[0]);
	buff += "向你发送文件 ";buff += fileName;
	int len = buff.GetLength();
	for (int i = 0; i<mListHost.GetItemCount(); i++)
	{
		int ii;
		if ( (ii= mListHost.GetItemState(i, LVIS_SELECTED))
			== LVIS_SELECTED /*|| mListHost.GetCheck(i)*/)
		{
			sockaddr_in remoteT /*= s_List.GetAt(i)*/;
			remoteT.sin_addr.S_un.S_addr = (s_List.GetAt(i)).sin_addr.S_un.S_addr;
			//::AfxMessageBox(inet_ntoa(s_List.GetAt(i).sin_addr));
			remoteT.sin_family = AF_INET;
			remoteT.sin_port = htons(MSGPORT);
			memset(remoteT.sin_zero, 0, 8);
			int ret = sendto(socketMsg, buff, len, 0, (sockaddr*)&remoteT, sizeof(remoteT));//建立文件连接，等待对方同意
			CreateFServer(fileName);
			break;
		}
	}
}

BOOL MyDlg::CreateFServer(char * file_name){
	//char file_name[FILE_NAME_MAX_SIZE+1]= fName;
	sockaddr_in server_addr; 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY; 
	server_addr.sin_port = htons(FILEPORT);

	// 创建socket 
	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (SOCKET_ERROR == m_Socket) 
	{ 
		::AfxMessageBox("Create Socket Error!"); //closesocket(c_Socket); 
				return 0;
	} 

	//绑定socket和服务端(本地)地址 
	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
	{ 
		::AfxMessageBox("Server Bind Failed: "+ WSAGetLastError()); 
		return 0;
	} 

	//监听 
	if (SOCKET_ERROR == listen(m_Socket, 10)) 
	{ 
		::AfxMessageBox("Server Listen Failed: "+ WSAGetLastError()); closesocket(m_Socket); 
				return 0;
	} 

	//while(1) 
	//{ 
		::AfxMessageBox("点确定开始传送..\n"); 

		sockaddr_in client_addr; 
		int client_addr_len = sizeof(client_addr); 

		m_Socket = accept(m_Socket, (sockaddr *)&client_addr, &client_addr_len); 
		if (SOCKET_ERROR == m_Socket) 
		{ 
			::AfxMessageBox("Server Accept Failed: "+ WSAGetLastError()); closesocket(m_Socket); 
				return 0;
		} 

		char buffer[BUFFER_SIZE];

		memset(buffer, 0, BUFFER_SIZE); 

		
		FILE * fp = fopen(file_name, "rb"); //windows下是"rb",表示打开一个只读的二进制文件 
		if (NULL == fp) 
		{ 
			::AfxMessageBox("File:  Not Found\n"); 
		} 
		else
		{ 
			memset(buffer, 0, BUFFER_SIZE); 
			int length = 0; 

			while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
			{ 
				if (send(m_Socket, buffer, length, 0) < 0) 
				{ 
					::AfxMessageBox("Send File:  Failed\n"); closesocket(m_Socket); 
				return 0;
				} 
				memset(buffer, 0, BUFFER_SIZE); 
			} 

			fclose(fp); 
			closesocket(m_Socket); 
		}
		
		::AfxMessageBox("File:  Transfer Successful!\n"); 

	//}
	return 1;
}
BOOL MyDlg::CreateFClient(char * file_name,sockaddr_in addr){
	//创建socket 
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (SOCKET_ERROR == c_Socket) 
	{ 
		::AfxMessageBox("Create Socket Error!"); closesocket(c_Socket); 
				return 0;
	} 

	//指定服务端的地址 
	sockaddr_in server_addr; 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.S_un.S_addr = addr.sin_addr.S_un.S_addr; 
	server_addr.sin_port = htons(FILEPORT); 

	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
	{ 
		::AfxMessageBox("Can Not Connect To Server IP!\n"); 
		closesocket(c_Socket); 
				return 0;
	} 

	//输入文件名 
	//char file_name[FILE_NAME_MAX_SIZE+1]="D:/1.jpg"; 

	char buffer[BUFFER_SIZE]; 
	FILE * fp = fopen(file_name, "wb"); //windows下是"wb",表示打开一个只写的二进制文件 
	if(NULL == fp) 
	{ 
		::AfxMessageBox("File:  Can Not Open To Write\n"); 
	} 
	else
	{ 
		memset(buffer, 0, BUFFER_SIZE); 
		int length = 0; 
		while ((length = recv(c_Socket, buffer, BUFFER_SIZE, 0)) > 0) 
		{ 
			if (fwrite(buffer, sizeof(char), length, fp) < length) 
			{ 
				::AfxMessageBox("File:  Write Failed\n"); 
				fclose(fp); 
				closesocket(c_Socket); 
				return 0;
			} 
			memset(buffer, 0, BUFFER_SIZE); 
		} 
		//TODO
	} 
	
	::AfxMessageBox("Receive File:  From client Successful!\n"); 
	fclose(fp); 
	closesocket(c_Socket);
	return 1;
}

void MyDlg::OnClickedButtonFlush()
{
	//FDialog dlg();
	//dlg.DoModal();
	s_List.RemoveAll();
	mListHost.DeleteAllItems();
	if(socketMC == INVALID_SOCKET)	::AfxMessageBox("组播套接字失败");
	if(!SendMC('J')) ::AfxMessageBox("组播发送失败");
}
