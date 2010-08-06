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
#include "MapGuideCommon.h"
#include "FeatureReaderToGeoJson.h"

c_FeatureReaderToGeoJson::c_FeatureReaderToGeoJson(void)
{
}

c_FeatureReaderToGeoJson::~c_FeatureReaderToGeoJson(void)
{
}

STRING ReplaceEscapeCharInJSON(CREFSTRING str)
{
  STRING newStr;
  size_t len = str.length();



  newStr.reserve(len);

  for(size_t i = 0; i < len; i++)
  {
    switch(str[i])
    {
      case '"' :
      {
        newStr += L"\\\"";
        break;
      }          
      case '\\' :
      {
        newStr += L"\\\\";
        break;
      }
      case '\b' :
      {
        newStr += L"\\b";
        break;
      }
      case '\f' :
      {
        newStr += L"\\f";
        break;
      }
      case '\n' :
      {
        newStr += L"\\n";
        break;
      }
      case '\r' :
      {
        newStr += L"\\r";
        break;
      }
      case '\t' :
      {
        newStr += L"\\t";
        break;
      }
      default :
      {
        if( str[i] <= 0x1F )
        {
          wchar_t buff[18];

          swprintf(buff,L"\\u%x",(int)str[i]);
          newStr += buff;
        }
        else
          newStr += str[i];
      }
    }
  }
  return newStr;
}

