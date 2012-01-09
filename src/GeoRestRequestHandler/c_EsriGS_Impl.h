//
//  Copyright (C) 2010 by  Haris Kurtagic  www.sl-king.com
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
#ifndef _c_EsriGS_Impl_h
#define _c_EsriGS_Impl_h

#include "c_EsriGS_Interface.h"
#include "Poco/XML/XMLWriter.h"
class c_RestConfig;
class c_RestRequest;
class c_CfgRepOdata;

class c_EsriGS_Impl : public c_EsriGS_Interface
{
public:
  c_EsriGS_Impl();
  ~c_EsriGS_Impl();

public:

  virtual bool MatchURI(c_CfgEsriGS_Folder *StartFolder,c_RestUriPathSegment* PathParams,c_CfgEsriGS_Folder *& MatchFolder,c_CfgEsriGS_Server *& MatchServer);

  virtual std::string& GetOdataSchemaName(std::string& name) const;
  virtual std::string& GetOdataEntityName(const c_CfgDataResource* Res,std::string& name) const;
  virtual std::string& GetOdataFullEntityName(const c_CfgDataResource* Res,std::string& fullname) const;
  
  virtual c_EsriGS_Interface::e_FormatType GetResponseFormatType(c_RestRequest* RestRequest);
  virtual void CreateCatalogDocument( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI,e_FormatType& Format  ); 
  
  virtual void CreateServiceMetadata( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI );
  virtual void Reader2AtomCollection(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation,c_RestDataReader* Reader,std::ostream & OutStream,e_FormatType& Format );
  virtual void Reader2AtomEntry(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation,c_RestDataReader* Reader
              ,std::ostream & OutStream,e_FormatType& Format,std::wstring& OneProperty,bool IsRawValue );

  void GetPropertyValueAsOdataLiteral_Atom(c_RestDataReader* Reader,const std::wstring& PropName,std::string& KeysVal);
  void GetPropertyValueAsOdataLiteral_URI(c_RestDataReader* Reader,const std::wstring& PropName,std::string& KeysVal);
  
  
  //void ParseKeyValueString( const std::wstring& KeyVal,MgClassDefinition* DataClassDef,MgPropertyCollection* Props,std::wstring& SingleValue );

  MgProperty* CreatePropertyValue(const std::wstring& Name,MgPropertyDefinition* PropDef,const std::wstring& Val);
  
  
  
protected:
 
  void GetOdataKeyValues(c_RestDataReader* Reader,MgClassDefinition* DataClassDef,std::string& KeysVal);
  void GetPropertyValueAsOdataLiteral_X(c_RestDataReader* Reader,const std::wstring& PropName,std::string& StrVal,bool IsUri);
  void CreateCatalogDocument_Json( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI,bool PrettyJson );
  void CreateCatalogDocument_Html( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI );
  void Reader2AtomEntry_Json(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation ,c_RestDataReader* Reader
                          ,std::ostream & OutStream,std::wstring& OneProperty,bool IsRawValue);
  void Reader2AtomEntry_Atom(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation ,c_RestDataReader* Reader
                                  ,std::ostream & OutStream,std::wstring& OneProperty,bool IsRawValue);
  void Reader2AtomCollection_Json(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation ,c_RestDataReader* Reader,std::ostream & OutStream);
  void Reader2AtomCollection_Atom(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation ,c_RestDataReader* Reader,std::ostream & OutStream);
  void WriteEntry_Atom(Poco::XML::XMLWriter& writer ,const c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation 
                      ,c_RestDataReader* Reader,std::wstring& OneProperty,const std::string& Updated,bool AddBase=false);
  void WriteEntry_Json(std::ostream & OutStream,const c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation 
                      ,c_RestDataReader* Reader,std::wstring& OneProperty);
  void WriteProperty_Json( std::ostream & OutStream, c_RestDataReader* Reader,MgPropertyDefinition* prop);
  void WriteProperty_Atom(Poco::XML::XMLWriter& writer,c_RestDataReader* Reader,MgPropertyDefinition* prop);

};

#endif
