//
//  Copyright (C) 2009 by  Haris Kurtagic  www.sl-king.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "StdAfx.h"
//#include <iostream>
#include <fstream>
#include "c_RestConfig.h"

#include "c_CfgDataSource.h"

#define D_DEFAULT_CFG_FILENAME "restcfg.xml"

#include "Poco/Foundation.h"
#include "Poco/Exception.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FileChannel.h"
#include "Poco/AsyncChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Path.h"
#include "Poco/DirectoryIterator.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/CDATASection.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/Document.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/AutoPtr.h"
#include <sstream>
#include "Poco/String.h"


using Poco::XML::Text;
using Poco::XML::CDATASection;
using Poco::XML::Element;
using Poco::XML::Document;
using Poco::XML::AutoPtr;
using Poco::XML::XMLString;

extern wchar_t g_AppFileName[MAX_PATH+2];

c_ExceptionRestConfig::c_ExceptionRestConfig(const char* Desc)
{
  Poco::UnicodeConverter::toUTF16(Desc,m_Desc);
}

c_RestConfig * g_RestConfig=NULL;

c_RestConfig* c_RestConfig::GetInstance()
{
  if( !g_RestConfig )
  {
    g_RestConfig = new c_RestConfig();
    g_RestConfig->ReadFromXML();
  }
  
  return g_RestConfig;
}
void c_RestConfig::DeleteInstance()
{
  if( g_RestConfig )
  {
    delete g_RestConfig;
    g_RestConfig=NULL;
  }
}


c_RestConfig::c_RestConfig(Poco::Logger* Log)
{
  if( !Log )
  {
    CreateLogger();
  }
  
  m_RestUriSeparator = "rest";
}

void c_RestConfig::CreateLogger()
{
  std::string s1;
  Poco::UnicodeConverter::toUTF8(g_AppFileName,s1);
  Poco::Path cfgpath(s1);  

  // check if "log" folder exists
  Poco::Path logdir = cfgpath;
  logdir.popDirectory();
  logdir.pushDirectory("log");
  logdir.setFileName("");
  Poco::File tf(logdir);
  if( tf.exists() && tf.isDirectory() )
  {
    cfgpath = logdir;
  }

  cfgpath.setFileName("rest.log");
  m_LogFileName = cfgpath.toString();

  CreateLogger( m_LogFileName.c_str() );
}

Poco::Logger& c_RestConfig::GetLogger()
{
  if( !m_Logger )
  {
    c_RestConfig::CreateLogger();
  }
  
  return *m_Logger;
}


c_RestConfig::~c_RestConfig(void)
{
}


void c_RestConfig::Clear()
{
  m_DataResourceVector.Clear();
}

void c_RestConfig::CreateLogger( const char* FileName )
{
  m_Logger = Poco::Logger::has("GeoRestLogger");

  if( !m_Logger )
  {   
    Poco::AutoPtr<Poco::PatternFormatter> formatter = new Poco::PatternFormatter("%Y-%m-%d %H:%M:%S.%c %N[%P]:%s:%q:  %t");
    Poco::AutoPtr<Poco::FormattingChannel> format_channel = new Poco::FormattingChannel(formatter);  
    Poco::AutoPtr<Poco::FileChannel> filechannel = new Poco::FileChannel(FileName);
    Poco::AutoPtr<Poco::AsyncChannel> asyncchannel = new Poco::AsyncChannel();
    format_channel->setChannel(filechannel);
    asyncchannel->setChannel(format_channel);
    //pFCFile->open();
    
    Poco::Logger::root().setChannel(asyncchannel);
    m_Logger = &Poco::Logger::get("GeoRestLogger"); 
  }
  
}

