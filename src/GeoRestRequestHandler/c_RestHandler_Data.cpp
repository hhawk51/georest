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

#include "stdafx.h"
#include <json/json.h>
//#include "RestUriFilterParser.h"
#include "RestDefs.h"
#include "c_RestPrimitiveValue.h"
#include "c_RestUriPathParam.h"

#include "c_RestMapNameToResourceId.h"
#include "c_RestMgSiteConnection.h"

//#include "RestFetchData.h"
#include "RestConfigInclude.h"
#include "c_RestFetchImage.h"


#include "c_RestHandler_Data.h"





c_RestHandler* c_RestHandler_Data::CreateObject( c_RestRequest* Request )
{
  return new c_RestHandler_Data(Request); 
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
c_RestHandler_Data::c_RestHandler_Data(c_RestRequest *Request)
{
    InitializeCommonParameters(Request);
    
        
    Ptr<c_RestUriPathSegment> path_params = Request->GetUriPathParameters();
    
    
    //m_FormatType = L"";
    STRING  datatagname;
    
    
    if( path_params->NextSegment() )
    {
      datatagname = path_params->GetCurrentSegmentName();
      if( path_params->NextSegment() )
      {
        m_UriParam_DataFilter = path_params->GetCurrentSegmentName();            
      }
    }
    

    
    m_RestRequest->m_CfgDataResource = c_RestMapNameToResourceId::GetDataResource(datatagname);
    //m_RestRequest->m_DataLayerConfig = c_RestMapNameToResourceId::GetCfgDataForClass(m_RestRequest->m_DataClassName);
    
    if( m_RestRequest->m_CfgDataResource )
    {
           
    
      m_IsSitemapXml = false;
      if( wcsicmp(m_UriParam_DataFilter.c_str(),L"sitemap.xml") == 0)
      {
        m_RestRequest->m_CfgRepresentation = m_RestRequest->m_CfgDataResource->FindRepresentation(m_UriParam_DataFilter.c_str()); 
        //m_FormatType = L"xml";
        m_IsSitemapXml = true;
      }
      else
      {
        m_Filter = m_UriParam_DataFilter;
        STRING::size_type dotpos = m_UriParam_DataFilter.rfind(L".");
        if( dotpos != STRING::npos )
        {
          STRING::size_type len = m_UriParam_DataFilter.length();
          STRING typestr = m_UriParam_DataFilter.substr(dotpos,len-dotpos);
          
          m_RestRequest->m_CfgRepresentation = m_RestRequest->m_CfgDataResource->FindRepresentation(typestr.c_str());
          m_Filter = m_UriParam_DataFilter.substr(0,dotpos);
          
          
        }
      }
    }
    //m_FeatureSourceId = pathparams->GetParameterValue(D_REST_URI_SEGMENT_FDO_DATASOURCE);
    //m_RestRequest->m_DataClassName = pathparams->GetParameterValue(D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS);
    
    
}

    



/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// c_RestResponse
/// This contains the response (including c_RestResult and StatusCode) from the server.
/// </returns>
void c_RestHandler_Data::Execute(c_RestResponse& HttpResponse)
{
  Ptr<c_RestResult> hResult = HttpResponse.GetResult();
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest->GetRequestParam();

  


MG_TRY()
  
  if( !m_RestRequest->m_CfgDataResource )
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Undefined Resource! Unable to map URL to Resource. Check configuration file.", NULL);
  }
  if( !m_RestRequest->m_CfgRepresentation )
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Undefined Representation! Unable to match representation for the URL. Check configuration file.", NULL);
  }

// Now check if method is allowed on that representation
  
  std::wstring wsmethod = MgUtil::MultiByteToWideChar(m_RestRequest->GetHttpMethodString());
  if( m_RestRequest->m_CfgRepresentation->IsAccess(wsmethod.c_str(),NULL,NULL) != e_AccessOK)
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Method is not allowed on Representation!", NULL);
  }
  
  if( m_RestRequest->m_CfgRepresentation ) 
  {
    HttpResponse.SetContentType(m_RestRequest->m_CfgRepresentation->m_MimeType);
  }
  else
  {
    HttpResponse.SetContentType(MgMimeType::Xml);    
  }

    //
    // 1. URI for getting data about MAP
    // HttpMethod: GET URI: REST/FDO/DATASOURCE[Id]/CLASSNAME[]/FATURE[Filter]
    
// Check which Resource is requested
// 

  // Current Index should be on Map path parameter
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();
  
  switch( m_RestRequest->GetHttpMethod() )
  {
    case c_RestUri::e_HttpMethod_Get:
    {
    // now this Path Segment is last or there is a 
    // another segment with Image.Png, Image.Jpg or Extent      
      
      Execute_Get_Class_Feature(HttpResponse);
    }
    break;
       
    case c_RestUri::e_HttpMethod_Post:
    {
      // This one is creating new Map in Session by opening MapDefinition
      Execute_Post_Class_Feature(HttpResponse);      
    }
    break;
    
    case c_RestUri::e_HttpMethod_Put:
    {
      
      Execute_Put_Class_Feature(HttpResponse);      
    }
    break;
    
    case c_RestUri::e_HttpMethod_Delete:
      Execute_Delete_Class_Feature(HttpResponse);  
    break;
    
    default:
    break;
  }


  MG_CATCH(L"REST_Request_Hello.Execute")                                                      

    if (mgException != NULL)                                                  
    {                                                                         
      if (hResult != NULL)                                                  
      {                                                                      
        hResult->SetErrorInfo(m_RestRequest, mgException);                   
      }                                                                     
      (*mgException).AddRef();                                              
      mgException->Raise();                                                 
    } 


}

bool c_RestHandler_Data::ParseFilterQueryParam(const wstring& Param,int& Level,e_BoolOper& BoolOper,e_CompOper& CompOper,wstring& PropertyName)
{
  wstring item,rest;
  
  rest = Param;
  int underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;
  
  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);
  if( wcsicmp(item.c_str(),L"filter") != 0 ) return false;
  
  // now check for number of level
  underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;
  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);
  Level = _wtoi(item.c_str());
  
  // check for Bool oper
  underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;
  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);
  
  if( wcsicmp(item.c_str(),L"AND") == 0 )
  {
    BoolOper = e_AND;
  }
  else
  {
    if( wcsicmp(item.c_str(),L"OR") == 0 )
    {
      BoolOper = e_OR;
    }
    else
      return false;
  }
  
  // check for comp. operator
  underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;
  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);
  
  if( wcsicmp(item.c_str(),L"EQUAL") == 0 )
  {
    CompOper = e_EQUAL;
  }
  else
  {
    if( wcsicmp(item.c_str(),L"LIKE") == 0 )
    {
      CompOper = e_LIKE;
    }
    else
    {
      if( wcsicmp(item.c_str(),L"LIKERIGHT") == 0 )
      {
        CompOper = e_LIKERIGHT;
      }
      else
      {
        if( wcsicmp(item.c_str(),L"LIKELEFT") == 0 )
        {
          CompOper = e_LIKELEFT;
        }
        else
        {
          if( wcsicmp(item.c_str(),L"GTE") == 0 )
          {
            CompOper = e_GREATER_EQUAL;
          }
          else
          {
            if( wcsicmp(item.c_str(),L"GT") == 0 )
            {
              CompOper = e_GREATER;
            }
            else
            {
              if( wcsicmp(item.c_str(),L"LTE") == 0 )
              {
                CompOper = e_LESSER_EQUAL;
              }
              else
              {
                if( wcsicmp(item.c_str(),L"LT") == 0 )
                {
                  CompOper = e_LESSER;
                }
                else
                {
                  return false;
                }
              }
            }
          }
         
        }
      }
      
    }
    
  }
  
// now rest is property name
  PropertyName = rest;
  if( PropertyName.length() <= 0 ) return false;
  
  return true;
  
}


