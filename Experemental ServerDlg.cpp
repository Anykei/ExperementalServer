
// Experemental ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Experemental Server.h"
#include "Experemental ServerDlg.h"
#include "afxdialogex.h"
#include <vector>
#include <iterator>
#include "Folder.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About


int Dir::countDir = 0;
int Dir::lastIdDir = 0;
int File::lastId = 0;

bool folderEmpty(CString *str);
bool FolderEnd(CString *str);

UINT TreadControlFileModify(LPVOID data);



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CExperementalServerDlg dialog



CExperementalServerDlg::CExperementalServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXPEREMENTALSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CExperementalServerDlg::~CExperementalServerDlg()
{
}

void CExperementalServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, mTreeView);
	DDX_Control(pDX, IDC_BUTTON1, addFolderButton);
	DDX_Control(pDX, IDC_BUTTON2, addFileButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, treeDeleteButton);
}

BEGIN_MESSAGE_MAP(CExperementalServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CExperementalServerDlg::OnBnClickedButtonAddFIle)
	ON_BN_CLICKED(IDC_BUTTON1, &CExperementalServerDlg::OnBnClickedButtonAddFolder)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CExperementalServerDlg::OnTvnSelchangedTreeSelectItem)
	ON_BN_CLICKED(IDC_BUTTON3, &CExperementalServerDlg::OnBnClickedButton3)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CExperementalServerDlg::OnNMClickTree)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CExperementalServerDlg::OnBnClickedDeleteButton)
	ON_BN_CLICKED(IDC_OPEN_BUTTON, &CExperementalServerDlg::OnBnClickedOpenButtonOpen)
END_MESSAGE_MAP()


// CExperementalServerDlg message handlers

BOOL CExperementalServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//setlocale(LC_ALL, "rus");
	LPCTSTR lpsz = _wsetlocale(LC_CTYPE, _T("rus"));
	if (0 == lpsz)
	{
		//std::cerr << "Failed " << endl;
		return -1;
	}
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	mTreeView.InsertItem(L"ROOT", TVI_ROOT, TVI_FIRST);
	downloadingTree();
	enableControls();

	MyFolder::createFolder(&CString( L"ROOT"));
	MyFolder::createFolder(&CString(L"SAVE"));

	//AfxBeginThread(TreadControlFileModify, this);
	AfxBeginThread(TreadControlFileModify,(LPVOID*)this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CExperementalServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExperementalServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExperementalServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


class CInputDlg : public CDialog
{
public:
	CInputDlg();
	//{{AFX_DATA(CInputDlg)
	enum { IDD = IDD_SET_TEXT };
	CPoint m_point;

	//{{AFX_VIRTUAL(CInputDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	//{{AFX_MSG(CInputDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetText();
	DECLARE_MESSAGE_MAP()
public:
	CString sText;
	CEdit labelNameFolder;
};
CInputDlg::CInputDlg() : CDialog(CInputDlg::IDD)
{
	
}
void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, labelNameFolder);
}
BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, &CInputDlg::OnGetText)
END_MESSAGE_MAP()

void CInputDlg::OnGetText()
{
	CWnd* pWnd = AfxGetMainWnd();
	if (pWnd == NULL) return;
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT1);
	CString s;
	pEdt->GetWindowText(s);

	pEdt->GetWindowTextW(sText);
	
}

BOOL CInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetWindowPos(NULL, m_point.x + 900, m_point.y + 400, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	CEdit* pEdt = (CEdit*)GetDlgItem(IDC_EDIT1);
	return TRUE;
}

void CExperementalServerDlg::OnBnClickedButtonAddFIle()
{
	HTREEITEM hRoot = mTreeView.GetSelectedItem();
	CFileDialog FileDlg(FALSE, CString(".txt"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, CString("All Files (*.*)|*.*||"""));

	CString path, name , fPath;
	if (FileDlg.DoModal() == IDOK)
	{
		path = FileDlg.GetPathName();
		name = FileDlg.GetFileName();
		
	}

	Data *d = IL->getData(hRoot);
	if (!d) fPath = "ROOT\\";
	else fPath = d->getPath() + d->getName() + L"\\";

	if (name.GetLength() > 1)
	{
		File *f = new File(name, fPath, 0, 0);
		if (f->copyFile(&path, &(f->getPath() + f->getName()), File::Flag::COPY))
		{
			createItemFile(hRoot, f);
			OnBnClickedButton3();
		}
	}
	// TODO: Add your control notification handler code here
}

void CExperementalServerDlg::OnBnClickedButtonAddFolder()
{
	CInputDlg dialog;
	CString name;
	if (dialog.DoModal() == IDOK) {
		
		name = dialog.sText; // Do something
	}
	else {
		return;
	}
	if (name.GetLength() > 1)
	{
		Dir *d = new Dir(name, L"");
		createItemFolder(mTreeView.GetSelectedItem(), nullptr, d);
		OnBnClickedButton3();
	}
}

void CExperementalServerDlg::OnTvnSelchangedTreeSelectItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	enableControls();
	*pResult = 0;
}


