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



#include "c_ODataTest.h"
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


#define D_XMLNS_ATOM "http://www.w3.org/2005/Atom"
#define D_XMLNS_APP "http://www.w3.org/2007/app"

CPPUNIT_TEST_SUITE_REGISTRATION(c_ODataTest);


c_ODataTest::c_ODataTest()
{
}


c_ODataTest::~c_ODataTest()
{
}

extern wchar_t g_AppFileName[MAX_PATH+2];;




void c_ODataTest::setUp()
{
  
  m_GisHttpServer = c_GisHttpServer::CreateServer();
  m_GisHttpServer->StartServer();
}

void c_ODataTest::tearDown()
{
  m_GisHttpServer->StopServer();
  delete m_GisHttpServer;
}

#define D_MY_FEED_TITLE "My Feed Title"
#define D_MY_FEED_AUTHOR_NAME "My Feed Author Name"
#define D_MY_FEED_AUTHOR_URI "My Feed Author Uri"
#define D_MY_FEED_AUTHOR_EMAIL "My Feed Author Email"

char ODataSample[]="<RestConfig>\
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

void c_ODataTest::TestServiceDocument()
{
  c_RestConfig * restcfg = c_RestConfig::GetInstance();
  restcfg->Clear();
  restcfg->ReadFromString(ODataSample,"/templates/");
  
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc");
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
  //std::ostringstream ostr;
  //Poco::StreamCopier::copyStream(rs, ostr);
  //std::string str1 = ostr.str();
  
  Poco::XML::InputSource src(rs);
  Poco::XML::DOMParser parser;
  Poco::AutoPtr<Poco::XML::Document> pdoc = parser.parse(&src);
  Poco::AutoPtr<Poco::XML::NodeList>  nlist = pdoc->getElementsByTagName("service");
  if( nlist->length() == 0 )
  {
    CPPUNIT_FAIL("No 'service' in document!");
  }
  Poco::XML::Element* node_service = (Poco::XML::Element*)nlist->item(0);
  nlist = node_service->getElementsByTagNameNS(D_XMLNS_APP,"workspace");
  if( nlist->length() == 0 )
  {
    CPPUNIT_FAIL("No 'workspace' in document!");
  }
  Poco::XML::Element* node_workspace = (Poco::XML::Element*)nlist->item(0);
  
  nlist = node_workspace->getElementsByTagNameNS(D_XMLNS_APP,"collection");
  if( nlist->length() != 1 )
  {
    CPPUNIT_FAIL("Has to be one 'collection' in document!");
  }
  
  Poco::XML::Element* node_collection = (Poco::XML::Element*)nlist->item(0);
  nlist = node_collection->getElementsByTagNameNS(D_XMLNS_ATOM,"title");
  if( nlist->length() != 1 )
  {
    CPPUNIT_FAIL("Has to be atom:title of 'collection' in document!");
  }
  Poco::XML::Element* node_atomtitle = (Poco::XML::Element*)nlist->item(0);
  std::string title = node_atomtitle->innerText();
  if( Poco::icompare(title,"ODataParcels") !=0 )
  {
    CPPUNIT_FAIL("Wrong atom:title value!. Expecting 'ODataParcels'.");
  }
}

