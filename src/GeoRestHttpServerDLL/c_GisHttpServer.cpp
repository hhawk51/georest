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

#include "Poco/Path.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"


#include "c_GisHttpServer.h"

#include "c_RestRequest.h"
#include "c_RestConfig.h"

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;


// using istringstream constructors.
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

extern Poco::Path g_AppFolder;

class RestRequestHandler: public HTTPRequestHandler
{
public:
  RestRequestHandler()
  {
  }

  void handleFileRequest(HTTPServerRequest& request,HTTPServerResponse& response)
  {
    
    Poco::Path fpath = g_AppFolder;
    fpath.pushDirectory("www");
    const std::string& agenturi = request.getURI();
    fpath.setFileName(agenturi);
    
    string file_ext = fpath.getExtension();
    
    string content_type="*/*";
    /*
    string accept_header = request.get("Accept");
    if( accept_header.length()>0 )
    {
      content_type = accept_header;
    }
    */
    if( file_ext == "html" )
    {
      content_type = "text/html";
    }
    else
    if( file_ext == "xml" )
    {
      content_type = "text/xml";
    }
    else
    if( file_ext == "css" )
    {
      content_type = "text/css";
    }else
    if( file_ext == "js" )
    {
      content_type = "text/javascript";
    }else
    if( file_ext == "htm" )
    {
      content_type = "text/html"; 
    }else
    
    if( file_ext == ".jpeg" )
    {
      content_type = "image/jpeg";
    }else
    if( file_ext == ".gif" )
    {
      content_type = "image/gif";
    }else
    if( file_ext == ".tiff" )
    {
      content_type = "image/tiff";
    }else
    if( file_ext == ".kml" )
    {
      content_type = "application/vnd.google-earth.kml+xml";
    }else
    if( file_ext == ".kmz" )
    {
      content_type = "application/vnd.google-earth.kmz";
    }
   
    
    response.sendFile(fpath.toString(),content_type);
  
  }
  void handleRequest(HTTPServerRequest& request,
    HTTPServerResponse& response)
  {
    /*
    Application& app = Application::instance();
    app.logger().information("Request from "
      + request.clientAddress().toString());
    */
    
    /*
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("text/plain");
    std::ostream& outstr2 = response.send(); 
    outstr2 << "Hello back from REST";
    return;
    */
    
    
   
    
    const std::string& agenturi = request.getURI();
    const std::string& httpmethod = request.getMethod();
    
    bool isrest_uri=false;
    string uri_rest,uri_base;

    c_RestConfig * restcfg = c_RestConfig::GetInstance();
    const char *resturi_separator = restcfg->GetRestUriSeparator();

    basic_string <char>::size_type rpos = agenturi.find("/REST/");
    if( rpos == string::npos )
    {
      rpos = agenturi.find("/rest/");

    }
    if( rpos != string::npos )
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
      return handleFileRequest(request,response);
    }
    
    std::string post_data;
    if( httpmethod == "POST" || httpmethod == "PUT" )
    {
    
      std::istream& istr = request.stream();      
      //istr >> post_data;
      static const int eof = std::char_traits<char>::eof();
      int ch = istr.get();
      while (ch != eof)
      {
        post_data += (char) ch;
        ch = istr.get();
      }
    }
    Ptr<c_RestRequest> restrequest = new c_RestRequest(agenturi,uri_base,uri_rest,httpmethod,post_data);
    
    Ptr<c_RestResponse> restresponse = restrequest->Execute();
    //c_RestResponse_HttpData* http_data = restresponse->PrepareHttpData(restrequest);
    c_RestResponse_HttpData* http_data = restresponse->GetHttpData();
    
    //std::istream istr(http_data->GetHeader());
    
    Poco::Net::MessageHeader& headers = http_data->GetMsgHeaders();
    
    char temphead[64];
    sprintf(temphead,"%d",http_data->GetStatus());
    response.setReason(http_data->GetStatusReason());
    response.setStatus(temphead);
    //response.setStatusAndReason((Poco::Net::HTTPResponse::HTTPStatus)http_data->GetStatus(),http_data->GetStatusReason());
    response.swap(headers);
    //headers.operator []
    sprintf(temphead,"%d OK",http_data->GetStatus());
    response.set("Status",temphead);
    
    //std::istringstream istr(http_data->GetHeader());
    //response.read(istr);
    
    Ptr<MgByteReader> bytereader = http_data->GetContentByteReader();
    
    if( bytereader.p )
    {
      //MgByteSource* bytes = bytereader->GetByteSource();
      //ByteSourceImpl* bsrcimpl = bytes->GetSourceImpl();
      std::ostream& outstr = response.send(); 
      
      unsigned char buf[4096];
      DWORD dwSize;
      int nBytes = bytereader->Read(buf,4096);
      while (nBytes > 0)
      {
        dwSize = nBytes;
        //response.sendBuffer(buf,nBytes); 
        outstr.write((char*)&buf[0],nBytes);
        nBytes = bytereader->Read(&buf[0],4096);                                
      }
      
      
    }
    else
    {    
      std::ostream& outstr = response.send(); 
      
      outstr << http_data->GetContentString();
    }
    return ;
    
    //response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_IMPLEMENTED);
    //response.send();
    //return;
    
    Timestamp now;
    std::string dt(DateTimeFormatter::format(now, DateTimeFormat::SORTABLE_FORMAT));

    
    //REST_HandleUri();


    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    std::ostream& ostr = response.send();
    ostr << "<html><head><title>GIS Web Services </title>";
    ostr << "</head>";
    ostr << "<body><p style=\"text-align: center; font-size: 24px;\">GIS Web Services powered by King.RESTful MapGuide</p><p style=\"text-align: center; "
      "font-size: 12px;\">";
    ostr << dt;
    ostr << "</p></body></html>";
    
    
  }

  void HandleRestRequest(HTTPServerRequest& request,
    HTTPServerResponse& response)
  {

   

  }


};

class RestRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
  RestRequestHandlerFactory()
      
      {
      }

      HTTPRequestHandler* createRequestHandler(
        const HTTPServerRequest& request)
      {
        return new RestRequestHandler();
        /*
        if (request.getURI() == "/")
          return new RestRequestHandler(_format);
        else
          return 0;
        */
      }

private:
  
};

c_GisHttpServer::c_GisHttpServer(Poco::Net::HTTPServer* Server)
{
  m_Server = Server;
}

c_GisHttpServer::~c_GisHttpServer(void)
{
  delete m_Server;
}

c_GisHttpServer* c_GisHttpServer::CreateServer(int Port/*=99*/)
{
  

  ServerSocket svs(Port);
  
  
  return new c_GisHttpServer( new HTTPServer (new RestRequestHandlerFactory(), svs, new HTTPServerParams) );
     
}

void c_GisHttpServer::StartServer()
{
  m_Server ->start();

}
void c_GisHttpServer::StopServer()
{
  m_Server ->stop();
  
}
