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

#include "StdAfx.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"

#include "c_RestDataTransform.h"


c_RestDataTransform::c_RestDataTransform(void)
{
}

c_RestDataTransform::~c_RestDataTransform(void)
{
}


MgGeometry* c_RestDataTransform::GeoJsonToMgGeometry(const Json::Value& GJsonGeometry)
{
  MgGeometryFactory factory;
  const Json::Value& geomtype = GJsonGeometry["type"];
  const Json::Value& geomcoordinates = GJsonGeometry["coordinates"];
  
  MgGeometry* newgeom = NULL;
  if( geomtype.asString() == "Point" )
  {
    Ptr<MgCoordinateXY> coord = new MgCoordinateXY(geomcoordinates[0].asDouble(), geomcoordinates[1].asDouble());
    newgeom = factory.CreatePoint(coord);
    
  } else if( geomtype.asString() == "LineString" )
  {
    const Json::Value& linestring = geomcoordinates;
    if( linestring.isNull() || !linestring.isArray() )
    {
      return NULL;
    }

    Ptr<MgCoordinateCollection> coordcol = GeoJsonToCoordinates(linestring);
    
    newgeom = factory.CreateLineString(coordcol);
    
  } else if( geomtype.asString() == "Polygon" )
  {
    const Json::Value& exterior = geomcoordinates[0];
    if( exterior.isNull() || !exterior.isArray() )
    {
      return NULL;
    }
    
    Ptr<MgCoordinateCollection> coordCol = GeoJsonToCoordinates(exterior);
    
    Ptr<MgLinearRing> extRing = factory.CreateLinearRing(coordCol);      
    newgeom = factory.CreatePolygon(extRing, NULL);
    
    
  } else if( geomtype.asString() == "MultiLineString" )
  {
    const Json::Value& linestring_array = geomcoordinates;
    if( linestring_array.isNull() || !linestring_array.isArray() )
    {
      return NULL;
    }
  
    Ptr<MgLineStringCollection> linecoll = new MgLineStringCollection();
    for(int lineind=0;lineind<linestring_array.size();lineind++)
    {
      const Json::Value& linestring = linestring_array[lineind];
      Ptr<MgCoordinateCollection> coordcol = GeoJsonToCoordinates(linestring);
      Ptr<MgLineString> line = factory.CreateLineString(coordcol);
      linecoll ->Add(line);
    }

    newgeom = factory.CreateMultiLineString(linecoll);
  } else if( geomtype.asString() == "MultiPolygon" )
  {

  } 
  
  return newgeom; 
}

MgCoordinateCollection* c_RestDataTransform::GeoJsonToCoordinates(const Json::Value& CoordinateArray)
{
  if( CoordinateArray.isNull() || !CoordinateArray.isArray() ) return NULL;
  Ptr<MgCoordinateCollection> coordcol = new MgCoordinateCollection();
  double x,y;
  int ind =0;
  int numcoords = CoordinateArray.size();
  while(ind<numcoords)
  {
    const Json::Value& point = CoordinateArray[ind++]; // point
    if( point.isNull() || !point.isArray() ) return NULL;
    
    x = point[0].asDouble();
    y = point[1].asDouble();

    Ptr<MgCoordinate> coord = new MgCoordinateXY(x, y);
    coordcol->Add(coord);     
  }
  
  return SAFE_ADDREF(coordcol.p);
}

