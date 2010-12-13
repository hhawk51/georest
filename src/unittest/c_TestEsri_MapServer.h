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

#ifndef _c_EsriGS_MapServer_h
#define _c_EsriGS_MapServer_H



#include "CppUnit/TestCase.h"
#include "c_GisHttpServer.h"


class c_TestEsri_MapServer : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( c_TestEsri_MapServer );
    CPPUNIT_TEST( TestMServerDocument );
    CPPUNIT_TEST( TestExportMapImage );
    //CPPUNIT_TEST( TestOdataCollection ); 
    //CPPUNIT_TEST( TestOdataCollection_Json ); 
    //CPPUNIT_TEST( TestNotFound ); 
    //CPPUNIT_TEST( TestParseKeyValueString ); 
    //CPPUNIT_TEST( TestParseOdataLiteralValues ); 
    //CPPUNIT_TEST( TestParseOdataFilter ); 
    //CPPUNIT_TEST( TestOdataOneProperty ); 
    //CPPUNIT_TEST( TestOrderBy ); 
    
    
    
  CPPUNIT_TEST_SUITE_END();
  
public:
  c_TestEsri_MapServer(void);
public:
  ~c_TestEsri_MapServer(void);
   //! \brief Set up context before running a test.
  virtual void setUp() ;

  //! Clean up after the test run.
  virtual void tearDown() ;
  
  void TestExportMapImage();   
  void TestMServerDocument();
  
protected:
  c_GisHttpServer* m_GisHttpServer;  
};





#endif // c_CfgTest_INCLUDED