void c_RestConfig::Read( Poco::XML::Document* PocoDoc, const char* TemplateFolder )
{
  try
  {
    
    AutoPtr<Poco::XML::NodeList>  nlist = PocoDoc->getElementsByTagName("RestConfig");
    if( nlist->length() == 0 )
    {
      m_Logger->warning("c_RestConfig::ReadFromXML : There is no 'RestConfig' element in configuration file! No data will be read from config file.");
      return;
    }
    if( nlist->length() > 1 )
    {
      m_Logger->warning("c_RestConfig::ReadFromXML : More then one 'RestConfig' element in configuration file! Only first one will be used.");
    }

    Poco::XML::Element* node_restconfig = (Poco::XML::Element*)nlist->item(0);

    nlist = node_restconfig->getElementsByTagName("Data");
    if( nlist->length() > 1 )
    {
      m_Logger->warning("c_RestConfig::ReadFromXML : More then one 'Data' element in configuration file! Only first one will be used.");
    }
    Poco::XML::Element* node_data = (Poco::XML::Element*)nlist->item(0);


    // Read Resources
    {
      nlist = node_data->getElementsByTagName("Resource");
      for(int ind=0;ind<nlist->length();ind++)
      {
        Poco::XML::Element* node_layer = (Poco::XML::Element*)nlist->item(ind);
        c_CfgDataResource* datalayer = ParseResource_v2(node_layer,TemplateFolder);
        if( datalayer && !m_DataResourceVector.Add(datalayer) )
        {
          
          std::string errmsg;errmsg.reserve(512);
          errmsg = "c_RestConfig::ReadFromXML : Duplicated UriTag '";
          errmsg.append(datalayer->GetUriTag());
          errmsg.append("'. Resource will not be added.");
          m_Logger->warning();
          delete datalayer;
        }
      }
    }

    // Read old version of config "Data"
    {
      Poco::Path tempfolders(TemplateFolder);
      tempfolders.pushDirectory("HtmlTemplates");
      nlist = node_data->getElementsByTagName("Layer");
      for(int ind=0;ind<nlist->length();ind++)
      {
        Poco::XML::Element* node_layer = (Poco::XML::Element*)nlist->item(ind);

        c_CfgDataResource* datalayer = ParseResource_v1(node_layer,tempfolders.toString().c_str());
        if( datalayer && !m_DataResourceVector.Add(datalayer) )
        {
          m_Logger->warning("c_RestConfig::ReadFromXML : Duplicated 'UriTag'. Data Layer will not be added.");
          delete datalayer;
        }
      }
    }
  }
  catch (Poco::Exception& exc)
  {	  
    m_Logger->error(exc.message().c_str());
    throw c_ExceptionRestConfig(exc.message().c_str());

  }
  
}

void c_RestConfig::ReadFromXML( const char* FileName, const char* TemplateFolder )
{
  // Parse an XML document from standard input
	// and use a NodeIterator to print out all nodes.
  Poco::File xmlfile(FileName);
  if( !xmlfile.exists() || !xmlfile.isFile() ) return;
  
  std::string noticemsg;
  noticemsg.reserve(512);
  noticemsg = "c_RestConfig::ReadFromXML : Reading File:'";
  noticemsg.append(FileName);
  noticemsg.append("'");
	m_Logger->notice(noticemsg);
	std::ifstream file;
	
	file.open(FileName);
	if( !file.is_open() )
	{
	  std::string errmsg = "c_RestConfig::ReadFromXML : unable to open file '";
	  errmsg = errmsg + FileName;
	  errmsg = errmsg + "'";
	  m_Logger->error(errmsg);
	  throw c_ExceptionRestConfig(L"Unable to open config file!");
	}
	Poco::XML::InputSource src(file);
	try
	{
		Poco::XML::DOMParser parser;
		AutoPtr<Poco::XML::Document> pdoc = parser.parse(&src);
		Read(pdoc,TemplateFolder);
		
	}
	catch (Poco::Exception& exc)
	{	  
	  file.close();
	  m_Logger->error(exc.message().c_str());
	  throw c_ExceptionRestConfig(exc.message().c_str());
		
	}
	file.close();
}

void c_RestConfig::ReadXMLFromFolder(const char * Folder)
{
  //std::string s1;
  //Poco::UnicodeConverter::toUTF8(Folder,s1);
  Poco::Path cfgpath(Folder);  
  cfgpath.setFileName(D_DEFAULT_CFG_FILENAME);
  std::string fname_cfg = cfgpath.toString();  

  try
  {
    ReadFromXML(fname_cfg.c_str(),Folder); 
  }
  catch (...) // ignore those error.. like file not exists
  {
  	
  }
  
  


  // read from subfolders
  // create "conf" path
  Poco::Path confpath(Folder);  
 

  Poco::DirectoryIterator it(confpath);
  Poco::DirectoryIterator end;
  while (it != end)
  {

    if( it->isDirectory() && it->canRead() )
    {
      //std::string path = ;
      Poco::Path subcfg(it.path());
      subcfg = subcfg.makeDirectory();
      ReadXMLFromFolder(subcfg.toString().c_str());
      
      
      //subcfg.setFileName(D_DEFAULT_CFG_FILENAME);
      //std::string fname_cfg = subcfg.toString();  
      //ReadFromXML(fname_cfg.c_str());
    }

    ++it;
  }
}

void c_RestConfig::ReadFromXML()
{
  
  std::string s1;
  Poco::UnicodeConverter::toUTF8(g_AppFileName,s1);
  Poco::Path cfgpath(s1);  
  cfgpath.setFileName("");
  string folder = cfgpath.toString();
  cfgpath.setFileName(D_DEFAULT_CFG_FILENAME);
  std::string fname_cfg = cfgpath.toString();  
  
  // Check cfg file in binary folder (only for older version compability)
  try
  {  
    ReadFromXML(fname_cfg.c_str(),folder.c_str());
  }
  catch(...) // just ignore erros - if there is no file
  {
  
  }
  
  
  // create "conf" path
  Poco::Path confpath(cfgpath);  
  try
  {
    confpath.setFileName("");
    confpath.popDirectory();
    confpath.pushDirectory("conf");
    ReadXMLFromFolder(confpath.toString().c_str());
  }
  catch(...)
  {  
  }
  
  
}