/*
 {   "type": "FeatureCollection"
     ,"members": [ 
                    {"type": "Feature","id": "id0", "geometry": {"type": "LineString", "coordinates": [[102.0, 0.0], [103.0, 1.0], [104.0, 0.0], [105.0, 1.0] ] }, "properties": { "prop0": "value0","prop1": "value1"}}
                   ,{"type": "Feature","id": "id1","geometry": {"type": "Polygon","coordinates": [[[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.0, 0.0]]]},"properties": {"prop0": "value0","prop1": "value1"}}
                 ]
 };
*/
void c_FeatureReaderToGeoJson::ToGeoJson(c_RestDataReader* FeatureReader, string& GJsonStr,int StartIndex,int MaxCount)
{
  if(MaxCount > 500 ) MaxCount=500;
  
  Ptr<MgClassDefinition> classDef = FeatureReader->GetClassDefinition();

  int featurecount=0;
  bool ismorerecords=false;
  
  if (classDef != NULL)
  {
    STRING class_geomname = classDef->GetDefaultGeometryPropertyName();
    
    Ptr<MgPropertyDefinitionCollection> identprop_coll = classDef->GetIdentityProperties();  
    STRING identprop_name=L"";
    
    if( identprop_coll.p && (identprop_coll->GetCount() == 1) )
    {
      int ind=0;
      Ptr<MgPropertyDefinition> identprop = identprop_coll->GetItem(ind);
      identprop_name = identprop->GetName();
    }
    
    // TODO: define a schema for this XML
    // TODO: rename FeatureSet element to avoid conflict with FeatureSet-1.0.0.xsd?
    GJsonStr.append("{ \"type\": \"FeatureCollection\" ");
    GJsonStr.append(",\"features\" : [");
    
    //classDef->ToXml(str);
    //str += "<Features>";
    
    bool isfirstfeature=true;
    bool includeType=false;
    
    bool isnext=true;
    int skipstart = StartIndex;
    if( skipstart > 0 )
    {
      
      while ( (skipstart>0) && (isnext=FeatureReader->ReadNext())==true  )
      {
        skipstart--;
      }
      
    }
    if( isnext )
    {
        
      while ( FeatureReader->ReadNext() )
      {
        if( MaxCount >= 0 )
        {
          
          if( featurecount >= MaxCount ) 
          {
            ismorerecords=true;
            break; // go out
          }
        }
        
        
            
        if( !isfirstfeature ) GJsonStr += ",";
        GJsonStr.append("{");
        
        std::string identprop_value="";
        
        GJsonStr.append("\"type\":\"Feature\"");
        
        bool isfirst_prop = true;
        std::string props_str; // properties like string: "porp1" :  "string1" , "prop2" : 34
        std::string geom_val;
        
        int propcount = FeatureReader->GetPropertyCount();
        int propind=0;
        while( propind < propcount )
        {
          STRING propname = FeatureReader->GetPropertyName(propind);
          std::string prop_val;
          if( FeatureReader->IsNull(propname) ) 
          {
            prop_val = "null";
          }
          else
          {
            //FeatureReader->GetPropertyType()
            switch( FeatureReader->GetPropertyType(propname) )
            {
              
                case MgPropertyType::Geometry:
                {
                // check if this geomrty is default (main) geometry
                  if( class_geomname == propname )
                  {    
                    prop_val = "null";   
                    MG_TRY()                     
                    Ptr<MgByteReader> bytereader = FeatureReader->GetGeometry(propname);
                    if (bytereader != NULL)
                    {
                      MgAgfReaderWriter agfReader;
                      Ptr<MgGeometry> geom = agfReader.Read(bytereader);
                      ToGeoJson(geom,geom_val);

                    }
                    MG_CATCH_AND_RELEASE() // just release in case of invalid geomtries
                  }
                  else
                  {
                 // right now it seems like GeoJson is thinking os Feature as data with one geometry
                 // disagree but not sure how to handle features  with multiple geometries ( geometries as properties)
                 // I could simply put geometry into new object and assign it to property - it would work just fine with GeoJson
                 // but I believe GeoJson would need more systematic support of this kind of features
                  }
                }
                break;
                case MgPropertyType::String:
                {         
                  prop_val = "\"";
                  STRING escstr = ReplaceEscapeCharInJSON(FeatureReader->GetString(propname));
                  prop_val.append(MgUtil::WideCharToMultiByte(escstr));              
                  prop_val.append("\"");
                }
                break;
                case MgPropertyType::DateTime:
                {  
                           
                    Ptr<MgDateTime> dtPtr = FeatureReader->GetDateTime(propname);
                    if (dtPtr != NULL)
                    {
                      STRING dateStr = dtPtr->ToString();
                      prop_val = MgUtil::WideCharToMultiByte(dateStr);
                    }
                    else
                    {
                      prop_val = "null";
                    }
                 
                }
                break;
                
                case MgPropertyType::Double:
                  {
                    
                    std::string tmp = "";
                    MgUtil::DoubleToString(FeatureReader->GetDouble(propname), tmp);
                    
                    prop_val = tmp;
                  
                  }
                  break;
                case MgPropertyType::Single:
                  {
                    std::string tmp = "";
                    if( !FeatureReader->IsNull(propname) ) MgUtil::DoubleToString(FeatureReader->GetSingle(propname), tmp);

                    prop_val = tmp;
                    //ToGeoJson(geom,prop_val,includeType);           
                  }
                  break;
                case MgPropertyType::Int64:
                {
                  std::string tmp = "";
                  MgUtil::Int64ToString(FeatureReader->GetInt64(propname), tmp);
                  prop_val = tmp;
                  //ToGeoJson(geom,prop_val,includeType);           
                }
                break;
                case MgPropertyType::Int32:
                  {
                    std::string tmp = "";
                    MgUtil::Int32ToString(FeatureReader->GetInt32(propname), tmp);
                    prop_val = tmp;
                    //ToGeoJson(geom,prop_val,includeType);           
                  }
                  break;
                case MgPropertyType::Int16:
                  {
                    std::string tmp = "";
                    MgUtil::Int32ToString(FeatureReader->GetInt16(propname), tmp);
                    prop_val = tmp;
                    //ToGeoJson(geom,prop_val,includeType);           
                  }
                  break;
                
              
            }
          }
          
          if( prop_val.length() > 0 )
          {
            if( !isfirst_prop )
            {
              props_str += ",";
              
            }
            props_str += "\"" + MgUtil::WideCharToMultiByte(propname) + "\":" + prop_val;
            
            if( propname == identprop_name )
            {
              identprop_value = prop_val;
            }
            
            isfirst_prop = false;
          }
          
          propind++;
        }      
        
        if( geom_val.length() > 0 )
        {
          GJsonStr.append(",\"geometry\": ");
          GJsonStr.append(geom_val);

        }
        
        if( (identprop_name.length()>0) && (identprop_value.length() > 0) )
        {
          GJsonStr.append(",\"id\": { \"");
          GJsonStr.append(MgUtil::WideCharToMultiByte(identprop_name));
          GJsonStr.append("\":");
          GJsonStr.append(identprop_value);
          GJsonStr.append("}");
        }
        
        
        if( props_str.length() > 0 )
        {
          GJsonStr.append(",\"properties\": {");
          GJsonStr.append(props_str);
          GJsonStr.append("}");

        }
        
        GJsonStr.append("}");
        
        isfirstfeature = false;
        
        featurecount++;
        
      };//end of while
      GJsonStr.append("]");
      GJsonStr.append("}");
    }
  }
}




void c_FeatureReaderToGeoJson::ToGeoJson(MgGeometryProperty* Geom,string &str,bool includeType)
{
  //str += "<" + rootElmName + ">";
  str.append("{");
  
  str.append(MgUtil::WideCharToMultiByte(Geom->GetName()));

  //str += "<Name>";
  //str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
  if (includeType)
  {
    str.append("<Type>geometry</Type>");
  }
  //str += "<Value>";

  Ptr<MgByteReader> byteReader = Geom->GetValue();

  if (byteReader != NULL)
  {
    MgAgfReaderWriter agfReader;
    Ptr<MgGeometry> geom = agfReader.Read(byteReader);
    ToGeoJson(geom,str);
   
  }

  str.append("}");
  //str += "</Value>";
  //str += "</" + rootElmName + ">";
}

