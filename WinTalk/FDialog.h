#pragma once


// FDialog dialog

class FDialog : public CDialog
{
	DECLARE_DYNAMIC(FDialog)

public:
	FDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~FDialog();
	BOOL SetMText(CString);
	CString GetMText();
// Dialog Data
	enum { IDD = IDD_DIALOG_RECV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString recvText;
};
