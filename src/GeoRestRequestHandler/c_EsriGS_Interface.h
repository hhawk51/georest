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
#ifndef _c_EsriGS_Interface_h
#define _c_EsriGS_Interface_h

#define D_XMLS_APP "http://www.w3.org/2007/app"
#define D_XMLS_ATOM "http://www.w3.org/2005/Atom"

#define D_XMLS_ODATA_M "http://schemas.microsoft.com/ado/2007/08/dataservices/metadata"
#define D_XMLS_ODATA_D "http://schemas.microsoft.com/ado/2007/08/dataservices"

#define D_XMLS_EDM "http://schemas.microsoft.com/ado/2007/05/edm"
#define D_XMLS_EDMX "http://schemas.microsoft.com/ado/2007/06/edmx"

#define D_XMLS_GEO "http://schemas.georest.org/geodata/2010/06/geo"
#define D_XMLS_GEO_PREFIX "g"


class c_RestConfig;
class c_CfgDataResource;
class c_RestDataReader;
class c_CfgRepOdata;
class c_RestRequest;
class MgClassDefinition;
class MgPropertyCollection;
class c_CfgEsriGS_Folder;
class c_CfgEsriGS_Server;

class REST_REQUEST_HANDLER_API c_EsriGS_Interface
{

public:
  static c_EsriGS_Interface* Impl(); 

public:
  enum e_FormatType
  {
    e_Html,
    e_Json,
    e_PrettyJson
  };

public:
  virtual bool MatchURI(c_CfgEsriGS_Folder *StartFolder,c_RestUriPathSegment* PathParams,c_CfgEsriGS_Folder *& MatchFolder,c_CfgEsriGS_Server *& MatchServer) = 0;
  virtual std::string& GetOdataSchemaName(std::string& name) const = 0;
  virtual std::string& GetOdataEntityName(const c_CfgDataResource* Res,std::string& name) const = 0;
  virtual std::string& GetOdataFullEntityName(const c_CfgDataResource* Res,std::string& fullname) const = 0;
  
  virtual e_FormatType GetResponseFormatType(c_RestRequest* RestRequest) = 0;
  virtual void CreateCatalogDocument( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI,e_FormatType& Format ) = 0;
  
  virtual void CreateServiceMetadata( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI ) = 0;
  virtual void Reader2AtomCollection( c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation,c_RestDataReader* Reader,std::ostream & OutStream,e_FormatType& Format ) = 0;
  virtual void Reader2AtomEntry( c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation,c_RestDataReader* Reader
    ,std::ostream & OutStream,e_FormatType& Format,std::wstring& OneProperty,bool IsRawValue ) = 0;
  
  virtual void GetPropertyValueAsOdataLiteral_URI(c_RestDataReader* Reader,const std::wstring& PropName,std::string& StrVal)=0;
  virtual void GetPropertyValueAsOdataLiteral_Atom(c_RestDataReader* Reader,const std::wstring& PropName,std::string& StrVal)=0;
    
  
  virtual MgProperty* CreatePropertyValue(const std::wstring& Name,MgPropertyDefinition* PropDef,const std::wstring& Val) = 0;
  
  
};

#endif