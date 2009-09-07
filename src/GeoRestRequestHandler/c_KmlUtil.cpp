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
#include "c_KmlUtil.h"

#include "boost/scoped_ptr.hpp"
#include "kml/convenience/convenience.h"
#include "kml/dom.h"
#include "kml/engine.h"
#include "kml/base/file.h"

c_KmlUtil::c_KmlUtil(void)
{
}

c_KmlUtil::~c_KmlUtil(void)
{
}

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

//
// ToKml.. retunr KML representation of geometry
// If geometry is polygon it will return also multi geometry containing centroid and polygon itself 
//
void c_KmlUtil::ToKml( MgGeometry* Geometry,std::string& Kml,std::string& KmlCentroid,double& CentroidX,double& CentroidY,std::string& KmlMultiPolygonAndCentroid )
{
  //FdoPtr<FdoFgfGeometryFactory> gf = FdoFgfGeometryFactory::GetInstance();
  //FdoIGeometry* igeom = gf->CreateGeometryFromFgf(FdoGeom);
  
  if( Geometry )
  {  
    switch( Geometry->GetGeometryType() )
    {
      case MgGeometryType::Point:
      {
        MgPoint * ipoint = (MgPoint*)Geometry;
        Ptr<MgCoordinate> coord = ipoint->GetCoordinate();
        int orddim = coord->GetDimension();
        
        double x,y,z;
        
        x= coord->GetX();
        y= coord->GetY();
        if( orddim & MgCoordinateDimension::XYZ )    z = coord->GetZ();
        else z=0;
        
        kmldom::KmlFactory* factory = kmldom::KmlFactory::GetFactory();  // Create <coordinates>.  
        kmldom::CoordinatesPtr coordinates = factory->CreateCoordinates();  // Create <coordinates>-122.0816695,37.42052549<coordinates>  
        coordinates->add_latlngalt(y,x,z);  // Create <Point> and give it <coordinates>.  
        kmldom::PointPtr point = factory->CreatePoint();  
        point->set_coordinates(coordinates);  // point takes ownership
        
        Kml = kmldom::SerializePretty(point);
        
      }
      break; 
      case MgGeometryType::LineString:
      {
        MgLineString * iline = (MgLineString*)Geometry;
        
        
        //int orddim = iline->GetDimensionality();
        Ptr<MgCoordinateIterator> coords = iline->GetCoordinates();
        
        
        kmldom::KmlFactory* factory = kmldom::KmlFactory::GetFactory();  // Create <coordinates>.  
        kmldom::CoordinatesPtr coordinates = factory->CreateCoordinates();  // Create <coordinates>-122.0816695,37.42052549<coordinates>  
        
        double x,y,z;
        while(coords->MoveNext())
        {
          Ptr<MgCoordinate> coord = coords->GetCurrent();
          x= coord->GetX();
          y= coord->GetY();
          
          if( coord->GetDimension() & MgCoordinateDimension::XYZ )    z= coord->GetZ();
          else z=0;
          
          
          coordinates->add_latlngalt(y,x,z);  // Create <Point> and give it <coordinates>.  
        }
        
        
        kmldom::LineStringPtr kmlline = factory->CreateLineString();
        kmlline->set_coordinates(coordinates);  // point takes ownership
        kmlline->set_tessellate(1);

        Kml = kmldom::SerializePretty(kmlline);
        
      }
      break;
      case MgGeometryType::Polygon:
      {
        MgPolygon * ipolygon = (MgPolygon*)Geometry;
        if( !ipolygon ) break;
        
        Ptr<MgLinearRing> ring = ipolygon->GetExteriorRing();
        if( !ring ) break;
        Ptr<MgCoordinateIterator> coords = ring->GetCoordinates();
        if( !coords ) break;
        
        
        kmldom::KmlFactory* factory = kmldom::KmlFactory::GetFactory();  // Create <coordinates>.  
        kmldom::CoordinatesPtr coordinates = factory->CreateCoordinates();  // Create <coordinates>-122.0816695,37.42052549<coordinates>  

        double x,y,z;
        while(coords->MoveNext())
        {
          Ptr<MgCoordinate> coord = coords->GetCurrent();
          x= coord->GetX();
          y= coord->GetY();
          if( coord->GetDimension() & MgCoordinateDimension::XYZ )    z= coord->GetZ();
          else z=0;
          coordinates->add_latlngalt(y,x,z);  // Create <Point> and give it <coordinates>.  
        }


        kmldom::LinearRingPtr kmlring = factory->CreateLinearRing();
        kmlring->set_coordinates(coordinates);  // point takes ownership

        kmldom::OuterBoundaryIsPtr kmloutboundary = factory->CreateOuterBoundaryIs();
        kmloutboundary->set_linearring(kmlring);
        
        kmldom::PolygonPtr kmlpolygon = factory->CreatePolygon();
        kmlpolygon->set_outerboundaryis(kmloutboundary);
        kmlpolygon->set_tessellate(1);

        Kml = kmldom::SerializePretty(kmlpolygon);
        
        Ptr<MgPoint> centroid = ipolygon->GetPointInRegion();
        
        if( centroid )
        {
          Ptr<MgCoordinate> coord = centroid->GetCoordinate();
          CentroidX = coord->GetX();
          CentroidY = coord->GetY();
          kmldom::PointPtr kml_centroid = factory->CreatePoint();  
          SetCoordinates(centroid,kml_centroid);
          
          kmldom::MultiGeometryPtr kmlmulti = factory->CreateMultiGeometry();
          kmlmulti->add_geometry(kml_centroid);
          kmlmulti->add_geometry(kmlpolygon);
          
          KmlCentroid = kmldom::SerializePretty(kml_centroid);
          KmlMultiPolygonAndCentroid = kmldom::SerializePretty(kmlmulti);
        }
        
        
      }
      break;
    }
  }
  
  
}