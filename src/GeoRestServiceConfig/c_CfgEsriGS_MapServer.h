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


#ifndef _c_CfgEsriGS_MapServer_h
#define _c_CfgEsriGS_MapServer_h

#include <vector>


// class: c_CfgEsriGS_MS_Layer is layer defined in MapServer
class REST_CONFIG_API c_CfgEsriGS_MS_Layer 
{
public:
  c_CfgEsriGS_MS_Layer (int Id,const std::wstring& Name,bool DefVisibility);

public:    
  std::wstring m_Name;
  int m_Id;
  bool m_DefaultVisibility;
  
};
typedef std::vector<c_CfgEsriGS_MS_Layer *> t_c_CfgEsriGS_MS_LayerVector;


class REST_CONFIG_API c_CfgEsriGS_MapServer : public  c_CfgEsriGS_Server
{
public:
  c_CfgEsriGS_MapServer();
  c_CfgEsriGS_MapServer(const std::wstring& Name);
  ~c_CfgEsriGS_MapServer();
  
  virtual void WriteToJson(std::ostream& Out ,bool Pretty) const;
  virtual void ReadFromJson(const std::string& InJson);
  bool AddLayer( c_CfgEsriGS_MS_Layer* Layer );
  c_CfgEsriGS_MS_Layer* FindLayer( int Id );
  c_CfgEsriGS_MS_Layer* FindLayer( const std::wstring& Name );
public:
  
  
  std::wstring m_MapGuideMapDef; // MapGuide mapdefinition: e.g. Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition
  std::wstring m_MapGuideServerIP;
  int m_MapGuideServerPort;
  std::wstring m_MapGuideUsername;
  std::wstring m_MapGuidePassword;
  
  t_c_CfgEsriGS_MS_LayerVector m_Layers;

};



#endif
