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

#ifndef _c_GmlUtil_h
#define _c_GmlUtil_h
class MgGeometry;
class REST_REQUEST_HANDLER_API c_GmlUtil
{
public:
  c_GmlUtil(void);
  ~c_GmlUtil(void);
  
public:
  //static void ToKml(MgGeometry* Geometry,string& Kml);  
  //static void ToKml( MgGeometry* Geometry,std::string& Kml,std::string& KmlCentroid,std::string& KmlMultiPolygonAndCentroid );
  static void ToGML( MgGeometry* Geometry,std::string& StrGml);
  
protected:
  /// <summary>Serializes the geometry object to OGC format.</summary>
  /// <param name="geometry">An instance of geometry.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the geometry.</param>
  /// <param name="srsName">SRS name.</param>
  /// <returns>Returns nothing</returns>
  static void SerializeGeometry(MgGeometry* geometry, std::string& writer, std::string& srsName);

  /// <summary>Serialzies the point object to OGC format.</summary>
  /// <param name="point">An instance of the point type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the point.</param>
  /// <returns>Returns nothing</returns>
  static void SerializePoint(MgPoint* point, std::string& writer);

  /// <summary>Serializes the line string object to OGC format.</summary>
  /// <param name="lineString">An instance of the line string type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the line string.</param>
  /// <returns>Returns nothing</returns>
  static void SerializeLineString(MgLineString* lineString, std::string& writer);

  /// <summary>Serializes the linear ring object to OGC format.</summary>
  /// <param name="linearRing">An instance of the linear ring type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the linear ring.</param>
  /// <returns>Returns nothing</returns>
  static void SerializeLinearRing(MgLinearRing* linearRing, std::string& writer);

  /// <summary>Serializes the polygon object to OGC format.</summary>
  /// <param name="linearRing">An instance of the polygon type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the polygon.</param>
  /// <returns>Returns nothing</returns>
  static void SerializePolygon(MgPolygon* polygon, std::string& writer, std::string& srsName);

  /// <summary>Serializes the multipoint object to OGC format.</summary>
  /// <param name="linearRing">An instance of the multipoint type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the multipoint.</param>
  /// <returns>Returns nothing</returns>
  static void SerializeMultiPoint(MgMultiPoint* multiPoint, std::string& writer, std::string& srsName);

  /// <summary>Serializes the multi line string object to OGC format.</summary>
  /// <param name="linearRing">An instance of the multi line string type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the multi line string.</param>
  /// <returns>Returns nothing</returns>
  static void SerializeMultiLineString(MgMultiLineString* mlString, std::string& writer);

  /// <summary>Serializes the multi polygon object to OGC format.</summary>
  /// <param name="linearRing">An instance of the multi polygon type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the multi polygon.</param>
  /// <returns>Returns nothing</returns>
  static void SerializeMultiPolygon(MgMultiPolygon*, std::string& writer, std::string& srsName);

  /// <summary>Serializes the multi geometry object to OGC format.</summary>
  /// <param name="linearRing">An instance of the multi geometry type.</param>
  /// <param name="writer">An instance of FdoXmlWriter to serialize the multi geometry.</param>
  /// <returns>Returns nothing</returns>
  static void SerializeMultiGeometry(MgMultiGeometry*, std::string& writer, std::string& srsName);

private:
  /// <summary>Gets the coordinates of the FdoIDirectPosition object.</summary>
  /// <param name="position">An instance of FdoIDirectPosition type.</param>
  /// <returns>Returns the corrdinates as string.</returns>
  static void SerializeCoordinate( MgCoordinate* position,std::string& writer);
  static void SerializeCoordinates(MgCoordinateIterator* coords, std::string& writer);

};

#endif
