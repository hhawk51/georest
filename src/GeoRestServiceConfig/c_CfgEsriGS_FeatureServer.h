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


#ifndef _c_CfgEsriGS_FeatureServer_h
#define _c_CfgEsriGS_FeatureServer_h

#include <vector>




class REST_CONFIG_API c_CfgEsriGS_FServer_Layer 
{
  public:
    c_CfgEsriGS_FServer_Layer (const std::wstring& Name);
    
  public:    
    std::wstring m_Name;
    int m_Id;
    
    std::wstring m_DataUripart;
    std::string m_Json;
};
typedef std::vector<c_CfgEsriGS_FServer_Layer *> t_c_CfgEsriGS_FServer_LayerVector;

class REST_CONFIG_API c_CfgEsriGS_FeatureServer : public  c_CfgEsriGS_Server
{
public:
  c_CfgEsriGS_FeatureServer();
  c_CfgEsriGS_FeatureServer(const std::wstring& Name);
  ~c_CfgEsriGS_FeatureServer();
  bool AddLayer( c_CfgEsriGS_FServer_Layer* layer );
  virtual void WriteToJson(std::ostream& Out ,bool Pretty) const;
  virtual void ReadFromJson(const std::string& InJson);
public:
  
  
  t_c_CfgEsriGS_FServer_LayerVector m_Layers;
};



#endif
