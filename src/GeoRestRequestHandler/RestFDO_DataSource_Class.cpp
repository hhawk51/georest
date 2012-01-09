//
//  Copyright (C) 2007 by Haris Kurtagic
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

#include "HttpHandler.h"
#include "WebApp.h"
#include "RestSessionMap.h"
#include "RestUriFilterParser.h"
#include "RestDefs.h"



#include <json/json.h>


MgRestRequestResponseHandler* MgRestFDO_DataSource_Class::CreateObject(MgRestRequest* request) 
{ 
  return new MgRestFDO_DataSource_Class(request); 
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
MgRestFDO_DataSource_Class::MgRestFDO_DataSource_Class(MgRestRequest *Request)
{
    InitializeCommonParameters(Request);
    
    Ptr<MgHttpRequestParam> pathparams = Request->GetUriPathParameters();
    

    
    
    m_FeatureSourceId = pathparams->GetParameterValue(D_REST_URI_SEGMENT_FDO_DATASOURCE);
    m_ClassName = pathparams->GetParameterValue(D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS);
    
    
}

    



/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgRestFDO_DataSource_Class::Execute(MgHttpResponse& HttpResponse)
{
  Ptr<MgHttpResult> hResult = HttpResponse.GetResult();
  Ptr<MgHttpRequestParam> query_params = m_hRequest->GetRequestParam();

  MG_HTTP_HANDLER_TRY()

    //
    // 1. URI for getting data about MAP
    // HttpMethod: GET URI: REST/FDO/DATASOURCE[Id]/CLASSNAME[]/FATURE[Filter]
    
// Check which Resource is requested
// 

  // Current Index should be on Map path parameter
  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();
  
  switch( m_RestRequest->GetHttpMethod() )
  {
    case MgRestRequest::e_HttpMethod_Get:
    {
    // now this Path Segment is last or there is a 
    // another segment with Image.Png, Image.Jpg or Extent
      if( path_params->NextParameter() )
      {
        if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS_FEATURE)==0 )
        {

          Execute_Get_Class_Feature(HttpResponse);
        }        
        
      }
      else
      {
      // not next path segment this is /session/map resource
      }
    }
    break;
       
    case MgRestRequest::e_HttpMethod_Post:
    {
      // This one is creating new Map in Session by opening MapDefinition
      
    }
    break;
    
    case MgRestRequest::e_HttpMethod_Put:
    {
      if( path_params->NextParameter() )
      {
        if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS_FEATURE)==0 )
        {

          Execute_Put_Class_Feature(HttpResponse);
        }        

      }
      else
      {
        // not next path segment this is /session/map resource
      }
    }
    break;
    
    case MgRestRequest::e_HttpMethod_Delete:
    break;
    
    default:
    break;
  }


  MG_CATCH(L"REST_Request_Hello.Execute")                                                      

    if (mgException != NULL)                                                  
    {                                                                         
      if (hResult != NULL)                                                  
      {                                                                      
        hResult->SetErrorInfo(m_hRequest, mgException);                   
      }                                                                     
      (*mgException).AddRef();                                              
      mgException->Raise();                                                 
    } 


}


