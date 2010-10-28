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


#include "c_RestHandler_EsriGS.h"
#include "c_FdoSelectFeatures.h"
#include "c_RestResult.h"
#include "c_OData_Interface.h"

#include <sstream>
#include "Services/FeatureQueryOptions.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/StringTokenizer.h"
#include "c_FdoFilter.h"
#include "c_Odata_FilterParser.h"
#include "c_RestHandler_Data.h"
#include "c_EsriGS_Interface.h"


c_RestHandler* c_RestHandler_EsriGS::CreateObject( c_RestRequest* Request )
{
  return new c_RestHandler_EsriGS(Request); 
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
c_RestHandler_EsriGS::c_RestHandler_EsriGS(c_RestRequest *Request)
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
void c_RestHandler_EsriGS::Execute(c_RestResponse& HttpResponse)
{
  Ptr<c_RestResult> hResult = HttpResponse.GetResult();
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest->GetRequestParam();
  MgStringPropertyCollection* coll_params = query_params->GetParameters();
  
  if( coll_params->Contains(L"callback") )
  {
    STRING scall = coll_params->GetValue(L"callback");
    std::string strcallback;
    Poco::UnicodeConverter::toUTF8(scall,strcallback);
    HttpResponse.SetJsonpCallbackName(strcallback);
  }
   
  // Now parse resource path part of URI to see which resource is requested
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest->GetUriPathParameters();
  if( !path_params->NextSegment() )
  {
    
    return Execute_CatalogDocument(HttpResponse);
  }
 
   
  // now URI can point to folder or to one of servers
  c_RestConfig* georest_cfg = c_RestConfig::GetInstance();
  c_CfgEsriGS_Catalog* esrigs_catalog = (c_CfgEsriGS_Catalog*)georest_cfg->FindService(L"Esri.GS");
  if( !esrigs_catalog )
  {
    throw c_ExceptionHttpBadRequest("c_RestHandler_EsriGS::Execute : Esri.GS service is not installed!");
  }
   
  c_CfgEsriGS_Folder * esrigs_folder=NULL;
  c_CfgEsriGS_Server * esrigs_server=NULL;
  if( !c_EsriGS_Interface::Impl()->MatchURI(esrigs_catalog->GetRootFolder(),path_params,esrigs_folder,esrigs_server) )
  {
    throw c_ExceptionHttpBadRequest("c_RestHandler_EsriGS::Execute : Unable to match URI to service or folder!");
  }
  
  
  if( esrigs_folder && !esrigs_server )
  { 
    // It is folder
    return Execute_Folder(HttpResponse,esrigs_folder);
  }
    
  if( esrigs_folder && esrigs_server )
  { 
    // It is server
    return Execute_Server(HttpResponse,esrigs_folder,esrigs_server);
  }
  
  //throw new MgRuntimeException(L"c_RestHandler_EsriGS::Execute",__LINE__, __WFILE__, NULL, L"Undefined Resource! Unable to map URL to Resource.", NULL);
  throw c_ExceptionHttpInternalError("c_RestHandler_EsriGS::Execute : No match for URI. URI must match folder or server!");

}

void c_RestHandler_EsriGS::Execute_CatalogDocument( c_RestResponse& HttpResponse )
{
  Ptr<c_RestResult> result = HttpResponse.GetResult();


  c_EsriGS_Interface::e_FormatType response_format =  c_EsriGS_Interface::Impl()->GetResponseFormatType(m_RestRequest);

  std::wstring response_mime;
  if( response_format == c_EsriGS_Interface::e_Json || (response_format == c_EsriGS_Interface::e_PrettyJson))    
    response_mime = L"application/json";
  else
    response_mime = L"text/html";


  std::ostringstream ostr;
  c_EsriGS_Interface::Impl()->CreateCatalogDocument(c_RestConfig::GetInstance(),ostr,m_RestRequest->GetServiceURI(),response_format);


  
  Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());

  result->SetResultObject(value, L"text/plain"); // response_mime );
}


