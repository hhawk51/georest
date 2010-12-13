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

#ifndef _c_EsriGS_h
#define _c_EsriGS_H



#include "CppUnit/TestCase.h"
#include "c_GisHttpServer.h"


class c_EsriGS : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( c_EsriGS );
    CPPUNIT_TEST( TestCatalogDocument );
    CPPUNIT_TEST( TestSubFolderDocument );
    CPPUNIT_TEST( TestSubFolderDocument2 );
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
  c_EsriGS(void);
public:
  ~c_EsriGS(void);
   //! \brief Set up context before running a test.
  virtual void setUp() ;

  //! Clean up after the test run.
  virtual void tearDown() ;
  
  void TestCatalogDocument();   
  void TestOdataCollection();
  void TestNotFound();
  void TestParseKeyValueString();
  void TestParseOdataLiteralValues();
  void TestParseOdataFilter();
  void TestOdataOneProperty();
  void TestOrderBy();
  
  void JustMyTest();
  void TestOdataCollection_Json();
  void TestSubFolderDocument();
  void TestSubFolderDocument2();
protected:
  c_GisHttpServer* m_GisHttpServer;  
};





#endif // c_CfgTest_INCLUDED
