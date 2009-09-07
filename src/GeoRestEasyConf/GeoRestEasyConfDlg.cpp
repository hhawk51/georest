
// GeoRestEasyConfDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GeoRestEasyConf.h"
#include "GeoRestEasyConfDlg.h"
#include <string>

#include <ctemplate/template.h>
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/Exception.h"

#include "Poco/UnicodeConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define TYPE_FDO L"FDO"
#define TYPE_MAPGUIDE L"FDO"

// CAboutDlg dialog used for App About
extern wchar_t g_AppFileName[];

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGeoRestEasyConfDlg dialog




CGeoRestEasyConfDlg::CGeoRestEasyConfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeoRestEasyConfDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGeoRestEasyConfDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SOURCE, m_CtrlSource);
  DDX_Control(pDX, IDC_MAP, m_Ct_Map);
  DDX_Control(pDX, IDC_LAYER, m_Ct_Layer);
  DDX_Control(pDX, IDC_URIPART, m_Ct_UriPart);
}

BEGIN_MESSAGE_MAP(CGeoRestEasyConfDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, &CGeoRestEasyConfDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_CREATE, &CGeoRestEasyConfDlg::OnBnClickedCreate)
  ON_BN_CLICKED(IDC_BROWSE, &CGeoRestEasyConfDlg::OnBnClickedBrowse)
  ON_BN_CLICKED(IDC_KML, &CGeoRestEasyConfDlg::OnBnClickedKml)
  ON_EN_KILLFOCUS(IDC_URIPART, &CGeoRestEasyConfDlg::OnEnKillfocusUripart)
END_MESSAGE_MAP()


// CGeoRestEasyConfDlg message handlers

BOOL CGeoRestEasyConfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
	
	CheckDlgButton(IDC_SHP,1);
	CheckDlgButton(IDC_JSON,1);
	CheckDlgButton(IDC_XML,1);
	CheckDlgButton(IDC_KML,1);
	CheckDlgButton(IDC_KMZ,1);
	CheckDlgButton(IDC_HTML,1);
	CheckDlgButton(IDC_SITEMAP,1);
	CheckDlgButton(IDC_PNG,1);
	
	CheckDlgButton(IDC_GET,1);
	
	CheckDlgButton(IDC_MG_FEATURESOURCE,1);
	
	GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHP)->EnableWindow(FALSE);
	GetDlgItem(IDC_SDF)->EnableWindow(FALSE);
	
	
	GetDlgItem(IDC_FOLDER)->EnableWindow(FALSE);
	
	CheckDlgButton(IDC_GET,1);
	GetDlgItem(IDC_GET)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST)->EnableWindow(FALSE);
	GetDlgItem(IDC_PUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_OPTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_HEAD)->EnableWindow(FALSE);
	
  GetDlgItem(IDC_OPENLAYERS)->EnableWindow(FALSE);
  GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGeoRestEasyConfDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGeoRestEasyConfDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGeoRestEasyConfDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGeoRestEasyConfDlg::OnBnClickedOk()
{
  // TODO: Add your control notification handler code here
  OnOK();
}


CString CGeoRestEasyConfDlg::GetSourceType()
{
  if( IsDlgButtonChecked((INT32)IDC_SHP) )   return TYPE_FDO;
  if( IsDlgButtonChecked(IDC_SDF) )   return TYPE_FDO;
  if( IsDlgButtonChecked(IDC_MG_FEATURESOURCE) )   return TYPE_MAPGUIDE;
  
  return L"";
}
void CGeoRestEasyConfDlg::OnBnClickedBrowse()
{
  // TODO: Add your control notification handler code here
  // TODO: Add your control notification handler code here
  CString filter;

  CString provname;
  if( IsDlgButtonChecked(IDC_SHP) )
  {
    filter = "Shape files (*.shp)|*.shp|All Files (*.*)|*.*||";
  }
 
  if( IsDlgButtonChecked(IDC_SDF) )
  {
    filter = "SDF files (*.sdf)|*.sdf|All Files (*.*)|*.*||";
    //dlg.SetDefExt(L"sdf");
  }

  CFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST,filter);

  if( dlg.DoModal() == IDOK )
  {  
    CString fname = dlg.GetPathName();
    CString provname;
    m_CtrlSource.SetWindowText(fname);  
    
  }
}