MgPropertyCollection* c_RestDataTransform::XmlToFeature(MgClassDefinition* ClassDef,const std::string& XmlStr)
{
      STRING str_name;
      STRING str_val;
  
  
  MgXmlUtil XmlUtil(XmlStr);
  DOMElement* root = XmlUtil.GetRootNode();
 
  //DOMElement* felemnet = xmlutil.GetElementNode(root,"Feature");
  DOMElement* XmlFeature = root; // update body starts with <Feature>...</Feature>
  
  Ptr<MgPropertyDefinitionCollection> coll_propdef =  ClassDef->GetProperties();
  
  MgPropertyCollection* propcoll = new MgPropertyCollection();
  
  XERCES_CPP_NAMESPACE::DOMElement* el_prop  = XmlUtil.GetElementNode(XmlFeature,"Property",false);
  while(el_prop)
  {
    XERCES_CPP_NAMESPACE::DOMElement* el_name  = XmlUtil.GetElementNode(el_prop,"Name",false);
    XERCES_CPP_NAMESPACE::DOMElement* el_val  = XmlUtil.GetElementNode(el_prop,"Value",false);
    
    if( !el_name || !el_val ) return propcoll;
    
    
    XmlUtil.GetTextFromElement(el_name,str_name);
    XmlUtil.GetTextFromElement(el_val,str_val);
    
    
    Ptr<MgPropertyDefinition> propdef = coll_propdef->GetItem(str_name);
    if( propdef.p )
    {
      switch(propdef->GetPropertyType())
      {
        case  MgFeaturePropertyType::DataProperty:
        {
          MgDataPropertyDefinition* dataprop  = dynamic_cast<MgDataPropertyDefinition*>(propdef.p);
          
          switch(dataprop->GetDataType())
          {
            case MgPropertyType::String:
            {
              Ptr<MgStringProperty> prop = new MgStringProperty(str_name,str_val);
              propcoll->Add(prop);
            }
            break;
            case MgPropertyType::Int32:
            {
              long val = _wtol(str_val.c_str());
              Ptr<MgInt32Property> prop = new MgInt32Property(str_name,val);
              propcoll->Add(prop);
            }
            break;
            case MgPropertyType::Int16:
            {
              int val = _wtoi(str_val.c_str());
              Ptr<MgInt16Property> prop = new MgInt16Property(str_name,val);
              propcoll->Add(prop);
            }
            break;
            case MgPropertyType::Double:
              {
                double val = _wtof(str_val.c_str());
                Ptr<MgDoubleProperty> prop = new MgDoubleProperty(str_name,val);
                propcoll->Add(prop);
              }
              break;
            case MgPropertyType::Single:
              {
                double val = _wtof(str_val.c_str());
                Ptr<MgSingleProperty> prop = new MgSingleProperty(str_name,val);
                propcoll->Add(prop);
              }
              break;
            case MgPropertyType::DateTime:
              {

                MgDateTime val(str_val);

                Ptr<MgDateTimeProperty> prop = new MgDateTimeProperty(str_name,&val);
                propcoll->Add(prop);
              }
              break;
            case MgPropertyType::Byte:
              {
                int val = _wtoi(str_val.c_str());
                Ptr<MgByteProperty> prop = new MgByteProperty(str_name,val);
                propcoll->Add(prop);
              }
              break;
            case MgPropertyType::Boolean:
              {
                int val = _wtoi(str_val.c_str());
                Ptr<MgBooleanProperty> prop = new MgBooleanProperty(str_name,val);
                propcoll->Add(prop);
              }
              break;
          }
        }
        break;
        case  MgFeaturePropertyType::GeometricProperty:
        {
          
          MgGeometricPropertyDefinition* geomprop  = dynamic_cast<MgGeometricPropertyDefinition*>(propdef.p);
          
          MG_TRY()
          MgWktReaderWriter wktReader;
          Ptr<MgGeometry> geomval;
          geomval = wktReader.Read(str_val);
          
          if( geomval.p != NULL )
          {
            MgAgfReaderWriter agfrw;
            Ptr<MgByteReader> bytes = agfrw.Write(geomval);
            Ptr<MgGeometryProperty> geom = new MgGeometryProperty(str_name,bytes);
            propcoll->Add(geom);
          }
          MG_CATCH_AND_RELEASE()
        }
        break;
      }
    }
    
    DOMNode* nextnode = XmlUtil.GetNextSibling(el_prop);
    if( nextnode && nextnode->getNodeType() == DOMNode::ELEMENT_NODE )
    {
      el_prop = reinterpret_cast<DOMElement*>(nextnode);      
    }
    else
    {
      el_prop = NULL;
    }
  }
  
  return propcoll;   
}//end of XmlToFeature

