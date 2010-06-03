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


#include "GeosUtil.h"
#include "c_GmlUtil.h"

#include "boost/scoped_ptr.hpp"

c_GmlUtil::c_GmlUtil(void)
{
}

c_GmlUtil::~c_GmlUtil(void)
{
}

void c_GmlUtil::ToGML( MgGeometry* Geometry,std::string& StrGml)
{
  std::string srsName;
  c_GmlUtil::SerializeGeometry(Geometry, StrGml, srsName);
  
}



void c_GmlUtil::SerializeGeometry(MgGeometry* geometry, std::string& writer, std::string& srsName)
{
  
  switch( geometry->GetGeometryType())
  {
  case MgGeometryType::Point:			
    c_GmlUtil::SerializePoint((MgPoint*)geometry, writer);			
    break;
  case MgGeometryType::LineString:
    c_GmlUtil::SerializeLineString((MgLineString*)geometry, writer);			
    break;
  case MgGeometryType::Polygon:
    c_GmlUtil::SerializePolygon((MgPolygon*)geometry, writer, srsName);
    break;
  case MgGeometryType::MultiPoint:
    c_GmlUtil::SerializeMultiPoint((MgMultiPoint*)geometry, writer, srsName);
    break;
  case MgGeometryType::MultiLineString:
    c_GmlUtil::SerializeMultiLineString((MgMultiLineString*)geometry, writer);
    break;
  case MgGeometryType::MultiPolygon:
    c_GmlUtil::SerializeMultiPolygon((MgMultiPolygon*)geometry, writer, srsName);
    break;
  case MgGeometryType::MultiGeometry:
    c_GmlUtil::SerializeMultiGeometry((MgMultiGeometry*)geometry, writer, srsName);
    break;

  case MgGeometryType::CurveString:
  case MgGeometryType::CurvePolygon:
  case MgGeometryType::MultiCurveString:
  case MgGeometryType::MultiCurvePolygon:
    // TODO: what to do?
    throw new MgRuntimeException(L"c_GmlUtil::SerializeGeometry",__LINE__, __WFILE__, NULL, L"Unsupported geometry types", NULL);
    break;
  }
}

void c_GmlUtil::SerializeCoordinate(MgCoordinate* position,std::string& writer)
{
  static const int SIZE = 256;
  char temp[SIZE];
  _snprintf(temp, SIZE - 1, "%lf,%lf", position->GetX(), position->GetY());
  writer += temp;

  int dimension = position->GetDimension();
  if (dimension & MgCoordinateDimension::XYZ)
  {
    _snprintf(temp, SIZE - 1, ",%lf", position->GetZ());
    writer += temp;
  }
  if (dimension & MgCoordinateDimension::M)
  {
    _snprintf(temp, SIZE - 1, ",%lf", position->GetM());
    writer += temp;
  }

  
}

void c_GmlUtil::SerializePoint(MgPoint* point, std::string& writer)
{	
  Ptr<MgCoordinate> pos = point->GetCoordinate();	
  writer += "<gml:Point>";
  writer += "<gml:coordinates>";	
  c_GmlUtil::SerializeCoordinate(pos,writer);
  writer += "</gml:coordinates>";
  writer += "</gml:Point>";
}

void c_GmlUtil::SerializeCoordinates(MgCoordinateIterator* coords, std::string& writer)
{
  if( !coords ) return;
  
  writer += "<gml:coordinates>";

  // The coordinates are separated by a blank(" ").
  if (coords->MoveNext())
  {
    Ptr<MgCoordinate> coord = coords->GetCurrent();

    SerializeCoordinate(coord,writer);
  }

  while (coords->MoveNext())
  {
    writer += " ";
    Ptr<MgCoordinate> coord = coords->GetCurrent();

    SerializeCoordinate(coord,writer);	
  }
  
  writer += "</gml:coordinates>";
  
}

void c_GmlUtil::SerializeLineString(MgLineString* lineString, std::string& writer)
{
  Ptr<MgCoordinateIterator> coords = lineString->GetCoordinates();
  
  writer += "<gml:LineString>";
  
  SerializeCoordinates(coords,writer);
  
  writer += "</gml:LineString>";
}