bool ElementExists(Poco::XML::Element* ParentNode,const char* ElemName)
{
  Poco::XML::Element* node_uritag = ParentNode->getChildElement(ElemName);  
  return node_uritag ? true : false;  
}

void GetElementTextValue(Poco::XML::Element* ParentNode,const char* ElemName,std::wstring& TextValue)
{
  Poco::XML::Element* node_uritag = ParentNode->getChildElement(ElemName);  
  XMLString uritag;
  if( node_uritag ) 
  {
    uritag = node_uritag->innerText();
    Poco::UnicodeConverter::toUTF16(uritag,TextValue);
  }
}

void GetElementAttribute(Poco::XML::Element* Elem,const char* Attribute,std::wstring& TextValue)
{
  //XMLString attr = ;
  
  Poco::UnicodeConverter::toUTF16(Elem->getAttribute(Attribute),TextValue);
}

void GetHtmlTemplateData_v1(Poco::XML::Element* node_html,c_CfgRepTemplate* RepTemplate)
{
  
  GetElementTextValue(node_html,D_XMLTAG_HTMLTEMPLATE_SINGLE,RepTemplate->m_HtmlTemp_Single);
    
  GetElementTextValue(node_html,D_XMLTAG_HTMLTEMPLATE_MANY,RepTemplate->m_HtmlTemp_Many);
  
  GetElementTextValue(node_html,D_XMLTAG_HTMLTEMPLATE_ZERO,RepTemplate->m_HtmlTemp_Zero);
  
  GetElementTextValue(node_html,D_XMLTAG_HTMLTEMPLATE_ERROR,RepTemplate->m_HtmlTemp_Error);
  
  GetElementTextValue(node_html,D_XMLTAG_HTMLTEMPLATE_SECTION,RepTemplate->m_HtmlTemp_Section);        
  
  GetElementTextValue(node_html,D_XMLTAG_HTMLTEMPLATE_PREFIX,RepTemplate->m_HtmlTemp_Prefix);        
  
  // reader additional data to be fetched for html template
  Poco::AutoPtr<Poco::XML::NodeList> list_adata = node_html->getElementsByTagName(D_XMLTAG_HTMLTEMPLATE_ADATA);
  int count_feat = list_adata->length();
  for(int ind=0;ind<count_feat;ind++)
  {
    Poco::XML::Element* elem_adata= (Poco::XML::Element*)list_adata->item(ind);
	  
    c_CfgRepTemplateExtraData* adata = new c_CfgRepTemplateExtraData();
	  
    GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_URI,adata->m_FetchUri);
    GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_SECTION,adata->m_HtmlTemp_Section);
    GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_DATA_SECTION,adata->m_HtmlTemp_DataSection);
    GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_PREFIX,adata->m_HtmlTemp_Prefix);
	  
    RepTemplate->m_RepTemplateExtraDataVector.push_back(adata);
  }
  
  
  c_CfgAccessMethod* get_method = new c_CfgAccessMethod();
  get_method->m_HttpMethod = L"GET";
  RepTemplate->AddMethod(get_method);
  
  //
  // <MaxBBOX>
  //   <Width>
  //   <Height>
  // </MaxBBOX>
  //
  Poco::XML::Element* max_bbox = node_html->getChildElement(D_XMLTAG_HTMLTEMPLATE_MAX_BBOX);    
  if( max_bbox )
  {
    std::wstring temp;
    wchar_t *errpt;
    GetElementTextValue(max_bbox,D_XMLTAG_HTMLTEMPLATE_MAX_BBOX_HEIGHT,temp);
    if( temp.length() > 0 )
    {
      get_method->m_MaxBBoxHeight = wcstod(temp.c_str(),&errpt);
      if( !*errpt ) get_method->m_IsMaxBBoxHeight = true;
    }
    
    temp=L"";
    GetElementTextValue(max_bbox,D_XMLTAG_HTMLTEMPLATE_MAX_BBOX_WIDTH,temp);
    if( temp.length() > 0 )
    {
      get_method->m_MaxBBoxWidth= wcstod(temp.c_str(),&errpt);
      if( !*errpt ) get_method->m_IsMaxBBoxWidth= true;
    }
  }
  
  {
    std::wstring temp;
    wchar_t *errpt;
    GetElementTextValue(node_html,D_XMLTAG_HTMLTEMPLATE_MAX_COUNT,temp);
    if( temp.length() > 0 )
    {
      get_method->m_MaxCount= wcstol(temp.c_str(),&errpt,10);
      if( !*errpt && get_method->m_MaxCount>0 ) get_method->m_IsMaxCount= true;
    }
  }
  
}

