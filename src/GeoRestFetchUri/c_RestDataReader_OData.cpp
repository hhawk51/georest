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

#include "c_RestDataReader.h"

#include "c_RestDataReader_OData.h"
#include "Poco\DOM\DOMParser.h"
#include "Poco\DOM\Element.h"
#include "Poco\DOM\Node.h"
#include "c_ExceptionGeoREST.h"
#include "Poco\UnicodeConverter.h"
#include "Poco\NumberFormatter.h"
#include "c_OData_ValueParser.h"


c_RestDataReader_OData::c_RestDataReader_OData( MgClassDefinition* ClassDef,std::istream& InStream )
  : m_InStream(InStream), m_XmlInputStream(InStream)
{
  m_MgClassDefinition = SAFE_ADDREF(ClassDef);
  if( m_MgClassDefinition )
  {
    m_MgClassProperties = m_MgClassDefinition->GetProperties();
  }
  else
  {
    m_MgClassProperties = NULL;
  }
  m_ReadCount=-1;
  
  m_EntryElement = 0;
}

MgClassDefinition* c_RestDataReader_OData::GetClassDefinition()
{
  return SAFE_ADDREF(m_MgClassDefinition.p);
}


bool c_RestDataReader_OData::ReadNext()
{
  if( m_ReadCount < 0)  
  {
  // initialize xml reader  
    m_ReadCount = 0;
    m_XmlDoc= m_DomParser.parse(&m_XmlInputStream);
    
    // now try  get first entry - element
    m_EntryElement = m_XmlDoc->documentElement();
    if( m_EntryElement )
    {
      if( m_EntryElement->tagName().compare("feed") == 0)
      {
        m_EntryElement = m_EntryElement->getChildElement("entry");
      }    
      else
      {
        if( m_EntryElement->tagName().compare("entry") )
        {
          m_EntryElement;
        }
      }  
    }            
  }
  
  if( !m_EntryElement ) return false;
  m_PropertiesList = NULL;
  
  Poco::XML::Element * next_elem_entry=NULL;
  Poco::XML::Node * next_node;
  next_node = m_EntryElement->nextSibling();
  while(next_node )
  {
    if( next_node->nodeType()==Poco::XML::Node::ELEMENT_NODE )
    {
      //next_elem_entry = ;
      if( static_cast<Poco::XML::Element*>(next_node)->nodeName().compare("entry") == 0 )
      {
        next_elem_entry=static_cast<Poco::XML::Element*>(next_node);
        break;
      }              
    }
    next_node = next_node->nextSibling();
  }
  
  if( !next_elem_entry )
  {
    m_EntryElement = NULL;
    return false;
  }
  
  m_EntryElement = next_elem_entry;
  
  
  Poco::XML::Element *content =  m_EntryElement->getChildElement("content");
  if( !content ) 
  {
    throw c_ExceptionODataInvalidAtomEntry("No 'content' in atom entry!");
  }
  Poco::XML::Element *properties =  content->getChildElement("properties");
  if( !properties ) 
  {
    throw c_ExceptionODataInvalidAtomEntry("No 'properties' in content of atom entry!");
  }
  
  m_ReadCount++;
  m_PropertiesList = properties->getElementsByTagName("Name");
  
  return true;
  
}



bool c_RestDataReader_OData::FindProperty( const std::string& propertyName) const
{
  if( m_PropertiesList.isNull() ) return false;
  
  Poco::XML::Element* prop;
  int count = m_PropertiesList->length();
  for(int ind=0;ind<count;ind++)
  {
    prop = (Poco::XML::Element*)m_PropertiesList->item(ind);
    if( prop->nodeName().compare(propertyName) == 0 )
    {
      m_MType = prop->getAttribute("type");
      m_MNull = prop->getAttribute("null");
      m_PropertyStrVal = prop->innerText();
      return true;
    }
  }
  
  return false;
}