void c_GmlUtil::SerializeLinearRing(MgLinearRing* linearRing, std::string& writer)
{
  Ptr<MgCoordinateIterator> coords = linearRing->GetCoordinates();
  
  writer += "<gml:LinearRing>";
  SerializeCoordinates(coords,writer);
  writer += "</gml:LinearRing>";
}

void c_GmlUtil::SerializePolygon(MgPolygon* polygon, std::string& writer, std::string& srsName)
{
  if( !polygon ) return;
  
  if( srsName.length() > 0 )
  {
    writer += "<gml:Polygon srsName='";
    writer += srsName;
    writer += "'>";
  }
  else
    writer += "<gml:Polygon>";
  

  // serialize the exterior ring
  Ptr<MgLinearRing> outer = polygon->GetExteriorRing();
  writer += "<gml:outerBoundaryIs>";
  c_GmlUtil::SerializeLinearRing(outer, writer);
  writer += "</gml:outerBoundaryIs>";

  int cntRing = polygon->GetInteriorRingCount();	
  for (int i=0; i<cntRing; i++)
  {
    Ptr<MgLinearRing> inner = polygon->GetInteriorRing(i);
    writer += "<gml:innerBoundaryIs>";
    c_GmlUtil::SerializeLinearRing(inner, writer);
    writer += "</gml:innerBoundaryIs>";
  }
  
  writer += "</gml:Polygon>";
}

void c_GmlUtil::SerializeMultiPoint(MgMultiPoint* multiPoint, std::string& writer,  std::string& srsName)
{
  if( srsName.length() > 0 )
  {
    writer += "<gml:MultiPoint srsName='";
    writer += srsName;
    writer += "'>";
  }
  else
    writer += "<gml:Polygon>";

  writer += "<gml:pointMember>";
  
  int cntPoint = multiPoint->GetCount();
  for (int i=0; i<cntPoint; i++)
  {
    Ptr<MgPoint> point = multiPoint->GetPoint(i);
    c_GmlUtil::SerializePoint(point, writer);
  }
  writer += "</gml:pointMember>";
  
  writer += "</gml:MultiPoint>";
}

void c_GmlUtil::SerializeMultiLineString(MgMultiLineString* mlString, std::string& writer)
{
  writer += "<gml:MultiLineString>";
  writer += "<gml:lineStringMember>";
  
  int cntLineString = mlString->GetCount();
  for (int i=0; i<cntLineString; i++)
  {
    Ptr<MgLineString> lString = mlString->GetLineString(i);
    c_GmlUtil::SerializeLineString(lString, writer);
  }
  writer += "</gml:lineStringMember>";
  writer += "</gml:MultiLineString>";
}

void c_GmlUtil::SerializeMultiPolygon(MgMultiPolygon* mPolygon, std::string& writer,  std::string& srsName)
{
  writer += "<gml:MultiPolygon>";
  writer += "<gml:polygonMember>";
  
  int cntPolygon = mPolygon->GetCount();
  for (int i=0; i<cntPolygon; i++)
  {
    Ptr<MgPolygon> polygon = mPolygon->GetPolygon(i);
    c_GmlUtil::SerializePolygon(polygon, writer, srsName);
  }
  writer += "</gml:polygonMember>";
  writer += "</gml:MultiPolygon>";
}

void c_GmlUtil::SerializeMultiGeometry(MgMultiGeometry* mGeometry, std::string& writer,  std::string& srsName)
{
  writer += "<gml:MultiGeometry>";
  writer += "<gml:geometryMember>";
  
  int cntGeometry = mGeometry->GetCount();
  for (int i=0; i<cntGeometry; i++)
  {
    Ptr<MgGeometry> geometry = mGeometry->GetGeometry(i);
    c_GmlUtil::SerializeGeometry(geometry, writer, srsName);
  }
  writer += "</gml:geometryMember>";
  writer += "</gml:MultiGeometry>";
}