void GetTemplateData_v2(Poco::XML::Element* node_html,c_CfgRepTemplate* RepTemplate)
{

  GetElementAttribute(node_html,D_XMLTAG_SECTION,RepTemplate->m_HtmlTemp_Section);        

  GetElementAttribute(node_html,D_XMLTAG_PREFIX,RepTemplate->m_HtmlTemp_Prefix);        
  
  {    
    Poco::XML::Element* node_file;
    
    node_file = node_html->getChildElement(D_XMLTAG_SINGLE);  
    if( node_file )
      GetElementAttribute(node_file,"file",RepTemplate->m_HtmlTemp_Single);

    node_file = node_html->getChildElement(D_XMLTAG_MANY);  
    if( node_file )
      GetElementAttribute(node_file,"file",RepTemplate->m_HtmlTemp_Many);

    node_file = node_html->getChildElement(D_XMLTAG_ZERO);  
    if( node_file ) 
      GetElementAttribute(node_file,"file",RepTemplate->m_HtmlTemp_Zero);

    node_file = node_html->getChildElement(D_XMLTAG_ERROR);  
    if( node_file )
      GetElementAttribute(node_file,"file",RepTemplate->m_HtmlTemp_Error);    
  }
  

  // reader additional data to be fetched for html template
  Poco::AutoPtr<Poco::XML::NodeList> list_adata = node_html->getElementsByTagName(D_XMLTAG_HTMLTEMPLATE_ADATA);
  int count_feat = list_adata->length();
  for(int ind=0;ind<count_feat;ind++)
  {
    Poco::XML::Element* elem_adata= (Poco::XML::Element*)list_adata->item(ind);

    c_CfgRepTemplateExtraData* adata = new c_CfgRepTemplateExtraData();
    GetElementAttribute(elem_adata,D_XMLTAG_ADATA_RESOURCE,adata->m_FetchUri);
    GetElementAttribute(elem_adata,D_XMLTAG_ADATA_SECTION,adata->m_HtmlTemp_Section);
    GetElementAttribute(elem_adata,D_XMLTAG_ADATA_ROWSECTION,adata->m_HtmlTemp_DataSection);
    GetElementAttribute(elem_adata,D_XMLTAG_ADATA_PREFIX,adata->m_HtmlTemp_Prefix);
    //GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_URI,adata->m_FetchUri);
    //GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_SECTION,adata->m_HtmlTemp_Section);
    //GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_DATA_SECTION,adata->m_HtmlTemp_DataSection);
    //GetElementTextValue(elem_adata,D_XMLTAG_HTMLTEMPLATE_ADATA_PREFIX,adata->m_HtmlTemp_Prefix);

    RepTemplate->m_RepTemplateExtraDataVector.push_back(adata);
  }


  
}

c_CfgAccessMethod* c_RestConfig::ParseMethod( Poco::XML::Element* XmlMethod)
{
  std::wstring name;
  GetElementAttribute(XmlMethod,"name",name);
  
  c_CfgAccessMethod* method=NULL;
  if( wcsicmp( name.c_str(),L"GET")==0 )
  {
    method = new c_CfgAccessMethod();
    method->m_HttpMethod = name;
    
    
    wchar_t *errpt;
    std::wstring attrval;
    
    GetElementAttribute(XmlMethod,"maxcount",attrval);    
    if( attrval.length() > 0 )
    {    
      method->m_MaxCount= wcstod(attrval.c_str(),&errpt);
      if( !*errpt && method->m_MaxCount>0 ) method->m_IsMaxCount= true;
    }
    
    
    GetElementAttribute(XmlMethod,"maxbboxwidth",attrval);    
    if( attrval.length() > 0 )
    {
      method->m_MaxBBoxWidth= wcstod(attrval.c_str(),&errpt);
      if( !*errpt ) method->m_IsMaxBBoxWidth= true;
    }
    
    GetElementAttribute(XmlMethod,"maxbboxheight",attrval);    
    if( attrval.length() > 0 )
    {
      method->m_MaxBBoxHeight= wcstod(attrval.c_str(),&errpt);
      if( !*errpt ) method->m_IsMaxBBoxHeight= true;
    }

  }
  if( wcsicmp( name.c_str(),L"PUT")==0  )
  {
    method = new c_CfgAccessMethod();
    method->m_HttpMethod = name;
  }
  if( wcsicmp( name.c_str(),L"POST")==0 )
  {
    method = new c_CfgAccessMethod();
    method->m_HttpMethod = name;
  }
  if( wcsicmp( name.c_str(),L"DELETE")==0 )
  {
    method = new c_CfgAccessMethod();
    method->m_HttpMethod = name;
  }
  if( wcsicmp( name.c_str(),L"OPTION")==0 )
  {
    method = new c_CfgAccessMethod();
    method->m_HttpMethod = name;
  }
  if( wcsicmp( name.c_str(),L"HEADER")==0 )
  {
    method = new c_CfgAccessMethod();
    method->m_HttpMethod = name;
  }
  
  return method;
}