bool CExperementalServerDlg::is_file(HTREEITEM i)
{
	if (Data *d = IL->getData(i))
	{
		if (d->getType() == TypeData::FILE) return true;
	}
	return false;
}

bool is_folder(HTREEITEM i)
{
	if (Data *d = IL->getData(i))
	{
		if (d->getType() == TypeData::FOLDER) return true;
	}
	return false;
}

void CExperementalServerDlg::enableControls()
{
	HTREEITEM hRoot = mTreeView.GetSelectedItem();
	if (hRoot == mTreeView.GetRootItem())
	{
		treeDeleteButton.EnableWindow(FALSE);
		addFileButton.EnableWindow(TRUE);
		addFolderButton.EnableWindow(TRUE);
	}
	else if (hRoot == NULL)
	{
		addFileButton.EnableWindow(FALSE);
		addFolderButton.EnableWindow(FALSE);
		treeDeleteButton.EnableWindow(FALSE);
	}
	else
	{
		if (!is_file(hRoot))
		{
			addFileButton.EnableWindow(TRUE);
			addFolderButton.EnableWindow(TRUE);
			treeDeleteButton.EnableWindow(TRUE);
		}
		else
		{
			addFileButton.EnableWindow(FALSE);
			addFolderButton.EnableWindow(FALSE);
			treeDeleteButton.EnableWindow(TRUE);
		}
	}
}

/*

	CFile file, copy;

	file.Open(name, CFile::modeRead);
	copy.Open(L"cop.pdf", CFile::modeReadWrite| CFile::modeCreate);
	int size = find_buf_size(file.GetLength());

	for (int i = 0; i < file.GetLength(); i += size)
	{
		std::vector<char> b(size);
		file.Read(b.data(), size);
		copy.Write(b.data(), size);
	}

	file.Close();
	copy.Close();
	*/

void CExperementalServerDlg::OnBnClickedButton3()
{
	lock_guard<mutex> lock (treeLock);
	HTREEITEM hRoot = mTreeView.GetRootItem();
	HTREEITEM hCild = mTreeView.GetChildItem(hRoot);
	CStdioFile file;
	if (file.Open(L"treeSave1.txt", CFile::modeReadWrite | CFile::modeCreate))
	{
		saveTree(hCild, &file);
	}
	file.Close();
}

void CExperementalServerDlg::saveTree(HTREEITEM hRoot, CStdioFile *fileStream)
{
	while (hRoot)
	{
		if (is_file(hRoot))
		{
			File *file = (File*)IL->getData(hRoot);
			CString fileMod = file->getStrData().GetBuffer();
			fileStream->WriteString(file->getStrData().GetBuffer());
		}
		else if(is_folder(hRoot))
		{ 
			Dir *d = (Dir*)IL->getData(hRoot);
			auto dat = d->getStrData().GetBuffer();
			fileStream->WriteString(d->getStrData().GetBuffer());
			fileStream->WriteString(L"{\n");
			HTREEITEM hCild = mTreeView.GetChildItem(hRoot);
			saveTree(hCild, fileStream);
			fileStream->WriteString(L"}\n");
		} else
		{/*error read tree*/ }
		hRoot = mTreeView.GetNextSiblingItem(hRoot);
	}
}

void CExperementalServerDlg::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = mTreeView.GetSelectedItem();
	if (!hItem)
	{
		mTreeView.SetItemState(hItem, 0, LVIS_SELECTED);
		addFileButton.EnableWindow(TRUE);
		addFolderButton.EnableWindow(TRUE);
	}	
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CExperementalServerDlg::downloadingTree()
{
	lock_guard<mutex> lock(treeLock);
	HTREEITEM hRoot = mTreeView.GetRootItem();
	CStdioFile file;
	if (!file.Open(L"treeSave1.txt", CFile::modeRead)) return;
	CString data;
	while (file.GetPosition() != file.GetLength())
	{
		CString buf;
		file.ReadString(buf);
		data += buf + L"\n";
	}
	file.Close();
	while (data.GetLength() > 0)
	{
		createTree(&data, hRoot);
	}	
}

