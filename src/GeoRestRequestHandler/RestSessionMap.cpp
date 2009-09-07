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

MgRestRequestResponseHandler* MgRestSessionMap::CreateObject(MgRestRequest* request) 
{ 
  return new MgRestSessionMap(request); 
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
MgRestSessionMap::MgRestSessionMap(MgRestRequest *Request)
{
    InitializeCommonParameters(Request);
    
    Ptr<MgHttpRequestParam> pathparams = Request->GetUriPathParameters();
    
    // Get any map name
    m_WebLayout = pathparams->GetParameterValue(D_REST_URI_SEGMENT_WEBLAYOUT);
    
    m_MapName = pathparams->GetParameterValue(D_REST_URI_SEGMENT_MAP);

    

}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgRestSessionMap::Execute(MgHttpResponse& HttpResponse)
{
  Ptr<MgHttpResult> hResult = HttpResponse.GetResult();
  Ptr<MgHttpRequestParam> query_params = m_hRequest->GetRequestParam();

  MG_HTTP_HANDLER_TRY()

    //
    // 1. URI for getting data about MAP
    // HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]
    //
    // 2. URI creating new MAP in Session
    // HttpMethod: POST URI: REST/SESSION[Id]/MAP ? MapDefName={Map Definition Name}
    // 
    // 3. Get Image Extent
    // HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]/Extent
    //
    // 4. Get Image ( return png )
    // HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]/Image.Png[...ImageCreationParameters...] {.jpg }
    //
    // 5. Set Image Extent
    // HttpMethod: PUT URI: REST/SESSION[Id]/MAP[Id]/Extent ? {SETCETNTERX=..}&{SETCENTERY=..}&..   
    //

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
        if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_MAP_IMAGE_PNG)==0 )
        {

          Execute_Get_SessionMapImage_Png(HttpResponse);
        }
        else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_MAP_IMAGE_JPG)==0 )
        {
          Execute_Get_SessionMapImage_Jpg(HttpResponse);
        }
        else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_MAP_QUERYMAPFEATURES)==0 )
        {
          Execute_Get_SessionMap_QueryMapFeatures(HttpResponse);
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


void MgRestSessionMap::Execute_Get_SessionMap_QueryMapFeatures(MgHttpResponse& HttpResponse)
{
  
  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();
  
  STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_MAP_QUERYMAPFEATURES);
  Ptr<MgHttpRequestParam> filterparams = new MgHttpRequestParam();
  RestUriFilterParser::Parse(filter, filterparams);
  
//-------------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------------
  STRING  m_layerNames;
  STRING  m_geometry;
  STRING  m_selectionVariant;
  STRING  m_featureFilter;
  INT32   m_maxFeatures;
  INT32   m_layerAttributeFilter;
  bool    m_persist;
  
  

  // Get the requested layer names
  m_layerNames = filterparams->GetParameterValue(MgHttpResourceStrings::reqRenderingLayerNames);

  // Get maximum number of features to select
  string maxFeatures = MgUtil::WideCharToMultiByte(filterparams->GetParameterValue(MgHttpResourceStrings::reqRenderingMaxFeatures));
  if(maxFeatures.length() == 0)
    m_maxFeatures = -1;
  else
    m_maxFeatures = atoi(maxFeatures.c_str());

  // Get the geometry WKT string
  m_geometry = filterparams->GetParameterValue(MgHttpResourceStrings::reqRenderingGeometry);

  // Get the selection variant
  m_selectionVariant = filterparams->GetParameterValue(MgHttpResourceStrings::reqRenderingSelectionVariant);

  // Flag indicating if the selection should be stored in the session repository
  m_persist = filterparams->GetParameterValue(MgHttpResourceStrings::reqRenderingPersist) == L"1";

  // Get the layer attribute filter bitmask
  // 1=Visible
  // 2=Selectable
  // 4=HasTooltips
  string layerAttributeFilter = MgUtil::WideCharToMultiByte(filterparams->GetParameterValue(MgHttpResourceStrings::reqRenderingLayerAttributeFilter));
  if(layerAttributeFilter.length() == 0)
    m_layerAttributeFilter = 3; //visible and selectable
  else
    m_layerAttributeFilter = atoi(layerAttributeFilter.c_str());

  // Get the feature filter
  m_featureFilter = filterparams->GetParameterValue(MgHttpResourceStrings::reqRenderingFeatureFilter); 
  
  // Create the list of layers to include in the selection
  Ptr<MgStringCollection> layerNames = MgStringCollection::ParseCollection(m_layerNames, L",");

  // Create the selection geometry
  MgWktReaderWriter wktReader;
  Ptr<MgGeometry> filterGeometry = wktReader.Read(m_geometry);

  // Create the selection variant
  INT32 selectionVariant = 0;
  if(m_selectionVariant.length() > 0)
  {
    if (m_selectionVariant == L"TOUCHES")
      selectionVariant = MgFeatureSpatialOperations::Touches;
    else if (m_selectionVariant == L"INTERSECTS")
      selectionVariant = MgFeatureSpatialOperations::Intersects;
    else if (m_selectionVariant == L"WITHIN")
      selectionVariant = MgFeatureSpatialOperations::Within;
    else    // add more values if necessary
    {
      MgStringCollection arguments;
      arguments.Add(L"0");
      arguments.Add(m_selectionVariant);

      throw new MgInvalidArgumentException(L"MgHttpQueryMapFeatures.Execute",
        __LINE__, __WFILE__, &arguments, L"MgInvalidFeatureSpatialOperation", NULL);
    }
  }

  // Call the HTML controller to process the request
  MgHtmlController controller(m_siteConn);
  Ptr<MgByteReader> featureDescriptionInfo = controller.QueryMapFeatures(
    m_MapName, layerNames, filterGeometry, selectionVariant, m_featureFilter, m_maxFeatures, m_persist, m_layerAttributeFilter);

  // Set the result
  Ptr<MgHttpResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(featureDescriptionInfo, featureDescriptionInfo->GetMimeType()); 
  
}//end of MgRestSessionMap::Execute_Get_SessionMap_QueryMapFeatures

