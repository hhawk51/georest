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

#ifndef _c_RestDataTransform_h
#define _c_RestDataTransform_h

#include <json/json.h>

class  REST_FETCHURI_API c_RestDataTransform
{
public:
  c_RestDataTransform(void);
public:
  ~c_RestDataTransform(void);
  
public: 
  static MgGeometry* GeoJsonToMgGeometry(const Json::Value& GJsonGeometry);
  static MgCoordinateCollection* GeoJsonToCoordinates(const Json::Value& CoordinateArray);
  static MgPropertyCollection* XmlToFeature(MgClassDefinition* ClassDef,const std::string& XmlStr);

};

#endif