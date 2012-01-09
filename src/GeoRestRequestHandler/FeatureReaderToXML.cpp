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
#include "RestConfigInclude.h"
#include "c_RestRequest.h"
#include "FeatureReaderToXML.h"
#include "RestConfigInclude.h"
#include "c_RestMapNameToResourceId.h"

//#include "..\HttpHandler\Rest_MapNameToResourceId.h"

c_FeatureReaderToXML::c_FeatureReaderToXML(void)
{
}

c_FeatureReaderToXML::~c_FeatureReaderToXML(void)
{
  
}

MgPropertyCollection* c_FeatureReaderToXML::GetPropertyCollection(c_RestDataReader* FeatureReader)
{
  MgPropertyCollection* prop_coll = new MgPropertyCollection();
  
  int count = FeatureReader->GetPropertyCount();
  for(int ind=0;ind<count;ind++)
  {
    STRING propname = FeatureReader->GetPropertyName(ind);
    int ptype = FeatureReader->GetPropertyType(propname);
    switch( ptype )
    {
      case MgPropertyType::Geometry:
      {
        if( FeatureReader->IsNull(propname ) )
        {
          Ptr<MgGeometryProperty> prop = new MgGeometryProperty(propname,NULL);
          prop->SetNull(true);
          prop_coll->Add(prop);
        }
        else
        {
          /*
          int geomlength;
          BYTE_ARRAY_OUT geombytes = FeatureReader->GetGeometry(propname,geomlength);
          Ptr<MgByteReader> bytes = new MgByteReader( geombytes,geomlength,L"" );
          */
          Ptr<MgByteReader> bytes = FeatureReader->GetGeometry(propname);
          Ptr<MgGeometryProperty> prop = new MgGeometryProperty(propname,bytes);
          prop_coll->Add(prop);  
        }
        
      }
      break;
      case MgPropertyType::Int32:
      {
        if( FeatureReader->IsNull(propname ) )
        {
          Ptr<MgInt32Property> prop = new MgInt32Property(propname,0);
          prop->SetNull(true);
          prop_coll->Add(prop);
        }
        else
        {
          Ptr<MgInt32Property> prop = new MgInt32Property(propname,FeatureReader->GetInt32(propname));
          prop_coll->Add(prop);
        }
      }
      break;
      case MgPropertyType::Int16:
      {
        if( FeatureReader->IsNull(propname ) )
        {
          Ptr<MgInt16Property> prop = new MgInt16Property(propname,0);
          prop->SetNull(true);
          prop_coll->Add(prop);
        }
        else
        {
          Ptr<MgInt16Property> prop = new MgInt16Property(propname,FeatureReader->GetInt16(propname));
          prop_coll->Add(prop);
        }
      }
      break;
      case MgPropertyType::Double:
      {
        if( FeatureReader->IsNull(propname ) )
        {
          Ptr<MgDoubleProperty> prop = new MgDoubleProperty(propname,0.0);
          prop->SetNull(true);
          prop_coll->Add(prop);
        }
        else
        {
          Ptr<MgDoubleProperty> prop = new MgDoubleProperty(propname,FeatureReader->GetDouble(propname));
          prop_coll->Add(prop);
        }
      }
      break;
      case MgPropertyType::Single:
      {
        if( FeatureReader->IsNull(propname ) )
        {
          Ptr<MgSingleProperty> prop = new MgSingleProperty(propname,0.0);
          prop->SetNull(true);
          prop_coll->Add(prop);
        }
        else
        {
          Ptr<MgSingleProperty> prop = new MgSingleProperty(propname,FeatureReader->GetSingle(propname));
          prop_coll->Add(prop);
        }
      }
      break;
      case MgPropertyType::DateTime:
      {
        if( FeatureReader->IsNull(propname ) )
        {
          Ptr<MgDateTimeProperty> prop = new MgDateTimeProperty(propname,NULL);
          prop->SetNull(true);
          prop_coll->Add(prop);
        }
        else
        {
          Ptr<MgDateTimeProperty> prop = new MgDateTimeProperty(propname,FeatureReader->GetDateTime(propname));
          prop_coll->Add(prop);
        }
      }
      break;
      case MgPropertyType::String:
      {
        if( FeatureReader->IsNull(propname ) )
        {
          Ptr<MgStringProperty> prop = new MgStringProperty(propname,L"");
          prop->SetNull(true);
          prop_coll->Add(prop);
        }
        else
        {
          Ptr<MgStringProperty> prop = new MgStringProperty(propname,FeatureReader->GetString(propname));
          prop_coll->Add(prop);
        }
      }
      break;
    }
  }
  
  
            
  return  prop_coll;
}

