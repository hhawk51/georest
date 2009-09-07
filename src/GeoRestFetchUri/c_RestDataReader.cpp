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

#include "c_RestDataReader.h"
#include "c_MgServerFeatureUtil.h"


c_RestDataReader_FdoFeatureReader::c_RestDataReader_FdoFeatureReader( FdoIConnection* FdoConnection,FdoIFeatureReader* FdoReader )
{
  m_FdoConnection = FDO_SAFE_ADDREF(FdoConnection);
  m_FdoReader = FDO_SAFE_ADDREF(FdoReader);
}


c_RestDataReader_MgFeatureReader::c_RestDataReader_MgFeatureReader( MgProxyFeatureReader* Reader)
{
  m_FeatureReader = SAFE_ADDREF(Reader);
}



bool c_RestDataReader_MgFeatureReader::ReadNext()
{
  return m_FeatureReader->ReadNext();
}
MgClassDefinition* c_RestDataReader_MgFeatureReader::GetClassDefinition()
{
  return m_FeatureReader->GetClassDefinition();
}

bool c_RestDataReader_MgFeatureReader::IsNull( CREFSTRING propertyName )
{
  return m_FeatureReader->IsNull(propertyName);
}

bool c_RestDataReader_MgFeatureReader::GetBoolean( CREFSTRING propertyName )
{
  return m_FeatureReader->GetBoolean(propertyName);
}

BYTE c_RestDataReader_MgFeatureReader::GetByte( CREFSTRING propertyName )
{
  return m_FeatureReader->GetByte(propertyName);
}

MgDateTime* c_RestDataReader_MgFeatureReader::GetDateTime( CREFSTRING propertyName )
{
  return m_FeatureReader->GetDateTime(propertyName);
}

float c_RestDataReader_MgFeatureReader::GetSingle( CREFSTRING propertyName )
{
  return m_FeatureReader->GetSingle(propertyName);
}

double c_RestDataReader_MgFeatureReader::GetDouble( CREFSTRING propertyName )
{
  return m_FeatureReader->GetDouble(propertyName);
}

INT16 c_RestDataReader_MgFeatureReader::GetInt16( CREFSTRING propertyName )
{
  return m_FeatureReader->GetInt16(propertyName);
}

INT32 c_RestDataReader_MgFeatureReader::GetInt32( CREFSTRING propertyName )
{
  return m_FeatureReader->GetInt32(propertyName);
}

INT64 c_RestDataReader_MgFeatureReader::GetInt64( CREFSTRING propertyName )
{
  return m_FeatureReader->GetInt64(propertyName);
}

STRING c_RestDataReader_MgFeatureReader::GetString( CREFSTRING propertyName )
{
  return m_FeatureReader->GetString(propertyName);
}

MgByteReader* c_RestDataReader_MgFeatureReader::GetBLOB( CREFSTRING propertyName )
{
  return m_FeatureReader->GetBLOB(propertyName);
}

MgByteReader* c_RestDataReader_MgFeatureReader::GetCLOB( CREFSTRING propertyName )
{
  return m_FeatureReader->GetCLOB(propertyName);
}

MgByteReader* c_RestDataReader_MgFeatureReader::GetGeometry( CREFSTRING propertyName )
{
  return m_FeatureReader->GetGeometry(propertyName);
}

MgRaster* c_RestDataReader_MgFeatureReader::GetRaster( CREFSTRING propertyName )
{
  return m_FeatureReader->GetRaster(propertyName);
}

INT32 c_RestDataReader_MgFeatureReader::GetPropertyCount()
{
  return m_FeatureReader->GetPropertyCount();
}

STRING c_RestDataReader_MgFeatureReader::GetPropertyName( INT32 index )
{
  return m_FeatureReader->GetPropertyName(index);
}

INT32 c_RestDataReader_MgFeatureReader::GetPropertyType( CREFSTRING propertyName )
{
  return m_FeatureReader->GetPropertyType(propertyName);
}

void c_RestDataReader_MgFeatureReader::Close()
{
  m_FeatureReader->Close();
}

//*************************************************************
//*
//*            c_RestDataReader_FdoFeatureReader
//*
//****************************************************************

MgClassDefinition* c_RestDataReader_FdoFeatureReader::GetClassDefinition()
{
  FdoPtr<FdoClassDefinition> fdoclass = m_FdoReader->GetClassDefinition();
  return c_MgServerFeatureUtil::GetMgClassDefinition(fdoclass, false);
}


bool c_RestDataReader_FdoFeatureReader::ReadNext()
{
  return m_FdoReader->ReadNext();
}
bool c_RestDataReader_FdoFeatureReader::IsNull( CREFSTRING propertyName )
{
  return m_FdoReader->IsNull(propertyName.c_str());
}

bool c_RestDataReader_FdoFeatureReader::GetBoolean( CREFSTRING propertyName )
{
  return m_FdoReader->GetBoolean(propertyName.c_str());
}