bool c_RestHandler_Data::CreateFilter_BBOX(CREFSTRING BBoxParamValue,MgClassDefinition* ClassDef,MgFeatureQueryOptions*qryOptions,REFSTRING FilterStr)
{
  bool isbbox=false;
  STRING bbox_paramval = BBoxParamValue;
  {
    
    Ptr<MgGeometry> buffergeom;
    double x1,y1,x2,y2;
    if( swscanf(bbox_paramval.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) == 4 )
    {
      isbbox=true;

    }
    else
    {
      // check if it is http url , then fetch that data and take those envelope as bbox

      if( wcsicmp(bbox_paramval.substr(0,7).c_str(),L"buffer(") == 0 )
      {
        bbox_paramval = bbox_paramval.substr(7,bbox_paramval.length()-8); // minus last ')'

        STRING::size_type iColon = bbox_paramval.find(',');
        if(iColon != STRING::npos) 
        {
          STRING::size_type len = bbox_paramval.length();
          STRING diststr = bbox_paramval.substr(0,iColon);
          double dist;
          if( swscanf(diststr.c_str(),L"%lf",&dist) == 1 )
          {


            STRING httpparamval = bbox_paramval.substr(iColon+1,len-iColon);

            if( wcsicmp(httpparamval.substr(0,5).c_str(),L"http:") == 0 )
            {
              buffergeom = c_RestFetchSource::FetchBuffer(httpparamval.c_str(),dist);
              isbbox=true;
            }
          }
        }
      }
      else
      { 
        if( wcsicmp(bbox_paramval.substr(0,5).c_str(),L"http:") == 0 )
        {
          c_RestFetchSource::FetchEnvelope(bbox_paramval.c_str(),x1,y1,x2,y2);
          isbbox=true;
        }
      }
    }
    if( isbbox )
    {
      // create geometry spatial filter
      Ptr<MgGeometry> filterGeometry;
      if( buffergeom.p )
      {
        filterGeometry = buffergeom;
      }
      else
      {
        // geomstr = POLYGON(x1 y1, x2 y1, x2 y2, x1 y2, x1 y1)
        wchar_t geomstr[1024];
        swprintf(geomstr,L"POLYGON((%.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf ))",x1,y1 ,x2,y1 ,x2,y2 ,x1,y2 ,x1,y1);
        MgWktReaderWriter wktReader;
        filterGeometry = wktReader.Read(geomstr);
      }
      STRING geometryname = ClassDef->GetDefaultGeometryPropertyName();


      if( geometryname.length() >0 )
      {
        //STRING geomstr = m_DataFilter.substr(5,m_DataFilter.length()-6);
        MgWktReaderWriter wktReader;
        STRING fgeomstr = wktReader.Write(filterGeometry);
        //filterGeometry->ToAwkt()

        if( filterGeometry.p != NULL )
        {
          //GEOMETRY ENVELOPEINTERSECTS GeomFromText(' POLYGON(())')
          FilterStr = geometryname + L" ENVELOPEINTERSECTS GeomFromText('" + fgeomstr + L"')";
          //FilterStr = geometryname + L" INTERSECTS GeomFromText('" + fgeomstr + L"')";
          INT32 selectionVariant = MgFeatureSpatialOperations::EnvelopeIntersects;
          if( qryOptions ) qryOptions->SetSpatialFilter(geometryname,filterGeometry,selectionVariant);
        }
      }
    }

  }
  
  return isbbox;
}//end of c_RestHandler_Data::CreateFilter_BBOX


void c_RestHandler_Data::CreateFilterString(MgClassDefinition* ClassDef,MgFeatureQueryOptions*qryOptions,REFSTRING FilterStr)
{
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();
  
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest->GetRequestParam();
  if( query_params ->ContainsParameter(L"filter") )
  {
    STRING paramval = query_params->GetParameterValue(L"filter");
    FilterStr=paramval;
    if( qryOptions ) qryOptions->SetFilter(paramval);
  
  }
  
  if( query_params ->ContainsParameter(L"orderfields") )
  {
    STRING paramval = query_params->GetParameterValue(L"orderfields");
    Ptr<MgStringCollection> ofields = MgStringCollection::ParseCollection(paramval,L",");
    
    STRING paramdir = query_params->GetParameterValue(L"orderdir");
    int direction;
    if( _wcsicmp(paramdir.c_str(), L"desc") == 0 )
    {
      direction = MgOrderingOption::Descending;
    }
    else
      direction = MgOrderingOption::Ascending;
      
    if( qryOptions ) qryOptions->SetOrderingFilter(ofields,direction);

  }
  
// beside filter string check for filter_ parameters  in URL
// filter arameter is constructed from 5 parts like :
// filter_n_booloper_operation_property
// where
// filter_ is fixed literal
// n.. is number which defines level of nested in filter
// booloper...is either AND or OR
// operation...is EQUAL, LIKE, LIKERIGHT, LIKELEFT, LT (LESS), GT (GREATER). LTE (LESSTHEN), GTE (GRAETERTHEN)
// property...is name of property to be filter by ( property LIKE 'upd%' )
//
// example: http://localhost/mgrest/rest/data/parcel/.html?filter_1_and_likeright_NAME=u
//
  wstring filter_urlparams;
  MgStringPropertyCollection* uri_params = query_params->GetParameters();
  int count = uri_params->GetCount();
  for(int ind=0;ind<count;ind++)
  {
    //STRING paramname = uri_params->GetName(ind);
    Ptr<MgStringProperty> strptr = uri_params->GetItem(ind);
    
    wstring name = strptr->GetName();
    wstring val = strptr->GetValue();
    
    int level; e_BoolOper booloper; e_CompOper compoper; wstring propname;
    if( ParseFilterQueryParam(name,level,booloper,compoper,propname) )
    {
      
    // add that to filter
      switch( compoper )
      {
        case e_LIKE:
          val = L"%" + val + L"%";
        break;  
        case e_LIKELEFT:
          val = L"%" + val;
        break;  
        case e_LIKERIGHT:
          val = val + L"%";
        break;  
      }
    
    // if property is string then I need to add ' ' to value string
      Ptr<MgPropertyDefinitionCollection> propdefcoll = ClassDef->GetProperties();
      Ptr<MgPropertyDefinition> propdef = propdefcoll->FindItem(propname);
      if( propdef.p )
      {
        if( propdef->GetPropertyType() == MgFeaturePropertyType::DataProperty )
        {
          MgDataPropertyDefinition *dataprop = (MgDataPropertyDefinition*)propdef.p;
          if( dataprop->GetDataType() == MgPropertyType::String )
          {
          // need to check if their is ' character
            basic_string <char>::size_type index = val.find('\'');
            while(index != string::npos )
            {
              val = val.replace(index,1,2,'\'');
              index = val.find('\'',index+2);
            }
            
            val = L"'" + val + L"'";            
          }
        }        
      }
    // combine with previous filter
      if( filter_urlparams.length() > 0 )
      {
        switch( booloper )
        {
          case e_AND:
            filter_urlparams = filter_urlparams + L" AND ";
          break;
          case e_OR:
            filter_urlparams = filter_urlparams + L" OR ";
          break;
        }
      }
    // add property name
      filter_urlparams += propname;
      
    // add an operator
      switch( compoper )
      {
        case e_EQUAL:
          filter_urlparams = filter_urlparams + L" = " + val;
        break;
        case e_LIKE:
        case e_LIKERIGHT:
        case e_LIKELEFT:
          filter_urlparams = filter_urlparams + L" LIKE " + val;
        break;
        case e_GREATER:
          filter_urlparams = filter_urlparams + L" > " + val;
        break;
        case e_GREATER_EQUAL:
          filter_urlparams = filter_urlparams + L" >= " + val;
        break;
        case e_LESSER:
          filter_urlparams = filter_urlparams + L" < " + val;
        break;
        case e_LESSER_EQUAL:
          filter_urlparams = filter_urlparams + L" <= " + val;
        break;
      }
    }
  }
  
  // if iflter URL parametrs are defined set them
  // also check if 'filter' was defined in url...if it was then combine both with AND
  if( filter_urlparams.length() > 0 )
  {
    if( FilterStr.length() > 0 )
    {
      FilterStr = FilterStr + L" AND " + filter_urlparams;
    }
    else
      FilterStr = filter_urlparams;
    if( qryOptions ) qryOptions->SetFilter(FilterStr);
  
  }
  
  
  if( query_params ->ContainsParameter(L"bbox") )
  {
    STRING paramval = query_params->GetParameterValue(L"BBOX");
    CreateFilter_BBOX(paramval,ClassDef,qryOptions,FilterStr);    
    
  }
  if( query_params ->ContainsParameter(L"ibox") )
  {
    double x1,y1,x2,y2;
    STRING paramval = query_params->GetParameterValue(L"ibox");
    if( swscanf(paramval.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) == 4 )
    {
       // create geometry spatial filter
      STRING geometryname = ClassDef->GetDefaultGeometryPropertyName();
      
      
      if( geometryname.length() >0 )
      {
        //STRING geomstr = m_DataFilter.substr(5,m_DataFilter.length()-6);
        
        // geomstr = POLYGON(x1 y1, x2 y1, x2 y2, x1 y2, x1 y1)
        wchar_t geomstr[1024];
        swprintf(geomstr,L"POLYGON((%.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf ))",x1,y1 ,x2,y1 ,x2,y2 ,x1,y2 ,x1,y1);
        
        MgWktReaderWriter wktReader;
        Ptr<MgGeometry> filterGeometry;
        filterGeometry = wktReader.Read(geomstr);
        
        if( filterGeometry.p != NULL )
        {
          //GEOMETRY ENVELOPEINTERSECTS GeomFromText(' POLYGON(())')
          FilterStr = geometryname + L" INTERSECTS GeomFromText('" + geomstr + L"')";
          INT32 selectionVariant = MgFeatureSpatialOperations::Intersects;
          if( qryOptions ) qryOptions->SetSpatialFilter(geometryname,filterGeometry,selectionVariant);
        }
      }
    }
  }
  
  if( m_Filter.length() > 0 )
  {
    STRING rectstr = m_Filter.substr(0,5);
    if( wcsicmp(rectstr.c_str(),L"RECT")==0 )
    {
    //
      STRING paramname = L"RECT";
      double x1,y1,x2,y2;
      if( swscanf(path_params->GetParameterValue(m_UriParam_DataFilter).c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) == 4 )
      {
      // create geometry spatial filter
        STRING geometryname = ClassDef->GetDefaultGeometryPropertyName();
        
        
        if( geometryname.length() >0 )
        {
          //STRING geomstr = m_DataFilter.substr(5,m_DataFilter.length()-6);
          
          // geomstr = POLYGON(x1 y1, x2 y1, x2 y2, x1 y2, x1 y1)
          wchar_t geomstr[1024];
          swprintf(geomstr,L"POLYGON((%.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf ))",x1,y1 ,x2,y1 ,x2,y2 ,x1,y2 ,x1,y1);
          
          MgWktReaderWriter wktReader;
          Ptr<MgGeometry> filterGeometry;
          filterGeometry = wktReader.Read(geomstr);
          
          if( filterGeometry.p != NULL )
          {
            //GEOMETRY ENVELOPEINTERSECTS GeomFromText(' POLYGON(())')
            FilterStr = geometryname + L" ENVELOPEINTERSECTS GeomFromText('" + geomstr + L"')";
            INT32 selectionVariant = MgFeatureSpatialOperations::EnvelopeIntersects;
            if( qryOptions ) qryOptions->SetSpatialFilter(geometryname,filterGeometry,selectionVariant);
          }
        }
      }
      
    }
    else
    {
    // create filter by identity property
      Ptr<MgPropertyDefinitionCollection> propdefcoll = ClassDef->GetIdentityProperties();
      if( propdefcoll->GetCount() != 1 )
      {
        throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Class has more than one identity property. Unable to query by identity!", NULL);
      }
      int ind=0;
      bool isstring=false;
      Ptr<MgPropertyDefinition> propdef = propdefcoll->GetItem(ind);
      if( propdef->GetPropertyType() == MgFeaturePropertyType::DataProperty )
      {
        MgDataPropertyDefinition *dataprop = (MgDataPropertyDefinition*)propdef.p;
        if( dataprop->GetDataType() == MgPropertyType::String )
        {
          isstring = true;
        }
      }
      
      STRING filter;
      filter = propdef->GetName();
      filter += L"=";
      if( isstring )
      {
        filter += L"'" + m_Filter + L"'";
      }
      else
        filter += m_Filter;
      
      FilterStr = filter;
      if( qryOptions ) qryOptions->SetFilter(filter);
      
    }
  }
}


