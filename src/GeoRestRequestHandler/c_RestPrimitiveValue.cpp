//
//  Copyright (C) 2009 by Haris Kurtagic
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
#include "c_RestPrimitiveValue.h"


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new c_RestPrimitiveValue object initialized
/// with a boolean value
/// </summary>
c_RestPrimitiveValue::c_RestPrimitiveValue(bool value)
{
    m_value.b = value;
    m_PrimitiveType = e_PrimitiveTypeBool;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new c_RestPrimitiveValue object initialized
/// with an integer value
/// </summary>
c_RestPrimitiveValue::c_RestPrimitiveValue(INT32 value)
{
    m_value.i = value;
    m_PrimitiveType = e_PrimitiveTypeInt;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new c_RestPrimitiveValue object initialized
/// with a string value
/// </summary>
c_RestPrimitiveValue::c_RestPrimitiveValue(CREFSTRING value)
{
    if(!(m_value.pstr = new STRING(value)))
        throw new MgOutOfMemoryException(L"c_RestPrimitiveValue", __LINE__, __WFILE__, NULL, L"", NULL);
    m_PrimitiveType = e_PrimitiveTypeString;
}


c_RestPrimitiveValue::c_RestPrimitiveValue(const std::string& value)
{
  if(!(m_value.pstr_utf8 = new std::string(value)))
    throw new MgOutOfMemoryException(L"c_RestPrimitiveValue", __LINE__, __WFILE__, NULL, L"", NULL);
  m_PrimitiveType = e_PrimitiveTypeStringUTF8;
}
//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor. This will clean up the value.
/// </summary>
c_RestPrimitiveValue::~c_RestPrimitiveValue()
{
    if(m_PrimitiveType == e_PrimitiveTypeString)
        delete m_value.pstr;
        
    if(m_PrimitiveType == e_PrimitiveTypeStringUTF8)
      delete m_value.pstr_utf8;        
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the type of the data held by this object
/// </summary>
c_RestPrimitiveValue::e_PrimitiveType c_RestPrimitiveValue::GetType()
{
    return m_PrimitiveType;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the boolean primitive value
/// </summary>
bool c_RestPrimitiveValue::GetBoolValue()
{
    return m_value.b;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the integer primitive value
/// </summary>
INT32 c_RestPrimitiveValue::GetIntegerValue()
{
    return m_value.i;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the string primitive value
/// </summary>
STRING c_RestPrimitiveValue::GetStringValue()
{
    return *m_value.pstr;
}

const std::string& c_RestPrimitiveValue::GetStringUTF8Value() const
{
  return *m_value.pstr_utf8;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get a string representation of the primitive value
/// </summary>
STRING c_RestPrimitiveValue::ToString()
{

    switch(m_PrimitiveType)
    {
        case e_PrimitiveTypeBool:
            return m_value.b? L"True": L"False";

        case e_PrimitiveTypeInt:
        {
            wchar_t buf[30];
            swprintf(buf, 30, L"%d", m_value.i);
            return buf;
        }
        case e_PrimitiveTypeString:
            return *m_value.pstr;
        case e_PrimitiveTypeStringUTF8:
        {
          return MgUtil::MultiByteToWideChar(*m_value.pstr_utf8);
          
        }
    }

    assert(false);
    return L"";
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get a string representation of the primitive value
/// </summary>
std::string c_RestPrimitiveValue::ToStringUTF8()
{

  switch(m_PrimitiveType)
  {
  case e_PrimitiveTypeBool:
    return m_value.b? "True": "False";

  case e_PrimitiveTypeInt:
    {
      char buf[30];
      sprintf(buf, "%d", m_value.i);
      return buf;
    }
    case e_PrimitiveTypeString:
    {
      return MG_WCHAR_TO_CHAR(*m_value.pstr);
      
    }
  case e_PrimitiveTypeStringUTF8:
    return *m_value.pstr_utf8;
  }

  assert(false);
  return "";
}

void c_RestPrimitiveValue::Dispose()
{
    delete this;
}

INT32 c_RestPrimitiveValue::GetClassId()
{
    return m_cls_id;
}
