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
#include "c_RestPrimitiveValue.h"
#include "c_RestRequest.h"
#include "c_RestResponse.h"
#include "c_RestUriPathParam.h"
#include "c_RestUriRequestParam.h"
#include "c_RestHandler.h"
#include "c_RestHandler_Data.h"


//#include "RestDefs.h"

#include <algorithm>
using namespace std;




/// <summary>
/// Constructor. Initialize member variables.
/// </summary>
c_RestRequest::c_RestRequest(const std::string& AgentUri,const std::string& BaseUri,const std::string& RestUri,const std::string& HttpMethod,const std::string& XmlPostData)
  : m_RestUri(AgentUri,BaseUri,RestUri,HttpMethod,XmlPostData)  
{
   m_CfgDataResource = NULL;
   m_CfgRepresentation = NULL;
   //m_BaseUri = BaseUri;
   //m_RestUri = RestUri;
   //m_HttpMethod = HttpMethod;
   //m_UriPathParameters = new c_RestUriPathParam();
   
   
   
}

/// <summary>
/// Default constructor to shut Ptr<> up.
/// </summary>
c_RestRequest::c_RestRequest()
{
  m_RestResultObjectStatus = e_Ok;
  m_CfgDataResource = NULL;
  m_CfgRepresentation = NULL;
  //m_UriPathParameters = new c_RestUriPathParam();
}



c_RestUriPathParam* c_RestRequest::GetUriPathParameters()
{
  //return SAFE_ADDREF((c_RestUriPathParam*)m_UriPathParameters);
  
  return m_RestUri.GetUriPathParameters();
}

c_RestUriRequestParam* c_RestRequest::GetRequestParam()
{
  //return SAFE_ADDREF((c_RestUriPathParam*)m_UriPathParameters);

  return m_RestUri.GetRequestParam();
}

void c_RestRequest::REST_Request_Hello(c_RestRequest *RestRequest,c_RestResponse& HttpResponse)
{

  Ptr<c_RestResult> hResult;

  MG_TRY()

  hResult = HttpResponse.GetResult();

STRING ecshostr = L"Hello back from GeoREST - GeoSpatial RESTful Web Services v1.3";

Ptr<c_RestUriRequestParam> queryparams = GetRequestParam();
STRING callback = queryparams->GetParameterValue(D_REST_JSONP_CALLBACK_STR);

STRING ret;

if( callback.size() > 0 )
{
  ret = callback;
  ret.append(L"('");
  ret.append(ecshostr);
  ret.append(L"');");
}
else
{
  ret = ecshostr;
}


Ptr<c_RestPrimitiveValue> value = new c_RestPrimitiveValue(ret);
if(!value)
throw new MgOutOfMemoryException(L"MgHttpCreateSession.Execute", __LINE__, __WFILE__, NULL, L"", NULL);


hResult->SetResultObject(value, MgMimeType::Text);


MG_CATCH(L"REST_Request_Hello.Execute")                                                      

if (mgException != NULL)                                                  
{                                                                         
  if (hResult != NULL)                                                  
  {                                                                      
    hResult->SetErrorInfo(RestRequest, mgException);                   
  }                                                                     
  (*mgException).AddRef();                                              
  mgException->Raise();                                                 
} 

}//end of MgHttpRestRequest::REST_Request_Hello

