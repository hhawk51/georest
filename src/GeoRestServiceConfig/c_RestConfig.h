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


#ifndef _c_RestConfig_h
#define _c_RestConfig_h

#include "c_CfgDataResource.h"
#include "c_CfgDataSource.h"
#include "Poco\Logger.h"
namespace Poco
{
  class Logger;
  namespace XML
  {
    class Element;
    class Document;
  };
};


class c_ExceptionRestConfig 
{
  public:
    c_ExceptionRestConfig(const wchar_t* Desc) { m_Desc = Desc;  }
    c_ExceptionRestConfig(const char* Desc);
    std::wstring m_Desc;
};

class REST_CONFIG_API c_RestConfig
{
public:
  static c_RestConfig* GetInstance();
  static void DeleteInstance();
  
  
  const c_CfgDataResource* FindUriTag(const std::wstring& UriTag);
  const c_CfgDataResource* FindClassName(const std::wstring& ClassName);
  
  int GetResourceCount();
  const c_CfgDataResource* GetResource(int Index);
  
  
  const char* GetRestUriSeparator() { return m_RestUriSeparator.c_str(); }
  void ReadFromString( const std::string& XmlCfg, const char* TemplateFolder );
  
  void Clear();
  
  Poco::Logger& GetLogger();

protected:
  c_RestConfig(Poco::Logger* Log=NULL);
  ~c_RestConfig(void);
  
  void ReadFromXML(  ); // this one will read default cfg file, which is: same folder as .dll and named 
  void ReadXMLFromFolder(const char * Folder);
  void ReadFromXML( const char* FileName, const char* TemplateFolder );

protected:  
  void CreateLogger( );
  void CreateLogger( const char* FileName );
  
  
	c_CfgDataResource* ParseResource_v1(Poco::XML::Element* Layer,const char* TemplateFolder);  
  c_CfgDataResource* ParseResource_v2( Poco::XML::Element* XmlResource,const char* TemplateFolder );
  c_CfgRepresentation* ParseRepresentation_v2( Poco::XML::Element* Layer );
  c_CfgAccessMethod* ParseMethod( Poco::XML::Element* XmlMethod);
  c_CfgDataSource* ParseSource( Poco::XML::Element* XmlSource);
  void Read( Poco::XML::Document* PocoDoc, const char* TemplateFolder );
  void GetOData(Poco::XML::Element* XmlRepresentation,c_CfgRepOdata* Rep);
  void GetOData_ElemOverride(Poco::XML::Element* XmlElemOverride,c_AtomElementOverride* ElemOverride);
  Poco::Logger* m_Logger;
  std::string m_LogFileName; // if logger is not set in constructor than in constructor name for log file is created and logger itself
  
  
  std::string m_RestUriSeparator;
  
  c_CfgDataResourceVector m_DataResourceVector;
};

#endif