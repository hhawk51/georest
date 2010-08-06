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

#include "stdafx.h"
#include <json/json.h>
//#include "RestUriFilterParser.h"
#include "RestDefs.h"
#include "c_RestPrimitiveValue.h"
#include "c_RestUriPathParam.h"
#include "c_ExceptionGeoREST.h"

#include "c_RestMapNameToResourceId.h"
#include "c_RestMgSiteConnection.h"

//#include "RestFetchData.h"
#include "RestConfigInclude.h"
#include "c_RestFetchImage.h"


#include "c_RestHandler_OData.h"
#include "c_FdoSelectFeatures.h"
#include "c_RestResult.h"
#include "c_OData_Interface.h"

#include <sstream>
#include "Services/FeatureQueryOptions.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/StringTokenizer.h"
#include "c_FdoFilter.h"
#include "c_Odata_FilterParser.h"


c_RestHandler* c_RestHandler_OData::CreateObject( c_RestRequest* Request )
{
  return new c_RestHandler_OData(Request); 
}


/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
c_RestHandler_OData::c_RestHandler_OData(c_RestRequest *Request)
{
    InitializeCommonParameters(Request);
}

    



/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// c_RestResponse
/// This contains the response (including c_RestResult and StatusCode) from the server.
/// </returns>
void c_RestHandler_OData::Execute(c_RestResponse& HttpResponse)
{
  Ptr<c_RestResult> hResult = HttpResponse.GetResult();
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest->GetRequestParam();

   
  // Now parse resource path part of URI to see which resource is requested
  Ptr<c_RestUriPathParam> path_params = m_RestRequest->GetUriPathParameters();
  if( !path_params->NextParameter() )
  {
    
    return Execute_ServiceDocument(HttpResponse);
  }
  
  // there is resource path
  // it could be $metadata or could be name of some recource from config
  // http://GeoREST.org/OData.svc/$metadata?$format=json or http://GeoREST.org/OData.svc/Parcel?$format=json
  STRING nextparam = path_params->GetCurrentParameterName();
  if( wcscmp(nextparam.c_str(),L"$metadata") == 0 )
  {
    return Execute_ServiceMetadata(HttpResponse);
  }
    
  // it can be now one of uritags for data source  
  // like /rest/odata.svc/Parcel/... or /rest/odata.svc/Parcel(1)...
  STRING resource_uritag;
  STRING keys_string;
  // check if there is "(" in tag
  
  keys_string = path_params->GetCurrentParameterValue();
  resource_uritag = nextparam;
  
  
  // Check HTTP method is get or ..
  if( m_RestRequest->GetHttpMethod() == c_RestUri::e_HttpMethod_Get )
  {
    return Execute_GET_OData(HttpResponse,resource_uritag,keys_string);
  }
  else
  {
    //throw new MgRuntimeException(L"c_RestHandler_OData::Execute",__LINE__, __WFILE__, NULL, L"Not Implemented. Only GET method supported!", NULL);  
    throw c_ExceptionHttpNotImplemented("Not Implemented. Only HTTP GET method supported!");
  }
  
  
  //throw new MgRuntimeException(L"c_RestHandler_OData::Execute",__LINE__, __WFILE__, NULL, L"Undefined Resource! Unable to map URL to Resource.", NULL);
  throw c_ExceptionHttpInternalError("c_RestHandler_OData::Execute : It shouldn't reach here!");

}

void c_RestHandler_OData::Execute_ServiceDocument( c_RestResponse& HttpResponse )
{
  Ptr<c_RestResult> result = HttpResponse.GetResult();
  
  c_OData_Interface::e_OdataFormatType response_format =  c_OData_Interface::Impl()->GetResponseFormatType(m_RestRequest);
  
  std::wstring response_mime;
  if( response_format == c_OData_Interface::e_OdataJson )    
    response_mime = L"application/json";
  else
    response_mime = L"application/xml";
    
  
  std::ostringstream ostr;
  c_OData_Interface::Impl()->CreateServiceDocument(c_RestConfig::GetInstance(),ostr,m_RestRequest->GetServiceURI(),response_format);
  

  Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());
  
  result->SetResultObject(value, response_mime );
}

void c_RestHandler_OData::Execute_ServiceMetadata( c_RestResponse& HttpResponse )
{
  Ptr<c_RestResult> result = HttpResponse.GetResult();


  std::ostringstream ostr;
  c_OData_Interface::Impl()->CreateServiceMetadata(c_RestConfig::GetInstance(),ostr,m_RestRequest->GetServiceURI());


  Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());

  result->SetResultObject(value, L"application/xml");
}


