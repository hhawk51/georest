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
#include "MapGuideCommon.h"
#include "GeosUtil.h"
#include "c_GeoRssUtil.h"

#include "boost/scoped_ptr.hpp"

c_GeoRssUtil::c_GeoRssUtil(void)
{
}

c_GeoRssUtil::~c_GeoRssUtil(void)
{
}

/*
void SetCoordinates( MgPoint* Geometry,kmldom::PointPtr& KmlPoint )
{
  Ptr<MgCoordinate> coord = Geometry->GetCoordinate();
  int orddim = coord->GetDimension();
  
  double x,y,z;
        
  x= coord->GetX();
  y= coord->GetY();
  if( orddim & MgCoordinateDimension::XYZ )    coord->GetY();
  else z=0;
  
  kmldom::KmlFactory* factory = kmldom::KmlFactory::GetFactory();  // Create <coordinates>.  
  kmldom::CoordinatesPtr coordinates = factory->CreateCoordinates();  // Create <coordinates>-122.0816695,37.42052549<coordinates>  
  coordinates->add_latlngalt(y,x,z);  // Create <Point> and give it <coordinates>.  
  
  KmlPoint->set_coordinates(coordinates);  // point takes ownership
}
*/
//
// ToGeoRssSimple.. returns simple GeoRss of geomtry
//
//
void c_GeoRssUtil::ToGeoRssSimple( MgGeometry* Geometry,std::string& SimpleGeoRss)
{
  //FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
  //FdoIGeometry* igeom = gf->CreateGeometryFromFgf(FdoGeom);
  
  if( !Geometry ) return;
  
  
  switch( Geometry->GetGeometryType() )
  {
    case MgGeometryType::Point:   // <georss:point>45.256 -71.92</georss:point>
    {
      MgPoint * ipoint = (MgPoint*)Geometry;
      Ptr<MgCoordinate> coord = ipoint->GetCoordinate();
      int orddim = coord->GetDimension();
      
      double x,y,z;
      
      x= coord->GetX();
      y= coord->GetY();
      if( orddim & MgCoordinateDimension::XYZ )    z = coord->GetZ();
      else z=0;
      
      
      char buff[4*64+6];
  
      SimpleGeoRss = "<georss:point>";
      ::sprintf(buff, "%.7g",x);
      SimpleGeoRss += buff;
      SimpleGeoRss += " ";
      ::sprintf(buff, "%.7g", y);
      SimpleGeoRss += buff;
      SimpleGeoRss += "</georss:point>";
      
    }
    break; 
    case MgGeometryType::LineString:  // <georss:line>45.256 -110.45 46.46 -109.48 43.84 -109.86</georss:line>
    {
      MgLineString * iline = (MgLineString*)Geometry;
      
      
      //int orddim = iline->GetDimensionality();
      Ptr<MgCoordinateIterator> coords = iline->GetCoordinates();
      
      
      
      char buff[4*64+6];
      SimpleGeoRss = "<georss:line>";
            
      double x,y,z;
      while(coords->MoveNext())
      {
        Ptr<MgCoordinate> coord = coords->GetCurrent();
        x= coord->GetX();
        y= coord->GetY();
        
        if( coord->GetDimension() & MgCoordinateDimension::XYZ )    z= coord->GetZ();
        else z=0;
        
        ::sprintf(buff, "%.7g",x);
        SimpleGeoRss += buff;
        SimpleGeoRss += " ";
        ::sprintf(buff, "%.7g", y);
        SimpleGeoRss += buff;                
      }      
      SimpleGeoRss += "</georss:line>";
      
      
    }
    break;
    case MgGeometryType::Polygon:            // <georss:polygon>     45.256 -110.45 46.46 -109.48 43.84 -109.86 45.256 -110.45   </georss:polygon>
    {
      MgPolygon * ipolygon = (MgPolygon*)Geometry;
      if( !ipolygon ) break;
      
      Ptr<MgLinearRing> ring = ipolygon->GetExteriorRing();
      if( !ring ) break;
      Ptr<MgCoordinateIterator> coords = ring->GetCoordinates();
      if( !coords ) break;
      
      char buff[4*64+6];
      SimpleGeoRss = "<georss:polygon>";
      
      double x,y,z;
      while(coords->MoveNext())
      {
        Ptr<MgCoordinate> coord = coords->GetCurrent();
        x= coord->GetX();
        y= coord->GetY();
        if( coord->GetDimension() & MgCoordinateDimension::XYZ )    z= coord->GetZ();
        else z=0;
        
        ::sprintf(buff, "%.7g",x);
        SimpleGeoRss += buff;
        SimpleGeoRss += " ";
        ::sprintf(buff, "%.7g", y);
        SimpleGeoRss += buff;    
        
      }

      SimpleGeoRss += "</georss:polygon>";
      
    }
    break;
  }  
  
}
