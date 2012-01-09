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


#ifndef _c_CfgDataResource_h
#define _c_CfgDataResource_h

#include <vector>

#include "c_CfgRepresentation.h"


class c_CfgDataSource;




class REST_CONFIG_API c_CfgDataResource 
{
public:
  c_CfgDataResource(const wchar_t* UriTag,const char* TemplateFolder);
  ~c_CfgDataResource(void);
  
public:
  std::string m_TemplateFolder; // root folder for templates; Most usually it will be folder where this recource config was found
                                // this path is used for generating paths for templates defined in cfg. Template file names can be relative or absolute.
  
  std::wstring m_UriTag; // name to be used in Uri like parcel e.g. "..\rest\data\parcel"
  std::wstring m_RestUriPart; // if defined this uri will be used as base uri when generating links for html,xml. 
                              // This is usefull in case of redirections.
                              // default is /rest/data/
                              
  c_CfgDataSource  * m_DataSource; 
  
   
  t_CfgRepresentationVector m_Representations;
  
  
public:
  const std::string& GetUriTag() const;  
  
protected:
  mutable std::string m_UriTagUtf8; // utf 8 copy of m_UriTag string
  

public:
  
  void AddRepresentation(c_CfgRepresentation* Representation)
  {
    m_Representations.push_back(Representation);
  }
  
  const c_CfgRepresentation* FindRepresentation(const wchar_t* Pattern) const;
  const c_CfgRepresentation* FindRepresentation( c_CfgRepresentation::e_RepresentationType RepType ) const;

public:  
  
};


typedef std::vector<c_CfgDataResource*> t_CfgDataResourceVector;

class c_CfgDataResourceVector 
{
public:
  //c_CfgDataLayerVector(void);
  ~c_CfgDataResourceVector(void);
  void Clear();
protected:
  t_CfgDataResourceVector m_CfgDataResourceVector;
  
public:
  const c_CfgDataResource* FindUriTag(const std::wstring& UriTag) ;   
  bool  Add(c_CfgDataResource* DataLayer);
  
  int GetCount();
  const c_CfgDataResource* GetResource(int Index) ;   
  //const c_CfgDataResource* FindClassName(const std::wstring& ClassName) ;   
};

#endif