void c_FeatureReaderToXML::GetPropertyAsString(c_RestDataReader* FeatureReader,CREFSTRING PropName,REFSTRING StringValue)
{
  
  if( FeatureReader->IsNull(PropName) )
  {
    StringValue = L"";
    return;
  }
  
  int ptype = FeatureReader->GetPropertyType(PropName);
  switch( ptype )
  {
    case MgPropertyType::Geometry:
    {
      /*
      int geomlength;
      BYTE_ARRAY_OUT geombytes = FeatureReader->GetGeometry(PropName,geomlength);
      Ptr<MgByteReader> bytes = new MgByteReader( geombytes,geomlength,L"" );
      */
      Ptr<MgByteReader> bytes = FeatureReader->GetGeometry(PropName);
          
      MgAgfReaderWriter agfReader;                
      Ptr<MgGeometry> fgfgeom = agfReader.Read(bytes);
      
      MgWktReaderWriter wktReader;        
      StringValue = wktReader.Write(fgfgeom);      
    }
    break;
    case MgPropertyType::Int64:
      {
        INT64 val = FeatureReader->GetInt64(PropName);
        MgUtil::Int64ToString(val,StringValue);      
      }
      break;
    case MgPropertyType::Int32:
    {
        long val = FeatureReader->GetInt32(PropName);
        MgUtil::Int32ToString(val,StringValue);      
    }
    break;
    case MgPropertyType::Int16:
    {
        long val = FeatureReader->GetInt16(PropName);
        MgUtil::Int32ToString(val,StringValue);
    }
    break;
    case MgPropertyType::Single:
    {
        float val = FeatureReader->GetSingle(PropName);
        MgUtil::SingleToString(val,StringValue);
    }
    break;
    case MgPropertyType::Double:
    {
        double val = FeatureReader->GetDouble(PropName);
        MgUtil::DoubleToString(val,StringValue);
    }
    break;
    case MgPropertyType::DateTime:
    {
        Ptr<MgDateTime> val = FeatureReader->GetDateTime(PropName);
        StringValue = MgUtil::ReplaceEscapeCharInXml(val->ToString());
    }
    break;
    case MgPropertyType::String:
    {
        StringValue = MgUtil::ReplaceEscapeCharInXml(FeatureReader->GetString(PropName));      
    }
    break;
  }
     

}
void c_FeatureReaderToXML::GetPropertyAsString(c_RestDataReader* FeatureReader,CREFSTRING PropName,std::string& StringValue)
{
  
  if( FeatureReader->IsNull(PropName) )
  {
    StringValue = "";
    return;
  }
  
  int ptype = FeatureReader->GetPropertyType(PropName);
  switch( ptype )
  {
    case MgPropertyType::Geometry:
    {
      /*
      int geomlength;
      BYTE_ARRAY_OUT geombytes = FeatureReader->GetGeometry(PropName,geomlength);
      Ptr<MgByteReader> bytes = new MgByteReader( geombytes,geomlength,L"" );
      */    
      Ptr<MgByteReader> bytes = FeatureReader->GetGeometry(PropName);
      MgAgfReaderWriter agfReader;                
      Ptr<MgGeometry> fgfgeom = agfReader.Read(bytes);
      
      MgWktReaderWriter wktReader;        
      STRING wktstring = wktReader.Write(fgfgeom);
      StringValue = MgUtil::WideCharToMultiByte(wktstring);                  
    }
    break;
    case MgPropertyType::Int64:
    {
      INT64 val = FeatureReader->GetInt64(PropName);
      MgUtil::Int64ToString(val,StringValue);      
    }
    break;
    case MgPropertyType::Int32:
    {
        long val = FeatureReader->GetInt32(PropName);
        MgUtil::Int32ToString(val,StringValue);      
    }
    break;
    case MgPropertyType::Int16:
    {
        long val = FeatureReader->GetInt16(PropName);
        MgUtil::Int32ToString(val,StringValue);
    }
    break;
    case MgPropertyType::Byte:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = "";
      }
      else
      {
        BYTE val = FeatureReader->GetByte(PropName);
        MgUtil::Int32ToString((INT32)val,StringValue);
      }

    }
    break;
    case MgPropertyType::Boolean:
    {
      if( FeatureReader->IsNull(PropName) )
      {
        StringValue = "";
      }
      else
      {
        bool val = FeatureReader->GetBoolean(PropName);
        if( val )
          StringValue = "true";
        else
          StringValue = "false";

      }

    }
    break;
    case MgPropertyType::Single:
    {
        float val = FeatureReader->GetSingle(PropName);
        MgUtil::SingleToString(val,StringValue);
    }
    break;
    case MgPropertyType::Double:
    {
        double val = FeatureReader->GetDouble(PropName);
        MgUtil::DoubleToString(val,StringValue);
    }
    break;
    case MgPropertyType::DateTime:
    {
        Ptr<MgDateTime> val = FeatureReader->GetDateTime(PropName);
        STRING  ws = MgUtil::ReplaceEscapeCharInXml(val->ToString());
        MgUtil::WideCharToMultiByte(ws,StringValue);
        
    }
    break;
    case MgPropertyType::String:
    {
        STRING  ws = MgUtil::ReplaceEscapeCharInXml(FeatureReader->GetString(PropName));     
        MgUtil::WideCharToMultiByte(ws,StringValue); 
        
    }
    break;
  }
     

}

