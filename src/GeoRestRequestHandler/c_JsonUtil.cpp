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
#include "c_JsonUtil.h"
#include "Poco/UTF8Encoding.h"
#include "c_ExceptionGeoREST.h"
#include "../POCO/poco-1.3.5/Foundation/include/Poco/NumberFormatter.h"



std::string& c_JsonUtil::Utf8ToJsonString(const char* InUtf8,std::string& Json )
{
  
  
  Poco::UTF8Encoding utf8Encoding;
  const unsigned char* it  = (const unsigned char*)InUtf8;
  int len = strlen(InUtf8);
  //const unsigned char* end = (const unsigned char*) Utf8 + len;

  while (len>0)
  {
    int seq_len = utf8Encoding.sequenceLength(( const unsigned char*)it, len);
    if( seq_len <= 0 )
    {
      throw c_ExceptionGeoREST("Invalid sequence length! Unable to convert UTF8 string to Json string.");
    }
    
    if( seq_len == 1)
    {
      switch(*it)
      {
        case '"' :
        {
          Json.append("\\\"");
          break;
        }          
        case '\\' :
        {
          Json.append("\\\\");
          break;
        }
        case '\b' :
        {
          Json.append("\\b");
          break;
        }
        case '\f' :
        {
          Json.append("\\f");
          break;
        }
        case '\n' :
        {
          Json.append("\\n");
          break;
        }
        case '\r' :
        {
          Json.append("\\r");
          break;
        }
        case '\t' :
        {
          Json.append("\\t");
          break;
        }
        default :
        {
          Json.push_back(*it);
        }
      }

      
      it++;
    }
    else
    {
      for(int ind=0;ind<seq_len;ind++)
      {
        Json.append("\\x");
        Poco::NumberFormatter::appendHex(Json,*it);
        it++;
      }
    }
    len -= seq_len;
  }
  
  return Json;
}//end of c_JsonUtil::Utf8ToJsonString

void c_JsonUtil::AppendGeoJson(MgCoordinate* Coordinate,std::string& str)
{  
    std::string x,y;
    
    str.append("[");
    Poco::NumberFormatter::append(str,Coordinate->GetX());
    str.append(",");
    Poco::NumberFormatter::append(str,Coordinate->GetY());
    str.append("]");        
}
void c_JsonUtil::AppendGeoJson(MgCoordinateIterator* CoordIterator,std::string& str)
{
  bool isfirst = true;
  str.append("[");
  while( CoordIterator->MoveNext() )
  {
    Ptr<MgCoordinate> coord = CoordIterator->GetCurrent();    
    
    if( !isfirst ) str.append(",");
    AppendGeoJson(coord,str);
    
    isfirst = false;
  }
  str.append("]");
}

void c_JsonUtil::AppendGeoJson(MgGeometry* Geometry,std::string& str)
{
  if (Geometry != NULL)
  {
    INT32 type = Geometry->GetGeometryType();
    switch (type)
    {
      case MgGeometryType::Point:
      {

        MgPoint* point = (MgPoint*)Geometry;
        Ptr<MgCoordinate> coord = point->GetCoordinate();
        
        str.append("{ \"type\":\"Point\", \"coordinates\":");
        AppendGeoJson(coord,str);
        str.append("}");

      }
      break;
      case MgGeometryType::LineString:
      {
        MgLineString* linestring = (MgLineString*)Geometry;
        Ptr<MgCoordinateIterator> cords = linestring->GetCoordinates();
        
        
        str.append("{ \"type\":\"LineString\", \"coordinates\":");
        AppendGeoJson(cords,str);
        str.append("}");
        break;
      }
      case MgGeometryType::Polygon:
      {
        MgPolygon* poly = (MgPolygon*)Geometry;
        Ptr<MgLinearRing> extring = poly->GetExteriorRing();
        if( extring )
        {
          Ptr<MgCoordinateIterator> cords = extring->GetCoordinates();
          
          
          str.append("{ \"type\":\"Polygon\", \"coordinates\": [");
          AppendGeoJson(cords,str);
          str.append("] }");
        }

        break;
      }
      case MgGeometryType::MultiLineString:
      {
        MgMultiLineString* multiline = (MgMultiLineString*)Geometry;
        
        str.append("{\"type\": \"MultiLineString\",\"coordinates\": [");

        int count = multiline->GetCount();
        for(int ind=0;ind<count;ind++)
        {
          Ptr<MgLineString> line = multiline->GetLineString(ind);
          Ptr<MgCoordinateIterator> cords = line->GetCoordinates();
          

          if( ind > 0 ) str.append(",");
          AppendGeoJson(cords,str);          
        }

        str.append("]}");


        break;
      }
      case MgGeometryType::MultiPolygon:
      {
        MgMultiPolygon* multipoly = (MgMultiPolygon*)Geometry;

        break;
      }
    case MgGeometryType::MultiPoint:
      {
        MgMultiPoint* multipoint = (MgMultiPoint*)Geometry;

        break;
      }
    case MgGeometryType::MultiGeometry:
      {
        MgMultiGeometry* multigeom  = (MgMultiGeometry*)Geometry;

        break;
      }
    case MgGeometryType::CurveString:
      {
        //CreateCurveStringBuffer(bufferParams, (MgCurveString*)geometry, bufferPolygons);
        break;
      }
    case MgGeometryType::CurvePolygon:
      {
        //CreateCurvePolygonBuffer(bufferParams, (MgCurvePolygon*)geometry, bufferPolygons);
        break;
      }
    case MgGeometryType::MultiCurveString:
      {
        //CreateMultiCurveStringBuffer(bufferParams, (MgMultiCurveString*)geometry, bufferPolygons);
        break;
      }
    case MgGeometryType::MultiCurvePolygon:
      {
        //CreateMultiCurvePolygonBuffer(bufferParams, (MgMultiCurvePolygon*)geometry, bufferPolygons);
        break;
      }
    default:
      {
        STRING buffer;
        MgUtil::Int32ToString(type, buffer);

        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"c_FeatureReaderToGeoJson::ToGeoJson",
          __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryType", NULL);

      }
    }
  }
}//end of c_FeatureReaderToGeoJson::ToGeoJson