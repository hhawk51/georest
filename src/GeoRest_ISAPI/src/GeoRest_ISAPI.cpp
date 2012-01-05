//
//  Copyright (C) 2004-2007 by Haris Kurtagic
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


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <httpext.h>
#include <stdio.h>
#include "threadpool.h"

#include "MapGuideCommon.h"
//#include "HttpHandler.h"
//#include "WebSupport.h"
#include "IsapiPostParser.h"
//#include "Base64.h"
#include "IsapiResponseHandler.h"
//#include "MapAgentCommon.h"
//#include "MapAgentGetParser.h"
#include "MapAgentStrings.h"

#include "RestConfigInclude.h"
#include "RestRequestHandlerInclude.h"
#include "c_RestRequest.h"
#include "c_RestResponse.h"
//#include "RestUriParser.h"
//#include "RestDefs.h"

STRING gConfigPath;

string g_HtmlTemplatePath;;

DWORD proccess_id;

BOOL WINAPI DllMain(IN HINSTANCE hinstDll, IN DWORD fdwReason, IN LPVOID lpvContext) 
{
  BOOL fReturn = TRUE;

  switch (fdwReason) {

    case DLL_PROCESS_ATTACH :
      proccess_id = GetCurrentProcessId();

      fReturn = InitThreadPool();

      break;
  }

  return fReturn;
}


BOOL WINAPI GetExtensionVersion(OUT HSE_VERSION_INFO *pVer)
{
  pVer->dwExtensionVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);

  lstrcpyn(pVer->lpszExtensionDesc, "MapGuide REST ISAPI Extension ", HSE_MAX_EXT_DLL_NAME_LEN);

  return TRUE;
}

/*
Description:

Demonstrate usage of persistent connections serviced by a thread pool.

Arguments:

pECB - points to the extension control block

Returns:

HSE_STATUS_PENDING if request was successfully queued 
HSE_SUCCESS_AND_KEEP_CONN if request was served immediately
(presumably because the queue was full)
*/


DWORD WINAPI HttpExtensionProc(IN EXTENSION_CONTROL_BLOCK *pECB)
{
  DWORD dwSize;
  HSE_SEND_HEADER_EX_INFO HeaderExInfo;

  char szHeader[] = 
    "Connection: Keep-Alive\r\n"
    "Content-Length: %lu\r\n"
    "Content-type: text/html\r\n\r\n";

  char szBusyMessage[] = 
    "<html> <form method=get action=KeepAliveP.dll> <input type=submit> "
    "<br>pECB->ConnID=%lu  <br>Server was too busy. </form></html>";

  char szBuffer[4096];
  char szBuffer2[4096];

  EnterCriticalSection(&csQueueLock);

  if (!AddWorkQueueEntry(pECB)) {

    // if ECB could not be assigned 

    LeaveCriticalSection(&csQueueLock);

    sprintf(szBuffer2, szBusyMessage, pECB->ConnID);

    // Send outgoing header 

    sprintf(szBuffer, szHeader, strlen(szBuffer2));

    HeaderExInfo.pszHeader = szBuffer;
    HeaderExInfo.cchHeader = strlen( szBuffer );
    HeaderExInfo.pszStatus = "200 OK";
    HeaderExInfo.cchStatus = strlen( HeaderExInfo.pszStatus );
    HeaderExInfo.fKeepConn = TRUE;

    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &HeaderExInfo, NULL, NULL);

    // Send content 

    dwSize = strlen(szBuffer2);

    pECB->WriteClient(pECB->ConnID, szBuffer2, &dwSize, 0);

    return HSE_STATUS_SUCCESS_AND_KEEP_CONN;

  } else {

    // release 1 thread from the pool 

    ReleaseSemaphore(hWorkSem, 1, NULL);

    LeaveCriticalSection(&csQueueLock);
  }

  return HSE_STATUS_PENDING;
}


/*
Description:

This function is called when the WWW service is shutdown

Arguments:

dwFlags - HSE_TERM_ADVISORY_UNLOAD or HSE_TERM_MUST_UNLOAD

Return Value:

TRUE if extension is ready to be unloaded,
FALSE otherwise
*/
extern void CloseThreadPool(void);
BOOL WINAPI TerminateExtension(IN DWORD dwFlags)
{
  CloseThreadPool();
  return TRUE;
}