void c_RestHandler_EsriGS::Execute_ServiceDocument( c_RestResponse& HttpResponse )
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

void c_RestHandler_EsriGS::Execute_ServiceMetadata( c_RestResponse& HttpResponse )
{
  Ptr<c_RestResult> result = HttpResponse.GetResult();


  std::ostringstream ostr;
  c_OData_Interface::Impl()->CreateServiceMetadata(c_RestConfig::GetInstance(),ostr,m_RestRequest->GetServiceURI());


  Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());

  result->SetResultObject(value, L"application/xml");
}


// Execute_GET_OData .. creates response for HTTP GET on resource defined with "ResourceUriTag(KeysString)" e,g, Parcel(1)
// rest/odata.svc/Parcel(2) or ../Parcele(id=2)
void c_RestHandler_EsriGS::Execute_GET_OData(c_RestResponse& HttpResponse,const std::wstring& ResourceUriTag,const std::wstring& KeysString)
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
    //throw new MgRuntimeException(L"c_RestHandler_EsriGS::Execute",__LINE__, __WFILE__, NULL, L"Undefined Resource! Unable to map URL to Resource.", NULL);
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
    //throw new MgRuntimeException(L"c_RestHandler_EsriGS::Execute",__LINE__, __WFILE__, NULL, L"Not existing Representation! Resource doesn't support OData Representation.", NULL);
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
    //throw new MgRuntimeException(L"c_RestHandler_EsriGS::Execute",__LINE__, __WFILE__, NULL, L"Not existing Representation! Resource doesn't support OData Representation.", NULL);
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
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest->GetUriPathParameters();
  if( path_params->NextSegment() )
  {
    nextparam = path_params->GetCurrentSegmentName();
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
        if( path_params->NextSegment() )
        {
          nextparam = path_params->GetCurrentSegmentName();
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

void c_RestHandler_EsriGS::CreateQueryOptions(c_RestRequest* RestRequest,const std::wstring& KeysString,MgFeatureQueryOptions* QueryOptions)
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
  
  if( query_params ->ContainsParameter(L"bbox") )
  {
    STRING paramval = query_params->GetParameterValue(L"BBOX");
    STRING bbox_filterstr;
    c_RestHandler_Data::CreateFilter_BBOX(paramval,RestRequest->m_DataClassDef,QueryOptions,bbox_filterstr);    

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

void c_RestHandler_EsriGS::Execute_Folder( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder )
{
  Ptr<c_RestResult> result = HttpResponse.GetResult();


  c_EsriGS_Interface::e_FormatType response_format =  c_EsriGS_Interface::Impl()->GetResponseFormatType(m_RestRequest);

  std::wstring response_mime;
  if( response_format == c_EsriGS_Interface::e_Json || (response_format == c_EsriGS_Interface::e_PrettyJson))    
    response_mime = L"application/json";
  else
    response_mime = L"text/html";


  std::ostringstream ostr;
  bool prettyjson = response_format == c_EsriGS_Interface::e_PrettyJson;
  esrigs_folder->WriteToJson(ostr,prettyjson);
  
  Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());

  result->SetResultObject(value, response_mime );
}

void c_RestHandler_EsriGS::Execute_Server( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_Server * esrigs_server)
{
  Ptr<c_RestUriPathSegment> path_segments = m_RestRequest->GetUriPathParameters();
  if( !path_segments->NextSegment() )
  {
    Ptr<c_RestResult> result = HttpResponse.GetResult();

    c_EsriGS_Interface::e_FormatType response_format =  c_EsriGS_Interface::Impl()->GetResponseFormatType(m_RestRequest);

    std::wstring response_mime;
    if( response_format == c_EsriGS_Interface::e_Json || (response_format == c_EsriGS_Interface::e_PrettyJson))    
      response_mime = L"application/json";
    else
      response_mime = L"text/html";


    std::ostringstream ostr;
    bool prettyjson = response_format == c_EsriGS_Interface::e_PrettyJson;
    esrigs_server->WriteToJson(ostr,prettyjson);

    Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ostr.str());

    result->SetResultObject(value, response_mime );
    
    return;
    
  }
  
  STRING nextparam = path_segments->GetCurrentSegmentName();
  switch(esrigs_server->GetType())
  {
    case c_CfgEsriGS_Server::e_MapServer:
    {
      c_CfgEsriGS_MapServer* mapserver = (c_CfgEsriGS_MapServer*)esrigs_server;
      Execute_MapServer(HttpResponse,esrigs_folder,mapserver);
      return;
    }
    break;
  }
  
  
  
  throw c_ExceptionHttpBadRequest("c_RestHandler_EsriGS::Execute_Server invalid request");
}

void c_RestHandler_EsriGS::Execute_MapServer( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_MapServer * esrigs_server)
{
  Ptr<c_RestUriPathSegment> path_segments = m_RestRequest->GetUriPathParameters();
  
  STRING nextparam = path_segments->GetCurrentSegmentName();
  // check for operation
  if( nextparam.compare(L"export") == 0)
  {
    if( m_RestRequest->GetHttpMethod() == c_RestUri::e_HttpMethod_Get )
    {
      Execute_MapServer_ExportMap(HttpResponse,esrigs_folder,esrigs_server);
      return;
    }
    else
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer::Execute_MapServer: Only GET method is allowed on ExportMap Operation!");
    }
  }
  
  if( nextparam.compare(L"identify") == 0)
  {
    if( m_RestRequest->GetHttpMethod() == c_RestUri::e_HttpMethod_Get )
    {
      Execute_MapServer_Identify(HttpResponse,esrigs_folder,esrigs_server);
      return;
    }
    else
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer::Execute_MapServer: Only GET method is allowed on ExportMap Operation!");
    }
  }
  


  throw c_ExceptionHttpBadRequest("Execute_MapServer::Execute_Server invalid request");
}