BYTE c_RestDataReader_FdoFeatureReader::GetByte( CREFSTRING propertyName )
{
  return m_FdoReader->GetByte(propertyName.c_str());
}

MgDateTime* c_RestDataReader_FdoFeatureReader::GetDateTime( CREFSTRING propertyName )
{
  FdoDateTime dt = m_FdoReader->GetDateTime(propertyName.c_str());
  return new MgDateTime(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.seconds);
  //return m_FdoReader->GetDateTime(propertyName.c_str());
}

float c_RestDataReader_FdoFeatureReader::GetSingle( CREFSTRING propertyName )
{
  return m_FdoReader->GetSingle(propertyName.c_str());
}

double c_RestDataReader_FdoFeatureReader::GetDouble( CREFSTRING propertyName )
{
  return m_FdoReader->GetDouble(propertyName.c_str());
}

INT16 c_RestDataReader_FdoFeatureReader::GetInt16( CREFSTRING propertyName )
{
  return m_FdoReader->GetInt16(propertyName.c_str());
}

INT32 c_RestDataReader_FdoFeatureReader::GetInt32( CREFSTRING propertyName )
{
  return m_FdoReader->GetInt32(propertyName.c_str());
}

INT64 c_RestDataReader_FdoFeatureReader::GetInt64( CREFSTRING propertyName )
{
  return m_FdoReader->GetInt64(propertyName.c_str());
}

STRING c_RestDataReader_FdoFeatureReader::GetString( CREFSTRING propertyName )
{
  return m_FdoReader->GetString(propertyName.c_str());
}


MgByteReader* c_RestDataReader_FdoFeatureReader::GetGeometry( CREFSTRING propertyName )
{
  FdoPtr<FdoByteArray> fdo_bytes = m_FdoReader->GetGeometry(propertyName.c_str());
  Ptr<MgByteSource> byteSource = new MgByteSource(fdo_bytes->GetData(), fdo_bytes->GetCount());
  return byteSource->GetReader();
  
}

INT32 c_RestDataReader_FdoFeatureReader::GetPropertyCount()
{
  FdoPtr<FdoClassDefinition> fdo_class = m_FdoReader->GetClassDefinition();
  FdoPtr<FdoPropertyDefinitionCollection> props = fdo_class->GetProperties();
  return props->GetCount();
}

STRING c_RestDataReader_FdoFeatureReader::GetPropertyName( INT32 index )
{
  FdoPtr<FdoClassDefinition> fdo_class = m_FdoReader->GetClassDefinition();
  FdoPtr<FdoPropertyDefinitionCollection> props = fdo_class->GetProperties();
  FdoPtr<FdoPropertyDefinition> prop = props->GetItem(index);
  
  return prop->GetName();
}


INT32 c_RestDataReader_FdoFeatureReader::GetPropertyType( CREFSTRING propertyName )
{
  FdoPtr<FdoClassDefinition> fdo_class = m_FdoReader->GetClassDefinition();
  FdoPtr<FdoPropertyDefinitionCollection> props = fdo_class->GetProperties();
  FdoPtr<FdoPropertyDefinition> prop = props->GetItem(propertyName.c_str());

  //INT32 mgtype=0;
  if( prop.p )
  {
    FdoPropertyType fdotype = prop->GetPropertyType();
    
    switch (fdotype)
    {
      // If geometric property, return geometric data type.
      case FdoPropertyType_GeometricProperty:
      {
        return MgPropertyType::Geometry;
        break;
      }
      case FdoPropertyType_DataProperty:
      {
        FdoDataPropertyDefinition *dataprop = (FdoDataPropertyDefinition *)prop.p;
        
        switch(dataprop->GetDataType())
        {
          case FdoDataType_Boolean:  
            return MgPropertyType::Boolean;
          break;
          case FdoDataType_Byte:     return MgPropertyType::Byte;
          case FdoDataType_DateTime : return MgPropertyType::DateTime;
          case FdoDataType_Double :   return MgPropertyType::Double;
          case FdoDataType_Int16 :    return MgPropertyType::Int16;
          case FdoDataType_Int32 :    return MgPropertyType::Int32;
          case FdoDataType_Int64 :    return MgPropertyType::Int64;
          case FdoDataType_Single :   return MgPropertyType::Single;
          case FdoDataType_String :   return MgPropertyType::String;
          case FdoDataType_BLOB :     return MgPropertyType::Blob;
          case FdoDataType_CLOB :     return MgPropertyType::Clob;
        }
        
       

        
        break;
      }
      case FdoPropertyType_RasterProperty:
      {
        return MgPropertyType::Raster;
        break;
      }
      default:
      {
        throw new MgInvalidPropertyTypeException(L"MgServerDataReader.GetPropertyType",
          __LINE__, __WFILE__, NULL, L"", NULL);
      }
    }
  }
  
  return 0;
}

void c_RestDataReader_FdoFeatureReader::Close()
{
  m_FdoReader->Close();
  m_FdoConnection->Close();
}