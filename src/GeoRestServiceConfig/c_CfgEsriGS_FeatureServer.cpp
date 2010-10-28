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
#include "c_CfgEsriGS_FeatureServer.h"
#include "Poco\UnicodeConverter.h"
#include "Poco\String.h"
#include "value.h"
#include "writer.h"
#include "reader.h"


c_CfgEsriGS_FeatureServer::c_CfgEsriGS_FeatureServer( const std::wstring& Name )
 : c_CfgEsriGS_Server(Name,c_CfgEsriGS_Server::e_FeatureServer)
{
  
}

c_CfgEsriGS_FeatureServer::c_CfgEsriGS_FeatureServer()
: c_CfgEsriGS_Server(L"",c_CfgEsriGS_Server::e_FeatureServer)
{

}
c_CfgEsriGS_FeatureServer::~c_CfgEsriGS_FeatureServer()
{
  {  
    t_c_CfgEsriGS_FServer_LayerVector::iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_FServer_Layer* datalayer = *iter;
      delete datalayer;
    }
    m_Layers.clear();
  }
}

bool c_CfgEsriGS_FeatureServer::AddLayer( c_CfgEsriGS_FServer_Layer* Layer )
{
  // check for id of layer
  if( Layer->m_Id < 0 )
  {
    int maxid = -1;
    t_c_CfgEsriGS_FServer_LayerVector::iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_FServer_Layer* datalayer = *iter;
      if( datalayer->m_Id > maxid ) maxid = datalayer->m_Id;
    } 
    
    Layer->m_Id = maxid + 1; 
  }
  else
  {
  // check if Id is ok
    t_c_CfgEsriGS_FServer_LayerVector::iterator iter;
    for ( iter = m_Layers.begin( ) ; iter != m_Layers.end( ) ; iter++ )
    {
      c_CfgEsriGS_FServer_Layer* datalayer = *iter;
      if( datalayer->m_Id == Layer->m_Id )
      {
        return false;
      }
    }
  }
  m_Layers.push_back(Layer);
  
  return true;
}

void c_CfgEsriGS_FeatureServer::WriteToJson( std::ostream& Out ,bool Pretty ) const
{
  Json::Value root_val(Json::objectValue);

  { 
    root_val["layers"] = Json::Value(Json::arrayValue); 
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

  }

  
  if( Pretty )    
  {
    Json::StyledStreamWriter sswrite;

    sswrite.write(Out,root_val);
  }
  else
  {
    Json::FastWriter fwrite;

    std::string outstr = fwrite.write(root_val);
    Out << outstr;
  }

}

void c_CfgEsriGS_FeatureServer::ReadFromJson( const std::string& InJson )
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
        std::wstring wname;
        Poco::UnicodeConverter::toUTF16(name,wname);
        c_CfgEsriGS_FServer_Layer* esrigs_layer = new c_CfgEsriGS_FServer_Layer(wname);
        esrigs_layer->m_Id = id;
        if( !AddLayer(esrigs_layer) )
        {
          delete esrigs_layer;
        }
      }    
    }
  }

  
}//end of c_CfgEsriGS_FeatureServer::ReadFromJson



c_CfgEsriGS_FServer_Layer::c_CfgEsriGS_FServer_Layer( const std::wstring& Name )
{
  m_Name = Name;
  m_Id = -1;  
}