/// <summary>
/// This method executes the request and returns the response
/// object to the client.
/// </summary>
/// <returns>Pointer to c_RestResponse object</returns>
c_RestResponse* c_RestRequest::Execute()
{
    Ptr<c_RestResponse> response;
    Ptr<c_RestResult> result;

    response = new c_RestResponse();
    result = response->GetResult();
    
    MG_TRY()

    Ptr<c_RestUriRequestParam> query_params = GetRequestParam();
    STRING callbackname = query_params->GetParameterValue(D_REST_JSONP_CALLBACK_STR);
    if( callbackname.length() > 0 )
    {
      std::string mbstr;
      MgUtil::WideCharToMultiByte(callbackname,mbstr);
      response->SetJsonpCallbackName(mbstr);
    }
    STRING restmethod_override = query_params->GetParameterValue(D_REST_METHOD_STR);
    if( restmethod_override.length() > 0 )
    {
      std::string mbstr;
      restmethod_override = MgUtil::ToUpper(restmethod_override);
      MgUtil::WideCharToMultiByte(restmethod_override,mbstr);
      
      SetHttpMethod(mbstr);
    }
    
    bool isrest = false;
    Ptr<c_RestUriPathParam> path_params = GetUriPathParameters();
    //MgStringPropertyCollection* paramlist = uri_params->GetParameters();
    int count = path_params->GetParametersCount();
    
    path_params->ResetParameterCurrentIndex();
    
    
    while( path_params->NextParameter() )
    {
      if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_REST)==0 )
      {
        break;
      }      
    }
    
    //int ind_param = ind_reststart;
    if( !path_params->IsEndOfParameters() )
    {
      
      {
        isrest = true;
        if( path_params->NextParameter() )
        {
          if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_HELLO)==0 )
          {
            
            REST_Request_Hello(this,*response);
          }
          else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_SESSSION)==0 )
          {              
            if( path_params->NextParameter() )
            {
              if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_MAP)==0 )
              {
                // Get handler to requested function
                //Ptr<c_RestHandler> exechandler = MgRestSessionMap::CreateObject(this);

                // Execute request
                //if(exechandler != NULL) 
                //  exechandler->Execute(*response);
              }
            }
            else
            {
              //REST_Request_CreateSession(this,*response);
            }
          }
          else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_TASK)==0 )
          {
            // Get handler to requested function
            //Ptr<c_RestHandler> exechandler = MgRestTask::CreateObject(this);

            // Execute request
            //if(exechandler != NULL)
            //  exechandler->Execute(*response);
          } 
          else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_DATA)==0 )
          {
          /*
            STRING  datafilter;
            STRING  formatdesc;
            STRING  datatagname;
            if( path_params->NextParameter() )
            {
              datatagname = path_params->GetCurrentParameterName();
              if( path_params->NextParameter() )
              {
                datafilter = path_params->GetCurrentParameterName();
                if( path_params->NextParameter() )
                { 
                  formatdesc = path_params->GetCurrentParameterName();
                }
              
                
              }
              Ptr<c_RestHandler> exechandler = MgRestData::CreateObject(this,datatagname.c_str(),datafilter.c_str(),formatdesc.c_str());               
             // Execute request
              if(exechandler != NULL)
                exechandler->Execute(*response);
            }
          */
             Ptr<c_RestHandler> exechandler = c_RestHandler_Data::CreateObject(this);               
             // Execute request
              if(exechandler != NULL)
                exechandler->Execute(*response);
          }
          else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_FDO)==0 )
          {
            if( path_params->NextParameter() )
            {
              if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_FDO_DATASOURCE)==0 )
              {
                if( path_params->NextParameter() )
                {
                  if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_FDO_DATASOURCE_CLASS)==0 )
                  {
                    // Get handler to requested function
                    //Ptr<c_RestHandler> exechandler = MgRestFDO_DataSource_Class::CreateObject(this);

                    // Execute request
                    //if(exechandler != NULL)
                    //  exechandler->Execute(*response);
                  }
                }
              }
            }
            
          } 
          else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_LAYERDEFINITION)==0 )
          {
            // Get handler to requested function
            //Ptr<c_RestHandler> exechandler = MgRestLayerDefinition::CreateObject(this);

            // Execute request
            //if(exechandler != NULL)
            //  exechandler->Execute(*response);
          }
          else if( wcsicmp(path_params->GetCurrentParameterName().c_str(),D_REST_URI_SEGMENT_WEBLAYOUT)==0 )
          {
            
            // Get handler to requested function
            //Ptr<c_RestHandler> exechandler = MgRestWebLayout::CreateObject(this);

            // Execute request
            //if(exechandler != NULL)
            //  exechandler->Execute(*response);
          }
          else
          {
                    
            MgStringCollection arguments;
            arguments.Add(L"0");
            STRING wstr = MgUtil::MultiByteToWideChar(m_RestUri.GetRestUri());
            arguments.Add(wstr);
            throw new MgRuntimeException(L"MgHttpRestRequest.Execute",__LINE__, __WFILE__, &arguments, L"Unknow REST Request", NULL);
          }
        }
      }
    }
    
    if( isrest )
    {
      if( response->GetContentType().length() == 0 )
        response->SetContentType(result->GetResultContentType());
    }
    else
    {
      // response = MgHttpRequest::Execute();
    }
      
      
    response->PrepareHttpData(this);
    
    MG_CATCH(L"MgHttpRestRequest.Execute")
    if (mgException != NULL)                                                  
    {                                                                         
      if (result != NULL)                                                  
      {                                                                     
        result->SetErrorInfo(this, mgException);    
        response->PrepareHttpData(this);               
      }                                                                     
      //(*mgException).AddRef();                                              
      //mgException->Raise();                                                 
    } 
      
    return SAFE_ADDREF((c_RestResponse*)response);
}




void c_RestRequest::Dispose()
{
    delete this;
}

INT32 c_RestRequest::GetClassId()
{
    return m_cls_id;
}

c_RestRequest::~c_RestRequest()
{

}


c_RestUri::e_HttpMethod c_RestRequest::GetHttpMethod()
{
  return m_RestUri.GetHttpMethod();   
}
