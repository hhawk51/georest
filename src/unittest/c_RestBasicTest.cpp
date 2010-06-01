//
//  Copyright (C) 2004-2008 by SL-King d.o.o.
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
#include <fstream>



#include "c_RestBasicTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "Poco/Foundation.h"
#include "Poco/AutoPtr.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/Path.h"
#include "Poco/XML/XML.h"
#include "Poco/XML/XmlWriter.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/Node.h"
#include "Poco/DOM/DomWriter.h"


using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
//using Poco::StreamCopier;
using Poco::XML::Element;

CPPUNIT_TEST_SUITE_REGISTRATION(c_RestBasicTest);


c_RestBasicTest::c_RestBasicTest()
{
}


c_RestBasicTest::~c_RestBasicTest()
{
}

extern wchar_t g_AppFileName[MAX_PATH+2];;



c_GisHttpServer* g_GisHttpServer=NULL;
void c_RestBasicTest::setUp()
{
  if( g_GisHttpServer == NULL )
  {
    g_GisHttpServer = c_GisHttpServer::CreateServer(100);
    g_GisHttpServer->StartServer();
  }
  m_GisHttpServer = c_GisHttpServer::CreateServer();
  m_GisHttpServer->StartServer();
}

void c_RestBasicTest::tearDown()
{
  m_GisHttpServer->StopServer();
  delete m_GisHttpServer;
}

void c_RestBasicTest::TestEcho()
{
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/");
  Poco::Timespan timeout(2,0);
  s.setTimeout(timeout);
  s.sendRequest(request);
  
  HTTPResponse response;
  std::istream& rs = s.receiveResponse(response);
  if( response.getStatus() != HTTPResponse::HTTP_OK )
  {
    CPPUNIT_FAIL("Returned Http Status is not HTTP_OK!");
  }
  
  //assert (response.getContentLength() == HTTPTestServer::SMALL_BODY.length());
  //assert (response.getContentType() == "text/html");
  //std::ostringstream ostr;
  //StreamCopier::copyStream(rs, ostr);
  //assert (ostr.str() == HTTPTestServer::SMALL_BODY);
}

void c_RestBasicTest::TestRestHello()
{
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/hello");
  Poco::Timespan timeout(2,0);
  s.setTimeout(timeout);
  s.sendRequest(request);

  HTTPResponse response;
  std::istream& rs = s.receiveResponse(response);
  if( response.getStatus() != HTTPResponse::HTTP_OK )
  {
    CPPUNIT_FAIL("Returned Http Status is not HTTP_OK!");
  }

  //assert (response.getContentLength() == HTTPTestServer::SMALL_BODY.length());
  //assert (response.getContentType() == "text/html");
  //std::ostringstream ostr;
  //StreamCopier::copyStream(rs, ostr);
  //assert (ostr.str() == HTTPTestServer::SMALL_BODY);
}