void c_ODataTest::TestOdataCollection()
{
  c_RestConfig * restcfg = c_RestConfig::GetInstance();
  restcfg->Clear();
  restcfg->ReadFromString(ODataSample,"/templates/");

  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/ODataParcels");
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

  std::string contype = response.getContentType();
  std::stringstream ostr;
  Poco::StreamCopier::copyStream(rs, ostr);
  std::string str1 = ostr.str();
  Poco::Debugger::message(str1);
  
  Poco::XML::InputSource src(ostr);
  Poco::XML::DOMParser parser;
  Poco::AutoPtr<Poco::XML::Document> pdoc = parser.parse(&src);
  Poco::AutoPtr<Poco::XML::NodeList>  nlist = pdoc->getElementsByTagName("feed");
  if( nlist->length() == 0 )
  {
    CPPUNIT_FAIL("No atom:feed in document!");
  }
  Poco::XML::Element* node_feed = (Poco::XML::Element*)nlist->item(0);
  nlist = node_feed->getElementsByTagNameNS(D_XMLNS_ATOM,"title"); 
  if( nlist->length() == 0 )
  {
    CPPUNIT_FAIL("No atom:title for atom:feed !");
  }
  Poco::XML::Element* node_atomtitle = (Poco::XML::Element*)nlist->item(0);
  std::string title = node_atomtitle->innerText();
  if( Poco::icompare(title,D_MY_FEED_TITLE) !=0 )
  {
    CPPUNIT_FAIL("Wrong atom:title value!. Expecting '"D_MY_FEED_TITLE"'.");
  }
  
  // Check Author
  Poco::XML::Element* node_author = node_feed->getChildElementNS(D_XMLNS_ATOM,"author");
  if( !node_author ) CPPUNIT_FAIL("No atom:author for feed!");
  
  // Check Author Name
  Poco::XML::Element* node_name = node_author->getChildElementNS(D_XMLNS_ATOM,"name");
  if( !node_name ) CPPUNIT_FAIL("No atom:author name for feed!");
  if( Poco::icompare(node_name->innerText(),D_MY_FEED_AUTHOR_NAME)  ) CPPUNIT_FAIL("Invalid atom:author name !");
  
  // Check Author Uri
  Poco::XML::Element* node_uri = node_author->getChildElementNS(D_XMLNS_ATOM,"uri");
  if( !node_uri ) CPPUNIT_FAIL("No atom:author uri for uri!");
  if( Poco::icompare(node_uri->innerText(),D_MY_FEED_AUTHOR_URI)  ) CPPUNIT_FAIL("Invalid atom:author URI !");
  
  // Check Author Email
  Poco::XML::Element* node_email = node_author->getChildElementNS(D_XMLNS_ATOM,"email");
  if( !node_email ) CPPUNIT_FAIL("No atom:author email for feed!");
  if( Poco::icompare(node_email->innerText(),D_MY_FEED_AUTHOR_EMAIL)  ) CPPUNIT_FAIL("Invalid atom:author email !");
  
  nlist = node_feed->getElementsByTagNameNS(D_XMLNS_ATOM,"entry"); 
  if( nlist->length() == 0 )
  {
    CPPUNIT_FAIL("No atom:entry in document!");
  }
}

void c_ODataTest::TestNotFound()
{
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/xxx");
  Poco::Timespan timeout(10,0);
  s.setTimeout(timeout);
  s.sendRequest(request);
  
  HTTPResponse response;
  std::istream& rs = s.receiveResponse(response);
  Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
  if( status != HTTPResponse::HTTP_NOT_FOUND )
  {
    CPPUNIT_FAIL("Response HTTP Status code is not HTTP_NOT_FOUND!");
  }
  
}