void c_RestHandler_EsriGS::Execute_MapServer_ExportMap( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_MapServer * esrigs_server)
{
  Ptr<c_RestUriPathSegment> path_segments = m_RestRequest->GetUriPathParameters();
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  MgStringPropertyCollection* coll_params = query_params->GetParameters();
  
  STRING nextparam = path_segments->GetCurrentSegmentName();
  // check for operation
  if( m_RestRequest->GetHttpMethod() != c_RestUri::e_HttpMethod_Get )
  {
    throw c_ExceptionHttpBadRequest("Execute_MapServer::Execute_MapServer: Only GET method is allowed on ExportMap Operation!");
  }
  
  // get format
  STRING qparam_f = coll_params->GetValue(L"f");  
  if( qparam_f.compare(L"image")  )
  {
    throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Unknown response format! Supported format is 'image'");
  }
  
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(esrigs_server->m_MapGuideUsername,esrigs_server->m_MapGuidePassword
                                                                ,esrigs_server->m_MapGuideServerIP,esrigs_server->m_MapGuideServerPort);    

  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);
    
    
  
  Ptr<MgSelection> selection;
  
  STRING sessionid;
  Ptr<MgSite> mgSite = mgsiteconn->GetSite();
  sessionid = mgSite->CreateSession();
  //m_userInfo->SetMgSessionId( sessionid );

  Ptr<MgUserInformation> userinfo =  mgsiteconn->GetUserInfo();
  userinfo->SetMgSessionId( sessionid );
  
  Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(mgsiteconn->CreateService(MgServiceType::ResourceService));
  
  //-------------------------------------------------------
  // Create a new Map from MapDefinition in a Session
  //-------------------------------------------------------  
  Ptr<MgResourceIdentifier> resid_mapdef = new MgResourceIdentifier(esrigs_server->m_MapGuideMapDef);
  STRING map_resid;
  STRING mapname = L"KINGREST";
  map_resid = L"Session:" + sessionid + L"//" + mapname + L"." + MgResourceType::Map;
  
  
  Ptr<MgMap> map;
  map = new MgMap();
  
  
  map->Create(resourceService, resid_mapdef, mapname );  
  
  Ptr<MgLayerCollection> mglayers = map->GetLayers();
  int count = mglayers->GetCount();
  // Save the MgMap state
  
  Ptr<MgResourceIdentifier> resid_map = new MgResourceIdentifier(map_resid);
  map->Save(resourceService,resid_map);

  Ptr<MgEnvelope> extent = map->GetDataExtent();
  Ptr<MgCoordinate> ext_xy1 = extent->GetLowerLeftCoordinate();
  Ptr<MgCoordinate> ext_xy2 = extent->GetUpperRightCoordinate();
  
  selection = new MgSelection(map);
  selection->Save(resourceService, mapname);
    
    
  // get extents to retrieve
  double x1=0.0,y1=0.0,x2=0.0,y2=0.0;
  STRING qparam_bbox = coll_params->Contains(L"bbox") ? coll_params->GetValue(L"bbox") : L"";  
  if( qparam_bbox.length()==0  )
  {
    throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Missing 'bbox' query parameter!");
  }
  else
  {
    
    if( swscanf(qparam_bbox.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) != 4 )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'bbox' query parameter!");
    }
  }
  
  // get size of image and dpi
  int dwidth=400;
  int dheight=400;
  int dpi = 96;
  
  STRING qparam_size = coll_params->Contains(L"size") ? coll_params->GetValue(L"size") : L"";  
  if( qparam_size.length() > 0  )
  {
    if( swscanf(qparam_size.c_str(),L"%d,%d",&dwidth,&dheight) != 2 )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'size' query parameter!");
    }
  }
  STRING qparam_dpi = coll_params->Contains(L"dpi") ? coll_params->GetValue(L"dpi") : L"";  
  if( qparam_dpi.length() > 0  )
  {
    if( swscanf(qparam_dpi.c_str(),L"%d",&dpi) != 1 )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'dpi' query parameter!");
    }
  }

  Ptr<MgPropertyCollection> mapviewcommands = new MgPropertyCollection();
  STRING strval;
  Ptr<MgStringProperty> prop;
  {
    MgUtil::Int32ToString(dwidth,strval);
    prop = new MgStringProperty(L"SETDISPLAYWIDTH",strval);
    mapviewcommands->Add(prop);
  }
  {
    MgUtil::Int32ToString(dheight,strval);
    prop = new MgStringProperty(L"SETDISPLAYHEIGHT",strval);
    mapviewcommands->Add(prop);
  }
  {
    MgUtil::Int32ToString(dpi,strval);
    prop = new MgStringProperty(L"SETDISPLAYDPI",strval);
    mapviewcommands->Add(prop);
  }
  {
    double cx,cy;
    
    cx = (x1+x2) / 2.0;
    cy = (y1+y2) / 2.0;
    
    MgUtil::DoubleToString(cx,strval);
    prop = new MgStringProperty(L"SETVIEWCENTERX",strval);
    mapviewcommands->Add(prop);

    MgUtil::DoubleToString(cy,strval);
    prop = new MgStringProperty(L"SETVIEWCENTERY",strval);
    mapviewcommands->Add(prop);
    
    double scale;
    double mcsH = fabs(y2-y1);
    double mcsW = fabs(x2-x1); // xy2->GetX() - xy1->GetX();

    //mcsH = mcsH * zoomfactor;
    //mcsW = mcsW * zoomfactor;

    double metersperpixel  = 0.0254 / dpi;

    if (dheight * mcsW > dwidth * mcsH)
      scale = mcsW * map->GetMetersPerUnit() / (dwidth * metersperpixel); // width-limited
    else
      scale = mcsH * map->GetMetersPerUnit() / (dheight * metersperpixel); // height-limited
  

    MgUtil::DoubleToString(scale,strval);
    prop = new MgStringProperty(L"SETVIEWSCALE",strval);
    mapviewcommands->Add(prop);
  }

  STRING imageformat = MgImageFormats::Png8;  
  STRING qparam_imageformat = coll_params->Contains(L"format") ? coll_params->GetValue(L"format") : L"";  
  if( qparam_imageformat.length() > 0 )
  {
  
    STRING format = MgUtil::ToUpper(qparam_imageformat);
    if( format == L"PNG" ) 
      imageformat = MgImageFormats::Png;  
    else 
    {
      if( format == L"PNG24" ) imageformat = MgImageFormats::Png;  
      else 
      {
        if( format == L"PNG8" ) imageformat = MgImageFormats::Png8;  
        else 
        {
          if( format == L"JPG" ) imageformat = MgImageFormats::Jpeg;  
          else 
          {
            if( format == L"TIF" ) imageformat = MgImageFormats::Tiff;  
            else if( format == L"TIFF" ) imageformat = MgImageFormats::Tiff;  
          }
        }
      }
    }
  }
 
 
   
  c_RestFetchImage::ApplyMapViewCommands(map, mapviewcommands);
  
  
  // check for show/hide layers
  if( coll_params->Contains(L"layers") )
  {
    std::wstring attr_layers = coll_params->GetValue(L"layers");
    
    std::vector<int> lyids;
    e_LayersCmd lycmd;
    lycmd = ParseQueryLayers(attr_layers,lyids);
    if( lycmd == e_Invalid )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'layers' query value!");
    }
    Ptr<MgLayerCollection> map_layers = map->GetLayers();
    if( lycmd==e_Show || lycmd==e_Hide )
    {
      // because commands show and hide sets which are visible and which not
      // need to set other layers to opposite values - so when show or hide will be applied 
      // for selected layers only those will be shown or hidden
      t_c_CfgEsriGS_MS_LayerVector::const_iterator iter =  esrigs_server->m_Layers.begin();
      while(iter != esrigs_server->m_Layers.end())
      {
        int map_index = map_layers->IndexOf((*iter)->m_Name);
        if( map_index>=0 )
        {
          Ptr<MgLayerBase> layer_base = map_layers->GetItem(map_index);
          if( lycmd==e_Show )
            layer_base->SetVisible(false);  // has to be false for command show; first set all layers to unvisible and later show only ones set trough command 
          else
            layer_base->SetVisible(true);
        } 
        iter++;
      }
    }
    
    
    int size = lyids.size();
    std::vector<int>::iterator iterator = lyids.begin();
    
    while(iterator != lyids.end())
    {
      c_CfgEsriGS_MS_Layer* ms_layer = esrigs_server->FindLayer(*iterator);
      if( !ms_layer )
      {
        throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid id number in 'layers' query value!");
      }
      
      int map_index = map_layers->IndexOf(ms_layer->m_Name);
      if( map_index>=0 )
      {
        Ptr<MgLayerBase> layer_base = map_layers->GetItem(map_index);
        if( lycmd==e_Show || lycmd==e_Include )
          layer_base->SetVisible(true);
        else
          layer_base->SetVisible(false);
      }
      iterator++;
    }
    
    
   
  }
  
  
  // Create Proxy Rendering Service instance
  Ptr<MgRenderingService> rendering_service = (MgRenderingService*)(mgsiteconn->CreateService(MgServiceType::RenderingService));

  // Call the C++ API
  Ptr<MgByteReader> mapimage = rendering_service->RenderDynamicOverlay(map, selection, imageformat, true);
  
  //Ptr<MgByteReader> mapimage = c_RestFetchImage::GetDynamicMapOverlayImage_ViewCommands(mgsiteconn,mapname, imageformat , true,mapviewcommands);
  

  // Set the result
  Ptr<c_RestResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(mapimage, mapimage->GetMimeType());
  
  return;
}//end of c_RestHandler_EsriGS::Execute_MapServer_ExportMap

