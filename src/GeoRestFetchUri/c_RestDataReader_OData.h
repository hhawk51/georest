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

#ifndef _c_RestDataReader_OData_h
#define _c_RestDataReader_OData_h

#include "c_RestDataReader.h"
#include "Poco\SAX\InputSource.h"
#include "Poco\DOM\DOMParser.h"
#include "Poco\DOM\Document.h"
#include "Poco\AutoPtr.h"
#include "Poco\DOM\Element.h"
#include "Poco\DOM\NodeList.h"

class REST_FETCHURI_API c_RestDataReader_OData : public c_RestDataReader
{
public:
  c_RestDataReader_OData( MgClassDefinition* ClassDef,std::istream& InStream );
public:
  virtual bool ReadNext();  
  virtual MgClassDefinition* GetClassDefinition();
  virtual bool IsNull(CREFSTRING propertyName);

  virtual bool GetBoolean(CREFSTRING propertyName);

  virtual BYTE GetByte(CREFSTRING propertyName);

  virtual MgDateTime* GetDateTime(CREFSTRING propertyName);

  virtual float GetSingle(CREFSTRING propertyName);

  virtual double GetDouble(CREFSTRING propertyName);

  virtual INT16 GetInt16(CREFSTRING propertyName);

  virtual INT32 GetInt32(CREFSTRING propertyName);

  virtual INT64 GetInt64(CREFSTRING propertyName);

  virtual STRING GetString(CREFSTRING propertyName);


  virtual MgByteReader* GetGeometry(CREFSTRING propertyName);



  virtual INT32 GetPropertyCount();

  virtual STRING GetPropertyName(INT32 index);

  virtual INT32 GetPropertyType(CREFSTRING propertyName);

  virtual void Close();
  

protected:
  bool FindProperty( const std::string& propertyName) const;
  void CheckForValue(CREFSTRING propertyName) const;
  void ThrowException_InvalidValue( CREFSTRING propertyName, const std::string& ValueType );
  //void CheckValueType(CREFSTRING propertyName, MgPropertyType ptype);


protected:
  Ptr<MgClassDefinition> m_MgClassDefinition;
  Ptr<MgPropertyDefinitionCollection> m_MgClassProperties;
  std::istream& m_InStream;
  Poco::XML::InputSource m_XmlInputStream;
  Poco::XML::DOMParser m_DomParser;
  Poco::AutoPtr<Poco::XML::Document> m_XmlDoc;
  
  int m_ReadCount;
  Poco::XML::Element* m_EntryElement;
  Poco::AutoPtr<Poco::XML::NodeList> m_PropertiesList;
  
  mutable std::string m_MType;
  mutable std::string m_MNull;
  mutable std::string m_PropertyStrVal;
  
    
};



#endif