MgEnvelope* c_RestHandler_Data::GetFeatureExtent(MgFeatureReader *FeatureReader)
{
// Find geomtry property
  STRING geompropname;
  int count = FeatureReader->GetPropertyCount();
  for(int ind=0;ind<count;ind++)
  {
    STRING propname = FeatureReader->GetPropertyName(ind);
    int ptype = FeatureReader->GetPropertyType(propname);
    switch( ptype )
    {
      case MgPropertyType::Geometry:
      {
        geompropname = propname;                
      }
      break;
    }
  } 
      
  if( geompropname.length() == 0 )
  {
    return NULL;
  }      
  
  Ptr<MgEnvelope> e2;
  Ptr<MgGeometry> geom;
  MgAgfReaderWriter agfWriter;
  Ptr<MgByteReader> bytes;
  MgEnvelope* geomextent = NULL; 
  while ( FeatureReader->ReadNext() )
  {
    /*
    int geomlength;
    BYTE_ARRAY_OUT geombytes = FeatureReader->GetGeometry(geompropname,geomlength);
    Ptr<MgByteReader> bytes = new MgByteReader( geombytes,geomlength,L"" );
    */
    
    try
    {
      bytes = ((MgProxyFeatureReader*)FeatureReader)->GetGeometry(geompropname);
      geom = agfWriter.Read(bytes);
      e2 = geom->Envelope();
      if( !geomextent )
        geomextent = new MgEnvelope(e2);
      else
        geomextent->ExpandToInclude(e2);
    
    }
    catch (MgException* e)
    {
    	e->Release(); // just releasse - this exception beacuse there could be some bad geometries coming from fdo
    }
  }
    
    
  
  return geomextent;
}
MgEnvelope* c_RestHandler_Data::GetFeatureExtent(c_RestDataReader *FeatureReader)
{
  // Find geomtry property
  STRING geompropname;
  int count = FeatureReader->GetPropertyCount();
  for(int ind=0;ind<count;ind++)
  {
    STRING propname = FeatureReader->GetPropertyName(ind);
    int ptype = FeatureReader->GetPropertyType(propname);
    switch( ptype )
    {
    case MgPropertyType::Geometry:
      {
        geompropname = propname;                
      }
      break;
    }
  } 

  if( geompropname.length() == 0 )
  {
    return NULL;
  }      

  Ptr<MgEnvelope> e2;
  Ptr<MgGeometry> geom;
  MgAgfReaderWriter agfWriter;
  Ptr<MgByteReader> bytes;
  MgEnvelope* geomextent = NULL; 
  while ( FeatureReader->ReadNext() )
  {
    /*
    int geomlength;
    BYTE_ARRAY_OUT geombytes = FeatureReader->GetGeometry(geompropname,geomlength);
    Ptr<MgByteReader> bytes = new MgByteReader( geombytes,geomlength,L"" );
    */

    try
    {
      bytes = FeatureReader->GetGeometry(geompropname);
      geom = agfWriter.Read(bytes);
      e2 = geom->Envelope();
      if( !geomextent )
        geomextent = new MgEnvelope(e2);
      else
        geomextent->ExpandToInclude(e2);

    }
    catch (MgException* e)
    {
      e->Release(); // just releasse - this exception beacuse there could be some bad geometries coming from fdo
    }
  }



  return geomextent;
}
void c_RestHandler_Data::Execute_Get_Class_Feature(c_RestResponse& HttpResponse)
{
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() ==  c_CfgDataSource::e_MgFeatureSource )
  {
    Execute_Get_Feature_MapGuide(HttpResponse);
    return;
  }
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() ==  c_CfgDataSource::e_FDO )
  {
    Execute_Get_Feature_FDO(HttpResponse);
    return;
  }
  
  throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Unsupported Data Source! MapGuide Feature Source is required.", NULL);
}

