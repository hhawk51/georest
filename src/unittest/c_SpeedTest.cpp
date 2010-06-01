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



#include "c_SpeedTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"

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
using Poco::StreamCopier;
//using Poco::StreamCopier;
using Poco::XML::Element;


CPPUNIT_TEST_SUITE_REGISTRATION(c_SpeedTest);


c_SpeedTest::c_SpeedTest()
{
}


c_SpeedTest::~c_SpeedTest()
{
}

extern wchar_t g_AppFileName[MAX_PATH+2];;




void c_SpeedTest::setUp()
{
  
  //m_GisHttpServer = c_GisHttpServer::CreateServer();
  //m_GisHttpServer->StartServer();
}

void c_SpeedTest::tearDown()
{
  //m_GisHttpServer->StopServer();
  //delete m_GisHttpServer;
}



void c_SpeedTest::Test1()
{
  
  
  long msec1=0,msec2=0;
  
  for(int ind=0;ind<10;ind++)
  {
    HTTPClientSession s("localhost", 99);
    Poco::Timespan timeout(5,0);
    s.setTimeout(timeout);
    HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/data/Parcelfdo/.xml?count=300");
    clock_t elog_t1=clock();
    s.sendRequest(request);
    HTTPResponse response;
    std::istream& rs = s.receiveResponse(response);
    std::string str1;
    StreamCopier::copyToString(rs, str1);    
    int len = str1.length();
    if( response.getStatus() != HTTPResponse::HTTP_OK )
    {
      CPPUNIT_FAIL("Returned Http Status is not HTTP_OK!");
    }
    clock_t elog_t2=clock();
    msec1 += (long)((elog_t2-elog_t1)* CLOCKS_PER_SEC / 1000);
  }
  
  for(int ind=0;ind<10;ind++)
  {
    HTTPClientSession s("localhost", 99);
    Poco::Timespan timeout(5,0);
    s.setTimeout(timeout);
    HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/data/Parcel/.xml?count=300");
    clock_t elog_t1=clock();
    s.sendRequest(request);
    HTTPResponse response;
    std::istream& rs = s.receiveResponse(response);
    std::string str1;
    StreamCopier::copyToString(rs, str1);    
    int len = str1.length();
    if( response.getStatus() != HTTPResponse::HTTP_OK )
    {
      CPPUNIT_FAIL("Returned Http Status is not HTTP_OK!");
    }
    clock_t elog_t2=clock();
    msec2 += (long)((elog_t2-elog_t1)* CLOCKS_PER_SEC / 1000);
  }
  
  char buff[256];
  sprintf(buff,"FDO %.1lf MG %.1lf seconds",(double)msec1/1000.0,(double)msec2/1000.0);
  CPPUNIT_FAIL(buff);
  
  
  //sec +=  (double)msecs / 1000.0;

  
}
