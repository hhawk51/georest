//
//  Copyright (C) 2010 by Haris Kurtagic
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

#include "c_TestOdata_TopSkip.h"
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
#include "Poco/StreamCopier.h"
#include "Poco/Debugger.h"
//#include <istream>
//#include <ostream>
#include <sstream>
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/NodeList.h"
#include "c_OData_Interface.h"
#include "c_Odata_FilterParser.h"

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
//using Poco::StreamCopier;
using Poco::XML::Element;

CPPUNIT_TEST_SUITE_REGISTRATION(c_TestOdata_TopSkip);



extern wchar_t g_AppFileName[MAX_PATH+2];;

#define D_MY_FEED_TITLE "My Feed Title"
#define D_MY_FEED_AUTHOR_NAME "My Feed Author Name"
#define D_MY_FEED_AUTHOR_URI "My Feed Author Uri"
#define D_MY_FEED_AUTHOR_EMAIL "My Feed Author Email"

static char ODataSample[]="<RestConfig>\
 <Data>\
 <Resource uripart=\"ODataParcels\" >\
 <Source type=\"FDO\">\
 <Provider>OSGeo.SDF</Provider>\
 <ConnectionString>File=../data/Sheboygan/Parcels.sdf;ReadOnly=FALSE;</ConnectionString>\
 <FeatureClass>SHP_Schema:Parcels</FeatureClass>\
 </Source>\
 <Representation renderer=\"OData\">\
 <FeedOverride>\
 <Title>"D_MY_FEED_TITLE"</Title>\
 <Author>\
 <Name>"D_MY_FEED_AUTHOR_NAME"</Name>\
 <Uri>"D_MY_FEED_AUTHOR_URI"</Uri>\
 <Email>"D_MY_FEED_AUTHOR_EMAIL"</Email>\
 </Author>\
 </FeedOverride>\
 <EntryOverride>\
 <Title type=\"Property\">RPROPAD</Title>\
 <Author>\
 <Name type=\"Property\">RNAME</Name>\
 <Uri>My Entry Author Uri</Uri>\
 </Author>\
 </EntryOverride>\
 </Representation>\
 </Resource>\
 </Data>\
 </RestConfig>";


void c_TestOdata_TopSkip::setUp()
{

  m_GisHttpServer = c_GisHttpServer::CreateServer();
  m_GisHttpServer->StartServer();
}

void c_TestOdata_TopSkip::tearDown()
{
  m_GisHttpServer->StopServer();
  delete m_GisHttpServer;
}

c_TestOdata_TopSkip::c_TestOdata_TopSkip(void)
{
}

c_TestOdata_TopSkip::~c_TestOdata_TopSkip(void)
{
}

void c_TestOdata_TopSkip::TestTop()
{
  c_RestConfig * restcfg = c_RestConfig::GetInstance();
  restcfg->Clear();
  restcfg->ReadFromString(ODataSample,"/templates/");

  // test Top
  {
    HTTPClientSession s("localhost", 99);
    HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/ODataParcels/?$top=2");
    Poco::Timespan timeout(10,0);
    s.setTimeout(timeout);
    s.sendRequest(request);

    HTTPResponse response;
    std::istream& rs = s.receiveResponse(response);
    if( response.getStatus() != HTTPResponse::HTTP_OK )
    {
      std::stringstream ostr;
      Poco::StreamCopier::copyStream(rs, ostr);
      std::string str1 = ostr.str();
      Poco::Debugger::message(str1);
      CPPUNIT_FAIL("Returned Http Status is not HTTP_OK!");
    }  
    
    Poco::XML::InputSource src(rs);
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> pdoc = parser.parse(&src);
    Poco::AutoPtr<Poco::XML::NodeList>  nlist = pdoc->getElementsByTagName("entry");
    
    if( nlist->length() != 2 )
    {
      CPPUNIT_FAIL("Error: should return just 2 entries!");
    }
  }  
}//end of c_ODataTest::TestOrderBy