void c_RestHandler_Data::Execute_Get_Feature_FDO(c_RestResponse& HttpResponse)
{
  Ptr<c_RestResult> hResult = HttpResponse.GetResult();
    
  c_CfgDataSource_FDO *fdo_source;
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() !=  c_CfgDataSource::e_FDO )
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Unsupported Data Source! MapGuide Feature Source is required.", NULL);
  }
  fdo_source = (c_CfgDataSource_FDO *)m_RestRequest->m_CfgDataResource->m_DataSource;
    
  //if( wcsicmp(L"schema",m_FormatType.c_str())==0 )
  if( m_RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_FDO_Schema )
  {
    MG_TRY()
        
    // Create a byte reader.
    Ptr<MgByteReader> byteReader = c_RestFetchSource::Fetch_FdoSourceSchema(fdo_source);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());
    
    MG_CATCH_AND_THROW(L"Execute_Get_Class_Feature");    
    return;
  } 
    
  
  Ptr<MgFeatureQueryOptions> qryOptions = new MgFeatureQueryOptions();
  
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();
  
  MG_TRY()
  
  STRING schema;
  STRING classname;
  STRING::size_type iColon = fdo_source->GetFeatureClass().find(':');
  if(iColon != STRING::npos) {
    schema = fdo_source->GetFeatureClass().substr(0,iColon);
    classname   = fdo_source->GetFeatureClass().substr(iColon+1);
    
  }
  else
  {
    schema = L"";
    classname = fdo_source->GetFeatureClass();
  }
  
  m_RestRequest->m_DataClassDef = c_RestFetchSource::Fetch_MgClassDefinition(fdo_source); // service->GetClassDefinition(&resId, schema, classname);
  
  if( m_RestRequest->m_CfgRepresentation->IsBBoxHeightLimitSet() 
      || m_RestRequest->m_CfgRepresentation->IsBBoxWidthLimitSet() 
    )
  {
    if( query_params->ContainsParameter(L"BBOX") )
    {
      bool isbbox=false;
      double x1,y1,x2,y2;
      STRING paramval = query_params->GetParameterValue(L"BBOX");
      if( swscanf(paramval.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) == 4 )
      {
        double width = fabs(x1-x2);
        double height = fabs(y1-y2);
       
        if( !m_RestRequest->m_CfgRepresentation->IsHeightInsideLimit(height) 
            || 
            !m_RestRequest->m_CfgRepresentation->IsWidthInsideLimit(width)  
          )
        {
          m_RestRequest->m_RestResultObjectStatus = c_RestRequest::e_BBox_OutOfRange;
        
          return;
        }
        
      }
    }  
  }
  
  if( m_RestRequest->m_CfgRepresentation->IsMaxCountSet() )
  {
    if( query_params->ContainsParameter(L"count") )
    {
      STRING val = query_params->GetParameterValue(L"count");
      int rowcount = _wtoi(val.c_str());
      
      if( !m_RestRequest->m_CfgRepresentation->IsCountInsideLimit(rowcount) )
      {
        
        m_RestRequest->m_RestResultObjectStatus = c_RestRequest::e_Count_OutOfRange;
        
        return;
      }
    }
  }
  
  if( m_RestRequest->m_CfgRepresentation->m_OrderFields.length() != 0 )
  {
    Ptr<MgStringCollection> strcoll = MgStringCollection::ParseCollection( m_RestRequest->m_CfgRepresentation->m_OrderFields,L",");
    int direction = MgOrderingOption::Ascending; 
    if(m_RestRequest->m_CfgRepresentation->m_OrderDirection==c_CfgRepresentation::e_Desc )
    {
      direction = MgOrderingOption::Descending; 
    }
    qryOptions->SetOrderingFilter(strcoll, direction);
  }
  //if( m_Filter.length() > 0 )
  {
    
    STRING filterstr;
    CreateFilterString(m_RestRequest->m_DataClassDef,qryOptions,filterstr);
  }  

  
  if( query_params->ContainsParameter(L"count") )
  {
    STRING val = query_params->GetParameterValue(L"count");
    int rowcount = _wtoi(val.c_str());
    
    hResult->m_FeatureReader_Count = rowcount;
  }
  else
  {
    hResult->m_FeatureReader_Count = m_RestRequest->m_CfgRepresentation->GetDefaultCount();
  }
  
  if( query_params->ContainsParameter(L"start") )
  {
    STRING val = query_params->GetParameterValue(L"start");
    int sindex = _wtoi(val.c_str());
    
    hResult->m_FeatureReader_StartIndex = sindex-1;
  }
  
  
  Ptr<c_RestDataReader> featureReader = c_RestFetchSource::FetchSource(fdo_source,  qryOptions);
  
  
  //if( wcsicmp(L"png",m_FormatType.c_str())==0 )
  if( m_RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_PNG_MapGuide )
  {
    Ptr<MgEnvelope> extent;
    
    if( query_params->ContainsParameter(L"SETVIEWSCALE" ) && query_params->ContainsParameter(L"SETVIEWCENTERX" ) && query_params->ContainsParameter(L"SETVIEWCENTERY" ) )
    {    
      // if all view parameters are defined then don't calculate etxent               
    }  
    else
    {    
      extent = GetFeatureExtent(featureReader);
      if( !extent.p )
      {
        throw new MgRuntimeException(L"c_RestHandler_Data::Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"No geomtry extent!", NULL);
      }
    }
    
    
    
    Execute_Get_Image_Png(HttpResponse,extent.p,qryOptions);
  }
  else
  {
    hResult->SetResultObject(featureReader, m_RestRequest->m_CfgRepresentation->m_MimeType);
    hResult->m_IsSitemapXML = m_IsSitemapXml;
    
    
  }
  
  MG_CATCH_AND_THROW(L"Execute_Get_Class_Feature")  
  
}//end of c_RestHandler_Data::Execute_Get_Feature_FDO

