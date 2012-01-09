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



#include "c_TestEsri_FServer.h"
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
#include "c_CfgEsriGS_Catalog.h"

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
//using Poco::StreamCopier;
using Poco::XML::Element;




CPPUNIT_TEST_SUITE_REGISTRATION(c_TestEsri_FServer);


c_TestEsri_FServer::c_TestEsri_FServer()
{
}


c_TestEsri_FServer::~c_TestEsri_FServer()
{
}

extern wchar_t g_AppFileName[MAX_PATH+2];;




void c_TestEsri_FServer::setUp()
{
  
  m_GisHttpServer = c_GisHttpServer::CreateServer();
  m_GisHttpServer->StartServer();
}

void c_TestEsri_FServer::tearDown()
{
  m_GisHttpServer->StopServer();
  delete m_GisHttpServer;
}

#define D_MY_FEED_TITLE "My Feed Title"
#define D_MY_FEED_AUTHOR_NAME "My Feed Author Name"
#define D_MY_FEED_AUTHOR_URI "My Feed Author Uri"
#define D_MY_FEED_AUTHOR_EMAIL "My Feed Author Email"

static char EsriGSSample[]="<RestConfig>\
<Data>\
<Resource uripart=\"EGSParcels\" >\
<Source type=\"FDO\">\
<Provider>OSGeo.SDF</Provider>\
<ConnectionString>File=../data/Sheboygan/Parcels.sdf;ReadOnly=FALSE;</ConnectionString>\
<FeatureClass>SHP_Schema:Parcels</FeatureClass>\
</Source>\
</Resource>\
</Data>\
<Services>\
<EsriRest uripart=\"Esri.GS\">\
<Folder name=\"Sheboygan\" parent=\"\">\
  <Folder name=\"Sheboygan_Sub1\">\
  </Folder>\
  <FeatureServer name =\"Sheboygan/Parcels\" >\
    <Layer name=\"Sheboygan Parcels\">\
    <Data uripart=\"EGSParcels\"/>\
    <Json><![CDATA[  {\
    \"test_name\" : \"test\"\
    , \"test_id\" : 1\
    }\n\
    ]]></Json>\
    </Layer>\
  </FeatureServer>\
</Folder>\
</EsriRest>\
</Services>\
</RestConfig>";


void c_TestEsri_FServer::TestFServerDocument()
{
  try
  {
    c_RestConfig * restcfg = c_RestConfig::GetInstance();
    restcfg->Clear();
    restcfg->ReadFromString(EsriGSSample,"/templates/");

    HTTPClientSession s("localhost", 99);
    HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/Esri.GS/Sheboygan/Parcels/FeatureServer");
    Poco::Timespan timeout(2,0);
    s.setTimeout(timeout);
    s.sendRequest(request);

    HTTPResponse response; 
    std::istream& rs = s.receiveResponse(response);
    if( response.getStatus() != HTTPResponse::HTTP_OK )
    {
      CPPUNIT_FAIL("Returned Http Status is not HTTP_OK!");
    }

    std::string contype = response.getContentType();
    std::stringstream ostr;
    Poco::StreamCopier::copyStream(rs, ostr);


    c_CfgEsriGS_FeatureServer fserver;

    fserver.ReadFromJson(ostr.str());

    
  }
  catch(c_ExceptionRestConfig& exc)  
  {
    std::string err;
    Poco::UnicodeConverter::toUTF8(exc.m_Desc,err);
    CPPUNIT_FAIL(err.c_str());
  }
}//end of TestSubFolderDocument