bool c_RestDataReader_OData::IsNull( CREFSTRING propertyName )
{
  if( m_PropertiesList.isNull() ) return true;
  
  std::string prop_name;
  Poco::UnicodeConverter::toUTF8(propertyName,prop_name);
  
  if( !FindProperty(prop_name) ) return true;
  
  if( m_MNull.compare("true") == 0 ) return true;
  
  return false;
  
}

void c_RestDataReader_OData::CheckForValue(CREFSTRING propertyName) const
{
  if( !m_EntryElement )
  {
    throw c_ExceptionODataReader("OData Reader is not set on valid value! ");
  }
 
  std::string prop_name;
  Poco::UnicodeConverter::toUTF8(propertyName,prop_name);
  
  if( m_PropertiesList.isNull() ) 
  {
    std::string errstr = "Property '"+prop_name+"' is not found! Propety list is empty!";
    throw c_ExceptionODataReader(errstr);
  }
  
  if( !FindProperty(prop_name) ) 
  {
    std::string errstr = "Property '"+prop_name+"' is not found!";
    throw c_ExceptionODataReader(errstr);
  }

  if( m_MNull.compare("true") == 0 ) 
  {
    throw c_ExceptionODataReader("Property is NULL!");
  }  
}

/*
void c_RestDataReader_OData::CheckValueType(CREFSTRING propertyName, MgPropertyType ptype)
{
  if( m_MgClassProperties.p )
  {
    Ptr<MgPropertyDefinition> prop_def = m_MgClassProperties->FindItem(propertyName);
    if( prop_def->GetPropertyType() == MgFeaturePropertyType::GeometricProperty ) 
    {
      if( ptype == MgFeaturePropertyType::GeometricProperty )
      {
      
      }
    }
    else
    {
      throw c_ExceptionODataReader("Property is not of requested type!");
    }
    
    return;
  }
  
// If MgClassIs not defined tha check for   
  
  switch(MgPropType)
  {
    case MgPropertyType::Boolean:
    {
    
    }
    break;
  }
}
*/


void c_RestDataReader_OData::ThrowException_InvalidValue( CREFSTRING propertyName, const std::string& ValueType )
{
  std::string errstr;errstr.reserve(512);
  
  errstr = "OData reader: Invalid ";
  errstr.append(ValueType);
  errstr.append(" value for Property '");
  std::string tmp1;
  Poco::UnicodeConverter::toUTF8(propertyName,tmp1);
  errstr.append(tmp1);
  errstr.append("'!");
  
  if( m_PropertyStrVal.length() < 300 )
  {
    errstr.append(" Value = '" );
    errstr.append(m_PropertyStrVal );
    errstr.append("'" );
  }
  else
  {
    int len = m_PropertyStrVal.length();
    errstr.append(" Value is to large to display! Length is ");
    Poco::NumberFormatter::append(errstr,len);
    errstr.append(". First 200 characters are : '" );
    tmp1 = m_PropertyStrVal.substr(0,200);
    errstr.append("tmp1");
    errstr.append("'" );
  }
 
   throw c_ExceptionODataReader(errstr);    
}

bool c_RestDataReader_OData::GetBoolean( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  bool bval;
  if( c_OData_ValueParser::ParseBoolean(m_PropertyStrVal,bval) ) return bval;
  
  ThrowException_InvalidValue(propertyName,"Boolean");
}

BYTE c_RestDataReader_OData::GetByte( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  BYTE bval;
  if( c_OData_ValueParser::ParseByte(m_PropertyStrVal,bval) ) return bval;

  ThrowException_InvalidValue(propertyName,"Byte");
}

MgDateTime* c_RestDataReader_OData::GetDateTime( CREFSTRING propertyName )
{
  CheckForValue(propertyName);
  
  MgDateTime *ptr = new MgDateTime();
  
  if( c_OData_ValueParser::ParseDateTime(m_PropertyStrVal,ptr) ) return ptr;

  ThrowException_InvalidValue(propertyName,"DateTime");
}

float c_RestDataReader_OData::GetSingle( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  float val;

  if( c_OData_ValueParser::ParseSingle(m_PropertyStrVal,val) ) return val;

  ThrowException_InvalidValue(propertyName,"Single");
}