void c_RestConfig::GetOData_ElemOverride(Poco::XML::Element* XmlElemOverride,c_AtomElementOverride* ElemOverride)
{
  if( !XmlElemOverride ) return;
  
  bool is_type_property;
  std::wstring typestr;
  GetElementAttribute(XmlElemOverride,"type",typestr);
  
  /*if( Poco::icompare(typestr,L"Property") == 0 )
    is_type_property = true;
  else
    is_type_property = false;
  */
  
  std::string instr = XmlElemOverride->innerText();
  std::wstring overr_value;
  Poco::UnicodeConverter::toUTF16(instr,overr_value);
  
  ElemOverride->SetValue(overr_value,typestr);
}

void c_RestConfig::GetOData(Poco::XML::Element* XmlRepresentation,c_CfgRepOdata* Rep)
{
  {
  
  Poco::XML::Element* feedoverride = XmlRepresentation->getChildElement("FeedOverride");
  if( feedoverride )
  {
    Poco::XML::Element* authoroverride = feedoverride->getChildElement("Author");
    if( authoroverride )
    {
      Poco::XML::Element* overval = authoroverride->getChildElement("Name");
      GetOData_ElemOverride(overval,&Rep->m_FeedAuthor.m_AuthorName);
      
      overval = authoroverride->getChildElement("Uri");
      GetOData_ElemOverride(overval,&Rep->m_FeedAuthor.m_AuthorUri);
      
      overval = authoroverride->getChildElement("Email");
      GetOData_ElemOverride(overval,&Rep->m_FeedAuthor.m_AuthorEmail);
    }
    
    Poco::XML::Element* titleoverride = feedoverride->getChildElement("Title");
    GetOData_ElemOverride(titleoverride,&Rep->m_FeedTitle);
  }
  }
  {
  
  Poco::XML::Element* entryoverride = XmlRepresentation->getChildElement("EntryOverride");
  if( entryoverride )
  {
    Poco::XML::Element* authoroverride = entryoverride->getChildElement("Author");
    if( authoroverride )
    {
      Poco::XML::Element* overval = authoroverride->getChildElement("Name");
      GetOData_ElemOverride(overval,&Rep->m_EntryAuthor.m_AuthorName);

      overval = authoroverride->getChildElement("Uri");
      GetOData_ElemOverride(overval,&Rep->m_EntryAuthor.m_AuthorUri);

      overval = authoroverride->getChildElement("Email");
      GetOData_ElemOverride(overval,&Rep->m_EntryAuthor.m_AuthorEmail);
    }
    
    Poco::XML::Element* titleoverride = entryoverride->getChildElement("Title");
    GetOData_ElemOverride(titleoverride,&Rep->m_EntryTitle);
  }
  }
}