void MgRestFDO_DataSource_Class::Execute_Get_Class_Feature(MgHttpResponse& HttpResponse)
{
  
  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();
  
  STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS_FEATURE);
  Ptr<MgHttpRequestParam> filterparams = new MgHttpRequestParam();
  RestUriFilterParser::Parse(filter, filterparams);
  
  STRING operation;
  STRING geometry;
  STRING geometryname = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatGeomProperty);

  
  
  STRING spatialOp = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatSpatialOp);
  if(spatialOp.length() == 0)
    operation = MgFeatureSpatialOperations::Within;
  else
    operation = atoi(MgUtil::WideCharToMultiByte(spatialOp).c_str());

  geometry = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatGeometry);
  
  // Create the selection geometry
  MgWktReaderWriter wktReader;
  Ptr<MgGeometry> filterGeometry;
  if( geometry.length() > 0 ) filterGeometry = wktReader.Read(geometry);
  
  Ptr<MgHttpResult> hResult = HttpResponse.GetResult();

  MG_HTTP_HANDLER_TRY()


  MgResourceIdentifier resId(m_FeatureSourceId);

  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

  
  //get addtional params, determine which overload to call and make the call
  STRING filterText = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatFilter);
  STRING propsStr = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatProperties);

  STRING computedProperties = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatComputedProperties);
  STRING computedAliases = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatComputedAliases);

  Ptr<MgStringCollection> featureProps = MgStringCollection::ParseCollection(propsStr, L"\t");
  Ptr<MgStringCollection> computedProps = MgStringCollection::ParseCollection(computedProperties, L"\t");
  Ptr<MgStringCollection> aliases = MgStringCollection::ParseCollection(computedAliases, L"\t");

  Ptr<MgFeatureQueryOptions> qryOptions = new MgFeatureQueryOptions();
  
  
  INT32 selectionVariant = MgFeatureSpatialOperations::Intersects;
  
  if (operation == L"TOUCHES")
    selectionVariant = MgFeatureSpatialOperations::Touches;
  else if (operation == L"INTERSECTS")
    selectionVariant = MgFeatureSpatialOperations::Intersects;
  else if (operation == L"WITHIN")
    selectionVariant = MgFeatureSpatialOperations::Within;
  
  if( geometryname.length() >0 && filterGeometry.p != NULL )
  {
    qryOptions->SetSpatialFilter(geometryname,filterGeometry,selectionVariant);
  }
  
  if( filterText.length()>0 )
    qryOptions->SetFilter(filterText);

  if (featureProps.p != NULL) 
  {
    INT32 cnt = featureProps->GetCount();
    for (INT32 i=0; i < cnt; i++)
    {
      qryOptions->AddFeatureProperty(featureProps->GetItem(i));
    }
  }

  if ((computedProps.p != NULL) && (aliases.p != NULL))
  {
    INT32 aliasCnt = aliases->GetCount();
    INT32 computedCnt = computedProps->GetCount();

    if (aliasCnt == computedCnt)
    {
      for (INT32 j=0; j < aliasCnt; j++)
      {
        qryOptions->AddComputedProperty(aliases->GetItem(j), computedProps->GetItem(j));
      }
    }
    else
    {
      // throw exception here
    }
  }

  Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, m_ClassName, qryOptions);
  hResult->SetResultObject(featureReader, MgMimeType::Xml);
  
  MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"Execute_Get_Class_Feature")  
  
}//end of MgRestFDO_DataSource_Class::Execute_Get_Class_Feature


// Update of a feature
void MgRestFDO_DataSource_Class::Execute_Put_Class_Feature(MgHttpResponse& HttpResponse)
{
      Ptr<MgHttpResult> hResult = HttpResponse.GetResult();
      
  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();


// Filter which identifies Fetaure to be updated is set in URI segment
  STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS_FEATURE);
  Ptr<MgHttpRequestParam> filterparams = new MgHttpRequestParam();
  RestUriFilterParser::Parse(filter, filterparams);

  MG_HTTP_HANDLER_TRY()


  MgResourceIdentifier resId(m_FeatureSourceId); // resource identifier of feature data source


// Geometry is written in URI Query Parameter as a string parameter "_json"
// Geometry is formated in GeoJson format.
  Ptr<MgHttpRequestParam> query_params = m_RestRequest->GetRequestParam();
  STRING wjsonstr = query_params->GetParameterValue(D_REST_JSON_DATA);
  std::string jsonstr;
  MgUtil::WideCharToMultiByte(wjsonstr,jsonstr);

  Json::Value jsonroot;
  Json::Reader jsonreader;
  if( jsonreader.parse(jsonstr,jsonroot,false) == false ) // false parameter == no comments
  {
    MgStringCollection arguments;
    arguments.Add(L"0");
    
    throw new MgInvalidRestUriException(L"Execute_Put_Class_Feature",__LINE__, __WFILE__, &arguments, L"Error parsing JSON input data!", NULL);
  }
  
  if( jsonroot.type() != Json::objectValue )
  {
    MgStringCollection arguments;
    arguments.Add(L"0");
    throw new MgInvalidRestUriException(L"Execute_Put_Class_Feature",__LINE__, __WFILE__, &arguments, L"Error JSON input data. JSON is not an Object!", NULL);  
  }

  //const Json::Value& geomval = jsonroot["geometry"];
  const Json::Value& geomtype = jsonroot["type"];
  const Json::Value& geomcoordinates = jsonroot["coordinates"];
  Ptr<MgGeometry> geom = GeoJsonToMgGeometry(jsonroot);
  


  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

  
  STRING schema;
  STRING classname;
  STRING::size_type iColon = m_ClassName.find(':');
  if(iColon != STRING::npos) {
    schema = m_ClassName.substr(0,iColon);
    classname   = m_ClassName.substr(iColon+1);
    
  }
  else
  {
    schema = L"";
    classname =m_ClassName;
  }
  
  Ptr<MgClassDefinition> classDef = service->GetClassDefinition(&resId, schema, classname);
  
  //get addtional params, determine which overload to call and make the call
  STRING filterText = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatFilter);
  STRING propsStr = filterparams->GetParameterValue(MgHttpResourceStrings::reqFeatProperties);
  Ptr<MgStringCollection> featureProps = MgStringCollection::ParseCollection(propsStr, L"\t");
  
  