void c_FeatureReaderToGeoJson::ToGeoJson(MgInt64Property* Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";
  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
  if (includeType)
  {
    str += "<Type>int64</Type>";
  }
  str += "<Value>";
  std::string tmp = "";
  MgUtil::Int64ToString(this->GetValue(), tmp);
  str += tmp + "</Value>";
  str += "</" + rootElmName + ">";
 */
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  std::string tmp = "";
  MgUtil::Int64ToString(Prop->GetValue(), tmp);
  str += tmp;
 
}

void c_FeatureReaderToGeoJson::ToGeoJson(MgInt32Property* Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";

  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

  if (includeType)
  {
    str += "<Type>int32</Type>";
  }

  str += "<Value>";
  char buf[128]; buf[0] = 0;
#ifdef WIN32
  itoa(this->GetValue(), buf, 10);
#else
  snprintf(buf, 128, "%li", this->GetValue());
#endif
  str += std::string(buf) + "</Value>";

  str += "</" + rootElmName + ">";
*/
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  std::string tmp = "";
  MgUtil::Int32ToString(Prop->GetValue(), tmp);
  str += tmp;  
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
/// <returns>
/// The ByteReader
/// </returns>
void c_FeatureReaderToGeoJson::ToGeoJson(MgInt16Property *Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";

  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

  if (includeType)
  {
    str += "<Type>int16</Type>";
  }

  str += "<Value>";

  char buf[128]; buf[0] = 0;
#ifdef WIN32
  itoa(this->GetValue(), buf, 10);
#else
  snprintf(buf, 128, "%d", this->GetValue());
#endif

  str += std::string(buf) + "</Value>";

  str += "</" + rootElmName + ">";
*/  
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  std::string tmp = "";
  MgUtil::Int32ToString(Prop->GetValue(), tmp);
  str += tmp;  
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts to XML format
/// </summary>
void c_FeatureReaderToGeoJson::ToGeoJson(MgDoubleProperty*Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";

  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
  if (includeType)
  {
    str += "<Type>double</Type>";
  }

  string doubleStr = "";
  MgUtil::DoubleToString(m_value, doubleStr);

  str += "<Value>" + doubleStr + "</Value>";

  str += "</" + rootElmName + ">";
*/  
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  std::string tmp = "";
  MgUtil::DoubleToString(Prop->GetValue(), tmp);
  str += tmp;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
void c_FeatureReaderToGeoJson::ToGeoJson(MgByteProperty*Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";

  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
  if (includeType)
  {
    str += "<Type>byte</Type>";
  }
  str += "<Value>";
  str += MgUtil::Char2Hex(this->GetValue());
  str += "</Value>";

  str += "</" + rootElmName + ">";
  */
  
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  str += MgUtil::Char2Hex(Prop->GetValue());
  
  
}
/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
void c_FeatureReaderToGeoJson::ToGeoJson(MgBooleanProperty*Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";

  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

  if (includeType)
  {
    str += "<Type>boolean</Type>";
  }

  str += "<Value>";

  if( this->GetValue())
    str += "true" ;
  else
    str += "false" ;

  str += "</Value>";

  str += "</" + rootElmName + ">";
*/
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  str += Prop->GetValue() ? "true" : "false";
}
/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts to XML format
/// </summary>
void c_FeatureReaderToGeoJson::ToGeoJson(MgSingleProperty*Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";

  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

  if (includeType)
  {
    str += "<Type>single</Type>";
  }

  string singleStr = "";
  MgUtil::DoubleToString(m_value, singleStr);

  str += "<Value>" + singleStr + "</Value>";

  str += "</" + rootElmName + ">";
*/
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  string temp;
  MgUtil::DoubleToString(Prop->GetValue(), temp);
  str += temp;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Return XML for the property
/// </summary>
void c_FeatureReaderToGeoJson::ToGeoJson(MgStringProperty*Prop,string &str, bool onlyKey, bool includeType, string rootElmName)
{
/*
  if (!onlyKey)
  {
    str += "<" + rootElmName + ">";
    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
    if (includeType)
    {
      str += "<Type>string</Type>";
    }
    str += "<Value>" + MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(this->GetValue())) + "</Value>";
    str += "</" + rootElmName + ">";
  }
  else
  {
    str += "<Item>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Item>";
  }
*/
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  str += "\"";
  str += MgUtil::WideCharToMultiByte(Prop->GetValue());
  str += "\"";
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts to XML format
/// </summary>
void c_FeatureReaderToGeoJson::ToGeoJson(MgDateTimeProperty*Prop,string &str, bool includeType, string rootElmName)
{
/*
  str += "<" + rootElmName + ">";

  str += "<Name>";
  str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

  if (includeType)
  {
    str += "<Type>datetime</Type>";
  }

  str += "<Value>";
  char buf[128]; buf[0] = 0;
  Ptr<MgDateTime> dtPtr = this->GetValue();
  if (dtPtr != NULL)
  {
    STRING dateStr = dtPtr->ToString();
    str += MgUtil::WideCharToMultiByte(dateStr);
  }
  str += "</Value>";

  str += "</" + rootElmName + ">";
*/  
  str += MgUtil::WideCharToMultiByte(Prop->GetName());
  str += " : ";
  str += "\"";
  Ptr<MgDateTime> dtPtr = Prop->GetValue();
  if (dtPtr != NULL)
  {
    STRING dateStr = dtPtr->ToString();
    str += MgUtil::WideCharToMultiByte(dateStr);
  }  
  str += "\"";
}


void c_FeatureReaderToGeoJson::ToGeoJson(MgCoordinate* Coordinate,std::string& str)
{
  
    std::string x,y;
    
    MgUtil::DoubleToString(Coordinate->GetX(), x);
    MgUtil::DoubleToString(Coordinate->GetY(), y);
    str += "[" + x + "," + y + "]";
  
  /*
  switch( Coordinate->GetClassId() )
  {
    case Geometry_CoordinateXY:
    {
      std:string x,y;
      MgCoordinateXY* xy = (MgCoordinateXY*)Coordinate;
      MgUtil::DoubleToString(xy->GetX(), x);
      MgUtil::DoubleToString(xy->GetX(), y);
      str += "[" + x + "," + y + "]";
    }
    break;    
    case Geometry_CoordinateXYZ:
    {
      std:string x,y;
      MgCoordinateXYZ* xy = (MgCoordinateXY*)Coordinate;
      MgUtil::DoubleToString(xy->GetX(), x);
      MgUtil::DoubleToString(xy->GetX(), y);
      str += "[" + x + "," + y + "]";
    }
    break;
    case Geometry_CoordinateXYZM:
    break;
    case Geometry_CoordinateXYM:
    break;
  }
  */
}
void c_FeatureReaderToGeoJson::ToGeoJson(MgCoordinateIterator* CoordIterator,std::string& str)
{
  bool isfirst = true;
  str += "[";
  while( CoordIterator->MoveNext() )
  {
    Ptr<MgCoordinate> coord = CoordIterator->GetCurrent();    
    
    if( !isfirst ) str += ",";
    ToGeoJson(coord,str);
    
    isfirst = false;
  }
  str += "]";
}
void c_FeatureReaderToGeoJson::ToGeoJson(MgGeometry* Geometry,std::string& str)
{
  if (Geometry != NULL)
  {
    INT32 type = Geometry->GetGeometryType();
    switch (type)
    {
      case MgGeometryType::Point:
      {
        
        string coord_str;
        MgPoint* point = (MgPoint*)Geometry;
        Ptr<MgCoordinate> coord = point->GetCoordinate();
        ToGeoJson(coord,coord_str);
        str += "{ \"type\":\"Point\", \"coordinates\":" + coord_str + "}";
        
      }
      break;
      case MgGeometryType::LineString:
      {
        MgLineString* linestring = (MgLineString*)Geometry;
        Ptr<MgCoordinateIterator> cords = linestring->GetCoordinates();
        string coord_str;
        ToGeoJson(cords,coord_str);
        str += "{ \"type\":\"LineString\", \"coordinates\":" + coord_str + "}";
        break;
      }
      case MgGeometryType::Polygon:
      {
        MgPolygon* poly = (MgPolygon*)Geometry;
        Ptr<MgLinearRing> extring = poly->GetExteriorRing();
        if( extring )
        {
          Ptr<MgCoordinateIterator> cords = extring->GetCoordinates();
          string coord_str;
          ToGeoJson(cords,coord_str);
          str += "{ \"type\":\"Polygon\", \"coordinates\": [" + coord_str + "] }";
        }

        break;
      }
      case MgGeometryType::MultiLineString:
      {
        MgMultiLineString* multiline = (MgMultiLineString*)Geometry;
        str+="{\"type\": \"MultiLineString\",\"coordinates\": [";
        
        int count = multiline->GetCount();
        for(int ind=0;ind<count;ind++)
        {
          Ptr<MgLineString> line = multiline->GetLineString(ind);
          Ptr<MgCoordinateIterator> cords = line->GetCoordinates();
          string coord_str;
          ToGeoJson(cords,coord_str);
          
          if( ind > 0 ) str+=",";
          str +=coord_str ;
        }
              
        str+="]}";

        
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
}



