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

void c_GmlUtil::ToGML( MgGeometry* Geometry,std::string& StrGml,int Format )
{
  std::string srsName;
  c_GmlUtil::SerializeGeometry(Geometry, StrGml, srsName,Format);
  
}



void c_GmlUtil::SerializeGeometry(MgGeometry* geometry, std::string& writer, std::string& srsName,int Format)
{
  
  switch( geometry->GetGeometryType())
  {
  case MgGeometryType::Point:			
    c_GmlUtil::SerializePoint((MgPoint*)geometry, writer,Format);
    break;
  case MgGeometryType::LineString:
    c_GmlUtil::SerializeLineString((MgLineString*)geometry, writer,Format);
    break;
  case MgGeometryType::Polygon:
    c_GmlUtil::SerializePolygon((MgPolygon*)geometry, writer, srsName,Format);
    break;
  case MgGeometryType::MultiPoint:
    c_GmlUtil::SerializeMultiPoint((MgMultiPoint*)geometry, writer, srsName,Format);
    break;
  case MgGeometryType::MultiLineString:
    c_GmlUtil::SerializeMultiLineString((MgMultiLineString*)geometry, writer,Format);
    break;
  case MgGeometryType::MultiPolygon:
    c_GmlUtil::SerializeMultiPolygon((MgMultiPolygon*)geometry, writer, srsName,Format);
    break;
  case MgGeometryType::MultiGeometry:
    c_GmlUtil::SerializeMultiGeometry((MgMultiGeometry*)geometry, writer, srsName,Format);
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

void c_GmlUtil::SerializeCoordinate(MgCoordinate* position,std::string& writer,int Format)
{
  static const int SIZE = 256;
  char temp[SIZE];


  //_snprintf(temp, SIZE - 1, "%lf,%lf", position->GetY(), position->GetX());
  _snprintf(temp, SIZE - 1, "%lf %lf", position->GetX(), position->GetY());


 // std::replace(temp[0], temp[SIZE-1], ',', '.');
    
  writer += temp;

  int dimension = position->GetDimension();
  if (dimension & MgCoordinateDimension::XYZ)
  {
    _snprintf(temp, SIZE - 1, ",%lf", position->GetZ());
   //std::replace(temp[0], temp[SIZE-1], ',', '.');
    writer += temp;
  }
  if (dimension & MgCoordinateDimension::M)
  {
    _snprintf(temp, SIZE - 1, ",%lf", position->GetM());
    writer += temp;
  }

  
}

void c_GmlUtil::SerializePoint(MgPoint* point, std::string& writer,int Format)
{	
  Ptr<MgCoordinate> pos = point->GetCoordinate();	
  writer += "<gml:Point>";
  
  int dim = point->GetDimension();
  
  
  std::stringstream sstr;
  sstr << "<gml:posList srsDimensions=\"";
  sstr << dim;
  sstr << "\">";
  //writer += "</gml:pos>";
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? sstr.str():"<gml:coordinates>";

  c_GmlUtil::SerializeCoordinate(pos,writer,Format);
  //writer += "</gml:pos>";
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "</gml:posList>":"</gml:coordinates>";
  writer += "</gml:Point>";
}

void c_GmlUtil::SerializeCoordinates(MgCoordinateIterator* coords, std::string& writer,int Format)
{
  if( !coords ) return;
  
  //writer += "<gml:posList>";
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "<gml:posList srsDimensions=\"2\">":"<gml:coordinates>";

  // The coordinates are separated by a blank(" ").
  if (coords->MoveNext())
  {
    Ptr<MgCoordinate> coord = coords->GetCurrent();
    
    SerializeCoordinate(coord,writer,Format);
  }

  while (coords->MoveNext())
  {
    writer += " ";
    Ptr<MgCoordinate> coord = coords->GetCurrent();

    SerializeCoordinate(coord,writer,Format);
  }
  
  //writer += "</gml:posList>";
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "</gml:posList>":"</gml:coordinates>";
  
}

void c_GmlUtil::SerializeLineString(MgLineString* lineString, std::string& writer,int Format)
{
  Ptr<MgCoordinateIterator> coords = lineString->GetCoordinates();
  
  writer += "<gml:LineString>";
  
  SerializeCoordinates(coords,writer,Format);
  
  writer += "</gml:LineString>";
}

void c_GmlUtil::SerializeLinearRing(MgLinearRing* linearRing, std::string& writer,int Format)
{
  Ptr<MgCoordinateIterator> coords = linearRing->GetCoordinates();
  
  writer += "<gml:LinearRing>";
  SerializeCoordinates(coords,writer,Format);
  writer += "</gml:LinearRing>";
}

void c_GmlUtil::SerializePolygon(MgPolygon* polygon, std::string& writer, std::string& srsName,int Format)
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
   
  //writer += "<gml:outerBoundaryIs>";
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "<gml:exterior>":"<gml:outerBoundaryIs>";
  c_GmlUtil::SerializeLinearRing(outer, writer,Format);
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "</gml:exterior>":"</gml:outerBoundaryIs>";
  //writer += "</gml:outerBoundaryIs>";

  int cntRing = polygon->GetInteriorRingCount();	
  for (int i=0; i<cntRing; i++)
  {
    Ptr<MgLinearRing> inner = polygon->GetInteriorRing(i);
    writer += "<gml:innerBoundaryIs>";
    c_GmlUtil::SerializeLinearRing(inner, writer,Format);
    writer += "</gml:innerBoundaryIs>";
  }
  
  writer += "</gml:Polygon>";
}

void c_GmlUtil::SerializeMultiPoint(MgMultiPoint* multiPoint, std::string& writer,  std::string& srsName,int Format)
{
  if( srsName.length() > 0 )
  {
    writer += "<gml:MultiPoint srsName='";
    writer += srsName;
    writer += "'>";
  }
  else
    writer += "<gml:Polygon>";

  //writer += "<gml:pointMember>";
  
  int cntPoint = multiPoint->GetCount();
  for (int i=0; i<cntPoint; i++)
  {
	  writer += "<gml:pointMember>";
    Ptr<MgPoint> point = multiPoint->GetPoint(i);
    c_GmlUtil::SerializePoint(point, writer,Format);
	  writer += "</gml:pointMember>";
  }
  //writer += "</gml:pointMember>";
  
  writer += "</gml:MultiPoint>";
}

void c_GmlUtil::SerializeMultiLineString(MgMultiLineString* mlString, std::string& writer,int Format)
{
  writer += "<gml:MultiLineString>";
  //writer += "<gml:lineStringMember>";
  
  int cntLineString = mlString->GetCount();
  for (int i=0; i<cntLineString; i++)
  {
	  writer += "<gml:lineStringMember>";
    Ptr<MgLineString> lString = mlString->GetLineString(i);
    c_GmlUtil::SerializeLineString(lString, writer,Format);
	  writer += "</gml:lineStringMember>";
  }
  //writer += "</gml:lineStringMember>";
  writer += "</gml:MultiLineString>";
}

void c_GmlUtil::SerializeMultiPolygon(MgMultiPolygon* mPolygon, std::string& writer,  std::string& srsName,int Format)
{
  //writer += "<gml:MultiPolygon>";
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "<gml:MultiSurface>":"<gml:MultiPolygon>";
  
  int cntPolygon = mPolygon->GetCount();
  for (int i=0; i<cntPolygon; i++)
  {
	  //writer += "<gml:polygonMember>";
	  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "<gml:surfaceMember>":"<gml:polygonMember>";
    Ptr<MgPolygon> polygon = mPolygon->GetPolygon(i);
    c_GmlUtil::SerializePolygon(polygon, writer, srsName,Format);
	  //writer += "</gml:polygonMember>";
	  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "</gml:surfaceMember>":"</gml:polygonMember>";
  }
//  writer += "</gml:polygonMember>";
  //writer += "</gml:MultiPolygon>"; 
  writer += (Format == c_GmlUtil::e_GmlFormat::GML311) ? "</gml:MultiSurface>":"</gml:MultiPolygon>";
}

void c_GmlUtil::SerializeMultiGeometry(MgMultiGeometry* mGeometry, std::string& writer,  std::string& srsName,int Format)
{
  writer += "<gml:MultiGeometry>";
  //writer += "<gml:geometryMember>";
  
  int cntGeometry = mGeometry->GetCount();
  for (int i=0; i<cntGeometry; i++)
  {
	  writer += "<gml:geometryMember>";
    Ptr<MgGeometry> geometry = mGeometry->GetGeometry(i);
    c_GmlUtil::SerializeGeometry(geometry, writer, srsName,Format);
	  writer += "</gml:geometryMember>";
  }
  //writer += "</gml:geometryMember>";
  writer += "</gml:MultiGeometry>";
}


