//
// SharedMemoryTest.cpp
//
// $Id: //poco/1.3/Foundation/testsuite/src/SharedMemoryTest.cpp#1 $
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "SharedMemoryTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/SharedMemory.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/Exception.h"


using Poco::SharedMemory;


SharedMemoryTest::SharedMemoryTest(const std::string& name): CppUnit::TestCase(name)
{
}


SharedMemoryTest::~SharedMemoryTest()
{
}


void SharedMemoryTest::testCreate()
{
	SharedMemory mem("hi", 4096, SharedMemory::AM_WRITE);
	assert (mem.end()-mem.begin() == 4096);
	mem.begin()[0] = 'A';
	mem.end()[-1] = 'Z';
}


void SharedMemoryTest::testCreateFromFile()
{
	Poco::Path p = findDataFile("testdata.txt");
	Poco::File f(p);
	assert (f.exists() && f.isFile());
	SharedMemory mem(f, SharedMemory::AM_READ);
	assert (mem.end() > mem.begin()); // valid?
	assert (mem.end() - mem.begin() == f.getSize());
	assert (mem.begin()[0] == 'A');
	assert (mem.end()[-5] == 'Z');
}


Poco::Path SharedMemoryTest::findDataFile(const std::string& afile)
{
	Poco::Path root;
	root.makeAbsolute();
	Poco::Path result;
	while (!Poco::Path::find(root.toString(), "data", result))
	{
		root.makeParent();
		if (root.toString().empty() || root.toString() == "/")
			throw Poco::FileNotFoundException("Didn't find data subdir");
	}
	result.makeDirectory();
	result.setFileName(afile);
	Poco::File aFile(result.toString());
	if (!aFile.exists() || (aFile.exists() && !aFile.isFile()))
		throw Poco::FileNotFoundException("Didn't find file " + afile);
	
	return result;
}


void SharedMemoryTest::setUp()
{
}


void SharedMemoryTest::tearDown()
{
}


CppUnit::Test* SharedMemoryTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("SharedMemoryTest");

	CppUnit_addTest(pSuite, SharedMemoryTest, testCreate);
	CppUnit_addTest(pSuite, SharedMemoryTest, testCreateFromFile);

	return pSuite;
}