void c_RestHandler_Data::Execute_Get_Feature_MapGuide(c_RestResponse& HttpResponse)
{
  Ptr<c_RestResult> hResult = HttpResponse.GetResult();

  c_CfgDataSource_MgFeatureSource *mg_fsource;
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() !=  c_CfgDataSource::e_MgFeatureSource )
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Unsupported Data Source! MapGuide Feature Source is required.", NULL);
  }
  mg_fsource = (c_CfgDataSource_MgFeatureSource *)m_RestRequest->m_CfgDataResource->m_DataSource;

  //if( wcsicmp(L"schema",m_FormatType.c_str())==0 )
  if( m_RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_FDO_Schema )
  {
    MG_TRY()
      MgResourceIdentifier resId(mg_fsource->m_MgFeatureSource);

    //Ptr<c_RestUriRequestParam> hrParam = m_hRequest->GetRequestParam();
    //STRING schema = hrParam->GetParameterValue(MgHttpResourceStrings::reqFeatSchema);

    Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(L"Anonymous",L"",mg_fsource->GetServerIP(),mg_fsource->GetServerPort());    
    // Create Proxy Feature Service instance
    Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);

    // call the C++ APIs
    STRING xmlSchema = service->DescribeSchemaAsXml(&resId, L"");

    // Convert to multibyte
    string mbXmlSchema = MgUtil::WideCharToMultiByte(xmlSchema);

    // Create a byte reader.
    Ptr<MgByteReader> byteReader = MgUtil::GetByteReader(mbXmlSchema, (STRING*)&MgMimeType::Xml);

    //Convert to alternate response format, if necessary
    //ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_CATCH_AND_THROW(L"Execute_Get_Class_Feature");    
    return;
  } 

  // Create Proxy Feature Service instance
  //Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
  //Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(L"Anonymous",L"");    
  //Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(mg_fsource->GetUsername(),mg_fsource->GetPassword(),mg_fsource->GetServerIP(),mg_fsource->GetServerPort());    
  // Create Proxy Feature Service instance
  //Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);

  MgResourceIdentifier resId(mg_fsource->m_MgFeatureSource);
  Ptr<MgFeatureQueryOptions> qryOptions = new MgFeatureQueryOptions();
  
  if( m_RestRequest->m_CfgRepresentation->m_OrderFields.length() != 0 )
  {
    Ptr<MgStringCollection> strcoll = MgStringCollection::ParseCollection( m_RestRequest->m_CfgRepresentation->m_OrderFields,L",");
    int direction = MgOrderingOption::Ascending; 
    if(m_RestRequest->m_CfgRepresentation->m_OrderDirection==c_CfgRepresentation::e_Desc )
    {
      direction = MgOrderingOption::Descending; 
    }
    qryOptions->SetOrderingFilter(strcoll, direction);
  }

  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();

  MG_TRY()

  STRING schema;
  STRING classname;
  STRING::size_type iColon = mg_fsource->m_MgFeatureSourceClassName.find(':');
  if(iColon != STRING::npos) {
    schema = mg_fsource->m_MgFeatureSourceClassName.substr(0,iColon);
    classname   = mg_fsource->m_MgFeatureSourceClassName.substr(iColon+1);

  }
  else
  {
    schema = L"";
    classname =mg_fsource->m_MgFeatureSourceClassName;
  }

  m_RestRequest->m_DataClassDef = c_RestFetchSource::Fetch_MgClassDefinition(mg_fsource); // service->GetClassDefinition(&resId, schema, classname);

  if( m_RestRequest->m_CfgRepresentation->IsBBoxHeightLimitSet() 
    || m_RestRequest->m_CfgRepresentation->IsBBoxWidthLimitSet() 
    )
  {
    if( query_params->ContainsParameter(L"BBOX") )
    {
      bool isbbox=false;
      double x1,y1,x2,y2;
      STRING paramval = query_params->GetParameterValue(L"BBOX");
      if( swscanf(paramval.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) == 4 )
      {
        double width = fabs(x1-x2);
        double height = fabs(y1-y2);

        if( !m_RestRequest->m_CfgRepresentation->IsHeightInsideLimit(height) 
          || 
          !m_RestRequest->m_CfgRepresentation->IsWidthInsideLimit(width)  
          )
        {
          m_RestRequest->m_RestResultObjectStatus = c_RestRequest::e_BBox_OutOfRange;

          return;
        }

      }
    }  
  }

  if( m_RestRequest->m_CfgRepresentation->IsMaxCountSet() )
  {
    if( query_params->ContainsParameter(L"count") )
    {
      STRING val = query_params->GetParameterValue(L"count");
      int rowcount = _wtoi(val.c_str());

      if( !m_RestRequest->m_CfgRepresentation->IsCountInsideLimit(rowcount) )
      {

        m_RestRequest->m_RestResultObjectStatus = c_RestRequest::e_Count_OutOfRange;

        return;
      }
    }
  }


  //if( m_Filter.length() > 0 )
  {

    STRING filterstr;
    CreateFilterString(m_RestRequest->m_DataClassDef,qryOptions,filterstr);
  }  


  if( query_params->ContainsParameter(L"count") )
  {
    STRING val = query_params->GetParameterValue(L"count");
    int rowcount = _wtoi(val.c_str());

    hResult->m_FeatureReader_Count = rowcount;
  }
  else
  {
    hResult->m_FeatureReader_Count = m_RestRequest->m_CfgRepresentation->GetDefaultCount();
  }
  if( query_params->ContainsParameter(L"start") )
  {
    STRING val = query_params->GetParameterValue(L"start");
    int sindex = _wtoi(val.c_str());

    hResult->m_FeatureReader_StartIndex = sindex-1;
  }

  //Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, mg_fsource->m_MgFeatureSourceClassName, qryOptions);
  Ptr<c_RestDataReader> featureReader = c_RestFetchSource::FetchSource(mg_fsource,qryOptions); 


  //if( wcsicmp(L"png",m_FormatType.c_str())==0 )
  if( m_RestRequest->m_CfgRepresentation->GetType() == c_CfgRepresentation::e_PNG_MapGuide )
  {
    Ptr<MgEnvelope> extent;

    if( query_params->ContainsParameter(L"SETVIEWSCALE" ) && query_params->ContainsParameter(L"SETVIEWCENTERX" ) && query_params->ContainsParameter(L"SETVIEWCENTERY" ) )
    {    
      // if all view parameters are defined then don't calculate etxent               
    }  
    else
    {    
      extent = GetFeatureExtent(featureReader);
      if( !extent.p )
      {
        throw new MgRuntimeException(L"c_RestHandler_Data::Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"No geomtry extent!", NULL);
      }
    }



    Execute_Get_Image_Png(HttpResponse,extent.p,qryOptions);
  }
  else
  {
    hResult->SetResultObject(featureReader, m_RestRequest->m_CfgRepresentation->m_MimeType);
    hResult->m_IsSitemapXML = m_IsSitemapXml;


  }

  MG_CATCH_AND_THROW(L"Execute_Get_Class_Feature")  

}//end of c_RestHandler_Data::Execute_Get_Feature_MapGuide

/*
void MgRestData::Execute_Get_Image_Png(MgHttpResponse& HttpResponse,MgEnvelope* ZoomTo,MgFeatureQueryOptions *QueryOptions)
{

  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();
  Ptr<MgHttpRequestParam> query_params = m_RestRequest-> GetRequestParam();

  //STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_MAP_IMAGE_PNG);  
  //Ptr<MgHttpRequestParam> filterparams = new MgHttpRequestParam();
  //RestUriFilterParser::Parse(filter, filterparams);

  MgHtmlController controller(mgsiteconn);

  // Get the map view commands
  //Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();
  //Ptr<MgPropertyCollection> mapViewCommands = params->GetParameters()->GetPropertyCollection();
  //Ptr<MgPropertyCollection> mapViewCommands = filterparams->GetParameters()->GetPropertyCollection();


  STRING map_resid;
  STRING mapname;
  Ptr<MgMap> map;
  Ptr<MgSelection> selection;

  Ptr<MgPropertyCollection> mapviewcommands = new MgPropertyCollection();
  {
    STRING sessionid;
    Ptr<MgSite> mgSite = mgsiteconn->GetSite();
    sessionid = mgSite->CreateSession();
    m_userInfo->SetMgSessionId( sessionid );

    //-------------------------------------------------------
    // Create a new Map from MapDefinition in a Session
    //-------------------------------------------------------  
    map = new MgMap();
    Ptr<MgResourceIdentifier> resid_mapdef = new MgResourceIdentifier(m_MapDefId);


    mapname = L"KINGREST";

    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(CreateService(MgServiceType::ResourceService));

    map->Create(resourceService, resid_mapdef, mapname );  
    // Save the MgMap state
    map_resid = L"Session:" + sessionid + L"//" + mapname + L"." + MgResourceType::Map;
    Ptr<MgResourceIdentifier> resid_map = new MgResourceIdentifier(map_resid);
    map->Save(resourceService,resid_map);


    selection = new MgSelection(map);
    if( m_RestRequest->m_DataLayerConfig && m_RestRequest->m_DataLayerConfig->m_MapSelectionLayer.length()>0 )
    {
      MgResourceIdentifier resId(m_FeatureSourceId);
      Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
      Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, m_RestRequest->m_DataClassName, QueryOptions);

      //Ptr<MgResourceIdentifier> layerresidid = new MgResourceIdentifier(m_RestRequest->m_DataLayerConfig->m_MapSelectionLayer);
      //Ptr<MgLayer> layerdef = new  MgLayer(layerid,resourceService);
      Ptr<MgLayerBase> layer;
      Ptr<MgLayerCollection> layers = map->GetLayers();


      int layerid =layers->IndexOf(m_RestRequest->m_DataLayerConfig->m_MapSelectionLayer);
      if( layerid >= 0 )
      {
        layer = layers->GetItem(layerid);
      }
      //auto_ptr<MdfModel::LayerDefinition> layerdef( MgLayerBase::GetLayerDefinition(resourceService, layerid));         
      selection->AddFeatures(layer,featureReader,0);

      featureReader->Close();
    }
    selection->Save(resourceService, mapname);

    Ptr<MgReadOnlyLayerCollection> selLayers = selection->GetLayers();
    int sel_layercount = selLayers ? selLayers->GetCount() : 0;

    MgStringPropertyCollection* coll_params = query_params->GetParameters();

    int dwidth=260;
    int dheight=260;
    int dpi = 96;

    STRING strval;
    Ptr<MgStringProperty> prop;

    if( coll_params->Contains(L"SETDISPLAYWIDTH" ) )
    {


      STRING val = coll_params->GetValue(L"SETDISPLAYWIDTH");   
      dwidth = MgUtil::StringToDouble(val);     
      if( dwidth <= 0 ) dwidth=260;
    }



    {
      MgUtil::Int32ToString(dwidth,strval);
      prop = new MgStringProperty(L"SETDISPLAYWIDTH",strval);
      mapviewcommands->Add(prop);
    }

    if( coll_params->Contains(L"SETDISPLAYHEIGHT" ) )
    {
      STRING val = coll_params->GetValue(L"SETDISPLAYHEIGHT");   
      dheight = MgUtil::StringToDouble(val);     
      if( dheight <= 0 ) dheight=260;
    }



    {
      MgUtil::Int32ToString(dheight,strval);
      prop = new MgStringProperty(L"SETDISPLAYHEIGHT",strval);
      mapviewcommands->Add(prop);
    }

    if( coll_params->Contains(L"SETDISPLAYDPI" ) )
    {
      STRING val = coll_params->GetValue(L"SETDISPLAYDPI");   
      dpi = MgUtil::StringToDouble(val);     
      if( dpi <= 0 ) dpi=96;
    }



    {
      MgUtil::Int32ToString(dpi,strval);
      prop = new MgStringProperty(L"SETDISPLAYDPI",strval);
      mapviewcommands->Add(prop);
    }

    Ptr<MgCoordinate> xy1 = ZoomTo->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> xy2 = ZoomTo->GetUpperRightCoordinate();

    double cx = ((double)xy1->GetX() + (double)xy2->GetX()) / 2.0;
    double cy = ((double)xy1->GetY() + (double)xy2->GetY()) / 2.0;




    MgUtil::DoubleToString(cx,strval);
    prop = new MgStringProperty(L"SETVIEWCENTERX",strval);
    mapviewcommands->Add(prop);

    MgUtil::DoubleToString(cy,strval);
    prop = new MgStringProperty(L"SETVIEWCENTERY",strval);
    mapviewcommands->Add(prop);

    double zoomfactor=1.3;
    if( coll_params->Contains(L"ZOOMFACTOR" ) )
    {
      STRING val = coll_params->GetValue(L"ZOOMFACTOR");   
      zoomfactor = MgUtil::StringToDouble(val);     
      if( zoomfactor <= 0.0 ) zoomfactor = 1.3;
    }
    else
    {
      if( m_RestRequest->m_DataLayerConfig->m_MapZoomFactor.length() > 0 )
      {
        zoomfactor = MgUtil::StringToDouble(m_RestRequest->m_DataLayerConfig->m_MapZoomFactor);     
        if( zoomfactor <= 0.0 ) zoomfactor = 1.3;
      }
    }

    double scale;
    double mcsH = xy2->GetY() - xy1->GetY();
    double mcsW = xy2->GetX() - xy1->GetX();

    mcsH = mcsH * zoomfactor;
    mcsW = mcsW * zoomfactor;

    double metersperpixel  = 0.0254 / dpi;

    if (dheight * mcsW > dwidth * mcsH)
      scale = mcsW * map->GetMetersPerUnit() / (dwidth * metersperpixel); // width-limited
    else
      scale = mcsH * map->GetMetersPerUnit() / (dheight * metersperpixel); // height-limited


    MgUtil::DoubleToString(scale,strval);
    prop = new MgStringProperty(L"SETVIEWSCALE",strval);
    mapviewcommands->Add(prop);

  }

  Ptr<MgByteReader> mapimage = controller.GetDynamicMapOverlayImage_ViewCommands(mapname, MgImageFormats::Png, true,mapviewcommands);
  //Ptr<MgByteReader> mapimage = controller.GetMapImage(map,selection, MgImageFormats::Png, mapviewcommands,true,true);

  // Set the result
  Ptr<MgHttpResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(mapimage, mapimage->GetMimeType());
}
*/


