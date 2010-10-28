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


#ifndef _c_CfgService_h
#define _c_CfgService_h

#include <vector>




class REST_CONFIG_API c_CfgService 
{
public:
  enum e_ServiceType  {
    e_EsriGS_Catalog
  };

protected:
  c_CfgService(const wchar_t* UriTag,e_ServiceType Type);
  
public:  
  ~c_CfgService(void);
  
public:
  e_ServiceType m_Type;
  std::wstring m_UriTag; // name to be used in Uri like parcel e.g. "..\rest\data\parcel"
  std::wstring m_RestUriPart; // if defined this uri will be used as base uri when generating links for html,xml. 
                              // This is usefull in case of redirections.
                              // default is /rest/data/
                              
  
  
public:
  const std::string& GetUriTag() const;  
  
protected:
  mutable std::string m_UriTagUtf8; // utf 8 copy of m_UriTag string
  

public:
  
  
public:  
  
};

typedef std::vector<c_CfgService*> t_CfgServiceVector;

class c_CfgServiceVector 
{
public:
  //c_CfgDataLayerVector(void);
  ~c_CfgServiceVector(void);
  void Clear();
protected:
  t_CfgServiceVector m_CfgServiceVector;

public:
  const c_CfgService* FindUriTag(const std::wstring& UriTag) const;   
  c_CfgService* FindUriTag(const std::wstring& UriTag) ;   
  bool  Add(c_CfgService* DataLayer);

  int GetCount();
  
  const c_CfgService* GetService(int Index) const;   
  
};




#endif
