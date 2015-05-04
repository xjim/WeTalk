// FDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WinTalk.h"
#include "FDialog.h"


// FDialog dialog

IMPLEMENT_DYNAMIC(FDialog, CDialog)

FDialog::FDialog(CWnd* pParent /*=NULL*/)
	: CDialog(FDialog::IDD, pParent)
	, recvText(_T(""))
{

}

FDialog::~FDialog()
{
}



void FDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, recvText);
	DDV_MaxChars(pDX, recvText, 250);
}


BEGIN_MESSAGE_MAP(FDialog, CDialog)
END_MESSAGE_MAP()

BOOL FDialog::SetMText(CString s){
	recvText = s;
	return UpdateData(FALSE);
}
CString FDialog::GetMText(){
	UpdateData();
	CString s = recvText;
	return s;
}
// FDialog message handlers