void CExperementalServerDlg::createTree(CString *str, HTREEITEM hRoot)
{
	CString namefolder;
	int posEndStr = str->Find(L"\n");
	auto subStr = str->Mid(0, posEndStr);
	str->Delete(0, posEndStr + 1);
	
	if (subStr.Find(L"type") >= 0)
	{
		auto type = firstSubStrDelim(&subStr, L"=", L";");
		switch (strToTypeData(type))
		{
		case TypeData::FILE:
			createItemFile(hRoot, new File(subStr));
			break;
		case TypeData::FOLDER:
			createItemFolder(hRoot, str, new Dir(subStr));
			break;
		case TypeData::UNCNOWN:
			
		default:
			return;
			break;
		}
	}
}

bool folderEmpty(CString *str)
{
	if (!str)return true;
	int posFirst = str->Find(L"{\n");
	int posTwo = str->Find(L"}\n");
	if ((posTwo - posFirst) > 2)
	{
		str->Delete(posFirst, 2);
		return false;
	}
	else
	{
		str->Delete(0, 4);
		return true;
	}
}
bool FolderEnd(CString *str)
{
	if (str->Find(L"}\n") == 0)
	{
		str->Delete(0, 2);
		return true;
	}
	else return false;
}

void CExperementalServerDlg::OnBnClickedDeleteButton()
{
	HTREEITEM deleteItem = mTreeView.GetSelectedItem();
	deleteTreeItem(deleteItem);
	OnBnClickedButton3();
	// TODO: Add your control notification handler code here
}

void CExperementalServerDlg::deleteTreeItem(HTREEITEM hRoot)
{
	lock_guard<mutex> lock(treeLock);
	Data *d = IL->getData(hRoot);
	if (d->getType() == TypeData::FOLDER) {
		if (deleteFolderItem(hRoot)) return;
	}
	else
	{
		if (deleteFileItem(hRoot)) return;
	}
	delete d;
}

bool CExperementalServerDlg::deleteFolderItem(HTREEITEM delItem)
{
	while (auto child = mTreeView.GetChildItem(delItem))
	{
		if (child == FALSE)break;
		deleteTreeItem(child);
	}
	if (IL->removeItem(delItem)) 
	{
		mTreeView.DeleteItem(delItem);
		return true;
	}
	else { return false; }//error delete
}

bool CExperementalServerDlg::deleteFileItem(HTREEITEM delFile)
{
	if (IL->removeItem(delFile)) {
		mTreeView.DeleteItem(delFile);
		return true;
	}
	else return false;
	/*
	File *f = (File*)mTreeView.GetItemData(delFile);
	if (f->deleteFile())
	{
		mTreeView.DeleteItem(delFile);
		return true;
	}
	else { return false; }//error;*/
}

void CExperementalServerDlg::createItemFile(HTREEITEM hRoot, File *setData)
{
	IL->addItem(mTreeView.InsertItem(setData->getName(), hRoot), (Data*)setData);
	/*
	if (setData) mTreeView.SetItemData(mTreeView.InsertItem(setData->getName(), hRoot), (DWORD_PTR)setData);
	else mTreeView.SetItemData(, (DWORD_PTR)new File(L"New file",path,0,0));	*/
}

void CExperementalServerDlg::createItemFolder(HTREEITEM hRoot, CString *data, Dir *setData)
{
	CString path;
	Data *d = IL->getData(hRoot);
	if (!d) path = "ROOT\\";
	else path = d->getPath() + d->getName() + L"\\";
	HTREEITEM chItem;

	
	setData->setPath(path);
	chItem = mTreeView.InsertItem(setData->getName(), hRoot);
	IL->addItem(chItem, (Data*)setData);

	setData->create();
	if (folderEmpty(data)) return;

	while (!FolderEnd(data))
	{
		createTree(data, chItem);
	}
}

void CExperementalServerDlg::OnBnClickedOpenButtonOpen()
{
	auto hItem = mTreeView.GetSelectedItem();
	Data *d = IL->getData(hItem);
	if (!d)return;
	if (d->getType() == TypeData::FILE)
	{
		File *file = (File*)d;
		auto name = d->getPath() + d->getName();
		if(file->fileExist())
			ShellExecute(NULL, L"open", name, NULL, NULL, SW_SHOW);
	}
	// TODO: Add your control notification handler code here
}

/*thread*/
UINT TreadControlFileModify(LPVOID data)
{
	Sleep(1000);
	if (!data) return 1;
	CExperementalServerDlg *dlg = (CExperementalServerDlg*)data;
	while (true)
	{
		auto lostList = IL->fileExistList();

		if (lostList.size() != 0)
		{
			/*act with lost files*/
		}
		//dlg->OnBnClickedButton3();
		Sleep(10000);
	}
	return 0;
}