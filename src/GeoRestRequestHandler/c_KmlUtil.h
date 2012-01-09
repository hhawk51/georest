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

#ifndef _c_KmlUtil_h
#define _c_KmlUtil_h
class MgGeometry;
class c_KmlUtil
{
public:
  c_KmlUtil(void);
  ~c_KmlUtil(void);
  
public:
  //static void ToKml(MgGeometry* Geometry,string& Kml);  
  //static void ToKml( MgGeometry* Geometry,std::string& Kml,std::string& KmlCentroid,std::string& KmlMultiPolygonAndCentroid );
  static void ToKml( MgGeometry* Geometry,std::string& Kml,std::string& KmlCentroid,double& CentroidX,double& CentroidY,std::string& KmlMultiPolygonAndCentroid );
};

#endif