void c_FeatureReaderToXML::ToFeatureXML(c_RestDataReader* FeatureReader,std::string& XmlStr)
{
      std::string strval;
  XmlStr = "<Feature>";
  
  
  int count = FeatureReader->GetPropertyCount();
  for(int ind=0;ind<count;ind++)
  {
    XmlStr += "<Property>";
    STRING propname = FeatureReader->GetPropertyName(ind);
    std::string mb_propname;
    MgUtil::WideCharToMultiByte(propname,mb_propname);
    XmlStr += "<Name>";    
    XmlStr += mb_propname;
    XmlStr += "</Name>";
    if( FeatureReader->IsNull(propname) )
    {
      XmlStr += "<Value>";  
      XmlStr += "</Value>";  
    }
    else
    {
      GetPropertyAsString(FeatureReader,propname,strval);
      XmlStr += "<Value>"; 
       
      XmlStr += strval;
      XmlStr += "</Value>";  
    }
    
    XmlStr += "</Property>";
  }
  
  
  XmlStr += "</Feature>";
              
  
}


void c_FeatureReaderToXML::ToXml(c_RestDataReader* FeatureReader, std::string& XmlStr,int StartIndex,int MaxCount)
{
  
    //if(MaxCount > 500 ) MaxCount=500;
    
    Ptr<MgClassDefinition> classDef = FeatureReader->GetClassDefinition();

    if (classDef != NULL)
    {
        // TODO: define a schema for this XML
        // TODO: rename FeatureSet element to avoid conflict with FeatureSet-1.0.0.xsd?
        XmlStr += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        XmlStr += "\n";
        XmlStr += "<FeatureSet>";
        //classDef->ToXml(XmlStr);
        XmlStr += "\n";
        XmlStr += "<Features>";
        
        bool isnext=true;
        if( StartIndex > 0 )
        {
          
          while ( (StartIndex>0) && (isnext=FeatureReader->ReadNext())==true  )
          {
            StartIndex--;
          }
          
        }
        if( isnext )
        {
          std::string featstring;
          int count=0;
          while ( FeatureReader->ReadNext() )
          {
            if( MaxCount >= 0 )
            {
              if( count >= MaxCount ) break; // go out
            }
            
            XmlStr += "\n";
            MG_TRY()
            featstring="";
            ToFeatureXML(FeatureReader,featstring);
            XmlStr += featstring;
            MG_CATCH_AND_RELEASE() // just ignore in case of invalid geometries
            
            /*
            Ptr<MgPropertyCollection> propCol = GetPropertyCollection(FeatureReader);
            INT32 cnt = propCol->GetCount();
            if (propCol != NULL && cnt > 0)
            {
              XmlStr += "\n";
              MG_TRY()
              featstring="";
              propCol->ToFeature(featstring);
              XmlStr += featstring;
              MG_CATCH_AND_RELEASE() // just ignore in case of invalid geometries
            }
            */
            count++;
          }
          
        }
        XmlStr += "\n";
        XmlStr += "</Features>";
        XmlStr += "\n";
        XmlStr += "</FeatureSet>";
    }



}