double c_RestDataReader_OData::GetDouble( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  double val;

  if( c_OData_ValueParser::ParseDouble(m_PropertyStrVal,val) ) return val;

  ThrowException_InvalidValue(propertyName,"Double");
}

INT16 c_RestDataReader_OData::GetInt16( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  short val;

  if( c_OData_ValueParser::ParseInt16(m_PropertyStrVal,val) ) return val;

  ThrowException_InvalidValue(propertyName,"Int64");
}

INT32 c_RestDataReader_OData::GetInt32( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  int val;

  if( c_OData_ValueParser::ParseInt32(m_PropertyStrVal,val) ) return val;

  ThrowException_InvalidValue(propertyName,"Int32");
}

INT64 c_RestDataReader_OData::GetInt64( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  INT64 val;

  if( c_OData_ValueParser::ParseInt64(m_PropertyStrVal,val) ) return val;

  ThrowException_InvalidValue(propertyName,"Int64");
}

STRING c_RestDataReader_OData::GetString( CREFSTRING propertyName )
{
  CheckForValue(propertyName);

  STRING val;
  std::string valutf8;
  if( c_OData_ValueParser::ParseString(m_PropertyStrVal,valutf8) ) 
  {
    Poco::UnicodeConverter::toUTF16(valutf8,val);
    return val;
  }

  ThrowException_InvalidValue(propertyName,"String");
}


MgByteReader* c_RestDataReader_OData::GetGeometry( CREFSTRING propertyName )
{
 throw c_ExceptionODataReader("OData reader: Geometry type is not supported!");

}

INT32 c_RestDataReader_OData::GetPropertyCount()
{
  if( !m_EntryElement )
  {
    throw c_ExceptionODataReader("OData Reader is not set on valid value! ");
  }

  if( m_PropertiesList.isNull() ) 
  {
    throw c_ExceptionODataReader("OData Reader: Property list is empty! ");
  }
  
  return m_PropertiesList->length();
}

STRING c_RestDataReader_OData::GetPropertyName( INT32 index )
{
  if( !m_EntryElement )
  {
    throw c_ExceptionODataReader("OData Reader is not set on valid value! ");
  }

  if( m_PropertiesList.isNull() ) 
  {
    throw c_ExceptionODataReader("OData Reader: Property list is empty! ");
  }
  if( (index<0) || (index >= m_PropertiesList->length()) ) 
  {
    throw c_ExceptionODataReader("OData Reader: GetPropertyName invalid index input value! ");
  }
  
  Poco::XML::Element* prop = (Poco::XML::Element*)m_PropertiesList->item(index);
  
  std::wstring tmputf16;
  Poco::UnicodeConverter::toUTF16(prop->nodeName(),tmputf16);
  return tmputf16;
    
}


INT32 c_RestDataReader_OData::GetPropertyType( CREFSTRING propertyName )
{
  if( !m_EntryElement )
  {
    throw c_ExceptionODataReader("OData Reader is not set on valid value! ");
  }

  std::string prop_name;
  Poco::UnicodeConverter::toUTF8(propertyName,prop_name);

  if( m_PropertiesList.isNull() ) 
  {
    std::string errstr = "Property '"+prop_name+"' is not found! Propety list is empty!";
    throw c_ExceptionODataReader(errstr);
  }

  if( !FindProperty(prop_name) ) 
  {
    std::string errstr = "Property '"+prop_name+"' is not found!";
    throw c_ExceptionODataReader(errstr);
  }
  
  if( m_MType.length() == 0 ) return MgPropertyType::String;
  
  int mgtype;
  if( c_OData_ValueParser::EdmDataType2MgPropertyType( m_MType,mgtype) )
  {
    return mgtype;
  }
  
  
  std::string errstr = "Invalid Edm Data Type '"+m_MType+"'!";
  throw c_ExceptionODataReader(errstr);
  
  
  
}

void c_RestDataReader_OData::Close()
{
}  