void c_RestHandler_Data::Execute_Get_Image_Png(c_RestResponse& HttpResponse,MgEnvelope* ZoomTo,MgFeatureQueryOptions *QueryOptions)
{
  
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  
  
  if( m_RestRequest->m_CfgRepresentation->GetType() != c_CfgRepresentation::e_PNG_MapGuide )
  {
    throw new MgRuntimeException(L"Execute_Get_Image_Png",__LINE__, __WFILE__, NULL, L"Unsupported Representation! MapGuide PNG representation is required.", NULL);
  }
  c_CfgRepresentation_Png8_MapGuide* rep_png = (c_CfgRepresentation_Png8_MapGuide*)m_RestRequest->m_CfgRepresentation;
  
  c_CfgDataSource_MgFeatureSource *mg_fsource;
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() !=  c_CfgDataSource::e_MgFeatureSource )
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Unsupported Data Source! MapGuide Feature Source is required.", NULL);
  }
  mg_fsource = (c_CfgDataSource_MgFeatureSource *)m_RestRequest->m_CfgDataResource->m_DataSource;
  
  //STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_MAP_IMAGE_PNG);  
  //Ptr<c_RestUriRequestParam> filterparams = new c_RestUriRequestParam();
  //RestUriFilterParser::Parse(filter, filterparams);
  
  //Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(L"Anonymous",L""); 
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(mg_fsource->GetUsername(),mg_fsource->GetPassword(),mg_fsource->GetServerIP(),mg_fsource->GetServerPort());    
     
  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);
  
  //MgHtmlController controller(mgsiteconn);
  
  // Get the map view commands
  //Ptr<c_RestUriRequestParam> params = m_hRequest->GetRequestParam();
  //Ptr<MgPropertyCollection> mapViewCommands = params->GetParameters()->GetPropertyCollection();
  //Ptr<MgPropertyCollection> mapViewCommands = filterparams->GetParameters()->GetPropertyCollection();
  
  
  STRING map_resid;
  STRING mapname;
  Ptr<MgMap> map;
  Ptr<MgSelection> selection;
  STRING imageformat = MgImageFormats::Png8;
  Ptr<MgPropertyCollection> mapviewcommands = new MgPropertyCollection();
  {
    STRING sessionid;
    Ptr<MgSite> mgSite = mgsiteconn->GetSite();
    sessionid = mgSite->CreateSession();
    //m_userInfo->SetMgSessionId( sessionid );
    
    Ptr<MgUserInformation> userinfo =  mgsiteconn->GetUserInfo();
    userinfo->SetMgSessionId( sessionid );
    
    //-------------------------------------------------------
    // Create a new Map from MapDefinition in a Session
    //-------------------------------------------------------  
      map = new MgMap();
      Ptr<MgResourceIdentifier> resid_mapdef = new MgResourceIdentifier(rep_png->m_MapDef);
      
     
      mapname = L"KINGREST";
   
      Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(mgsiteconn->CreateService(MgServiceType::ResourceService));
         
      map->Create(resourceService, resid_mapdef, mapname );  
      // Save the MgMap state
      map_resid = L"Session:" + sessionid + L"//" + mapname + L"." + MgResourceType::Map;
      Ptr<MgResourceIdentifier> resid_map = new MgResourceIdentifier(map_resid);
      map->Save(resourceService,resid_map);
      
      
      selection = new MgSelection(map);
      if( rep_png->m_MapSelectionLayer.length()>0 )
      {
        MgResourceIdentifier resId(mg_fsource->m_MgFeatureSource);
        Ptr<MgFeatureService> service = (MgFeatureService*)(mgsiteconn->CreateService(MgServiceType::FeatureService));
        Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, mg_fsource->m_MgFeatureSourceClassName, QueryOptions);
        
        //Ptr<MgResourceIdentifier> layerresidid = new MgResourceIdentifier(m_RestRequest->m_DataLayerConfig->m_MapSelectionLayer);
        //Ptr<MgLayer> layerdef = new  MgLayer(layerid,resourceService);
        Ptr<MgLayerBase> layer;
        Ptr<MgLayerCollection> layers = map->GetLayers();
        
        
        int layerid =layers->IndexOf(rep_png->m_MapSelectionLayer);
        if( layerid >= 0 )
        {
          layer = layers->GetItem(layerid);
        }
        //auto_ptr<MdfModel::LayerDefinition> layerdef( MgLayerBase::GetLayerDefinition(resourceService, layerid));    
        if( layer.p )     
          selection->AddFeatures(layer,featureReader,0);
        
        featureReader->Close();
      }
      selection->Save(resourceService, mapname);
      
      Ptr<MgReadOnlyLayerCollection> selLayers = selection->GetLayers();
      int sel_layercount = selLayers ? selLayers->GetCount() : 0;
      
      MgStringPropertyCollection* coll_params = query_params->GetParameters();
      
      int dwidth=260;
      int dheight=260;
      int dpi = 96;
      
      STRING strval;
      Ptr<MgStringProperty> prop;
      
      if( coll_params->Contains(L"SETDISPLAYWIDTH" ) )
      {
        
        
        STRING val = coll_params->GetValue(L"SETDISPLAYWIDTH");   
        dwidth = MgUtil::StringToDouble(val);     
        if( dwidth <= 0 ) dwidth=260;
      }
      
      
      
      {
        MgUtil::Int32ToString(dwidth,strval);
        prop = new MgStringProperty(L"SETDISPLAYWIDTH",strval);
        mapviewcommands->Add(prop);
      }
      
      if( coll_params->Contains(L"SETDISPLAYHEIGHT" ) )
      {
        STRING val = coll_params->GetValue(L"SETDISPLAYHEIGHT");   
        dheight = MgUtil::StringToDouble(val);     
        if( dheight <= 0 ) dheight=260;
      }
      
      
      
      {
        MgUtil::Int32ToString(dheight,strval);
        prop = new MgStringProperty(L"SETDISPLAYHEIGHT",strval);
        mapviewcommands->Add(prop);
      }
      
      if( coll_params->Contains(L"SETDISPLAYDPI" ) )
      {
        STRING val = coll_params->GetValue(L"SETDISPLAYDPI");   
        dpi = MgUtil::StringToDouble(val);     
        if( dpi <= 0 ) dpi=96;
      }
      
      
      
      {
        MgUtil::Int32ToString(dpi,strval);
        prop = new MgStringProperty(L"SETDISPLAYDPI",strval);
        mapviewcommands->Add(prop);
      }
      
      double cx = 0.0;
      double cy = 0.0;
      Ptr<MgCoordinate> xy2 = new MgCoordinateXY(0,0);
      Ptr<MgCoordinate> xy1 = new MgCoordinateXY(0,0);
      
      if( ZoomTo )
      {      
        xy1 = ZoomTo->GetLowerLeftCoordinate();
        xy2 = ZoomTo->GetUpperRightCoordinate();
        
        cx = ((double)xy1->GetX() + (double)xy2->GetX()) / 2.0;
        cy = ((double)xy1->GetY() + (double)xy2->GetY()) / 2.0;
      }
      
      if( coll_params->Contains(L"SETVIEWCENTERX" ) )
      {
        STRING val = coll_params->GetValue(L"SETVIEWCENTERX");   
        cx = MgUtil::StringToDouble(val);             
      }
      if( coll_params->Contains(L"SETVIEWCENTERY" ) )
      {
        STRING val = coll_params->GetValue(L"SETVIEWCENTERY");   
        cy = MgUtil::StringToDouble(val);             
      }
      
      
      
      MgUtil::DoubleToString(cx,strval);
      prop = new MgStringProperty(L"SETVIEWCENTERX",strval);
      mapviewcommands->Add(prop);
      
      MgUtil::DoubleToString(cy,strval);
      prop = new MgStringProperty(L"SETVIEWCENTERY",strval);
      mapviewcommands->Add(prop);
      
      double scale;
      if( coll_params->Contains(L"SETVIEWSCALE" ) )
      {
        STRING val = coll_params->GetValue(L"SETVIEWSCALE");   
        scale = MgUtil::StringToDouble(val);                
      }
      else
      {      
        double zoomfactor=1.3;
        if( coll_params->Contains(L"ZOOMFACTOR" ) )
        {
          STRING val = coll_params->GetValue(L"ZOOMFACTOR");   
          zoomfactor = MgUtil::StringToDouble(val);     
          if( zoomfactor <= 0.0 ) zoomfactor = 1.3;
        }
        else
        {
          if( rep_png->m_MapZoomFactor.length() > 0 )
          {
            zoomfactor = MgUtil::StringToDouble(rep_png->m_MapZoomFactor);     
            if( zoomfactor <= 0.0 ) zoomfactor = 1.3;
          }
        }
        
        double mcsH = xy2->GetY() - xy1->GetY();
        double mcsW = xy2->GetX() - xy1->GetX();
        
        mcsH = mcsH * zoomfactor;
        mcsW = mcsW * zoomfactor;
             
        double metersperpixel  = 0.0254 / dpi;
                          
        if (dheight * mcsW > dwidth * mcsH)
            scale = mcsW * map->GetMetersPerUnit() / (dwidth * metersperpixel); // width-limited
        else
            scale = mcsH * map->GetMetersPerUnit() / (dheight * metersperpixel); // height-limited
      }
          
      MgUtil::DoubleToString(scale,strval);
      prop = new MgStringProperty(L"SETVIEWSCALE",strval);
      mapviewcommands->Add(prop);
  
      if( coll_params->Contains(L"FORMAT" ) )
      {
        imageformat = coll_params->GetValue(L"FORMAT");   

      }  
  }
  
  
  
  
  Ptr<MgByteReader> mapimage = c_RestFetchImage::GetDynamicMapOverlayImage_ViewCommands(mgsiteconn,mapname, imageformat , true,mapviewcommands);
  //Ptr<MgByteReader> mapimage = controller.GetMapImage(map,selection, MgImageFormats::Png, mapviewcommands,true,true);

  // Set the result
  Ptr<c_RestResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(mapimage, mapimage->GetMimeType());
}



