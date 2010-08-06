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

#include "StdAfx.h"
#include "c_CfgDataResource.h"
#include "Poco\UnicodeConverter.h"

c_CfgDataResource::c_CfgDataResource(const wchar_t* UriTag,const char* TemplateFolder)
{
  m_TemplateFolder = TemplateFolder;
  m_UriTag = UriTag;
  m_DataSource = NULL;
  
  
}

c_CfgDataResource::~c_CfgDataResource(void)
{
  if( m_DataSource ) delete m_DataSource;
  
  {
    t_CfgRepresentationVector::iterator iter;
    for ( iter = m_Representations.begin( ) ; iter != m_Representations.end( ) ; iter++ )
    {
      c_CfgRepresentation * adata = *iter;
      delete adata;
    }
  }
}




const c_CfgRepresentation* c_CfgDataResource::FindRepresentation( const wchar_t* Pattern ) const
{
  t_CfgRepresentationVector::const_iterator iter;
  for ( iter = m_Representations.begin( ) ; iter != m_Representations.end( ) ; iter++ )
  {
    const c_CfgRepresentation * rep = *iter;
    if( wcsicmp(rep->m_Pattern.c_str(),Pattern) == 0 ) return rep;
  }
  return NULL;
}

const c_CfgRepresentation* c_CfgDataResource::FindRepresentation( c_CfgRepresentation::e_RepresentationType RepType ) const
{
  t_CfgRepresentationVector::const_iterator iter;
  for ( iter = m_Representations.begin( ) ; iter != m_Representations.end( ) ; iter++ )
  {
    const c_CfgRepresentation * rep = *iter;
    if( rep->GetType() == RepType ) return rep;
  }
  return NULL;
}

const std::string& c_CfgDataResource::GetUriTag() const
{
  if( m_UriTagUtf8.length()== 0 )
  {
  
    Poco::UnicodeConverter::toUTF8(m_UriTag,m_UriTagUtf8);
  }
  
  return m_UriTagUtf8;
}



c_CfgDataResourceVector::~c_CfgDataResourceVector( void )
{
  Clear();
}

void c_CfgDataResourceVector::Clear()
{
  t_CfgDataResourceVector::iterator iter;
  for ( iter = m_CfgDataResourceVector.begin( ) ; iter != m_CfgDataResourceVector.end( ) ; iter++ )
  {
    c_CfgDataResource * datalayer = *iter;
    delete datalayer;
  }
  m_CfgDataResourceVector.clear();
}

const c_CfgDataResource* c_CfgDataResourceVector::FindUriTag( const std::wstring& UriTag ) 
{
  t_CfgDataResourceVector::iterator iter;
  for ( iter = m_CfgDataResourceVector.begin( ) ; iter != m_CfgDataResourceVector.end( ) ; iter++ )
  {
    if( wcsicmp(UriTag.c_str(),(*iter)->m_UriTag.c_str()) == 0 ) return *iter;
  }
  
  return NULL;
}
/*
const c_CfgDataResource* c_CfgDataResourceVector::FindClassName( const std::wstring& ClassName ) 
{
  
  std::wstring::size_type iColon = ClassName.find(':');
  if(iColon != std::wstring::npos) 
  {
    // full name
    t_CfgDataResourceVector::iterator iter;
    for ( iter = m_CfgDataResourceVector.begin( ) ; iter != m_CfgDataResourceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ClassName.c_str()) == 0 ) return *iter;
    }  
  }
  else
  {
    // short name
    t_CfgDataResourceVector::iterator iter;
    for ( iter = m_CfgDataResourceVector.begin( ) ; iter != m_CfgDataResourceVector.end( ) ; iter++ )
    {
      if( wcsicmp(ClassName.c_str(),(*iter)->m_ShortClassName.c_str()) == 0 ) return *iter;
    }  
  }
  
  
  return NULL;
}
*/

bool c_CfgDataResourceVector::Add( c_CfgDataResource* DataLayer )
{
  if( FindUriTag(DataLayer->m_UriTag.c_str()) )
  {
    return false;
  }
  
  m_CfgDataResourceVector.push_back(DataLayer);
  return true;
}

int c_CfgDataResourceVector::GetCount()
{
  return m_CfgDataResourceVector.size();
}

const c_CfgDataResource* c_CfgDataResourceVector::GetResource( int Index )
{
  return m_CfgDataResourceVector.at(Index);
}

