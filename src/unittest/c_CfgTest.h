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


#ifndef c_CfgTest_INCLUDED
#define c_CfgTest_INCLUDED



#include "CppUnit/TestCase.h"


class c_CfgTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( c_CfgTest );
    CPPUNIT_TEST( TestReadCfg );    
  CPPUNIT_TEST_SUITE_END();
  
public:
  c_CfgTest(void);
public:
  ~c_CfgTest(void);
   //! \brief Set up context before running a test.
  virtual void setUp() ;

  //! Clean up after the test run.
  virtual void tearDown() ;
  
  void TestReadCfg();   
};





#endif // c_CfgTest_INCLUDED