// Create list of properties to be updated - Geometry only for now  
  Ptr<MgPropertyCollection> propvalue_coll = new MgPropertyCollection();
  
// create update of geomerty
  STRING geomname = classDef->GetDefaultGeometryPropertyName();
  MgAgfReaderWriter agfrw;
  Ptr<MgByteReader> agf =  agfrw.Write(geom);
  Ptr<MgGeometryProperty> propgeom = new MgGeometryProperty(geomname,agf);
  
  propvalue_coll->Add(propgeom);
  
  Ptr<MgUpdateFeatures> upd_cmd = new MgUpdateFeatures(m_ClassName,propvalue_coll,filter);
  
  Ptr<MgFeatureCommandCollection> commands = new MgFeatureCommandCollection();
  commands->Add(upd_cmd);
  service->UpdateFeatures(&resId, commands,false);
  
  //Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, m_ClassName, qryOptions);
  //hResult->SetResultObject(featureReader, MgMimeType::Xml);
  
  Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(L"Update OK");
  hResult->SetResultObject(value, MgMimeType::Text);

  MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"Execute_Get_Class_Feature")  

}//end of MgRestFDO_DataSource_Class::Execute_Get_Class_Feature


MgGeometry* MgRestFDO_DataSource_Class::GeoJsonToMgGeometry(const Json::Value& GJsonGeometry)
{
  MgGeometryFactory factory;
  const Json::Value& geomtype = GJsonGeometry["type"];
  const Json::Value& geomcoordinates = GJsonGeometry["coordinates"];
  
  MgGeometry* newgeom = NULL;
  if( geomtype.asString() == "Point" )
  {
    Ptr<MgCoordinateXY> coord = new MgCoordinateXY(geomcoordinates[0].asDouble(), geomcoordinates[1].asDouble());
    newgeom = factory.CreatePoint(coord);
    
  } else if( geomtype.asString() == "LineString" )
  {
    const Json::Value& linestring = geomcoordinates;
    if( linestring.isNull() || !linestring.isArray() )
    {
      return NULL;
    }

    Ptr<MgCoordinateCollection> coordcol = GeoJsonToCoordinates(linestring);
    
    newgeom = factory.CreateLineString(coordcol);
    
  } else if( geomtype.asString() == "Polygon" )
  {
    const Json::Value& exterior = geomcoordinates[0];
    if( exterior.isNull() || !exterior.isArray() )
    {
      return NULL;
    }
    
    Ptr<MgCoordinateCollection> coordCol = GeoJsonToCoordinates(exterior);
    
    Ptr<MgLinearRing> extRing = factory.CreateLinearRing(coordCol);      
    newgeom = factory.CreatePolygon(extRing, NULL);
    
    
  } else if( geomtype.asString() == "MultiLineString" )
  {
    const Json::Value& linestring_array = geomcoordinates;
    if( linestring_array.isNull() || !linestring_array.isArray() )
    {
      return NULL;
    }
  
    Ptr<MgLineStringCollection> linecoll = new MgLineStringCollection();
    for(int lineind=0;lineind<linestring_array.size();lineind++)
    {
      const Json::Value& linestring = linestring_array[lineind];
      Ptr<MgCoordinateCollection> coordcol = GeoJsonToCoordinates(linestring);
      Ptr<MgLineString> line = factory.CreateLineString(coordcol);
      linecoll ->Add(line);
    }

    newgeom = factory.CreateMultiLineString(linecoll);
  } else if( geomtype.asString() == "MultiPolygon" )
  {

  } 
  
  return newgeom; 
}

MgCoordinateCollection* MgRestFDO_DataSource_Class::GeoJsonToCoordinates(const Json::Value& CoordinateArray)
{
  if( CoordinateArray.isNull() || !CoordinateArray.isArray() ) return NULL;
  Ptr<MgCoordinateCollection> coordcol = new MgCoordinateCollection();
  double x,y;
  int ind =0;
  int numcoords = CoordinateArray.size();
  while(ind<numcoords)
  {
    const Json::Value& point = CoordinateArray[ind++]; // point
    if( point.isNull() || !point.isArray() ) return NULL;
    
    x = point[0].asDouble();
    y = point[1].asDouble();

    Ptr<MgCoordinate> coord = new MgCoordinateXY(x, y);
    coordcol->Add(coord);     
  }
  
  return SAFE_ADDREF(coordcol.p);
}