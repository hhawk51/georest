//
//  Copyright (C) 2010 by  Haris Kurtagic  www.sl-king.com
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

#include "StdAfx.h"
#include "c_CfgService.h"
#include "Poco\UnicodeConverter.h"

c_CfgService::c_CfgService(const wchar_t* UriTag,e_ServiceType Type)
//c_CfgService::c_CfgService()
{
  m_UriTag = UriTag;
  m_Type = Type;  
}



const std::string& c_CfgService::GetUriTag() const
{
  if( m_UriTagUtf8.length()== 0 )
  {
  
    Poco::UnicodeConverter::toUTF8(m_UriTag,m_UriTagUtf8);
  }
  
  return m_UriTagUtf8;
}

c_CfgService::~c_CfgService( void )
{

}
c_CfgServiceVector::~c_CfgServiceVector( void )
{
  Clear();
}

void c_CfgServiceVector::Clear()
{
  t_CfgServiceVector::iterator iter;
  for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
  {
    c_CfgService * datalayer = *iter;
    delete datalayer;
  }
  m_CfgServiceVector.clear();
}


const c_CfgService* c_CfgServiceVector::FindUriTag( const std::wstring& UriTag ) const
{
  t_CfgServiceVector::const_iterator iter;
  for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
  {
    if( wcsicmp(UriTag.c_str(),(*iter)->m_UriTag.c_str()) == 0 ) return *iter;
  }
  
  return NULL;
}
c_CfgService* c_CfgServiceVector::FindUriTag( const std::wstring& UriTag ) 
{
  t_CfgServiceVector::iterator iter;
  for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
  {
    if( wcsicmp(UriTag.c_str(),(*iter)->m_UriTag.c_str()) == 0 ) return *iter;
  }

  return NULL;
}
/*
const c_CfgService* c_CfgServiceVector::FindClassName( const std::wstring& ClassName ) 
{
  
  std::wstring::size_type iColon = ClassName.find(':');
  if(iColon != std::wstring::npos) 
  {
    // full name
    t_CfgServiceVector::iterator iter;
    for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ClassName.c_str()) == 0 ) return *iter;
    }  
  }
  else
  {
    // short name
    t_CfgServiceVector::iterator iter;
    for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ShortClassName.c_str()) == 0 ) return *iter;
    }  
  }
  
  
  return NULL;
}
*/

bool c_CfgServiceVector::Add( c_CfgService* DataLayer )
{
  if( FindUriTag(DataLayer->m_UriTag.c_str()) )
  {
    return false;
  }
  
  m_CfgServiceVector.push_back(DataLayer);
  return true;
}

int c_CfgServiceVector::GetCount()
{
  return m_CfgServiceVector.size();
}

const c_CfgService* c_CfgServiceVector::GetService( int Index ) const
{
  return m_CfgServiceVector.at(Index);
}



/*
const c_CfgService* c_CfgServiceVector::FindClassName( const std::wstring& ClassName ) 
{
  
  std::wstring::size_type iColon = ClassName.find(':');
  if(iColon != std::wstring::npos) 
  {
    // full name
    t_CfgServiceVector::iterator iter;
    for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ClassName.c_str()) == 0 ) return *iter;
    }  
  }
  else
  {
    // short name
    t_CfgServiceVector::iterator iter;
    for ( iter = m_CfgServiceVector.begin( ) ; iter != m_CfgServiceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ShortClassName.c_str()) == 0 ) return *iter;
    }  
  }
  
  
  return NULL;
}
*/


c_CfgCustomService::c_CfgCustomService( const wchar_t* UriTag,const wchar_t* Library ) : c_CfgService(UriTag,e_Custom_Service)
{
  m_Library = Library;
  m_LibraryLoaded=0;
  m_CreateRequestHandler=NULL;
}