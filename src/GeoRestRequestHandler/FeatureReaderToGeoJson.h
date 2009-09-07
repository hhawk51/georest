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

#ifndef _C_FEATUREREADERTOGEOJSON_H
#define _C_FEATUREREADERTOGEOJSON_H

// This class parsers REST URI requests in form/url-encoded format
class c_FeatureReaderToGeoJson
{
public:
    c_FeatureReaderToGeoJson(void);
    ~c_FeatureReaderToGeoJson(void);
    static void ToGeoJson(c_RestDataReader* FeatureReader, string& GJsonStr,int StartIndex,int MaxCount);
    
protected:    
    //static void ToGeoJson(MgGeometry* Geom,string &str,bool includeType);
    static void ToGeoJson(MgGeometryProperty* Geom,string &str,bool includeType);
    static void ToGeoJson(MgInt64Property* Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgInt32Property* Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgInt16Property *Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgDoubleProperty*Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgByteProperty*Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgBooleanProperty*Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgStringProperty*Prop,string &str, bool onlyKey, bool includeType, string rootElmName);
    static void ToGeoJson(MgSingleProperty*Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgDateTimeProperty*Prop,string &str, bool includeType, string rootElmName);
    static void ToGeoJson(MgCoordinate* Coordinate,std::string& str);
    static void ToGeoJson(MgGeometry* Geometry,std::string& str);
    static void ToGeoJson(MgCoordinateIterator* CoordIterator,std::string& str);
    
    

};

#endif
