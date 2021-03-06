
// Experemental ServerDlg.h : header file
//

#pragma once

#include "ItemController.h"
#include "NetworkManager.h"

// CExperementalServerDlg dialog
class CExperementalServerDlg : public CDialogEx
{
// Construction
public:
	CExperementalServerDlg(CWnd* pParent = nullptr);	// standard constructor
	~CExperementalServerDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPEREMENTALSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	bool is_file(HTREEITEM i);
	void enableControls();

	void saveTree(HTREEITEM hRoot, CStdioFile *file);
	void downloadingTree();
	void createTree(CString *str, HTREEITEM hRoot);
	void deleteTreeItem(HTREEITEM hRoot);
	void createItemFile(HTREEITEM hRoot, File *setData);
	void createItemFolder(HTREEITEM hRoot, CString *data, Dir *setData);
	void createItemFolder(HTREEITEM hRoot, Dir *setData);
	bool deleteFolderItem(HTREEITEM delItem);
	bool deleteFileItem(HTREEITEM delFile);

public:
	mutex treeLock;

	CTreeCtrl mTreeView;
	afx_msg void OnBnClickedButtonAddFIle();
	afx_msg void OnBnClickedButtonAddFolder();
	afx_msg void OnTvnSelchangedTreeSelectItem(NMHDR *pNMHDR, LRESULT *pResult);
	CButton addFolderButton;
	CButton addFileButton;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	CButton treeDeleteButton;
	afx_msg void OnBnClickedDeleteButton();
	afx_msg void OnBnClickedOpenButtonOpen();
};