/*
<Representation pattern=".html$" mimetype="text/html" >
<Templates>
<Multi file="buildings_html.tpl" />
<Single file="building_html.tpl" />
<None file="building_none_html.tpl" />
</Templates>
</Representation>
*/
c_CfgRepresentation* c_RestConfig::ParseRepresentation_v2( Poco::XML::Element* XmlRepresentation )
{
  std::wstring renderer;
  GetElementAttribute(XmlRepresentation,"renderer",renderer);
  
  std::wstring pattern;
  GetElementAttribute(XmlRepresentation,"pattern",pattern);
  
  std::wstring mimetype;
  GetElementAttribute(XmlRepresentation,"mimetype",mimetype);
  
  // if rendere is not odata then pattern has to be set
  if( ( pattern.length()==0) &&  wcsicmp( renderer.c_str(),L"OData") ) return NULL;
  
  
  c_CfgRepresentation* representaion = NULL;
  
  if( renderer.length()==0 || (wcsicmp( renderer.c_str(),L"Template")==0) ) 
  {
    if( mimetype.length()==0 ) return NULL;
    
    c_CfgRepTemplate * rep_template = new c_CfgRepTemplate(pattern.c_str(),mimetype.c_str());
    representaion = rep_template;
    Poco::XML::Element* xmltemp = XmlRepresentation->getChildElement("Templates");    
    if( xmltemp )
    {
      GetTemplateData_v2(xmltemp,rep_template);
    }
    
  }
  if( wcsicmp( renderer.c_str(),L"PNG8")==0 ) // built-in XML representation of data
  {
    
    // Create PNG representation
    c_CfgRepresentation_Png8_MapGuide * repr_png = new c_CfgRepresentation_Png8_MapGuide();
    representaion = repr_png;
    
    GetElementTextValue(XmlRepresentation,D_XMLTAG_MAPDEF,repr_png->m_MapDef);
    GetElementTextValue(XmlRepresentation,D_XMLTAG_DEFAULT_ZOOM_FACTOR,repr_png->m_MapZoomFactor);
    GetElementTextValue(XmlRepresentation,D_XMLTAG_SELECTION_LAYER,repr_png->m_MapSelectionLayer);
    GetElementTextValue(XmlRepresentation,D_XMLTAG_MG_MAP_FEATURE_SOURCE,repr_png->m_MapFeatureSource);
    GetElementTextValue(XmlRepresentation,D_XMLTAG_MG_MAP_FEATURE_SOURCE_FILTER,repr_png->m_MapFeatureSourceFilter);
  }
  
  if( wcsicmp( renderer.c_str(),L"XML")==0 ) // built-in XML representation of data
  {
    representaion = new c_CfgRepresentation_FeaturesXML();   
  }
  if( wcsicmp( renderer.c_str(),L"JSON")==0 ) // built-in JSON representation of data
  {
    representaion = new c_CfgRepresentation_FeaturesJSON();
  }
  if( wcsicmp( renderer.c_str(),L"FdoSchema")==0 ) // built-in XML representation of data
  {
    representaion = new c_CfgRepresentation_FDO_Schema();   
  }
  if( wcsicmp( renderer.c_str(),L"Sitemap")==0 ) // built-in JSON representation of data
  {
    representaion = new c_CfgRepresentation_Sitemap();
  }
  
  if( wcsicmp( renderer.c_str(),L"OData")==0 ) // built-in JSON representation of data
  {
    representaion = new c_CfgRepOdata();
    
    GetOData(XmlRepresentation,(c_CfgRepOdata*)representaion);
  }
  
 
  
  // for Representation read method access
  if( representaion )
  {
    AutoPtr<Poco::XML::NodeList>  nlist = XmlRepresentation->getElementsByTagName("Method");
    for(int ind=0;ind<nlist->length();ind++)
    {
      Poco::XML::Element* node_method = (Poco::XML::Element*)nlist->item(ind);
      c_CfgAccessMethod* method= c_RestConfig::ParseMethod( node_method);
      if( method )
      {
        representaion->AddMethod(method);
      }
      // get attribute name

    }
  }
  
  // for Representation read order
  if( representaion )
  {
    Poco::XML::Element* order = XmlRepresentation->getChildElement("Order");    
    if( order )
    {
      std::wstring fields;
      GetElementAttribute(order,"fields",fields);
      representaion->SetOrderFields(fields.c_str());
      
      std::wstring direction;
      GetElementAttribute(order,"direction",direction);
      if( direction.length() > 0)
      {      
        if( wcsicmp( direction.c_str(),L"DESC")==0 ) // built-in XML representation of data
          representaion->SetOrderDirection(c_CfgRepresentation::e_Desc);   
        else
          representaion->SetOrderDirection(c_CfgRepresentation::e_Asc);   
      }
    }
  }
  
  return representaion;
}

