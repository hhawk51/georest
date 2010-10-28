//
//  Copyright (C) 2009 by Haris Kurtagic
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

#ifndef _c_RestFetchSource_h
#define _c_RestFetchSource_h

#include "c_RestUriPathParam.h"
#include "c_RestDataReader.h"

namespace Poco
{
  class Logger;
  namespace XML
  {
    class Element;
  };
};

namespace ctemplate
{
class TemplateDictionary;
};


class c_ExceptionFetchUri : public MgSystemException
{
  
};

class MgService;
class c_CfgDataSource;
class c_CfgDataSource_MgFeatureSource;
class c_CfgDataSource_FDO;
class MgRestUriPathParam;
class MgHttpRequestParam;

class REST_FETCHURI_API c_RestFetchSource
{
public:
  
  
protected:
  c_RestFetchSource();
  ~c_RestFetchSource(void);
  
public:
  static c_RestDataReader* FetchSource(const c_CfgDataSource* DataSource,MgFeatureQueryOptions* QueryOptions);

  
  static void FetchEnvelope(const wchar_t* Uri,double& X1,double& Y1,double& X2,double& Y2);
  
  static MgGeometry* FetchBuffer( const wchar_t* Uri,double Distance);

  static void FetchFeaturesToDictionary( const std::string& Uri,ctemplate::TemplateDictionary*ParentDict,const std::string& SectionName,const std::string& SectionDataName,const std::string& NamePrefix );
  
  static MgClassDefinition* Fetch_MgClassDefinition( const c_CfgDataSource* DataSource );
  static MgClassDefinition* Fetch_MgClassDefinition_FDO( const c_CfgDataSource_FDO* FdoSource );
  static MgClassDefinition* Fetch_MgClassDefinition_MG( const c_CfgDataSource_MgFeatureSource* MgFeatureSource );
  
  static MgByteReader* Fetch_FdoSourceSchema( c_CfgDataSource_FDO* FdoSource );
  
  
protected:
  enum e_BoolOper { 
    e_AND,
    e_OR
  };
  enum e_CompOper { 
    e_EQUAL,
    e_LIKE,
    e_LIKERIGHT,
    e_LIKELEFT,
    e_GREATER,
    e_GREATER_EQUAL,
    e_LESSER,   
    e_LESSER_EQUAL
  };
    
protected:    
  MgService* CreateMapguideService(const std::wstring& UserName,const std::wstring& Password,INT16 serviceType);
  MgFeatureReader* FetchMgFeatureSource(const c_CfgDataSource_MgFeatureSource* DataSource);
  //void CreateFilterString(MgClassDefinition* ClassDef,MgRestUriPathParam*PathParam,MgHttpRequestParam* RequestParam,MgFeatureQueryOptions*qryOptions,REFSTRING FilterStr);
  static void CreateFilterString(MgClassDefinition* ClassDef,c_RestUriPathSegment*PathParam,c_RestUriRequestParam* RequestParam ,const wstring& UriPathFilter ,MgFeatureQueryOptions*qryOptions,REFSTRING FilterStr);
  static bool ParseFilterQueryParam(const wstring& Param,int& Level,e_BoolOper& BoolOper,e_CompOper& CompOper,wstring& PropertyName);
  
 
  static void HttpFetchFeaturesToDictionary( const std::string& Uri,ctemplate::TemplateDictionary*ParentDict,const std::string& SectionName,const std::string& SectionDataName,const std::string& NamePrefix ) ;
 
 
  
protected: 
  static MgEnvelope* XmlFeatureToEnvelope(Poco::XML::Element * XmlFeature);
  static MgGeometry* XmlFeature2Geometry(Poco::XML::Element * XmlFeature);
  static c_RestDataReader* FetchSource_MG(const c_CfgDataSource_MgFeatureSource* DataSource,MgFeatureQueryOptions* QueryOptions);
  static c_RestDataReader* FetchSource_FDO(const c_CfgDataSource_FDO* DataSource,MgFeatureQueryOptions* QueryOptions);


};

#endif