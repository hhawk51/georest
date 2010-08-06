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
#include "Poco/Random.h"
#include "Poco/XML/XML.h"
#include "Poco/XML/XmlWriter.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/Node.h"
#include "Poco/DOM/DomWriter.h"
#include "Poco/Timestamp.h"
#include "Poco/Net/NameValueCollection.h"


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
    int len = (int)str1.length();
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

void RandomString(Poco::Random& rnd,int string_len,std::wstring& str)
{
  float fl;
  str.clear();
  str.reserve(string_len+2);
  while(string_len--)
  {    
    fl = rnd.nextFloat() * 30.0 + 32.0; // from 10 - 20
    wchar_t ch = fl;
    str.append(&ch,1);
  }
}


struct MY_ILT
{
  bool operator() (const std::wstring& s1, const std::wstring& s2) const
  {
    return s1.compare(s2) < 0;
  }
};
typedef std::multimap<std::wstring, std::wstring, MY_ILT> MyHeaderMap;


std::string GetString(const std::string& s1)
{
  std::string res2 = s1+"54";
  return res2; 
}

// compares speed of MgStringPropertyCollection with std::vector<std::wstring>
void c_SpeedTest::TestMgStringPropertCollection()
{
  int num_random_strings = 7;
  Ptr<MgStringPropertyCollection> mg_props = new MgStringPropertyCollection();

  std::vector<std::wstring> std_names;
  std::vector<std::wstring> std_vals;
  
  
  MyHeaderMap mymap_props;

  Poco::Random rnd;
  for(int rcount=0;rcount<num_random_strings;rcount++) 
  {
    float fl = rnd.nextFloat() * 10.0 + 10.0; // from 10 - 20
    int string_len = fl;
    std::wstring new_name;
    RandomString(rnd,string_len,new_name);
    
    std::wstring new_val;
    new_val = new_name;
    new_val.append(L"_value");
    mg_props->Add(new_name,new_val);
    
    std_names.push_back(new_name);
    std_vals.push_back(new_val);
    
    mymap_props.insert(MyHeaderMap::value_type(new_name, new_val));
    
  }
   
  
  int num_search_for_val = 1000;
  clock_t mg_t1=clock();
  Poco::Timestamp mg_start;
  for(int rcount =0;rcount< num_search_for_val;rcount++)
  {
    float fl = rnd.nextFloat() * num_random_strings + 0; // from 0 - num_random_strings
    int string_index = fl;
    
    std::wstring prop_name = mg_props->GetName(string_index);
    
    //RandomString(rnd,16,prop_name);
    
    int found_ind = mg_props->IndexOf(prop_name);    
    std::wstring prop_val = found_ind >= 0 ? mg_props->GetValue(found_ind) : L"";
  }
  Poco::Timestamp::TimeDiff mg_elapsed = mg_start.elapsed();
  clock_t mg_t2=clock();
  long mg_msec = (long)((mg_t2-mg_t1)* CLOCKS_PER_SEC / 1000);
  
  clock_t std_t1=clock();
  Poco::Timestamp std_start;
  for(int rcount =0;rcount< num_search_for_val;rcount++)
  {
    float fl = rnd.nextFloat() * (float)num_random_strings + 0.0; // from 0 - num_random_strings
    int string_index = fl;
    
    std::wstring prop_name = std_names.at(string_index);

    //RandomString(rnd,16,prop_name);
    
    std::vector<std::wstring>::const_iterator names_iter = std_names.begin();
    
    std::wstring prop_val;
    bool found=false;
    int name_ind=0;
    while( names_iter !=std_names.end() )
    {   
      
      //if( Poco::icompare(*names_iter,prop_name) == 0 )
      if( prop_name.compare(*names_iter) == 0 )
      {
        found=true;
        prop_val = std_vals.at(name_ind);
        break;
      }
      names_iter++;
      name_ind++;
    }
    //if( !found )       CPPUNIT_FAIL("Error in test value not found!");
    
  }
  Poco::Timestamp::TimeDiff std_elapsed = std_start.elapsed();  
  clock_t std_t2=clock();
  long std_msec = (long)((std_t2-std_t1)* CLOCKS_PER_SEC / 1000);


//
// My Map test
//
  clock_t map_t1=clock();
  Poco::Timestamp map_start;
  for(int rcount =0;rcount< num_search_for_val;rcount++)
  {
    float fl = rnd.nextFloat() * (float)num_random_strings + 0.0; // from 0 - num_random_strings
    int string_index = fl;

    MyHeaderMap::const_iterator iter = mymap_props.begin();
    while(string_index--) iter++;
    std::wstring prop_name = iter->first;

    //RandomString(rnd,16,prop_name);

    std::wstring prop_val;
    MyHeaderMap::const_iterator iter2 = mymap_props.find(prop_name);
    if (iter2 != mymap_props.end())
      prop_val =iter2->second;        
        
   

  }
  Poco::Timestamp::TimeDiff map_elapsed = map_start.elapsed();  
  clock_t map_t2=clock();
  long map_msec = (long)((map_t2-map_t1)* CLOCKS_PER_SEC / 1000); 
 
 
  char buff[256];
  sprintf(buff,"MG(%ld) STD(%ld) MyMap(%ld). Times are in ms",mg_msec,std_msec,map_msec);
  std::string str;
  str.assign(buff);
  CPPUNIT_FAIL(buff);
//// final calc 
  Poco::Timestamp::TimeDiff std_mg_diff = (mg_elapsed - std_elapsed)/1000;;
 
 
 
  long diff =  std_msec - mg_msec;
  
  std::string s1 = "input string";
  std::string s2 = s1;
  
  const char* cp1 = s1.c_str();
  const char* cp2 = s2.c_str();
  
  std::string ret1;
  //ret1 =  = GetString(s1);
  GetString(s1);
  std::string ret2 = ret1;
}