// Initialize only once per process
static bool m_bInitialized = false;
void Initialize(EXTENSION_CONTROL_BLOCK *pECB)
{
  
  if (!m_bInitialized)
  {
    DWORD dwSize = 1024;
    char path[1024];
    pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::PhysicalPath, path, &dwSize);

    if (strlen(path) > 0)
    {
      gConfigPath = MgUtil::MultiByteToWideChar(string(path));
      g_HtmlTemplatePath = string(path);
      g_HtmlTemplatePath.append("HtmlTemplates\\");
    } 

    //STRING configFile = gConfigPath;
    //configFile.append(L"\\");
    //configFile.append(MapAgentStrings::WebConfig);

    MG_TRY()
      m_bInitialized = true;
      //MgInitializeWebTier(configFile);

    MG_CATCH_AND_THROW(L"IsapiAgent.Initialize");
  }
}

/*
Description:

Worker thread function - simulates extended processing of the HTTP request

Arguments:

pvThreadNum - thread number

Returns:

alsways returns 0
*/

/*
DWORD WINAPI Execute(EXTENSION_CONTROL_BLOCK *ECB)
{
  return HSE_STATUS_SUCCESS;
}
DWORD WINAPI HttpExtensionProc(IN EXTENSION_CONTROL_BLOCK *ECB)
*/
DWORD WINAPI Execute(EXTENSION_CONTROL_BLOCK *ECB)
{
  string uri_base;
  string agent_uri;
  Ptr<c_RestRequest> request;
  
  MG_TRY()
  
  Initialize(ECB);

  // Construct self Url.  It is embedded into the output stream
  // of some requests (like GetMap).  Use a fully qualified URL.
  // TODO: Do we need to worry about https:// here?
  DWORD size = 1024;

  char serverName[1024+8];
  ECB->GetServerVariable(ECB->ConnID, (LPSTR)MapAgentStrings::ServerName, serverName, &size);

  size = 1024;
  char serverPort[1024+8];
  ECB->GetServerVariable(ECB->ConnID, (LPSTR)MapAgentStrings::ServerPort, serverPort, &size);

  size = 4096;
  char httpuri_buff[4096+8];
  // I am not using ScriptName server variable because it will not retunr correct result if I send two slashes inside uri ( returns just one)
  ECB->GetServerVariable(ECB->ConnID, "HTTP_URL" /*(LPSTR)MapAgentStrings::ScriptName*/, httpuri_buff, &size);
  
  char httpuri_pathinfo[4096+8];
  // I am not using ScriptName server variable because it will not retunr correct result if I send two slashes inside uri ( returns just one)
  ECB->GetServerVariable(ECB->ConnID, "SCRIPT_NAME" /*(LPSTR)MapAgentStrings::ScriptName*/, httpuri_pathinfo, &size);
  
  //ECB->GetServerVariable(ECB->ConnID, (LPSTR)MapAgentStrings::ScriptName, scriptnamebuff2, &size);

  std::string httpfulluri = httpuri_buff;

  string url = MapAgentStrings::Http;
  if (NULL != serverName && NULL != serverPort && NULL != httpuri_buff)
  {
    url.append(serverName);
    if( strcmp(serverPort,"80") != 0 )
    {
      url += ':';
      url.append(serverPort);
    }    
    url.append(httpfulluri);
  }
  agent_uri = url;
  STRING wUrl = MgUtil::MultiByteToWideChar(url);
  std::string pathinfo = ECB->lpszPathInfo;
  
  // Figure out which HttpMethod is used
  // It is set in HttpRequest Header or it can be overriden in URI Qury parameters by _httpmethod parameter

  std::string httpmethod = ECB->lpszMethod;


//------------------------------------------------------
// Parse full path to Base URI (www.sl-kingcom/mapguide/ )and REST URI ( REST/
// wUrl is for existing HttpRequest to keep exisitn functionality
// htpmethod is method which is ude by http connection - it can be overriden by query Parameter "_method"
//------------------------------------------------------
  bool isrest_uri=false;
  string uri_rest;
  
  c_RestConfig * restcfg = c_RestConfig::GetInstance();
  const char *resturi_separator = restcfg->GetRestUriSeparator();
  
  basic_string <char>::size_type rpos = agent_uri.find("/REST/");
  if( rpos == string::npos )
  {
    rpos = agent_uri.find("/rest/");
    
  }
  if( rpos != string::npos )
  {
    uri_base = agent_uri.substr(0,rpos);
    uri_rest = agent_uri.substr(rpos,agent_uri.length()-rpos);
    isrest_uri=true;
  }
  else
  {
    uri_base = agent_uri;
    uri_rest = "";
  }
  /*
  // create uribase with full server name
  std::string serveruri = MapAgentStrings::Http;
  serveruri = serveruri + serverName;
  if( strcmp(serverPort,"80") != 0 )
  {
    serveruri =  serveruri + ":" + serverPort;
  }
  if( uri_base[0] == '/' )
    uri_base =  serveruri + uri_base;
  else
    uri_base =  serveruri + "/"  + uri_base;
  */
  
//------------------------------------------------------
// Create Rest Request
// wUrl is for existing HttpRequest to keep exisitn functionality
// htpmethod is method which is ude by http connection - it can be overriden by query Parameter "_method"
// uri_base is a base part of request ( www.sl-king.com )
// uri_rest is a rest part of request ( /REST/... )
//------------------------------------------------------
  
  std::string xmlpostdata = ""; //post xml data will be set beloow in isapipostparser call
  request = new c_RestRequest(agent_uri,uri_base,uri_rest,httpmethod,xmlpostdata); 
  
  Ptr<c_RestUriRequestParam> query_params = request->GetRequestParam();
  Ptr<c_RestUriPathSegment> path_params = request->GetUriPathParameters();

//------------------------------------------------------
// Parse input GET/POST Query Parameters
//------------------------------------------------------
  char* query = ECB->lpszQueryString;
  char* requestMethod = ECB->lpszMethod;

  IsapiPostParser postParser(ECB);

  if (NULL != requestMethod && ((strstr(requestMethod, "POST") != NULL) || (strstr(requestMethod, "PUT") != NULL) ) )  // NOXLATE
  {
    //MapAgentGetParser::Parse(query, query_params);
    c_RestUri::ParseQuery(query,query_params);
    // Must be a POST request
    postParser.Parse(query_params);
    
  }
  else if (NULL != query && strlen(query) > 0)
  {
    // If we have a QUERY_STRING then we are a GET request
    //MapAgentGetParser::Parse(query, query_params);
    c_RestUri::ParseQuery(query,query_params);
  }
    

//------------------------------------------------------
// Parse REST URI and create Path Parameters 
//------------------------------------------------------  
  //RestUriParser::Parse(http_fulluri, path_params);
  //c_RestUri::ParsePath(uri_rest, path_params);
  
  
//------------------------------------------------------
// Check for Session Id in REST PATH and added as Query Parameter so 
// exisitng HttpRequest can use that SessionId 
//------------------------------------------------------    
  
  /*
  if( path_params->ContainsParameter(D_REST_URI_SEGMENT_SESSSION) )
  {
    STRING sessionval = path_params->GetParameterValue(D_REST_URI_SEGMENT_SESSSION);
    if( sessionval.size() > 0 )
    {
      query_params->AddParameter(MgHttpResourceStrings::reqSession,sessionval);  
    }
  }
  */
  size = 1024;
  char buff[1204+1];
  buff[0]=0;
  if( ECB->GetServerVariable(ECB->ConnID, "HTTP_Accept", buff, &size) )
  {
    request->SetHeaderValue("Accept",buff);
  }
  buff[0]=0;
  
  if( ECB->GetServerVariable(ECB->ConnID, "HTTP_DataServiceVersion", buff, &size) )
  {
    request->SetHeaderValue("DataServiceVersion",buff);
  }
  buff[0]=0;
  if( ECB->GetServerVariable(ECB->ConnID, "HTTP_MaxDataServiceVersion", buff, &size) )
  {
    request->SetHeaderValue("MaxDataServiceVersion",buff);
  }
  
  if( ECB->GetServerVariable(ECB->ConnID, "Authorization", buff, &size) )
  {
    request->SetHeaderValue("Authorization",buff);
  }
  
  IsapiResponseHandler responseHandler(request,agent_uri,uri_base,ECB);
  Ptr<c_RestResponse> response = request->Execute();

  responseHandler.SendResponse(response,request);
  
  
  /*

  // Check for HTTP Basic Auth header
  size = 1024;
  char auth[1024];
  ECB->GetServerVariable(ECB->ConnID, (LPSTR)MapAgentStrings::HttpAuth, auth, &size);
  bool gotAuth = MapAgentCommon::ParseAuth(auth, query_params);

  if (!gotAuth)
  {
    // And check for a REMOTE_USER remapped header
    size = 1024;
    ECB->GetServerVariable(ECB->ConnID, (LPSTR)MapAgentStrings::HttpRemoteUser, auth, &size);
    gotAuth = MapAgentCommon::ParseAuth(auth, query_params);
  }

  Ptr<MgPropertyCollection> paramList = query_params->GetParameters()->GetPropertyCollection();
  if (paramList != NULL)
  {
    IsapiResponseHandler responseHandler(request,agent_uri,uri_base,ECB);

    // Check to be sure that we have some kind of credentials before continuing.  Either
    // username/password or sessionid.
    bool bValid = paramList->Contains(MgHttpResourceStrings::reqSession);
    // Strike two: no session? how about a username?
    if (!bValid)
      bValid = paramList->Contains(MgHttpResourceStrings::reqUsername);
    // Strike three: no username either? How about if it's an XML POST
    if (!bValid)
      bValid = query_params->GetXmlPostData().length() != 0;

    if (!bValid)
    {
      // Added that Anonymous is used by default
      if( isrest_uri )
      {
        Ptr<MgStringProperty> prop = new MgStringProperty(MgHttpResourceStrings::reqUsername,L"Anonymous");
        paramList->Add(prop);
        Ptr<MgStringProperty> prop2 = new MgStringProperty(MgHttpResourceStrings::reqPassword,L"");
        paramList->Add(prop2);
      }
      else
      {
      
        responseHandler.RequestAuth(); // Invalid authentication information is not fatal, we should continue.
        return HSE_STATUS_SUCCESS;
      }
    }

    Ptr<c_RestResponse> response = request->Execute();

    responseHandler.SendResponse(response);
  }
  */

  MG_CATCH(L"ISAPI execute")
  if (mgException != NULL)                                                  
  {       
    IsapiResponseHandler responseHandler(request,agent_uri,uri_base,ECB);
    responseHandler.SendError(mgException);    
    
  }   

  return HSE_STATUS_SUCCESS;
}

