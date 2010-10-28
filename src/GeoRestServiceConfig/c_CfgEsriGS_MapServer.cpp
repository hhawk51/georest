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

#include "StdAfx.h"
#include "c_CfgService.h"
#include "c_CfgEsriGS_MapServer.h"
#include "Poco\UnicodeConverter.h"
#include "Poco\String.h"
#include "value.h"
#include "writer.h"
#include "reader.h"


c_CfgEsriGS_MS_Layer::c_CfgEsriGS_MS_Layer( int Id,const std::wstring& Name,bool DefVisibility )
{
  m_Id = Id;
  m_Name = Name;
  m_DefaultVisibility = DefVisibility;
}

c_CfgEsriGS_MapServer::c_CfgEsriGS_MapServer( const std::wstring& Name )
 : c_CfgEsriGS_Server(Name,c_CfgEsriGS_Server::e_MapServer)
{
  m_MapGuideServerIP = L"127.0.0.1";
  m_MapGuideServerPort = 2812;

  m_MapGuideUsername = L"Anonymous";
  m_MapGuidePassword = L"";
}

c_CfgEsriGS_MapServer::c_CfgEsriGS_MapServer()
: c_CfgEsriGS_Server(L"",c_CfgEsriGS_Server::e_MapServer)
{
  m_MapGuideServerIP = L"127.0.0.1";
  m_MapGuideServerPort = 2812;

  m_MapGuideUsername = L"Anonymous";
  m_MapGuidePassword = L"";
}
c_CfgEsriGS_MapServer::~c_CfgEsriGS_MapServer()
{
  {  
    t_c_CfgEsriGS_MS_LayerVector::iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_MS_Layer* datalayer = *iter;
      delete datalayer;
    }
    m_Layers.clear();
  }
}

bool c_CfgEsriGS_MapServer::AddLayer( c_CfgEsriGS_MS_Layer* Layer )
{
  // check for id of layer
  if( Layer->m_Id < 0 )
  {
    int maxid = -1;
    t_c_CfgEsriGS_MS_LayerVector::iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_MS_Layer* datalayer = *iter;
      if( datalayer->m_Id > maxid ) maxid = datalayer->m_Id;
    } 

    Layer->m_Id = maxid+1; 
  }
  else
  {
    // check if Id is ok
    t_c_CfgEsriGS_MS_LayerVector::iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_MS_Layer* datalayer = *iter;
      if( datalayer->m_Id == Layer->m_Id )
      {
        return false;
      }
    }
  }
  m_Layers.push_back(Layer);

  return true;
}//end of c_CfgEsriGS_MapServer::AddLayer

c_CfgEsriGS_MS_Layer* c_CfgEsriGS_MapServer::FindLayer( int Id )
{
  // check if Id is ok
  t_c_CfgEsriGS_MS_LayerVector::iterator iter;
  for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
  {
    c_CfgEsriGS_MS_Layer* datalayer = *iter;
    if( datalayer->m_Id == Id )
    {
      return datalayer;
    }
  }
  return NULL;
}
c_CfgEsriGS_MS_Layer* c_CfgEsriGS_MapServer::FindLayer( const std::wstring& Name )
{
  // check if Id is ok
  t_c_CfgEsriGS_MS_LayerVector::iterator iter;
  for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
  {
    c_CfgEsriGS_MS_Layer* datalayer = *iter;
    if( datalayer->m_Name == Name )
    {
      return datalayer;
    }
  }
  
  return NULL;
}

