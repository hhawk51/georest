// GeoRestCGI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RestCommonInclude.h"
#include "RestRequestHandlerInclude.h"
#include "RestConfigInclude.h"
#include "c_RestRequest.h"
#include "c_RestConfig.h"

#include <stdlib.h>
#include <string>
#include <iostream>

#include <sstream>
using namespace std;

#include "MapAgentStrings.h"
#include "Poco\Net\HTTPResponse.h"

/*
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}
*/
int main (int argc, char* argv[])
{
#ifdef _DEBUG
  char szMessage [256];
  wsprintf (&szMessage[0], "Please attach a debugger to the process 0x%X (%s) and click OK",
    GetCurrentProcessId(), argv[0]);
  MessageBox(NULL, szMessage, "CGI Debug Time!",
    MB_OK|MB_SERVICE_NOTIFICATION);
#endif

  //printf("Content-type: text/html\n\n");
  //printf("GeoREST CGI : Hello");
  //std::cout << "Content-type: text/html\n\n";
  //std::cout << "GeoREST CGI : Hello2";
  //return 0;

  // Construct self Url.  It is embedded into the output stream
  // of some requests (like GetMap).  Use a fully qualified URL.
  char* serverName = getenv(MapAgentStrings::ServerName);
  char* serverPort = getenv(MapAgentStrings::ServerPort);
  char* scriptName = getenv(MapAgentStrings::ScriptName);
  char* remoteAddr = getenv(MapAgentStrings::RemoteAddr);
  char* httpClientIp = getenv(MapAgentStrings::HttpClientIp);
  char* httpXFF = getenv(MapAgentStrings::HttpXForwardedFor);
  char* secure = getenv(MapAgentStrings::Secure);
  std::string agenturi = secure != NULL && (!_stricmp(secure, "on") || !_stricmp(secure, "true")) ? MapAgentStrings::Https : MapAgentStrings::Http;  // NOXLATE
  if (NULL != serverName && NULL != serverPort && NULL != scriptName)
  {
    agenturi.append(serverName);
    agenturi += ':';
    agenturi.append(serverPort);
    agenturi.append(scriptName);
  }
  
  string uri_base,uri_rest;
  bool isrest_uri=false;
  
  std::basic_string <char>::size_type rpos = agenturi.find("/rest/");
  if( rpos == std::string::npos )
  {
    rpos = agenturi.find("/REST/");

  }
  if( rpos != std::string::npos )
  {
    uri_base = agenturi.substr(0,rpos);
    uri_rest = agenturi.substr(rpos,agenturi.length()-rpos);
    isrest_uri=true;
  }
  else
  {
    // it is not REST request

    uri_base = uri_base;
    uri_rest = "";
    printf("Content-type: text/html\n\n");
    printf("GeoREST CGI : It is not REST call! /rest/ keyword not found in URI.");
    return 0;
  }
  
  const std::string& httpmethod = getenv(MapAgentStrings::RequestMethod);
  
  std::string post_data;
  if( httpmethod == "POST" || httpmethod == "PUT" )
  {
    char* contentLength = getenv(MapAgentStrings::ContentLength);
    size_t nBytes = 0;
    if (NULL != contentLength)
    {
      nBytes = atoi(contentLength);
      if (nBytes > 0)
      {
        char* chbuf = (char*)malloc(nBytes+1);
        size_t readBytes = fread(chbuf, 1, nBytes, stdin);
        chbuf[readBytes]=0;
        post_data.append(chbuf);
        free(chbuf);
      }
    }
    
    
  }
  Ptr<c_RestRequest> restrequest = new c_RestRequest(agenturi,uri_base,uri_rest,httpmethod,post_data);
 
  Ptr<c_RestResponse> restresponse = restrequest->Execute();
  //c_RestResponse_HttpData* http_data = restresponse->PrepareHttpData(restrequest);
  c_RestResponse_HttpData* http_data = restresponse->GetHttpData();

  //std::istream istr(http_data->GetHeader());

  Poco::Net::MessageHeader& headers = http_data->GetMsgHeaders();
  char temphead[64];
  sprintf(temphead,"%d %s",http_data->GetStatus(),http_data->GetStatusReason());
  headers.set("Status",temphead);
  

  
  Ptr<MgByteReader> bytereader = http_data->GetContentByteReader();

  ostringstream ss;
  headers.write(ss);
  string s1 = ss.str();
  std::cout << s1;
  std::cout << "\r\n";
  
  
  //response.write(std::cout);
  if( bytereader.p )
  {
    //MgByteSource* bytes = bytereader->GetByteSource();
    //ByteSourceImpl* bsrcimpl = bytes->GetSourceImpl();
    //std::ostream& outstr = response.send(); 

    unsigned char buf[4096];
    DWORD dwSize;
    int nBytes = bytereader->Read(buf,4096);
    while (nBytes > 0)
    {
      dwSize = nBytes;
      //response.sendBuffer(buf,nBytes); 
      std::cout.write((char*)&buf[0],nBytes);
      nBytes = bytereader->Read(&buf[0],4096);                                
    }


  }
  else
  {   
    /* 
    //std::ostream& outstr = response.send(); 
    //std::cout << "HTTP/1.1 200 OK\r\n";
    //std::cout << "Content-Length: 64\r\n";
    std::cout << "Content-Type: text/plain; charset=utf-8\r\n";
    std::cout << "Status: 200 OK\r\n";
    std::cout << "\r\n";
    */
    
    std::cout << http_data->GetContentString();
        
  }
  return 0;
}