/*
  <?xml version="1.0" encoding="UTF-8"?>
  <urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
     <url>
        <loc>http://www.example.com/</loc>
        <lastmod>2005-01-01</lastmod>
        <changefreq>monthly</changefreq>
        <priority>0.8</priority>
     </url>
  </urlset> 

*/
void c_FeatureReaderToXML::ToSitemapXml(c_RestDataReader* FeatureReader, c_RestRequest* Request,const string& BaseUri ,string& XmlStr,int StartIndex,int MaxCount)
{
  
    Ptr<MgClassDefinition> classDef = FeatureReader->GetClassDefinition();

    if (classDef != NULL)
    {
      string resturiparam;
      STRING wresturiparam;
      STRING classname = classDef->GetName();
      
      //const c_CfgDataResource* cfgdata = c_RestMapNameToResourceId::GetCfgDataForClass(classname);  
      const c_CfgDataResource* cfgresource = Request->m_CfgDataResource;
      if( !cfgresource ) 
      {
        XmlStr += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        XmlStr += "<Error>Unknown class name:";
        
        string mb_classname;
        MgUtil::WideCharToMultiByte(classname,mb_classname);
        XmlStr += mb_classname;
        XmlStr += "</Error>";
        return ;
      }
      
      
      wresturiparam = cfgresource->m_UriTag;
      
      MgUtil::WideCharToMultiByte(wresturiparam,resturiparam);
      
      
      string rest_uri_part;
      if( cfgresource->m_RestUriPart.length() > 0 )
      {
        MgUtil::WideCharToMultiByte(cfgresource->m_RestUriPart,rest_uri_part);
        
      }
      else
      {
        rest_uri_part = "/rest/data/";
      }  
        
        // TODO: define a schema for this XML
        // TODO: rename FeatureSet element to avoid conflict with FeatureSet-1.0.0.xsd?
        XmlStr += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        XmlStr += "\n";
       
        bool isnext=true;
        if( StartIndex > 0 )
        {
          
          while ( (StartIndex>0) && (isnext=FeatureReader->ReadNext())==true  )
          {
            StartIndex--;
          }
          
        }
        if( isnext )
        {
          // get identity property
          
          Ptr<MgPropertyDefinitionCollection> idents = classDef->GetIdentityProperties();
          if( idents->GetCount() != 1) 
            return ;
            
          int ind=0;  
          Ptr<MgPropertyDefinition> identprop = idents->GetItem(ind);
          STRING identname = identprop->GetName();
          
          XmlStr += "<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">";
          
          STRING strval,loc;
          string mb_strval;
          int count=0;
          while ( FeatureReader->ReadNext() )
          {
            if( MaxCount >= 0 )
            {
              if( count >= MaxCount ) break; // go out
            }
            
            GetPropertyAsString(FeatureReader,identname,strval);
            XmlStr += "\n<url>";
            XmlStr += "\n<loc>";
            XmlStr += BaseUri ;   // http:://localhost/mgrest
            XmlStr += rest_uri_part;
            XmlStr += resturiparam;
            XmlStr += "/";
            MgUtil::WideCharToMultiByte(strval,mb_strval);
            XmlStr += mb_strval;
            XmlStr += ".html";
            XmlStr += "</loc>";
            XmlStr += "<changefreq>daily</changefreq>";
            XmlStr += "<priority>0.5</priority>";
            XmlStr += "\n</url>";
            
            count++;
          }  
          
          XmlStr += "\n</urlset>";
          
        }
      
    }



}//end of ToSitemapXml