void MgRestSessionMap::Execute_Get_SessionMapImage_Png(MgHttpResponse& HttpResponse)
{
  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();
  
  STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_MAP_IMAGE_PNG);
  
  Ptr<MgHttpRequestParam> filterparams = new MgHttpRequestParam();
  RestUriFilterParser::Parse(filter, filterparams);
  
  MgHtmlController controller(m_siteConn);
  
  // Get the map view commands
  Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();
  //Ptr<MgPropertyCollection> mapViewCommands = params->GetParameters()->GetPropertyCollection();
  Ptr<MgPropertyCollection> mapViewCommands = filterparams->GetParameters()->GetPropertyCollection();
  
  Ptr<MgByteReader> map = controller.GetDynamicMapOverlayImage_ViewCommands(m_MapName, MgImageFormats::Png, true,mapViewCommands);

  // Set the result
  Ptr<MgHttpResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(map, map->GetMimeType());
}

void MgRestSessionMap::Execute_Get_SessionMapImage_Jpg(MgHttpResponse& HttpResponse)
{
  Ptr<MgRestUriPathParam> path_params = m_RestRequest-> GetUriPathParameters();

  STRING filter = path_params->GetParameterValue(D_REST_URI_SEGMENT_MAP_IMAGE_JPG);

  Ptr<MgHttpRequestParam> filterparams = new MgHttpRequestParam();
  RestUriFilterParser::Parse(filter, filterparams);

  MgHtmlController controller(m_siteConn);

  // Get the map view commands
  Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();
  //Ptr<MgPropertyCollection> mapViewCommands = params->GetParameters()->GetPropertyCollection();
  Ptr<MgPropertyCollection> mapViewCommands = filterparams->GetParameters()->GetPropertyCollection();

  Ptr<MgByteReader> map = controller.GetDynamicMapOverlayImage_ViewCommands(m_MapName, MgImageFormats::Jpeg, true,mapViewCommands);

  // Set the result
  Ptr<MgHttpResult> result;
  result = HttpResponse.GetResult();
  result->SetResultObject(map, map->GetMimeType());
}