DWORD WINAPI WorkerFunction(IN LPVOID pvThreadNum)
{
  EXTENSION_CONTROL_BLOCK *pecb;
  DWORD dwRet, dwState, dwSize;
  HSE_SEND_HEADER_EX_INFO HeaderExInfo;

  DWORD dwThreadNum = (DWORD)pvThreadNum;

  // This header will be filled in with the content length 



  for (;;) {

    dwRet = WaitForSingleObject(hWorkSem, INFINITE);

    if (dwRet == WAIT_OBJECT_0) {

      EnterCriticalSection(&csQueueLock);

      if (GetWorkQueueEntry(&pecb)) {	

        // Found work to do 

        LeaveCriticalSection(&csQueueLock);

        Execute(pecb);

      } else {			

        // No item found is unexpected condition - exit thread 

        LeaveCriticalSection(&csQueueLock);

        ExitThread(0);
      }

    } else {

      // Leave the infinite loop 

      break;
    }
  }

  return 0;
}

/*
DWORD WINAPI WorkerFunction(IN LPVOID pvThreadNum)
{
  EXTENSION_CONTROL_BLOCK *pECB;
  DWORD dwRet, dwState, dwSize;
  HSE_SEND_HEADER_EX_INFO HeaderExInfo;

  DWORD dwThreadNum = (DWORD)pvThreadNum;

  // This header will be filled in with the content length 

  char szHeader[] = 
    "Connection: Keep-Alive\r\nContent-Length: %lu\r\n"
    "Content-type: text/html\r\n\r\n";
    
    //"Connection: Keep-Alive\r\n"
    


//  char szContent[] = 
//    "<html> <form method=get action=KeepAliveP.dll><input type=submit> " 
//    "<br>pECB->ConnID=%lu  <br>dwThreadNum=%lu</form></html>";
    

  
  
  char header_buff[4096];
  char content_buff[4096];

  for (;;) {

    dwRet = WaitForSingleObject(hWorkSem, INFINITE);

    if (dwRet == WAIT_OBJECT_0) {

      EnterCriticalSection(&csQueueLock);

      if (GetWorkQueueEntry(&pECB)) {	

        // Found work to do 
 
        LeaveCriticalSection(&csQueueLock);

        

        sprintf(content_buff, "<html><body> <br>lpszMethod=%s</br>"
                              "<br>lpszPathInfo=%s</br> </body></html>"
                              "<br>lpszPathTranslated=%s</br> </body></html>"
                              "<br>lpszQueryString=%s</br> </body></html>"
                              , pECB->lpszMethod
        , pECB->lpszPathInfo, pECB->lpszPathTranslated, pECB->lpszQueryString);
        
        // Send outgoing header 

        sprintf(header_buff, szHeader, strlen(content_buff));
        //sprintf(szBuffer, szHeader);

        HeaderExInfo.pszHeader = header_buff;
        HeaderExInfo.cchHeader = strlen( header_buff );
        HeaderExInfo.pszStatus = "200 OK";
        HeaderExInfo.cchStatus = strlen( HeaderExInfo.pszStatus );
        HeaderExInfo.fKeepConn = TRUE;

        pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &HeaderExInfo, NULL, NULL);

        // Simulate extended processing 

        Sleep(1000);

        // Send content 

        dwSize = strlen(content_buff);

        pECB->WriteClient(pECB->ConnID, content_buff, &dwSize, 0);

        // Tell IIS to keep the connection open 

        dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;

        pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);
        
        //pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, NULL, NULL, NULL);

      } else {			
 
        // No item found is unexpected condition - exit thread 

        LeaveCriticalSection(&csQueueLock);

        ExitThread(0);
      }

    } else {

      // Leave the infinite loop 

      break;
    }
  }

  return 0;
}
*/