// c_RestConfig::ParseResource_v1 parses XML for resource definition. It is old (first) version of configuration file
c_CfgDataResource* c_RestConfig::ParseResource_v1( Poco::XML::Element* Layer ,const char* TemplateFolder)
{
  
  
  wstring uritag;
  GetElementTextValue(Layer,D_XMLTAG_URITAG,uritag);  
  if( uritag.length()==0)
  {
    m_Logger->error("Empty 'UriTag'. Resource will not be created.");
    return NULL;
  }
  c_CfgDataResource* resource = new c_CfgDataResource(uritag.c_str(),TemplateFolder);
  
  GetElementTextValue(Layer,D_XMLTAG_RESTURIPART,resource->m_RestUriPart);
  
  c_CfgDataSource_MgFeatureSource *mgsource = new c_CfgDataSource_MgFeatureSource();
  resource->m_DataSource = mgsource;
  
  GetElementTextValue(Layer,D_XMLTAG_MG_FEATURESOURCE,mgsource->m_MgFeatureSource);
  
  GetElementTextValue(Layer,D_XMLTAG_MG_CLASSNAME,mgsource->m_MgFeatureSourceClassName);
  
 
  // Create PNG representation
  c_CfgRepresentation_Png8_MapGuide * repr_png = new c_CfgRepresentation_Png8_MapGuide();
  resource->AddRepresentation(repr_png);
  
  GetElementTextValue(Layer,D_XMLTAG_MG_MAPDEF,repr_png->m_MapDef);
  GetElementTextValue(Layer,D_XMLTAG_MG_MAP_ZOOM_FACTOR,repr_png->m_MapZoomFactor);
  GetElementTextValue(Layer,D_XMLTAG_MG_MAP_SELECTION_LAYER,repr_png->m_MapSelectionLayer);
  GetElementTextValue(Layer,D_XMLTAG_MG_MAP_FEATURE_SOURCE,repr_png->m_MapFeatureSource);
  GetElementTextValue(Layer,D_XMLTAG_MG_MAP_FEATURE_SOURCE_FILTER,repr_png->m_MapFeatureSourceFilter);
  
  
  
  {
    Poco::XML::Element* node_html = Layer->getChildElement(D_XMLTAG_HTMLTEMPLATE); // "Htmltemplate"
    
    if( node_html )
    {
      c_CfgRepTemplate * repr_template = new c_CfgRepTemplate(L".html",L"text/html");
      resource->AddRepresentation(repr_template);
      
  		GetHtmlTemplateData_v1(node_html,repr_template);  
       
    }
  }
  
  {
    Poco::XML::Element* node_kml = Layer->getChildElement(D_XMLTAG_KMLTEMPLATE); // "KmlTemplate"
    
    if( node_kml )
    {
      c_CfgRepTemplate * repr_kml = new c_CfgRepTemplate(L".kml",L"application/vnd.google-earth.kml+xml");
      resource->AddRepresentation(repr_kml);
  		GetHtmlTemplateData_v1(node_kml,repr_kml);  
  		
  		// add same for kmz
      c_CfgRepTemplate * repr_kmz = new c_CfgRepTemplate(L".kmz",L"application/vnd.google-earth.kmz");
      resource->AddRepresentation(repr_kmz);
      GetHtmlTemplateData_v1(node_kml,repr_kmz);  
       
    }
  }
  
  // Check for additional representations
  {
  
    AutoPtr<Poco::XML::NodeList>  nlist = Layer->getElementsByTagName("Representation");
    for(int ind=0;ind<nlist->length();ind++)
    {
      Poco::XML::Element* node_rep = (Poco::XML::Element*)nlist->item(ind);
      c_CfgRepresentation* cfgrep = ParseRepresentation_v2(node_rep);
      if( cfgrep ) resource->AddRepresentation(cfgrep);
      
    }
  }
  
  
  resource->AddRepresentation( new c_CfgRepresentation_FeaturesXML() );

  resource->AddRepresentation( new c_CfgRepresentation_FeaturesJSON() );
  
  // BY default create FDO chema representation and sitemp.xml representation
  resource->AddRepresentation( new c_CfgRepresentation_FDO_Schema() );
  
  resource->AddRepresentation( new c_CfgRepresentation_Sitemap() );
  
  
  
  
  return resource;
}//end of c_RestConfig::ParseResource_v1




c_CfgDataSource* c_RestConfig::ParseSource( Poco::XML::Element* XmlSource)
{
  c_CfgDataSource* source=NULL;
  // source type
  std::wstring source_type;
  GetElementAttribute(XmlSource,"type",source_type);
  if( wcsicmp(source_type.c_str(),L"MapGuide") == 0 ) 
  {
    c_CfgDataSource_MapGuide *mgsource = NULL;
    std::wstring featuresource,featureclass;
    
    GetElementTextValue(XmlSource,D_XMLTAG_FEATURESOURCE,featuresource);
    GetElementTextValue(XmlSource,D_XMLTAG_FEATURECLASS,featureclass);
    
    if( featuresource.length() != 0 &&  featureclass.length()!=0 )
    {
      c_CfgDataSource_MgFeatureSource *mgfeature = new c_CfgDataSource_MgFeatureSource();
      mgsource = mgfeature;
      mgfeature->SetMgFeatureSource(featuresource);
      mgfeature->SetMgFeatureSourceClassName(featureclass);
    }
    
    if( !mgsource )
    {
      m_Logger->error("c_RestConfig::ParseSource : Invalid 'MapGuide' source definition. Missing MapGuide source elements.");
      return NULL;
    }
    
    // Read other MapGuide source attributes
    std::wstring serverip,serverport;
    GetElementTextValue(XmlSource,D_XMLTAG_MG_SERVERIP,serverip);
    if( serverip.length() != 0 ) mgsource->SetServerIP(serverip);
    GetElementTextValue(XmlSource,D_XMLTAG_MG_SERVERPORT,serverport);
    if( serverport.length() != 0 ) 
    {
      wchar_t* errpt;
      long sport;
      sport = wcstol(serverport.c_str(),&errpt,10);
      if( !*errpt ) mgsource->SetServerPort(sport);
      
      
    }
    
    std::wstring username,password;
    if( ElementExists(XmlSource,D_XMLTAG_MG_USERNAME) )
    {
      GetElementTextValue(XmlSource,D_XMLTAG_MG_USERNAME,username);
      mgsource->SetUsername(username);
    }
    
    if( ElementExists(XmlSource,D_XMLTAG_MG_PASSWORD) )
    {
      GetElementTextValue(XmlSource,D_XMLTAG_MG_PASSWORD,password);
      mgsource->SetPassword(password);
    }
    
    source = mgsource;
  }
  
  if( wcsicmp(source_type.c_str(),L"FDO") == 0 ) 
  {
    c_CfgDataSource_FDO *fdosource = NULL;
    std::wstring provider,connstring,featureclass;
    
    
    GetElementTextValue(XmlSource,D_XMLTAG_FDO_PROVIDER,provider);
    GetElementTextValue(XmlSource,D_XMLTAG_FDO_CONNECTIONSTRING,connstring);
    GetElementTextValue(XmlSource,D_XMLTAG_FEATURECLASS,featureclass);
    
    if( provider.length()==0 )
    {
      m_Logger->error("Zero length provider name. Set 'provider' attribute in FDO source type!");
      return NULL;
    }
    
    fdosource = new c_CfgDataSource_FDO();
    fdosource->SetProvider(provider);
    fdosource->SetConnString(connstring);
    fdosource->SetFeatureClass(featureclass);
    source = fdosource;
  }
   
  if( !source )
  {
    m_Logger->error("c_RestConfig::ParseSource : Invalid 'Source' definition. Check Source type.");
  }
  
  return source;
}//end of  c_RestConfig::ParseSource


