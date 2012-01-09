// GeoRestCGI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RestCommonInclude.h"
#include "RestRequestHandlerInclude.h"
#include "RestConfigInclude.h"
#include "c_RestRequest.h"
#include "c_RestConfig.h"

#include "fcgi_stdio.h"
#include <stdlib.h>
#include <string>
#include <iostream>

#include <sstream>
using namespace std;

#include "MapAgentStrings.h"
#include "Poco\Net\HTTPResponse.h"
#include "Poco\UnicodeConverter.h"

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

#ifdef _WIN32
  // Set stdin and stdout to binary mode
  _setmode(0, _O_BINARY);
  _setmode(1, _O_BINARY);
#else
  //TODO: Does this need to be done under Linux?
#endif

  //printf("Content-type: text/html\n\n");
  //printf("GeoREST CGI : Hello");
  //std::cout << "Content-type: text/html\n\n";
  //std::cout << "GeoREST CGI : Hello2";
  //return 0;
  char* mentor1 = getenv("MENTOR_DICTIONARY_PATH");
  char* path1 = getenv("PATH");
  char* x1 = getenv("XALANROOT");
  
  FCGX_Request request;

  FCGX_Init();
  //FCGX_InitRequest(&request, 9001, 0);
  // Construct self Url.  It is embedded into the output stream
  // of some requests (like GetMap).  Use a fully qualified URL.
  
  while (FCGI_Accept() >= 0) 
  {
    char* mentor = getenv("MENTOR_DICTIONARY_PATH");
    char* path2 = getenv("PATH");
    char* x2 = getenv("XALANROOT");
    
  char* serverName = getenv(MapAgentStrings::ServerName);
  char* serverPort = getenv(MapAgentStrings::ServerPort);
  char* scriptName = getenv(MapAgentStrings::ScriptName);
  char* querystring = getenv(MapAgentStrings::QueryString);
  
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
    agenturi.append("?");
    agenturi.append(querystring);
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
    FCGI_fputs("Content-type: text/html\n\n",FCGI_stdout);
    FCGI_fputs("GeoREST CGI : It is not REST call! /rest/ keyword not found in URI.",FCGI_stdout);
    
    continue;
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
        size_t readBytes = FCGI_fread(chbuf, 1, nBytes, FCGI_stdin);
        
        chbuf[readBytes]=0;
        post_data.append(chbuf);
        free(chbuf);
      }
    }
  }
  
 
  
  Ptr<c_RestRequest> restrequest = new c_RestRequest(agenturi,uri_base,uri_rest,httpmethod,post_data);
  
  // add headers to it
  char* header_accept = getenv("HTTP_Accept");
  if( header_accept )
  {
    restrequest->SetHeaderValue("Accept",header_accept);
  }
  char* header_dsv = getenv("HTTP_DataServiceVersion");
  if( header_dsv )
  {
    restrequest->SetHeaderValue("DataServiceVersion",header_dsv);
  }
  char* header_max_dsvt = getenv("HTTP_MaxDataServiceVersion");
  if( header_max_dsvt )
  {
    restrequest->SetHeaderValue("MaxDataServiceVersion",header_max_dsvt);
  }
  
  char* auth = getenv("Authorization");
  if( auth )
  {
    restrequest->SetHeaderValue("Authorization",auth);
  }
  
  Ptr<c_RestResponse> restresponse = restrequest->Execute();
  
  c_RestResponse_HttpData* http_data = restresponse->GetHttpData(restrequest);

  //std::istream istr(http_data->GetHeader());

  Poco::Net::MessageHeader& headers = http_data->GetMsgHeaders();
  char temphead[64];
  sprintf(temphead,"%d %s",http_data->GetStatus(),http_data->GetStatusReason());
  headers.set("Status",temphead);
  

  
  Ptr<MgByteReader> bytereader = http_data->GetContentByteReader();

  ostringstream ss;
  headers.write(ss);
  string s1 = ss.str();
  //std::cout << s1;
  //std::cout << "\r\n";
  
  
  //FCGI_fwrite(s1.c_str(),sizeof(char),strlen(s1.c_str()),FCGI_stdout );
  FCGI_fputs(s1.c_str(),FCGI_stdout);
  FCGI_fputs("\r\n",FCGI_stdout);
  //FCGI_puts(s1.c_str());
  //FCGI_puts("\r\n");
  
  //response.write(std::cout);
  if( bytereader.p )
  {
    //MgByteSource* bytes = bytereader->GetByteSource();
    //ByteSourceImpl* bsrcimpl = bytes->GetSourceImpl();
    //std::ostream& outstr = response.send(); 

    unsigned char buf[4096];
    int nBytes = bytereader->Read(buf,4096);
    while (nBytes > 0)
    {
      //std::cout.write((char*)&buf[0],nBytes);
      buf[nBytes]=0;
      
      //printf((char*)&buf[0]);
      //FCGX_PutStr((const char *)&buf[0],nBytes, stdout->fcgx_stream);
      FCGI_fwrite(&buf[0],1,nBytes,FCGI_stdout);
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
    
    //std::cout << http_data->GetContentString();
    //FCGI_puts(http_data->GetContentString().c_str());
    FCGI_fputs(http_data->GetContentString().c_str(),FCGI_stdout);
        
  }
  
  }
  return 0;
}