void c_ODataTest::TestParseKeyValueString()
{
  Ptr<MgPropertyCollection> props = new MgPropertyCollection;
  std::wstring singlevalue;
  c_OData_Interface::Impl()->ParseKeyValueString(L"23",NULL,props,singlevalue);
  if( wcscmp(singlevalue.c_str(),L"23") != 0 )
  {
    CPPUNIT_FAIL("Wrong single value!");
  }
  
  c_OData_Interface::Impl()->ParseKeyValueString(L"'2=,&3'",NULL,props,singlevalue);
  if( wcscmp(singlevalue.c_str(),L"'2=,&3'") != 0 )
  {
    CPPUNIT_FAIL("Wrong single value!");
  }
  
  c_OData_Interface::Impl()->ParseKeyValueString(L"val1='2=,&3',val2=34,val3='dd'",NULL,props,singlevalue);
  if( props->GetCount() != 3 )
  {
    CPPUNIT_FAIL("Wrong count of property values!");
  }
  for(int ind=0;ind<props->GetCount();ind++)
  {
    Ptr<MgStringProperty> prop1 = (MgStringProperty*)props->GetItem(L"val1");
    if( wcscmp(prop1->GetValue().c_str(),L"'2=,&3'") != 0 ) CPPUNIT_FAIL("Wrong value of 'val1'!");
    Ptr<MgStringProperty> prop2 = (MgStringProperty*)props->GetItem(L"val2");
    if( wcscmp(prop2->GetValue().c_str(),L"34") != 0 ) CPPUNIT_FAIL("Wrong value of 'val2'!");
    Ptr<MgStringProperty> prop3 = (MgStringProperty*)props->GetItem(L"val3");
    if( wcscmp(prop3->GetValue().c_str(),L"'dd'") != 0 ) CPPUNIT_FAIL("Wrong value of 'val3'!");
  }
}