void c_RestHandler_EsriGS::Execute_MapServer_Identify( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_MapServer * esrigs_server)
{
  Ptr<c_RestUriPathSegment> path_segments = m_RestRequest->GetUriPathParameters();
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  MgStringPropertyCollection* coll_params = query_params->GetParameters();

  STRING nextparam = path_segments->GetCurrentSegmentName();
  // check for operation
  if( m_RestRequest->GetHttpMethod() != c_RestUri::e_HttpMethod_Get )
  {
    throw c_ExceptionHttpBadRequest("Execute_MapServer::Execute_MapServer: Only GET method is allowed on ExportMap Operation!");
  }

  // get format
  STRING qparam_f = coll_params->GetValue(L"f");  
  if( qparam_f.compare(L"image")  )
  {
    throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Unknown response format! Supported format is 'image'");
  }

  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(esrigs_server->m_MapGuideUsername,esrigs_server->m_MapGuidePassword
    ,esrigs_server->m_MapGuideServerIP,esrigs_server->m_MapGuideServerPort);    

  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);



  Ptr<MgSelection> selection;

  STRING sessionid;
  Ptr<MgSite> mgSite = mgsiteconn->GetSite();
  sessionid = mgSite->CreateSession();
  //m_userInfo->SetMgSessionId( sessionid );

  Ptr<MgUserInformation> userinfo =  mgsiteconn->GetUserInfo();
  userinfo->SetMgSessionId( sessionid );

  Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(mgsiteconn->CreateService(MgServiceType::ResourceService));

  //-------------------------------------------------------
  // Create a new Map from MapDefinition in a Session
  //-------------------------------------------------------  
  Ptr<MgResourceIdentifier> resid_mapdef = new MgResourceIdentifier(esrigs_server->m_MapGuideMapDef);
  STRING map_resid;
  STRING mapname = L"KINGREST";
  map_resid = L"Session:" + sessionid + L"//" + mapname + L"." + MgResourceType::Map;


  Ptr<MgMap> map;
  map = new MgMap();


  map->Create(resourceService, resid_mapdef, mapname );  

  Ptr<MgLayerCollection> mglayers = map->GetLayers();
  int count = mglayers->GetCount();
  // Save the MgMap state

  Ptr<MgResourceIdentifier> resid_map = new MgResourceIdentifier(map_resid);
  map->Save(resourceService,resid_map);

  Ptr<MgEnvelope> extent = map->GetDataExtent();
  Ptr<MgCoordinate> ext_xy1 = extent->GetLowerLeftCoordinate();
  Ptr<MgCoordinate> ext_xy2 = extent->GetUpperRightCoordinate();

  selection = new MgSelection(map);
  selection->Save(resourceService, mapname);


  // get extents to retrieve
  double x1=0.0,y1=0.0,x2=0.0,y2=0.0;
  STRING qparam_bbox = coll_params->Contains(L"bbox") ? coll_params->GetValue(L"bbox") : L"";  
  if( qparam_bbox.length()==0  )
  {
    throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Missing 'bbox' query parameter!");
  }
  else
  {

    if( swscanf(qparam_bbox.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) != 4 )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'bbox' query parameter!");
    }
  }

  // get size of image and dpi
  int dwidth=400;
  int dheight=400;
  int dpi = 96;

  STRING qparam_size = coll_params->Contains(L"size") ? coll_params->GetValue(L"size") : L"";  
  if( qparam_size.length() > 0  )
  {
    if( swscanf(qparam_size.c_str(),L"%d,%d",&dwidth,&dheight) != 2 )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'size' query parameter!");
    }
  }
  STRING qparam_dpi = coll_params->Contains(L"dpi") ? coll_params->GetValue(L"dpi") : L"";  
  if( qparam_dpi.length() > 0  )
  {
    if( swscanf(qparam_dpi.c_str(),L"%d",&dpi) != 1 )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'dpi' query parameter!");
    }
  }

  Ptr<MgPropertyCollection> mapviewcommands = new MgPropertyCollection();
  STRING strval;
  Ptr<MgStringProperty> prop;
  {
    MgUtil::Int32ToString(dwidth,strval);
    prop = new MgStringProperty(L"SETDISPLAYWIDTH",strval);
    mapviewcommands->Add(prop);
  }
  {
    MgUtil::Int32ToString(dheight,strval);
    prop = new MgStringProperty(L"SETDISPLAYHEIGHT",strval);
    mapviewcommands->Add(prop);
  }
  {
    MgUtil::Int32ToString(dpi,strval);
    prop = new MgStringProperty(L"SETDISPLAYDPI",strval);
    mapviewcommands->Add(prop);
  }
  {
    double cx,cy;

    cx = (x1+x2) / 2.0;
    cy = (y1+y2) / 2.0;

    MgUtil::DoubleToString(cx,strval);
    prop = new MgStringProperty(L"SETVIEWCENTERX",strval);
    mapviewcommands->Add(prop);

    MgUtil::DoubleToString(cy,strval);
    prop = new MgStringProperty(L"SETVIEWCENTERY",strval);
    mapviewcommands->Add(prop);

    double scale;
    double mcsH = fabs(y2-y1);
    double mcsW = fabs(x2-x1); // xy2->GetX() - xy1->GetX();

    //mcsH = mcsH * zoomfactor;
    //mcsW = mcsW * zoomfactor;

    double metersperpixel  = 0.0254 / dpi;

    if (dheight * mcsW > dwidth * mcsH)
      scale = mcsW * map->GetMetersPerUnit() / (dwidth * metersperpixel); // width-limited
    else
      scale = mcsH * map->GetMetersPerUnit() / (dheight * metersperpixel); // height-limited


    MgUtil::DoubleToString(scale,strval);
    prop = new MgStringProperty(L"SETVIEWSCALE",strval);
    mapviewcommands->Add(prop);
  }

  STRING imageformat = MgImageFormats::Png8;  
  STRING qparam_imageformat = coll_params->Contains(L"format") ? coll_params->GetValue(L"format") : L"";  
  if( qparam_imageformat.length() > 0 )
  {

    STRING format = MgUtil::ToUpper(qparam_imageformat);
    if( format == L"PNG" ) 
      imageformat = MgImageFormats::Png;  
    else 
    {
      if( format == L"PNG24" ) imageformat = MgImageFormats::Png;  
      else 
      {
        if( format == L"PNG8" ) imageformat = MgImageFormats::Png8;  
        else 
        {
          if( format == L"JPG" ) imageformat = MgImageFormats::Jpeg;  
          else 
          {
            if( format == L"TIF" ) imageformat = MgImageFormats::Tiff;  
            else if( format == L"TIFF" ) imageformat = MgImageFormats::Tiff;  
          }
        }
      }
    }
  }



  c_RestFetchImage::ApplyMapViewCommands(map, mapviewcommands);


  // check for show/hide layers
  if( coll_params->Contains(L"layers") )
  {
    std::wstring attr_layers = coll_params->GetValue(L"layers");

    std::vector<int> lyids;
    e_LayersCmd lycmd;
    lycmd = ParseQueryLayers(attr_layers,lyids);
    if( lycmd == e_Invalid )
    {
      throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid 'layers' query value!");
    }
    Ptr<MgLayerCollection> map_layers = map->GetLayers();
    if( lycmd==e_Show || lycmd==e_Hide )
    {
      // because commands show and hide sets which are visible and which not
      // need to set other layers to opposite values - so when show or hide will be applied 
      // for selected layers only those will be shown or hidden
      t_c_CfgEsriGS_MS_LayerVector::const_iterator iter =  esrigs_server->m_Layers.begin();
      while(iter != esrigs_server->m_Layers.end())
      {
        int map_index = map_layers->IndexOf((*iter)->m_Name);
        if( map_index>=0 )
        {
          Ptr<MgLayerBase> layer_base = map_layers->GetItem(map_index);
          if( lycmd==e_Show )
            layer_base->SetVisible(false);  // has to be false for command show; first set all layers to unvisible and later show only ones set trough command 
          else
            layer_base->SetVisible(true);
        } 
        iter++;
      }
    }


    int size = lyids.size();
    std::vector<int>::iterator iterator = lyids.begin();

    while(iterator != lyids.end())
    {
      c_CfgEsriGS_MS_Layer* ms_layer = esrigs_server->FindLayer(*iterator);
      if( !ms_layer )
      {
        throw c_ExceptionHttpBadRequest("Execute_MapServer_ExportMap: Invalid id number in 'layers' query value!");
      }

      int map_index = map_layers->IndexOf(ms_layer->m_Name);
      if( map_index>=0 )
      {
        Ptr<MgLayerBase> layer_base = map_layers->GetItem(map_index);
        if( lycmd==e_Show || lycmd==e_Include )
          layer_base->SetVisible(true);
        else
          layer_base->SetVisible(false);
      }
      iterator++;
    }



  }


  // Create Proxy Rendering Service instance
  Ptr<MgRenderingService> rendering_service = (MgRenderingService*)(mgsiteconn->CreateService(MgServiceType::RenderingService));

  // Call the C++ API
  Ptr<MgByteReader> mapimage = rendering_service->RenderDynamicOverlay(map, selection, imageformat, true);

  //Ptr<MgByteReader> mapimage = c_RestFetchImage::GetDynamicMapOverlayImage_ViewCommands(mgsiteconn,mapname, imageformat , true,mapviewcommands);


  // Set the result
  Ptr<c_RestResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(mapimage, mapimage->GetMimeType());

  return;
}//end of