// Execute_GET_OData .. creates response for HTTP GET on resource defined with "ResourceUriTag(KeysString)" e,g, Parcel(1)
// rest/odata.svc/Parcel(2) or ../Parcele(id=2)
void c_RestHandler_OData::Execute_GET_OData(c_RestResponse& HttpResponse,const std::wstring& ResourceUriTag,const std::wstring& KeysString)
{
  c_OData_Interface::e_OdataFormatType response_format =  c_OData_Interface::Impl()->GetResponseFormatType(m_RestRequest);
  std::wstring response_mime;
  if( response_format == c_OData_Interface::e_OdataJson )    
    response_mime = L"application/json";
  else
    response_mime = L"application/xml";
    
    
  Ptr<c_RestResult> result = HttpResponse.GetResult();
  m_RestRequest->m_CfgDataResource = c_RestMapNameToResourceId::GetDataResource(ResourceUriTag);

  if( !m_RestRequest->m_CfgDataResource )
  {    
    //throw new MgRuntimeException(L"c_RestHandler_OData::Execute",__LINE__, __WFILE__, NULL, L"Undefined Resource! Unable to map URL to Resource.", NULL);
    std::string errmsg; errmsg.reserve(1024);
    errmsg = "Undefined Resource! Unable to map URL segment '";
    std::string tmpstr;
    Poco::UnicodeConverter::toUTF8(ResourceUriTag,tmpstr);
    errmsg.append(tmpstr);
    errmsg.append("' to Resource.");
    throw c_ExceptionHttpNotFound(errmsg);
  }

  const c_CfgRepOdata* representation = (c_CfgRepOdata*)m_RestRequest->m_CfgDataResource->FindRepresentation(c_CfgRepresentation::e_OData);
  if( !representation )
  {
    //throw new MgRuntimeException(L"c_RestHandler_OData::Execute",__LINE__, __WFILE__, NULL, L"Not existing Representation! Resource doesn't support OData Representation.", NULL);
    std::string errmsg; errmsg.reserve(1024);
    errmsg = "No OData Representation! Resource '";
    std::string tmpstr;
    Poco::UnicodeConverter::toUTF8(ResourceUriTag,tmpstr);
    errmsg.append(tmpstr);
    errmsg.append("' doesn't support OData representation.");
    throw c_ExceptionHttpNotFound(errmsg);
  }
  
  m_RestRequest->m_DataClassDef = c_RestFetchSource::Fetch_MgClassDefinition(m_RestRequest->m_CfgDataResource->m_DataSource);
  if( !m_RestRequest->m_DataClassDef )
  {
    //throw new MgRuntimeException(L"c_RestHandler_OData::Execute",__LINE__, __WFILE__, NULL, L"Not existing Representation! Resource doesn't support OData Representation.", NULL);
    std::string errmsg; errmsg.reserve(1024);
    errmsg = "No Resource Class Definition! Unable to get Class Definition for Resource '";
    std::string tmpstr;
    Poco::UnicodeConverter::toUTF8(ResourceUriTag,tmpstr);
    errmsg.append(tmpstr);
    errmsg.append("'");
    throw c_ExceptionHttpInternalError(errmsg);
  }
   
  // Create Filter String
  Ptr<MgFeatureQueryOptions> query_options = new MgFeatureQueryOptions(); 
  
  CreateQueryOptions(m_RestRequest,KeysString,query_options);
  
  // Execute Fetch Data and Get Data Reader
  Ptr<c_RestDataReader> reader = c_RestFetchSource::FetchSource(m_RestRequest->m_CfgDataResource->m_DataSource,query_options);
  
  
  STRING nextparam;
  Ptr<c_RestUriPathParam> path_params = m_RestRequest->GetUriPathParameters();
  if( path_params->NextParameter() )
  {
    nextparam = path_params->GetCurrentParameterName();
  }
  
  
  
  // If $count then calculate count and return it
  if( KeysString.length() )
  {
    std::wstring one_prop_name; // if this will be set it means to return just value of one property
    bool isvalue = false; // could be that only raw value of one property is requested
  // request is for on entry not for collection
    if( nextparam.length() > 0 )
    {
      one_prop_name = nextparam;
      // it can be property name of class
      // or it can navigation property or complex property
      
      //it can be $links
      if( Poco::icompare(nextparam,L"$links") == 0)
      {
        throw c_ExceptionHttpBadRequest("System option '$links' is not supported!");
      }
      
      // for now i 
      Ptr<MgPropertyDefinitionCollection> props =  m_RestRequest->m_DataClassDef->GetProperties();
      if( !props->Contains(nextparam) )
      {
        std::string u8;
        Poco::UnicodeConverter::toUTF8(nextparam,u8);
        std::string errstr = "Entity property not found '";
        errstr.append(u8);
        errstr.append("!");
        throw c_ExceptionHttpBadRequest(errstr);
      }
      else
      {
      // now it can be $value
        if( path_params->NextParameter() )
        {
          nextparam = path_params->GetCurrentParameterName();
          //it can be $links
          if( Poco::icompare(nextparam,L"$value") )
          {
            std::string u8;
            Poco::UnicodeConverter::toUTF8(nextparam,u8);
            std::string errstr = "Unrecognized URI path segment '";
            errstr.append(u8);
            errstr.append("!");
            throw c_ExceptionHttpBadRequest(errstr);
            
          }   
          else
          {
            isvalue = true;
          } 
        }
      }
    }
    
    if( (one_prop_name.length() > 0) && isvalue)
    {
      if( reader->ReadNext() )
      {      
        std::string prop_val;
        if( reader->IsNull(one_prop_name))
          prop_val = "null";
        else
          c_OData_Interface::Impl()->GetPropertyValueAsOdataLiteral_URI(reader,one_prop_name,prop_val);
        Ptr<c_RestPrimitiveValue> retvalue = new c_RestPrimitiveValue(prop_val.c_str());
        result->SetResultObject(retvalue, L"text/plain; charset=UTF-8");  
      }
      else
      {
        throw c_ExceptionHttpBadRequest("Invalid key values ! Feature Reader is empy.");
      }
    }
    else
    {
      std::ostringstream ostr;
      c_OData_Interface::Impl()->Reader2AtomEntry(m_RestRequest,representation,reader,ostr,response_format,one_prop_name,isvalue);  
      Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());
      result->SetResultObject(value, response_mime);
    }
  }
  else
  {
  // it is collection
    if( nextparam.length() > 0 )
    {
      if( Poco::icompare(nextparam,L"$count") == 0)
      {
        int count=0;
        while(reader->ReadNext())
        {
          count++;
        }
        
        Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(count);
        result->SetResultObject(value, L"text/plain");
      }
      else
      {
        throw c_ExceptionHttpBadRequest("Invalid path segment! Only allowed path segment after collection is '$count'.");
      }
    }
    else
    {
      // Convert data to atom or json
      // check if only one property to return or multiple
      std::ostringstream ostr;
      c_OData_Interface::Impl()->Reader2AtomCollection(m_RestRequest,representation,reader,ostr,response_format);  
      Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());
      result->SetResultObject(value, response_mime);
    }
  }
  
  
  
  
}