void CGeoRestEasyConfDlg::OnBnClickedKml()
{
  // TODO: Add your control notification handler code here
}

void CGeoRestEasyConfDlg::OnEnKillfocusUripart()
{
  CString cs_uripart;
  GetDlgItemText(IDC_URIPART,cs_uripart);
  if( cs_uripart.GetLength()>0 )
  {

    // TODO: Add your control notification handler code here
    CString cs_section,cs_prefix;
    GetDlgItemText(IDC_SECTION,cs_section);
    if( cs_section.GetLength()==0)
    {
      SetDlgItemText(IDC_SECTION,cs_uripart);
    }
    GetDlgItemText(IDC_PREFIX,cs_prefix);
    if( cs_section.GetLength()==0)
    {
      cs_prefix = cs_uripart + L"_";
      SetDlgItemText(IDC_PREFIX,cs_prefix);
    }
  }
}

void CGeoRestEasyConfDlg::OnBnClickedCreate()
{
try
{

  if( IsDlgButtonChecked((INT32)IDC_SHP) )   
  {

  }
  CString map,layer;
  if( IsDlgButtonChecked((INT32)IDC_PNG) )   
  {
    m_Ct_Map.GetWindowText(map);
    if( map.GetLength()==0)
    {
      AfxMessageBox(L"For PNG Representation you need to enter MapGuide MapDefintion resource!");
      return ;
    }
  }
  
  std::string cfg_tpl;
  CString res_type = GetSourceType();
  if( res_type == TYPE_FDO )
  {
    cfg_tpl = "fdo.tpl";
  }
  if( res_type == TYPE_MAPGUIDE )
  {
    cfg_tpl = "mapguide.tpl";
  }
  
  CString cs_uripart;  
  GetDlgItemText(IDC_URIPART,cs_uripart);
  std::string uripart;
  Poco::UnicodeConverter::toUTF8(cs_uripart,uripart);
  if( cs_uripart.GetLength() == 0)
  {
    AfxMessageBox(L"You have to enter UriTag");
  }
  
  CString cs_fsource,cs_fclass;  
  GetDlgItemText(IDC_SOURCE,cs_fsource);
  GetDlgItemText(IDC_CLASS,cs_fclass);
  std::string fsource,fclass;
  Poco::UnicodeConverter::toUTF8(cs_fsource,fsource);
  Poco::UnicodeConverter::toUTF8(cs_fclass,fclass);
  
  CString cs_section,cs_prefix;  
  GetDlgItemText(IDC_SECTION,cs_section);
  GetDlgItemText(IDC_PREFIX,cs_prefix);
  std::string section,prefix;
  Poco::UnicodeConverter::toUTF8(cs_section,section);
  Poco::UnicodeConverter::toUTF8(cs_prefix,prefix);
  
  std::string temp;
  Poco::UnicodeConverter::toUTF8(g_AppFileName,temp);
  Poco::Path tplfolder(temp);
  Poco::Path ppath_conf(temp);
  ppath_conf.setFileName("");
  ppath_conf.popDirectory();
  ppath_conf.pushDirectory("conf");
  
  tplfolder.setFileName("");
  tplfolder.pushDirectory("EasyConf");
  
  std::string tpl_root = tplfolder.toString();
  
  tplfolder.setFileName(cfg_tpl);
  
  std::string tpl_fname = tplfolder.toString();
  
  ctemplate::Template::SetTemplateRootDirectory(tpl_root);
  
  ctemplate::TemplateDictionary dict_main("GEOREST.EASYCONF");
  dict_main.SetValue("EC_URIPART",uripart);
  ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_SOURCE");
  dict_section->SetValue("EC_FEATURESOURCE",fsource);
  dict_section->SetValue("EC_FEATURECLASS",fclass);
  
  if( IsDlgButtonChecked(IDC_XML))
  {
    ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_XML");
  }
  if( IsDlgButtonChecked(IDC_JSON))
  {
    ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_JSON");
  }
  if( IsDlgButtonChecked(IDC_HTML))
  {
    ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_HTML");
    
    dict_section->SetValue("EC_SECTION",section);
    dict_section->SetValue("EC_PREFIX",prefix);
    
    std::string single,many,none,error;
    single=uripart+"_html_single.tpl";
    many=uripart+"_html_many.tpl";
    none=uripart+"_html_none.tpl";
    error=uripart+"_html_error.tpl";
    
    dict_section->SetValue("EC_SINGLE",single);
    dict_section->SetValue("EC_MANY",many);
    dict_section->SetValue("EC_NONE",none);
    dict_section->SetValue("EC_ERROR",error);
  }
  if( IsDlgButtonChecked(IDC_KML))
  {
    ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_KML");
    dict_section->SetValue("EC_SECTION",section);
    dict_section->SetValue("EC_PREFIX",prefix);
    std::string single,many,none,error;
    single=uripart+"_kml_single.tpl";
    many=uripart+"_kml_many.tpl";
    none=uripart+"_kml_none.tpl";
    error=uripart+"_kml_error.tpl";

    dict_section->SetValue("EC_SINGLE",single);
    dict_section->SetValue("EC_MANY",many);
    dict_section->SetValue("EC_NONE",none);
    dict_section->SetValue("EC_ERROR",error);
  }
  if( IsDlgButtonChecked(IDC_KMZ))
  {
    ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_KMZ");
    dict_section->SetValue("EC_SECTION",section);
    dict_section->SetValue("EC_PREFIX",prefix);
    std::string single,many,none,error;
    single=uripart+"_kml_single.tpl";
    many=uripart+"_kml_many.tpl";
    none=uripart+"_kml_none.tpl";
    error=uripart+"_kml_error.tpl";

    dict_section->SetValue("EC_SINGLE",single);
    dict_section->SetValue("EC_MANY",many);
    dict_section->SetValue("EC_NONE",none);
    dict_section->SetValue("EC_ERROR",error);
  }
  if( IsDlgButtonChecked(IDC_SITEMAP))
  {
    ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_SITEMAP");
  }
  
  if( IsDlgButtonChecked(IDC_PNG))
  {
    ctemplate::TemplateDictionary* dict_section = dict_main.AddSectionDictionary("EC_PNG");
    
    CString cs_mapdef,cs_layer;  
    GetDlgItemText(IDC_MAP,cs_mapdef);
    GetDlgItemText(IDC_LAYER,cs_layer);
    std::string mapdef,layer;
    Poco::UnicodeConverter::toUTF8(cs_mapdef,mapdef);
    Poco::UnicodeConverter::toUTF8(cs_layer,layer);
    
    dict_section->SetValue("EC_PNG_MAPDEFINITION",mapdef);
    dict_section->SetValue("EC_PNG_SELECTIONLAYER",layer);
  }
 
  ctemplate::Template::ReloadAllIfChanged();
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_fname,ctemplate::DO_NOT_STRIP);
  if( !tpl )
  {
    std::wstring s1,s2;
    Poco::UnicodeConverter::toUTF16(tpl_fname,s2);
    s1 = L"Unable to load template file '";
    s1 = s1 + s2 + L"'!";
     AfxMessageBox(s1.c_str());
    return;
  }
  std::string restcfg_string;
  tpl->Expand(&restcfg_string,&dict_main);
  
// copy to conf folder
  Poco::Path ppath_restcfg(ppath_conf);
  ppath_restcfg.pushDirectory(uripart);
  Poco::File pfile_restcfg(ppath_restcfg.toString());
  pfile_restcfg.createDirectory();
  ppath_restcfg.setFileName("restcfg.xml");
  
  
  
  Poco::FileOutputStream fos(ppath_restcfg.toString());
  //assert (fos.good());
  fos << restcfg_string;
  fos.close();  
  
  AfxMessageBox(L"Done.");
}
catch (Poco::Exception& ex)
{
  std::string estr = ex.message();
  std::wstring wstr;
  Poco::UnicodeConverter::toUTF16(estr,wstr);
  AfxMessageBox(wstr.c_str());
}
catch (...)
{
  AfxMessageBox(L"Unknown exception!");
}
}