c_RestHandler_EsriGS::e_LayersCmd c_RestHandler_EsriGS::ParseQueryLayers( std::wstring& StrLayers,std::vector<int>& LayerIds )
{
  e_LayersCmd lycmd;
  
  int cmdsize = 0;
  // show:  hide:  include: exclude:
  if( StrLayers.compare(0,5,L"show:") ==0 )
  {
    lycmd = e_Show;
    cmdsize = 5;   
  }
  else
  {
    if( StrLayers.compare(0,5,L"hide:") ==0 )
    {
      lycmd = e_Hide;
      cmdsize = 5;   
    }
    else
    {
      if( StrLayers.compare(0,8,L"include:") ==0 )
      {
        lycmd = e_Include;
        cmdsize = 8;   
      }
      else
      {
        if( StrLayers.compare(0,8,L"exclude:") ==0 )
        {
          lycmd = e_Exclude;
          cmdsize = 8;   
        }
        else
        {
          return e_Invalid;
        }
      }
    }
  }
  
  
  int len = StrLayers.length();
  if( cmdsize< len)
  {
    int lastid=-1;
    const wchar_t *sd = StrLayers.data();
    sd  += cmdsize;
    int cnt = cmdsize;
    while(cnt<len)
    {
      if( *sd == L',')
      {
        if( lastid >= 0)
        {
          LayerIds.push_back(lastid);
        }
      }
      else
      {
      
        if( *sd >= 48 && *sd <= 57 )
        {
          if( lastid >= 0 ) lastid = (lastid*10) + (*sd-48);
          else lastid = (*sd-48);
        }
        else
        {
          return e_Invalid;
        }
      }
      
      cnt++;
      sd++;
    }
    
    if( lastid >= 0 ) LayerIds.push_back(lastid);
  }
  
  
  return lycmd;
}