void c_ODataTest::TestParseOdataLiteralValues()
{
//
// Test Byte
//
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::Byte);
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"1a");
  
  if( prop->GetPropertyType() != MgPropertyType::Byte ) CPPUNIT_FAIL("Wrong property type! Expected Byte");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! Expected val1");
  if( ((MgByteProperty*)(prop.p))->GetValue() != 0x1a ) CPPUNIT_FAIL("Wrong property value! Expected 0x1a");
  
  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"1ak");
    CPPUNIT_FAIL("Expected error exception.");
  }
  catch(...) {} // all ok it should throw
}  
//
//
// Test int16
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::Int16);
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"358");

  if( prop->GetPropertyType() != MgPropertyType::Int16 ) CPPUNIT_FAIL("Wrong property type! Expected int16");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! Expected val1");
  if( ((MgInt16Property*)(prop.p))->GetValue() != 358 ) CPPUNIT_FAIL("Wrong property value! Expected 358");

  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"100000");
    CPPUNIT_FAIL("Expected error exception.");
  }
  catch(...) {} // all ok it should throw
}
//
//
// Test int32
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::Int32);
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"101000");

  if( prop->GetPropertyType() != MgPropertyType::Int32 ) CPPUNIT_FAIL("Wrong property type! Expected int32");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! Expected val1");
  if( ((MgInt32Property*)(prop.p))->GetValue() != 101000 ) CPPUNIT_FAIL("Wrong property value! Expected 101000");

  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"101000f");
    CPPUNIT_FAIL("Expected error exception.");
  }
  catch(...) {} // all ok it should throw
}  
//
//
// Test int64
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::Int64);
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"110110110110110110L");

  if( prop->GetPropertyType() != MgPropertyType::Int64 ) CPPUNIT_FAIL("Wrong property type! Expected Int64");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! Expected val1");
  if( ((MgInt64Property*)(prop.p))->GetValue() != (INT64)(110110110110110110) ) CPPUNIT_FAIL("Wrong property value! Expected 110110110110110110");

  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"110110110110110110");
    CPPUNIT_FAIL("Expected error exception.");
  }
  catch(...) {} // all ok it should throw
}
//
//
// Test single
//
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::Single);
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"2.354f");

  if( prop->GetPropertyType() != MgPropertyType::Single ) CPPUNIT_FAIL("Wrong property type! Expected float");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! Expected val1");
  if( fabs(((MgSingleProperty*)(prop.p))->GetValue() - 2.354) > 0.00001 ) CPPUNIT_FAIL("Wrong property value! Expected 2.354");

  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"2.354n");
    CPPUNIT_FAIL("Expected error exception.");
  }
  catch(...) {} // all ok it should throw
} 
//
//
// Test double
//
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::Double);
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"2.354");

  if( prop->GetPropertyType() != MgPropertyType::Double ) CPPUNIT_FAIL("Wrong property type!");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! ");
  if( fabs(((MgDoubleProperty*)(prop.p))->GetValue() - 2.354) > 0.0000001 ) CPPUNIT_FAIL("Wrong property value! ");
  
  prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"2E+10");
  if( (((MgDoubleProperty*)(prop.p))->GetValue() - 2E+10) > 0.0000001 ) CPPUNIT_FAIL("Wrong property value! ");

  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"2.354f");
    CPPUNIT_FAIL("Expected error exception.");
  }
  catch(...) {} // all ok it should throw
}    
//
//
// Test String
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::String);
  
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"'abc'");
  if( prop->GetPropertyType() != MgPropertyType::String ) CPPUNIT_FAIL("Wrong property type! ");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! ");
  if( ((MgStringProperty*)(prop.p))->GetValue().compare(L"abc") != 0 ) CPPUNIT_FAIL("Wrong property value! ");
  
  prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"'O''Brien'");
  if( prop->GetPropertyType() != MgPropertyType::String ) CPPUNIT_FAIL("Wrong property type! ");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! ");
  if( ((MgStringProperty*)(prop.p))->GetValue().compare(L"O'Brien") != 0 ) CPPUNIT_FAIL("Wrong property value! ");

  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"abc");
    CPPUNIT_FAIL("Expected error exception.");
    prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"'abc");
    CPPUNIT_FAIL("Expected error exception.");
    prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"'abc'n");
    CPPUNIT_FAIL("Expected error exception.");
  }
  catch(...) {} // all ok it should throw
  
}  
//
//
// Test DateTime
//
{
  Ptr<MgDataPropertyDefinition> pdef = new MgDataPropertyDefinition(L"val1");
  pdef->SetDataType(MgPropertyType::DateTime);
  Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"datetime'2000-11-12T12:09'");

  if( prop->GetPropertyType() != MgPropertyType::DateTime ) CPPUNIT_FAIL("Wrong property type! ");
  if( wcscmp(prop->GetName().c_str(),L"val1") != 0  ) CPPUNIT_FAIL("Wrong property name! ");
  
  Ptr<MgDateTime> mgdt = ((MgDateTimeProperty*)(prop.p))->GetValue();
  
  if( mgdt->GetYear() !=2000 ) CPPUNIT_FAIL("Wrong property value! ");
  if( mgdt->GetMonth() != 11 ) CPPUNIT_FAIL("Wrong property value! ");
  if( mgdt->GetDay() != 12 ) CPPUNIT_FAIL("Wrong property value! ");
  if( mgdt->GetHour() != 12 ) CPPUNIT_FAIL("Wrong property value! ");
  if( mgdt->GetMinute() != 9 ) CPPUNIT_FAIL("Wrong property value! ");

  try
  {
    Ptr<MgProperty> prop = c_OData_Interface::Impl()->CreatePropertyValue(L"val1",pdef,L"2000-11-12T12:09");
    CPPUNIT_FAIL("Expected error exception.");
   
  }
  catch(...) {} // all ok it should throw

}      
  
  
}// end of c_ODataTest::TestParseOdataLiteralValues

