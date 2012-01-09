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

#include "stdafx.h"
//#include "WebApp.h"
#include "RestWebLayout.h"
#include "RestDefs.h"

MgRestRequestResponseHandler* MgRestWebLayout::CreateObject(MgRestRequest* request) 
{ 
  return new MgRestWebLayout(request); 
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
MgRestWebLayout::MgRestWebLayout(MgRestRequest *Request)
{
    InitializeCommonParameters(Request);
    
    Ptr<MgHttpRequestParam> params = Request->GetUriPathParameters();
    
    // Get any map name
    m_WebLayout = params->GetParameterValue(D_REST_URI_SEGMENT_WEBLAYOUT);

    

}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgRestWebLayout::Execute(MgHttpResponse& HttpResponse)
{
  Ptr<MgHttpResult> hResult = HttpResponse.GetResult();
  Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();

  MG_HTTP_HANDLER_TRY()

  STRING sessionId = m_userInfo->GetMgSessionId();
  
  Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(CreateService(MgServiceType::ResourceService));

  bool openedMap = false;
  // Create MgWebLayout
  
  Ptr<MgResourceIdentifier> resid = new MgResourceIdentifier(m_WebLayout);
  Ptr<MgWebLayout> weblayout = new MgWebLayout(resourceService,resid);
  
  STRING mapdefname = weblayout->GetMapDefinition();
  
  //Ptr<MgLayerGroupCollection> group_coll = map->GetLayerGroups();
  //Ptr<MgLayerCollection> layer_coll = map->GetLayers();

  STRING callback = params->GetParameterValue(D_REST_JSONP_CALLBACK_STR);

  // create JSON fromat
  STRING json = L"({\"m_MapDefName\":\"";
  json.append(mapdefname);
  json.append(L"\"})");

  STRING ret;

  if( callback.size() > 0 )
  {
    ret = callback;
    ret.append(L"('");
    ret.append(json);
    ret.append(L"');");
  }
  else
  {
    ret = json;
  }


  Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(ret);
  if(!value)
    throw new MgOutOfMemoryException(L"MgHttpCreateSession.Execute", __LINE__, __WFILE__, NULL, L"", NULL);




  Ptr<MgHttpResult> result;


  result = HttpResponse.GetResult();
  result->SetResultObject(value, MgMimeType::Text);


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
