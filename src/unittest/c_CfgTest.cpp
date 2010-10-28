//
//  Copyright (C) 2006-2010 by SL-King d.o.o.
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
#include "RestConfigInclude.h"

#include "c_CfgTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"


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
#include "RestRequestHandler.h"



using Poco::XML::Element;

CPPUNIT_TEST_SUITE_REGISTRATION(c_CfgTest);


c_CfgTest::c_CfgTest()
{
}


c_CfgTest::~c_CfgTest()
{
}

extern wchar_t g_AppFileName[MAX_PATH+2];;


void AddElementAndText(Poco::XML::Document* Doc,Poco::XML::Node* Parent,const char* StrElement,const char* StrText)
{
  Poco::AutoPtr<Poco::XML::Element> data_uritag = Doc->createElement(StrElement);
  Poco::AutoPtr<Poco::XML::Text> t1 = Doc->createTextNode(StrText);	  
  data_uritag->appendChild(t1);
  Parent->appendChild(data_uritag);
}

void WriteTestCfgXml_v1(const char* FileName)
{
  Poco::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
	
	Poco::AutoPtr<Poco::XML::Element> restcfg = pDoc->createElement("RestConfig");
	pDoc->appendChild(restcfg);
	Poco::AutoPtr<Poco::XML::Element> data = pDoc->createElement("Data");
	restcfg->appendChild(data);

  {
	  Poco::AutoPtr<Poco::XML::Element> layer = pDoc->createElement("Layer");
	  
	  AddElementAndText(pDoc,layer,"UriTag","Building");
	  AddElementAndText(pDoc,layer,"MapGuideFeatureSource","Library://Samples/Sheboygan/Data/BuildingOutlines.FeatureSource");
	  AddElementAndText(pDoc,layer,"ClassName","SHP_Schema:BuildingOutlines");
	  AddElementAndText(pDoc,layer,"MapGuideMapDefinition","Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
	  
	  Poco::AutoPtr<Poco::XML::Element> html = pDoc->createElement("HtmlTemplate");
	  
	  AddElementAndText(pDoc,html,"SingleFeature","building1.tpl");
	  AddElementAndText(pDoc,html,"ManyFeatures","building.tpl");	  
	  AddElementAndText(pDoc,html,"TemplatePrefix","BUILDING_");
	  AddElementAndText(pDoc,html,"TemplateSection","BUILDING");
	  
	  Poco::AutoPtr<Poco::XML::Element> adata = pDoc->createElement("AdditionalData");
	  AddElementAndText(pDoc,adata,"TemplateSection","PARCEL");
	  AddElementAndText(pDoc,adata,"FetchUri","http://www.sl-king.si/mgrest/rest/data/parcel/.xml?bbox={{BUILDING_GEOM}}");	  
	  AddElementAndText(pDoc,adata,"TemplatePrefix","PARCEL_");
	  html->appendChild(adata);
	  
	  layer->appendChild(html);
	  
	  data->appendChild(layer);
	}
	
	{
	  Poco::AutoPtr<Poco::XML::Element> layer = pDoc->createElement("Layer");
	  
	  AddElementAndText(pDoc,layer,"UriTag","Parcel");
	  AddElementAndText(pDoc,layer,"MapGuideFeatureSource","Library://Samples/Sheboygan/Data/Parcels.FeatureSource");
	  AddElementAndText(pDoc,layer,"ClassName","SHP_Schema:Parcels");
	  AddElementAndText(pDoc,layer,"MapGuideMapDefinition","Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
	  		  
		Poco::AutoPtr<Poco::XML::Element> html = pDoc->createElement("HtmlTemplate");
	  
	  AddElementAndText(pDoc,html,"SingleFeature","parcel1.tpl");
	  AddElementAndText(pDoc,html,"ManyFeatures","parcel.tpl");
	  AddElementAndText(pDoc,html,"TemplatePrefix","PARCEL_");
	  AddElementAndText(pDoc,html,"TemplateSection","PARCEL");
	  layer->appendChild(html);
	  
	  Poco::AutoPtr<Poco::XML::Element> adata = pDoc->createElement("AdditionalData");
	  AddElementAndText(pDoc,adata,"FetchUri","http://www.sl-king.si/mgrest/rest/data/building/{{PARCEL_ID}}.xml");	  
	  AddElementAndText(pDoc,adata,"TemplatePrefix","BUILDING_");
	  AddElementAndText(pDoc,html,"TemplateSection","BUILDING");
	  html->appendChild(adata);
	  
	  data->appendChild(layer);
	}

	
	
	Poco::XML::DOMWriter writer;
	writer.setNewLine("\n");
	writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
	
	std::ofstream file;
	file.open(FileName,std::ios_base::out);
	if( !file.is_open() )
	{
	  CPPUNIT_FAIL("Unable to open restcfg file to write");
	}
	writer.writeNode(file, pDoc);
	file.close();
}



void c_CfgTest::TestReadCfg()
{
try
{ 
  c_RestConfig::DeleteInstance();
  c_RestConfig * cfg_rest = c_RestConfig ::GetInstance();
  
  
  const c_CfgDataResource* res = cfg_rest->FindUriTag(L"building");
    if( !res ) CPPUNIT_FAIL("No building resource!");
    
  const c_CfgRepresentation* rep = res->FindRepresentation(L".XML");
  if( !rep ) CPPUNIT_FAIL("No XML representation!");
  
  e_REST_AccessCodes isaccess = rep->IsAccess(L"PUT",NULL,NULL);
  if( isaccess != e_AccessOK )
  {
    CPPUNIT_FAIL("PUT is not allowed!");
  }
  
  c_RestConfig::DeleteInstance();
}
catch (c_ExceptionRestConfig& e)
{
  std::string utf8;
  Poco::UnicodeConverter::toUTF8(e.m_Desc.c_str(),utf8);
	CPPUNIT_FAIL(utf8);
}
  
}

void c_CfgTest::setUp()
{

}

void c_CfgTest::tearDown()
{

}

