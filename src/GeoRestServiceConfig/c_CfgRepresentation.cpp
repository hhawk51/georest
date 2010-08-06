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
#include "c_CfgRepresentation.h"
#include "Poco\UnicodeConverter.h"
#include "Poco\String.h"



c_CfgRepresentation::c_CfgRepresentation(e_RepresentationType RepresentationType,const wchar_t* Pattern,const wchar_t* MimeType)
{
  m_RepresentationType = RepresentationType;
  m_Pattern = Pattern;
  m_MimeType = MimeType;
  
  m_OrderDirection = e_Asc;
  
  m_Cached_GET=NULL;
  //m_IsMaxBBoxHeight=false;
  //m_IsMaxBBoxWidth=false;;
  //m_IsMaxCount=false;
}

c_CfgRepresentation::~c_CfgRepresentation(void)
{
  
  t_CfgAccessMethodVector::iterator iter;
  for ( iter = m_CfgAccessMethodVector.begin( ) ; iter != m_CfgAccessMethodVector.end( ) ; iter++ )
  {
    c_CfgAccessMethod * adata = *iter;
    delete adata;
  }  
  
}

void c_CfgRepresentation::AddMethod( c_CfgAccessMethod* Method )
{
  m_CfgAccessMethodVector.push_back(Method);
  if( wcsicmp(Method->m_HttpMethod.c_str(),L"get")==0)
  {
    m_Cached_GET = Method;
  }
}

bool c_CfgRepresentation::IsCountInsideLimit( int Count ) const
{
  if( IsCountLimitSet() && m_Cached_GET)
  {
    if( Count>=0 && Count<=m_Cached_GET->m_MaxCount ) return true;

    return false;
  }

  return true;
}

bool c_CfgRepresentation::IsWidthInsideLimit( double Width ) const
{
  if( IsBBoxWidthLimitSet() && m_Cached_GET )
  {
    if( Width>=0 && Width<=m_Cached_GET->m_MaxBBoxWidth ) return true;

    return false;
  }

  return true;
}

bool c_CfgRepresentation::IsHeightInsideLimit( double Height ) const
{
  if( IsBBoxHeightLimitSet() && m_Cached_GET )
  {
    if( Height>=0 && Height<=m_Cached_GET->m_MaxBBoxHeight ) return true;

    return false;
  }

  return true;
}


e_REST_AccessCodes c_CfgRepresentation::IsAccess( const wchar_t*Method,const wchar_t*UserName,const wchar_t* Password ) const
{
  if( !Method )
  {
    return e_AccessOK ;
  }

  // If no list defined, then by default 'get' methods are allowed, others are not
  if( m_CfgAccessMethodVector.size() == 0 )
  {
    if( wcsicmp(Method,L"get") == 0 )
    {
      return e_AccessOK;
    }

    return e_NoAccess_FormatMethod;
  }
  

  // Find that Method In list
  t_CfgAccessMethodVector::const_iterator iter_method;

  for ( iter_method = m_CfgAccessMethodVector.begin( ) ; iter_method != m_CfgAccessMethodVector.end( ) ; iter_method++ )
  {       
    c_CfgAccessMethod * format_method = *iter_method;  
    if( wcsicmp(format_method->m_HttpMethod.c_str(),Method) == 0 )
    {
      // check if that user is allowed
      if( !UserName || !Password ) return e_AccessOK;

      if( format_method->m_Users.size() == 0 ) // no users defined, allow then
      {
        return e_AccessOK;  
      }

      // find match with a user
      t_CfgAccessFormatMethodUserVector::iterator iter_user;

      for ( iter_user = format_method->m_Users.begin( ) ; iter_user != format_method->m_Users.end( ) ; iter_user++ )
      {   
        c_CfgAccessMethodUser * format_method_user = *iter_user; 

        if( wcscmp(format_method_user->m_UserName.c_str(),UserName)==0 && wcscmp(format_method_user->m_Password.c_str(),Password)==0 )
        {
          return e_AccessOK;
        }            
      }

      return e_NoAccess_FormatMethodUser;
    }
  }

  // can't find method 
  return e_NoAccess_FormatMethod;  

  // Format is not allow
  return e_NoAccess_Format;


}//end of c_CfgDataResource::IsAllowed_HttpMethod


bool c_CfgRepresentation::IsBBoxHeightLimitSet() const
{
  return m_Cached_GET ? m_Cached_GET->m_IsMaxBBoxHeight : false;
}

bool c_CfgRepresentation::IsBBoxWidthLimitSet() const
{
  return m_Cached_GET ? m_Cached_GET->m_IsMaxBBoxWidth : false;
}

bool c_CfgRepresentation::IsCountLimitSet() const
{
  return m_Cached_GET ? m_Cached_GET->m_IsMaxCount : false;
}

long c_CfgRepresentation::GetMaxCount() const
{
  if( m_Cached_GET && m_Cached_GET->m_IsMaxCount ) return m_Cached_GET->m_MaxCount;
  
  return -1;
}



c_AtomElementOverride::c_AtomElementOverride()
{
  m_OverridType = e_Static;
  m_IsValueUTF8 = false;
}

const std::string& c_AtomElementOverride::GetValueUTF8() const
{
  if( !m_IsValueUTF8 )
  {
    Poco::UnicodeConverter::toUTF8(m_Value,m_ValueUTF8);
    m_IsValueUTF8=true;
  }

  return m_ValueUTF8;
}

void c_AtomElementOverride::SetValue( const std::wstring& Value,const std::wstring& OverType )
{
  SetOverrideType(OverType);
  m_Value = Value;
  m_IsValueUTF8 = false;
}

void c_AtomElementOverride::SetOverrideType( const std::wstring& OverType )
{
  if( Poco::icompare(OverType,L"Property") == 0 ) m_OverridType = e_Property;
  if( Poco::icompare(OverType,L"Template") == 0 ) m_OverridType = e_Template;
  if( Poco::icompare(OverType,L"Static") == 0 ) m_OverridType = e_Static;
}