// Update of a feature
void c_RestHandler_Data::Execute_Put_Class_Feature(c_RestResponse& HttpResponse)
{
      Ptr<c_RestResult> hResult = HttpResponse.GetResult();
      
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();

  c_CfgDataSource_MgFeatureSource *mg_fsource;
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() !=  c_CfgDataSource::e_MgFeatureSource )
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Unsupported Data Source! MapGuide Feature Source is required.", NULL);
  }
  mg_fsource = (c_CfgDataSource_MgFeatureSource *)m_RestRequest->m_CfgDataResource->m_DataSource;
  
  
// Filter which identifies Fetaure to be updated is set in URI segment
  
  MG_TRY()


  MgResourceIdentifier resId(mg_fsource->m_MgFeatureSource); // resource identifier of feature data source

  // Create Proxy Feature Service instance
  //Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(mg_fsource->GetUsername(),mg_fsource->GetPassword(),mg_fsource->GetServerIP(),mg_fsource->GetServerPort());    
  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);

  STRING schema;
  STRING classname;
  STRING::size_type iColon = mg_fsource->m_MgFeatureSourceClassName.find(':');
  if(iColon != STRING::npos) {
    schema = mg_fsource->m_MgFeatureSourceClassName.substr(0,iColon);
    classname   = mg_fsource->m_MgFeatureSourceClassName.substr(iColon+1);
    
  }
  else
  {
    schema = L"";
    classname = mg_fsource->m_MgFeatureSourceClassName;
  }
  
  m_RestRequest->m_DataClassDef = service->GetClassDefinition(&resId, schema, classname);
  

// Geometry is written in URI Query Parameter as a string parameter "_json"
// Geometry is formated in GeoJson format.
  
  
  string  postxml = query_params->GetXmlPostData();
  
  Ptr<MgPropertyCollection> propcoll = c_RestDataTransform::XmlToFeature(m_RestRequest->m_DataClassDef,postxml);

 

/*  
  //get addtional params, determine which overload to call and make the call
  
  
// Create list of properties to be updated - Geometry only for now  
  Ptr<MgPropertyCollection> propvalue_coll = new MgPropertyCollection();
  
// create update of geomerty
  STRING geomname = classDef->GetDefaultGeometryPropertyName();
  MgAgfReaderWriter agfrw;
  Ptr<MgByteReader> agf =  agfrw.Write(geom);
  Ptr<MgGeometryProperty> propgeom = new MgGeometryProperty(geomname,agf);
  
  propvalue_coll->Add(propgeom);
    
*/

  STRING filterstr;
  CreateFilterString(m_RestRequest->m_DataClassDef,NULL,filterstr);

  Ptr<MgUpdateFeatures> upd_cmd = new MgUpdateFeatures(mg_fsource->m_MgFeatureSourceClassName,propcoll,filterstr);
  
  Ptr<MgFeatureCommandCollection> commands = new MgFeatureCommandCollection();
  commands->Add(upd_cmd);
  Ptr<MgPropertyCollection> up_ret = service->UpdateFeatures(&resId, commands,false);
  
  //Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, m_RestRequest->m_DataClassName, qryOptions);
  //hResult->SetResultObject(featureReader, MgMimeType::Xml);
  
  STRING str = L"Update Done";
  Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(str);
  hResult->SetResultObject(value, MgMimeType::Text);

  MG_CATCH_AND_THROW(L"Execute_Get_Class_Feature")  

}//end of c_RestHandler_Data::Execute_Put_Class_Feature

// Insert of a feature
void c_RestHandler_Data::Execute_Post_Class_Feature(c_RestResponse& HttpResponse)
{
      Ptr<c_RestResult> hResult = HttpResponse.GetResult();
      
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();

  c_CfgDataSource_MgFeatureSource *mg_fsource;
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() !=  c_CfgDataSource::e_MgFeatureSource )
  {
    throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, NULL, L"Unsupported Data Source! MapGuide Feature Source is required.", NULL);
  }
  mg_fsource = (c_CfgDataSource_MgFeatureSource *)m_RestRequest->m_CfgDataResource->m_DataSource;