void c_RestHandler_OData::CreateQueryOptions(c_RestRequest* RestRequest,const std::wstring& KeysString,MgFeatureQueryOptions* QueryOptions)
{
  Ptr<c_RestUriRequestParam> query_params = RestRequest->GetRequestParam();
  
  std::wstring filter;
  filter.reserve(512);
  if( KeysString.length() > 0 )
  {
    Ptr<MgPropertyCollection> props = new MgPropertyCollection();
    std::wstring singleval;
    c_OData_Interface::Impl()->ParseKeyValueString(KeysString,RestRequest->m_DataClassDef,props,singleval);
    
    c_FdoFilter::CreateFdoFilterString(props,filter);
     
    QueryOptions->SetFilter(filter);   
    
    return;
  }
 
  STRING param_odata_filter = query_params->GetParameterValue(L"$filter");
  if( param_odata_filter.length() > 0  )
  {
    STRING fdo_filter;fdo_filter.reserve(param_odata_filter.length()+32);
    c_Odata_FilterParser::ODataFilter2FdoFilter(param_odata_filter,fdo_filter);;
    QueryOptions->SetFilter(fdo_filter);   
  }
  
  
  // check for $orderby
  STRING param_ordreby = query_params->GetParameterValue(L"$orderby");
  if( param_ordreby.length() > 0  )
  {
    //
    std::string orderby_u8;
    Poco::UnicodeConverter::toUTF8(param_ordreby,orderby_u8);
    
    bool isasc=false,isdesc=false;
    
    Ptr<MgStringCollection> order_properties = new MgStringCollection();
    
    // parse order by
    Poco::StringTokenizer orderby_tokenizer(orderby_u8,",",Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
    
    
    Poco::StringTokenizer::Iterator iter = orderby_tokenizer.begin();
    while( iter != orderby_tokenizer.end() )
    {
      if( iter->length() > 0)
      {
        size_t npos_space = iter->find(' ');
        if( npos_space == std::string::npos)
        {
          std::wstring u16;  
          Poco::UnicodeConverter::toUTF16(*iter,u16);
          order_properties->Add(u16)  ;
          
        }
        else
        {
          std::string o_name = iter->substr(0,npos_space);
          std::string o_dir;
          if( npos_space < (iter->length()-1) ) o_dir = iter->substr(npos_space+1,iter->length()-2);
          
          if( Poco::icompare(o_dir,"asc") == 0 ) { isasc=true; }
          else
          {
            if( Poco::icompare(o_dir,"desc") == 0 ) { isdesc=true; }
            else
            {
              throw c_ExceptionHttpBadRequest("Invalid Ordering direction option in $orderby! Expecting 'asc' or 'desc'.");
            }
          }
          
          std::wstring u16;  
          Poco::UnicodeConverter::toUTF16(o_name,u16);
          order_properties->Add(u16)  ;
        }
        iter++;
      }
    }
    
    if( order_properties->GetCount() > 0)
    {
      if( isdesc && !isasc )  // if descending was set andn asceding wasnt explicitly set use descending
        QueryOptions->SetOrderingFilter(order_properties,MgOrderingOption::Descending);
      else
        QueryOptions->SetOrderingFilter(order_properties,MgOrderingOption::Ascending);
      
    }
    
    //STRING fdo_filter;fdo_filter.reserve(param_odata_filter.length()+32);
    //c_Odata_FilterParser::ODataFilter2FdoFilter(param_odata_filter,fdo_filter);;
    //QueryOptions->SetFilter(fdo_filter);   
  }
}