// New config xml for resource
//
//  <Resource uripart="building">
//  <Source type="MapGuide">
//  <LayerDefinition>Library://Samples/Layers/UtilityWater.FeatureSource</LayerDefinition>
//  </Source>
//  <Representation renderer="XML" />
//  <Representation renderer="JSON" />
//  <Representation pattern=".html$" mimetype="text/html" >
//  <Templates>
//  <Multi file="buildings_html.tpl" />
//  <Single file="building_html.tpl" />
//  <None file="building_none_html.tpl" />
//  </Templates>
//  </Representation>
//  </Resource>
//
c_CfgDataResource* c_RestConfig::ParseResource_v2( Poco::XML::Element* XmlResource,const char* TemplateFolder )
{
  

  std::wstring uripart;
  GetElementAttribute(XmlResource,"uripart",uripart);
  if( uripart.length()==0 )
  {
    m_Logger->error("c_RestConfig::ParseResource_v2 : No attribute 'uripart' defined for Resource.");
    return NULL;
  }
  
  
  
  // Read Data Source
  c_CfgDataSource *res_source=NULL;
  {
    Poco::XML::Element* node_source = XmlResource->getChildElement(D_XMLTAG_SOURCE); // "KmlTemplate"
    if( !node_source )
    {
      m_Logger->error("c_RestConfig::ParseResource_v2 : No 'Source' defined for Resource. Missing 'Source' element in Resource.");
      return NULL;
    }
    
    res_source = ParseSource(node_source);
    if( !res_source )
    {
      return NULL;
    }        
  }
  
  c_CfgDataResource* resource = new c_CfgDataResource(uripart.c_str(),TemplateFolder);
  resource->m_UriTag = uripart;
  resource->m_DataSource = res_source;
   
  
  
  // Read Representations
  {

    AutoPtr<Poco::XML::NodeList>  nlist = XmlResource->getElementsByTagName("Representation");
    for(int ind=0;ind<nlist->length();ind++)
    {
      Poco::XML::Element* node_rep = (Poco::XML::Element*)nlist->item(ind);
      c_CfgRepresentation* cfgrep = ParseRepresentation_v2(node_rep);
      if( cfgrep ) resource->AddRepresentation(cfgrep);

    }
  }

/*
  resource->AddRepresentation( new c_CfgRepresentation_FeaturesXML() );

  resource->AddRepresentation( new c_CfgRepresentation_FeaturesJSON() );

  // BY default create FDO chema representation and sitemp.xml representation
  resource->AddRepresentation( new c_CfgRepresentation_FDO_Schema() );

  resource->AddRepresentation( new c_CfgRepresentation_Sitemap() );
*/



  return resource;
}//end of c_RestConfig::ParseResource_v2
const c_CfgDataResource* c_RestConfig::FindUriTag( const std::wstring& UriTag )
{
  return m_DataResourceVector.FindUriTag(UriTag);
}

void c_RestConfig::ReadFromString( const std::string& XmlCfg, const char* TemplateFolder )
{
  
  try
  {
    std::stringstream xmlstream(XmlCfg);
    Poco::XML::InputSource src(xmlstream);
    Poco::XML::DOMParser parser;
    AutoPtr<Poco::XML::Document> pdoc = parser.parse(&src);
    Read(pdoc,TemplateFolder);

  }
  catch (Poco::Exception& exc)
  {	  
    m_Logger->error(exc.message().c_str());
    throw c_ExceptionRestConfig(exc.message().c_str());

  }
}

int c_RestConfig::GetResourceCount()
{
  return m_DataResourceVector.GetCount();
}

const c_CfgDataResource* c_RestConfig::GetResource( int Index )
{
  return m_DataResourceVector.GetResource(Index);
}



/*
const c_CfgDataResource* c_RestConfig::FindClassName( const std::wstring& ClassName )
{
  return m_DataResourceVector.FindClassName(ClassName);
}
*/