// Filter which identifies Fetaure to be updated is set in URI segment
  
  MG_TRY()


  MgResourceIdentifier resId(mg_fsource->m_MgFeatureSource); // resource identifier of feature data source

  // Create Proxy Feature Service instance
  //Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
  //Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(L"Anonymous",L"");    
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(mg_fsource->GetUsername(),mg_fsource->GetPassword(),mg_fsource->GetServerIP(),mg_fsource->GetServerPort());    
  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);

  STRING schema;
  STRING classname;
  STRING::size_type iColon = mg_fsource->m_MgFeatureSourceClassName.find(':');
  if(iColon != STRING::npos) {
    schema = mg_fsource->m_MgFeatureSourceClassName.substr(0,iColon);
    classname   = mg_fsource->m_MgFeatureSourceClassName.substr(iColon+1);
    
  }
  else
  {
    schema = L"";
    classname =mg_fsource->m_MgFeatureSourceClassName;
  }
  
  m_RestRequest->m_DataClassDef = service->GetClassDefinition(&resId, schema, classname);
  

// Geometry is written in URI Query Parameter as a string parameter "_json"
// Geometry is formated in GeoJson format.
  
  
  string  postxml = query_params->GetXmlPostData();
  
  Ptr<MgPropertyCollection> propcoll = c_RestDataTransform::XmlToFeature(m_RestRequest->m_DataClassDef,postxml);

  

/*  
  //get addtional params, determine which overload to call and make the call
  
  
// Create list of properties to be updated - Geometry only for now  
  Ptr<MgPropertyCollection> propvalue_coll = new MgPropertyCollection();
  
// create update of geomerty
  STRING geomname = classDef->GetDefaultGeometryPropertyName();
  MgAgfReaderWriter agfrw;
  Ptr<MgByteReader> agf =  agfrw.Write(geom);
  Ptr<MgGeometryProperty> propgeom = new MgGeometryProperty(geomname,agf);
  
  propvalue_coll->Add(propgeom);
    
*/

  STRING filterstr;
  CreateFilterString(m_RestRequest->m_DataClassDef,NULL,filterstr);

  Ptr<MgInsertFeatures> ins_cmd = new MgInsertFeatures(mg_fsource->m_MgFeatureSourceClassName,propcoll);
  
  Ptr<MgFeatureCommandCollection> commands = new MgFeatureCommandCollection();
  commands->Add(ins_cmd);
  Ptr<MgPropertyCollection> ret_prop = service->UpdateFeatures(&resId, commands,false);
  
  //Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, m_RestRequest->m_DataClassName, qryOptions);
  //hResult->SetResultObject(featureReader, MgMimeType::Xml);
  
  if( ret_prop->GetCount() == 1 )
  {
    int index=0;
    Ptr<MgProperty> mgprop = ret_prop->GetItem(index);
    if( mgprop->GetPropertyType()==MgPropertyType::Feature)
    {
      MgFeatureProperty* ptrProp = (MgFeatureProperty*)mgprop.p;
      Ptr<MgFeatureReader> freader = ptrProp->GetValue();
      if( freader.p )
      {      
        
        MgProxyFeatureReader* proxy_reader = dynamic_cast<MgProxyFeatureReader*>(freader.p);
        proxy_reader->ToXml();
        
        //m_RestRequest->m_DataClassDef
        Ptr<MgClassDefinition> newclassdeff = new MgClassDefinition();
        newclassdeff->SetName(m_RestRequest->m_DataClassDef->GetName());
        newclassdeff->SetDefaultGeometryPropertyName(newclassdeff->GetDefaultGeometryPropertyName());
        Ptr<MgPropertyDefinitionCollection> keyprops = m_RestRequest->m_DataClassDef->GetIdentityProperties();
        Ptr<MgPropertyDefinitionCollection> newprops = newclassdeff->GetProperties();
        Ptr<MgPropertyDefinitionCollection> newkeyprops = newclassdeff->GetIdentityProperties();
        
        for(int ind=0;ind<keyprops->GetCount();ind++)
        {
          Ptr<MgPropertyDefinition> prop = keyprops->GetItem(ind);
          newprops->Add(prop);
          newkeyprops->Add(prop);
        }

        Ptr<c_RestDataReader> restreader = new c_RestDataReader_MgFeatureReader(proxy_reader,newclassdeff);
        
        hResult->SetResultObject(restreader, m_RestRequest->m_CfgRepresentation->m_MimeType);
      }
    }
    else
    {
      throw new MgRuntimeException(L"Execute_POST_Class_Feature",__LINE__, __WFILE__, NULL, L"Error: 'MgPropertyType::Feature' is not returned after insert.", NULL);
    }
    
  }
  else
  {  
    throw new MgRuntimeException(L"Execute_POST_Class_Feature",__LINE__, __WFILE__, NULL, L"Error: 'MgPropertyType::Feature' is not returned after insert.", NULL);
  }
  MG_CATCH_AND_THROW(L"Execute_Post_Class_Feature")  

}//end of c_RestHandler_Data::Execute_Post_Class_Feature





// Update of a feature
void c_RestHandler_Data::Execute_Delete_Class_Feature(c_RestResponse& HttpResponse)
{
      Ptr<c_RestResult> hResult = HttpResponse.GetResult();
      
  Ptr<c_RestUriRequestParam> query_params = m_RestRequest-> GetRequestParam();
  Ptr<c_RestUriPathSegment> path_params = m_RestRequest-> GetUriPathParameters();

  c_CfgDataSource_MgFeatureSource *mg_fsource;
  if( m_RestRequest->m_CfgDataResource->m_DataSource->GetSourceType() !=  c_CfgDataSource::e_MgFeatureSource )
  {
    throw new MgRuntimeException(L"Execute_Delete_Class_Feature",__LINE__, __WFILE__, NULL, L"Unsupported Data Source! MapGuide Feature Source is required.", NULL);
  }
  mg_fsource = (c_CfgDataSource_MgFeatureSource *)m_RestRequest->m_CfgDataResource->m_DataSource;

// Filter which identifies Fetaure to be updated is set in URI segment
  
  MG_TRY()


  MgResourceIdentifier resId(mg_fsource->m_MgFeatureSource); // resource identifier of feature data source

  // Create Proxy Feature Service instance
  //Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(L"Anonymous",L"");    
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(mg_fsource->GetUsername(),mg_fsource->GetPassword(),mg_fsource->GetServerIP(),mg_fsource->GetServerPort());    
  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);

  STRING schema;
  STRING classname;
  STRING::size_type iColon = mg_fsource->m_MgFeatureSourceClassName.find(':');
  if(iColon != STRING::npos) {
    schema = mg_fsource->m_MgFeatureSourceClassName.substr(0,iColon);
    classname   = mg_fsource->m_MgFeatureSourceClassName.substr(iColon+1);
    
  }
  else
  {
    schema = L"";
    classname =mg_fsource->m_MgFeatureSourceClassName;
  }
  
  m_RestRequest->m_DataClassDef = service->GetClassDefinition(&resId, schema, classname);
  

// Geometry is written in URI Query Parameter as a string parameter "_json"
// Geometry is formated in GeoJson format.
  
  

  STRING filterstr;
  CreateFilterString(m_RestRequest->m_DataClassDef,NULL,filterstr);

  Ptr<MgDeleteFeatures> upd_cmd = new MgDeleteFeatures(mg_fsource->m_MgFeatureSourceClassName,filterstr);
  
  Ptr<MgFeatureCommandCollection> commands = new MgFeatureCommandCollection();
  commands->Add(upd_cmd);
  Ptr<MgPropertyCollection> up_ret = service->UpdateFeatures(&resId, commands,false);
  
  //Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, m_RestRequest->m_DataClassName, qryOptions);
  //hResult->SetResultObject(featureReader, MgMimeType::Xml);
  
  STRING str = L"Update Done";
  Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(str);
  hResult->SetResultObject(value, MgMimeType::Text);

  MG_CATCH_AND_THROW(L"Execute_Get_Class_Feature")  

}//end of c_RestHandler_Data::Execute_Delete_Class_Feature