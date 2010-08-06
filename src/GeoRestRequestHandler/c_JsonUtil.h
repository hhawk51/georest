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

#ifndef _c_JsonUtil_h
#define _c_JsonUtil_h

class c_JsonUtil
{
public:
  
  static std::string&  Utf8ToJsonString(const char* Utf8,std::string& Json );
  static void AppendGeoJson(MgGeometry* Geometry,std::string& str);
  static void AppendGeoJson(MgCoordinate* Coordinate,std::string& str);
  static void AppendGeoJson(MgCoordinateIterator* CoordIterator,std::string& str);
};


#endif