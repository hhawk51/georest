//
//  Copyright (C) 2009 by  Haris Kurtagic  www.sl-king.com
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

//#include "RestSessionMap.h"
#include "RestDefs.h"
#include "c_RestUriFactory.h"
#include "HttpResourceStrings.h"

c_RestUriFactory * c_RestUriFactory ::CreateObject()
{
  return new c_RestUriFactory;
}

c_RestUriFactory::c_RestUriFactory()
{
}

void c_RestUriFactory::Dispose()
{
  delete this;
}

INT32 c_RestUriFactory::GetClassId()
{
  return m_cls_id;
}

c_RestUriFactory::~c_RestUriFactory()
{
}

void c_RestUriFactory::Create_MapDefinition(const std::string& MapDefName,std::string& Uri)
{
  Uri = "/REST/MapDefinition["+MapDefName+"]";
}
void c_RestUriFactory::Create_MapDefinition(const std::wstring& MapDefName,std::string& Uri)
{
  Create_MapDefinition(MgUtil::WideCharToMultiByte(MapDefName),Uri);
}

void c_RestUriFactory::Create_Session(const std::string& Session,std::string& Uri)
{
  Uri = "/REST/Session["+Session+"]";
}
void c_RestUriFactory::Create_Session(const std::wstring& Session,std::string& Uri)
{
  //Uri = "/REST/Session["+Session+"]";
  Create_Session(MgUtil::WideCharToMultiByte(Session),Uri);
}

void c_RestUriFactory::Create_Session_Map(const std::string& Session,const std::string& Map,std::string& Uri)
{
  Uri = "/REST/Session["+Session+"]/Map["+Map+"]";
}
void c_RestUriFactory::Create_Session_Map(const std::wstring& Session,const std::wstring& Map,std::string& Uri)
{
  //Uri = "/REST/Session[" + MgUtil::WideCharToMultiByte(Session) + "]/Map[" + MgUtil::WideCharToMultiByte(Map) + "]";
  Create_Session_Map(MgUtil::WideCharToMultiByte(Session),MgUtil::WideCharToMultiByte(Map),Uri);
}

void c_RestUriFactory::Create_Session_Map_ImagePng(const std::string& Session,const std::string& Map,std::string& Uri)
{
  Uri = "/REST/Session["+Session+"]/Map["+Map+"]/Image.Png";
}
void c_RestUriFactory::Create_Session_Map_ImagePng(const std::wstring& Session,const std::wstring& Map,std::string& Uri)
{
  Create_Session_Map_ImagePng(MgUtil::WideCharToMultiByte(Session),MgUtil::WideCharToMultiByte(Map),Uri);
}

void c_RestUriFactory::Create_Session_Map_QueryMapFeatures(const std::string& Session,const std::string& Map,std::string& Uri)
{
  Uri = "/REST/Session["+Session+"]/Map["+Map+"]/QueryMapFeatures";
}
void c_RestUriFactory::Create_Session_Map_QueryMapFeatures(const std::wstring& Session,const std::wstring& Map,std::string& Uri)
{
  Create_Session_Map_QueryMapFeatures(MgUtil::WideCharToMultiByte(Session),MgUtil::WideCharToMultiByte(Map),Uri);
}

void c_RestUriFactory::Create_LayerDef_LegendIcon( const std::string& LayerDefResId,double Scale,int ThemeCategory,int GeometryType,std::string& Uri )
{
  std::wstring tempstr,filter;    
  MgUtil::DoubleToString(Scale,tempstr);  
  filter = MgHttpResourceStrings::reqMappingScale + L"=" + tempstr;
  
  MgUtil::Int32ToString(GeometryType,tempstr);  
  filter = filter + L"&" + MgHttpResourceStrings::reqMappingType + L"=" + tempstr;
  
  MgUtil::Int32ToString(ThemeCategory,tempstr);  
  filter = filter + L"&" + MgHttpResourceStrings::reqMappingThemeCategory + L"=" + tempstr;
  
  Uri = "/REST/LayerDefinition["+LayerDefResId+"]/LegendIcon";  
  
  std::string str;
  MgUtil::WideCharToMultiByte(filter,str);
  Uri = Uri + "[" + str + "]";
}
void c_RestUriFactory::Create_LayerDef_LegendIcon( const std::wstring& LayerDefResId,double Scale,int ThemeCategory,int GeometryType,std::string& Uri )
{
  Create_LayerDef_LegendIcon( MgUtil::WideCharToMultiByte(LayerDefResId),Scale,ThemeCategory,GeometryType,Uri );
}


void c_RestUriFactory::Create_FDO_DataSource_Class_Feature(const std::string& DataSource,const std::string& ClassName,std::string& Uri)
{
  Uri = "/REST/FDO/DATASOURCE["+DataSource+"]/CLASS["+ClassName+"]/FEATURE";
}
void c_RestUriFactory::Create_FDO_DataSource_Class_Feature(const std::wstring& DataSource,const std::wstring& ClassName,std::string& Uri)
{
  Create_FDO_DataSource_Class_Feature(MgUtil::WideCharToMultiByte(DataSource),MgUtil::WideCharToMultiByte(ClassName),Uri);
}