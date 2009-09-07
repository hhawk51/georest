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

#ifndef _c_CfgDataSource_h
#define _c_CfgDataSource_h

#include "MapGuideCommon.h"

class REST_CONFIG_API c_CfgDataSource 
{
public:
    enum e_SourceType
    {
      e_Undefined,
      e_Http,
      e_MgLayerDefinition,
      e_MgFeatureSource,
      e_FDO,
    };


public:
  c_CfgDataSource(e_SourceType SourceType);
public:
  ~c_CfgDataSource(void);
  
public:  
  c_CfgDataSource::e_SourceType GetSourceType() const { return m_SourceType; }
  //void SetSourceType(c_CfgDataResourceSource::e_SourceType val) { m_SourceType = val; }
  
protected:
  e_SourceType m_SourceType;
  

};

// Defines FDO as Data source
// it is connection to data using FDO
class  REST_CONFIG_API c_CfgDataSource_FDO : public c_CfgDataSource
{   
public:
  ~c_CfgDataSource_FDO();
  c_CfgDataSource_FDO();

public:    

  const std::wstring& GetProvider() const { return m_Provider; }
  void SetProvider(const std::wstring& val) { m_Provider = val; }  

  const std::wstring& GetConnString() const { return m_ConnString; }
  void SetConnString(const std::wstring& val) { m_ConnString = val; }

  const std::wstring& GetFeatureClass() const { return m_FeatureClass; }
  void SetFeatureClass(const std::wstring& val) { m_FeatureClass = val; }
  
protected:    


  std::wstring m_Provider;

  std::wstring m_ConnString; // connection string
  std::wstring m_FeatureClass; // connection string


};

// Describes source of data; MapGuide featureSource+Class Nae, MG Layer Definition, 
// HTTP:\\ , FDO:\\King.Oracle?UserName=...&Password=..
class  REST_CONFIG_API c_CfgDataSource_MapGuide : public c_CfgDataSource
{   
  protected:
    ~c_CfgDataSource_MapGuide();
    c_CfgDataSource_MapGuide(e_SourceType SourceType);
    
  public:    
    
    const std::wstring& GetServerIP() const { return m_ServerIP; }
    void SetServerIP(const std::wstring& val) { m_ServerIP = val; }  
    
    int GetServerPort() const { return m_ServerPort; }
    void SetServerPort(int val) { m_ServerPort = val; }
    
    const std::wstring& GetUsername() const { return m_Username; }
    void SetUsername(std::wstring val) { m_Username = val; }
    
    const std::wstring& GetPassword() const { return m_Password; }
    void SetPassword(const std::wstring& val) { m_Password = val; }
   
  protected:    
    

    std::wstring m_ServerIP;
    
    int m_ServerPort;
    

    std::wstring m_Username;
    
    std::wstring m_Password;
    
    
    
};

// Describes source of data; MapGuide featureSource+Class Nae, MG Layer Definition, 
// HTTP:\\ , FDO:\\King.Oracle?UserName=...&Password=..
class  REST_CONFIG_API c_CfgDataSource_MgLayerDefinition : public c_CfgDataSource_MapGuide
{      
  public:
    c_CfgDataSource_MgLayerDefinition(const std::wstring& MgLayerDefinition);
    ~c_CfgDataSource_MgLayerDefinition();
    
    
  public:     
    const std::wstring& GetMgLayerDefinition() const { return m_MgLayerDefinition; }
    void SetMgLayerDefinition(const std::wstring& val) { m_MgLayerDefinition = val; }
   
  public:    
    
    std::wstring m_MgLayerDefinition;
    

    
};

// Describes source of data; MapGuide feature Class
class  REST_CONFIG_API c_CfgDataSource_MgFeatureSource : public c_CfgDataSource_MapGuide
{      
  public:
    c_CfgDataSource_MgFeatureSource( );
    ~c_CfgDataSource_MgFeatureSource();
    
  public:    
    
    
    const std::wstring& GetMgFeatureSource() const { return m_MgFeatureSource; }
    void SetMgFeatureSource(const std::wstring& val) { m_MgFeatureSource = val; }
    
    const std::wstring& GetMgFeatureSourceClassName() const { return m_MgFeatureSourceClassName; }
    void SetMgFeatureSourceClassName(const std::wstring& val) { m_MgFeatureSourceClassName = val; }
    
  public:    
    std::wstring m_MgFeatureSource;
    std::wstring m_MgFeatureSourceClassName;
    
};

#endif