void c_CfgEsriGS_MapServer::WriteToJson( std::ostream& Out ,bool Pretty ) const
{
  Json::Value root_val(Json::objectValue);

  
  root_val["serviceDescription"] = "service Description";
  root_val["description"] = "description";
  root_val["copyrightText"] = "copyrightText";
  
  /*
  root_val["documentInfo"]["Title"] = "Title";
  root_val["documentInfo"]["Author"] = "Author";
  root_val["documentInfo"]["Comments"] = "";
  root_val["documentInfo"]["Subject"] = "";
  root_val["documentInfo"]["Category"] = "";
  root_val["documentInfo"]["Keywords"] = "";
  */
  
  std::string name;
  Poco::UnicodeConverter::toUTF8(m_Name,name);
  root_val["mapName"] = name;
  
  Json::Value j_spref(Json::objectValue);
  j_spref["wkid"]=4326;
  root_val["spatialReference"] = j_spref;// "spatialReference" : {<spatialReference>},;
  
  
  // "supportedImageFormatTypes": "PNG32,PNG24,PNG,JPG,DIB,TIFF,EMF,PS,PDF,GIF,SVG,SVGZ",
  //root_val["supportedImageFormatTypes"] = "PNG24,PNG,JPG,DIB,TIFF,EMF,PS,PDF,GIF,SVG,SVGZ,AI";
  root_val["supportedImageFormatTypes"] = "PNG24,PNG,PNG8,JPG,GIF,TIF";
  root_val["singleFusedMapCache"] = (bool)false; // "singleFusedMapCache" : <true | false>,
  
  
  root_val["initialExtent"]["xmin"] = -87.76;
  root_val["initialExtent"]["ymin"] = 43.69;
  root_val["initialExtent"]["xmax"] = -87.69;
  root_val["initialExtent"]["ymax"] = 43.79;
  
  root_val["initialExtent"]["spatialReference"]["wkid"] = 4326;
  
  
  root_val["fullExtent"]["xmin"] = -87.76;
  root_val["fullExtent"]["ymin"] = 43.69;
  root_val["fullExtent"]["xmax"] = -87.69;
  root_val["fullExtent"]["ymax"] = 43.79;

  root_val["fullExtent"]["spatialReference"]["wkid"] = 4326;
  
    
  root_val["units"] = "esriDecimalDegrees";
    
    
  //root_val["capabilities"] = "Map,Data"; // "capabilities" : "Map,Query,Data"
  { 
  
    root_val["layers"] = Json::Value(Json::arrayValue); 
    
    t_c_CfgEsriGS_MS_LayerVector::const_iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_MS_Layer* datalayer = *iter;
      
      Json::Value layer_0(Json::objectValue);
      layer_0["id"]=datalayer->m_Id;
      std::string name;
      Poco::UnicodeConverter::toUTF8(datalayer->m_Name,name);
      layer_0["name"]=name;
      layer_0["parentLayerId"]=-1;
      layer_0["defaultVisibility"]=datalayer->m_DefaultVisibility;
      layer_0["subLayerIds"] = Json::Value (Json::nullValue) ;

      root_val["layers"].append(layer_0);
      
      
      
    }
    
    
    
    /*
    t_c_CfgEsriGS_FServer_LayerVector::const_iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_FServer_Layer * datalayer = *iter;
      std::string name;
      Json::Value layer_val(Json::objectValue);
      Poco::UnicodeConverter::toUTF8(datalayer->m_Name,name);
      layer_val["name"] = name;
      layer_val["id"] = datalayer->m_Id;
      
      root_val["layers"].append(layer_val);
    }
    */
  }

  //std::string tempstr = "{\"serviceDescription\":\"This service contains population density polygons, country boundaries, and city locations for the world. The map is color coded based on the number of persons per square mile (per every 1.609 kilometers square). Population data sources included national population censuses, the United Nations demographic yearbooks, and others. In general, data currency ranged from 1981 to 1994. This is a sample service hosted by ESRI, powered by ArcGIS Server. ESRI has provided this example so that you may practice using ArcGIS APIs for JavaScript, Flex, and Silverlight. ESRI reserves the right to change or remove this service at any time and without notice.\",\"mapName\":\"Layers\",\"description\":\"This service contains population density polygons, country boundaries, and city locations for the world. The map is color coded based on the number of persons per square mile (per every 1.609 kilometers square). Population data sources included national population censuses, the United Nations demographic yearbooks, and others. In general, data currency ranged from 1981 to 1994.\",\"copyrightText\":\"(c) ESRI and its data partners\",\"layers\":[{\"id\":0,\"name\":\"CEISEN Population\",\"parentLayerId\":-1,\"defaultVisibility\":true,\"subLayerIds\":null}],\"spatialReference\":{\"wkid\":4326},\"singleFusedMapCache\":false,\"initialExtent\":{\"xmin\":-187.354832172645,\"ymin\":-176.58960429297,\"xmax\":184.404362321765,\"ymax\":200.193363100014,\"spatialReference\":{\"wkid\":4326}},\"fullExtent\":{\"xmin\":-180,\"ymin\":-90,\"xmax\":180,\"ymax\":90,\"spatialReference\":{\"wkid\":4326}},\"units\":\"esriDecimalDegrees\",\"supportedImageFormatTypes\":\"PNG24,PNG,JPG,DIB,TIFF,EMF,PS,PDF,GIF,SVG,SVGZ,AI\",\"documentInfo\":{\"Title\":\"GlobalPopulation\",\"Author\":\"serverxadmin\",\"Comments\":\"\",\"Subject\":\"\",\"Category\":\"\",\"Keywords\":\"\"}}";
  //Out << tempstr;
  //return;
  
  if( Pretty )    
  {
    Json::StyledStreamWriter sswrite;

    sswrite.write(Out,root_val);
  }
  else
  {
    Json::FastWriter fwrite;

    std::string outstr = fwrite.write(root_val);
    if( outstr.length()>0 && outstr.at(outstr.length()-1) == '\n' )
    {
      outstr.erase(outstr.length()-1,1);
    }
    
    Out << outstr;
  }

}

void c_CfgEsriGS_MapServer::ReadFromJson( const std::string& InJson )
{ 
  Json::Value root_val;
  Json::Reader reader;
  if( !reader.parse(InJson,root_val,false)  )
  {
    std::string errmsg = reader.getFormatedErrorMessages();
    throw c_ExceptionRestConfig(errmsg.c_str());
  }

  {
    const Json::Value& layer_array = root_val["layers"];
    if( !layer_array.isNull() )
    {
      if( layer_array.type() != Json::arrayValue )
      {
        throw c_ExceptionRestConfig("Error parsing Json! 'layers' should be of type array.");
      }
      
      int size = layer_array.size();
      for(int ind=0;ind<size;ind++)
      {
        const Json::Value& folder = layer_array[ind];
        if( folder.type() != Json::objectValue )
        {
          throw c_ExceptionRestConfig("Error parsing Json! Values of 'layers' array should be of type object.");
        }
        std::string name = folder["name"].asString();
        int id = folder["id"].asInt();
        bool defvisibility = folder["defaultVisibility"].asBool();
        std::wstring wname;
        Poco::UnicodeConverter::toUTF16(name,wname);
        c_CfgEsriGS_MS_Layer* esrigs_layer = new c_CfgEsriGS_MS_Layer(id,wname,defvisibility);
        esrigs_layer->m_Id = id;
        if( !AddLayer(esrigs_layer) )
        {
          delete esrigs_layer;
        }
      } 
         
    }
  }

  
}//end of c_CfgEsriGS_MapServer::ReadFromJson