void c_ODataTest::TestParseOdataFilter()
{
  std::wstring odataf,fdof;
  odataf = L"Parcel eq 5";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
  
  odataf = L"substringof('city',ParcelName)";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
  if( fdof.compare(L"ParcelName LIKE '%city%'"))
    CPPUNIT_FAIL("Invalid Filter generated! ");
    
    
  odataf = L"substringof('city',(ParcelName))";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
  
  odataf = L"substringof(('city'),(ParcelName))";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
  
  odataf = L"startswith(ParcelName,'city')";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
  if( fdof.compare(L"ParcelName LIKE 'city%'"))
    CPPUNIT_FAIL("Invalid Filter generated! ");
    
  odataf = L"endswith(ParcelName,'city')";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
  if( fdof.compare(L"ParcelName LIKE '%city'"))
    CPPUNIT_FAIL("Invalid Filter generated! ");
    
  odataf = L"endswith(toupper(ParcelName),toupper('city'))";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
  if( fdof.compare(L"Upper(ParcelName) LIKE  Concat('%',Upper('city'))"))
    CPPUNIT_FAIL("Invalid Filter generated! ");    
      
  try
  {
    odataf = L"substringof('city,ParcelName)";
    c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
    CPPUNIT_FAIL("Invalid Filter generated! It should error out. ");
  }
  catch(...){}
  try
  {
    odataf = L"substringof('city',ParcelName))";
    c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
    CPPUNIT_FAIL("Invalid Filter generated! It should error out. ");
  }
  catch(...){}
  try
  {
    odataf = L"substringof(('city',ParcelName))";
    c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
    CPPUNIT_FAIL("Invalid Filter generated! It should error out. ");
  }
  catch(...){}
  try
  {
    odataf = L"substringof(('city',ParcelName)";
    c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
    CPPUNIT_FAIL("Invalid Filter generated! It should error out. ");
  }
  catch(...){}
  
  odataf = L"Parcel eq '5'";
  c_Odata_FilterParser::ODataFilter2FdoFilter(odataf,fdof);
}


void c_ODataTest::TestOdataOneProperty()
{
  c_RestConfig * restcfg = c_RestConfig::GetInstance();
  restcfg->Clear();
  restcfg->ReadFromString(ODataSample,"/templates/");

{
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/ODataParcels(1)/NAME");
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

  std::string contype = response.getContentType();
  std::stringstream ostr;
  Poco::StreamCopier::copyStream(rs, ostr);
  std::string str1 = ostr.str();
  Poco::Debugger::message(str1);

  Poco::XML::InputSource src(ostr);
  Poco::XML::DOMParser parser;
  Poco::AutoPtr<Poco::XML::Document> pdoc = parser.parse(&src);
  Poco::AutoPtr<Poco::XML::NodeList>  nlist = pdoc->getElementsByTagName("d:NAME");
  if( nlist->length() == 0 )
  {
    CPPUNIT_FAIL("No NAME in document!");
  }
}  
// check for JSON
{
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/ODataParcels(1)/NAME?$format=json");
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
}  
// check just value  
{
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/ODataParcels(1)/NAME/$value");
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
    CPPUNIT_FAIL("/ODataParcels(1)/NAME/$value failed. Returned Http Status is not HTTP_OK!");
  }
  
  if( response.getContentType().find("text/plain") == std::string::npos )
  {
    CPPUNIT_FAIL("/ODataParcels(1)/NAME/$value failed. Content type should be text/plain!");
  }
}  
}//end of  c_ODataTest::TestOdataOneProperty

void c_ODataTest::TestOdataCollection_Json()
{
  c_RestConfig * restcfg = c_RestConfig::GetInstance();
  restcfg->Clear();
  restcfg->ReadFromString(ODataSample,"/templates/");

  
  {
    HTTPClientSession s("localhost", 99);
    HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/ODataParcels/?$format=json");
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
    if( response.getContentType().find("application/json") == std::string::npos )
    {
    }
  } 
}//end of c_ODataTest::TestOdataCollection_Json

void c_ODataTest::TestOrderBy()
{
  c_RestConfig * restcfg = c_RestConfig::GetInstance();
  restcfg->Clear();
  restcfg->ReadFromString(ODataSample,"/templates/");
  
// test orderby
{
  HTTPClientSession s("localhost", 99);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/rest/OData.svc/ODataParcels/?$orderby=NAME&$format=json");
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
}  
}//end of c_ODataTest::TestOrderBy

void c_ODataTest::JustMyTest()
{
  int a = sizeof(long);
  int b = sizeof(__int32);
  
  int l = sizeof(long long);
  
  int c = a - b;
}

