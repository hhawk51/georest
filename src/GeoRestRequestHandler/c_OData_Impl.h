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
#ifndef _c_OData_Impl_h
#define _c_OData_Impl_h

#include "c_OData_Interface.h"
#include "Poco/XML/XMLWriter.h"
class c_RestConfig;
class c_RestRequest;
class c_CfgRepOdata;

class c_OData_Impl : public c_OData_Interface
{
public:
  c_OData_Impl();
  ~c_OData_Impl();

public:
  virtual std::string& GetOdataSchemaName(std::string& name) const;
  virtual std::string& GetOdataEntityName(const c_CfgDataResource* Res,std::string& name) const;
  virtual std::string& GetOdataFullEntityName(const c_CfgDataResource* Res,std::string& fullname) const;
  
  virtual c_OData_Interface::e_OdataFormatType GetResponseFormatType(c_RestRequest* RestRequest);
  virtual void CreateServiceDocument( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI,e_OdataFormatType& Format  ); 
  virtual void CreateServiceMetadata( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI );
  virtual void Reader2AtomCollection(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation,c_RestDataReader* Reader,std::ostream & OutStream,e_OdataFormatType& Format );
  virtual void Reader2AtomEntry(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation,c_RestDataReader* Reader
              ,std::ostream & OutStream,e_OdataFormatType& Format,std::wstring& OneProperty,bool IsRawValue );

  void GetPropertyValueAsOdataLiteral_Atom(c_RestDataReader* Reader,const std::wstring& PropName,std::string& KeysVal);
  void GetPropertyValueAsOdataLiteral_URI(c_RestDataReader* Reader,const std::wstring& PropName,std::string& KeysVal);
  
  
  void ParseKeyValueString( const std::wstring& KeyVal,MgClassDefinition* DataClassDef,MgPropertyCollection* Props,std::wstring& SingleValue );

  MgProperty* CreatePropertyValue(const std::wstring& Name,MgPropertyDefinition* PropDef,const std::wstring& Val);
  
  
  
protected:
 
  void GetOdataKeyValues(c_RestDataReader* Reader,MgClassDefinition* DataClassDef,std::string& KeysVal);
  void GetPropertyValueAsOdataLiteral_X(c_RestDataReader* Reader,const std::wstring& PropName,std::string& StrVal,bool IsUri);
  void CreateServiceDocument_Json( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI );
  void CreateServiceDocument_Atom( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI );
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
