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


#ifndef _c_CustomRenderer_h
#define _c_CustomRenderer_h

#include <vector>

#include "c_CfgRepresentation.h"


class c_CfgDataSource;




class REST_CONFIG_API c_CustomRenderer 
{
public:
  c_CustomRenderer(const wchar_t* UriTag);
  ~c_CustomRenderer(void);
  void LoadLibrary(const wchar_t* Library);

public:
  
  std::wstring m_Name; // name used in config file to 
  
  std::string m_Library; // if defined this uri will be used as base uri when generating links for html,xml. 
                              // This is useful in case of redirections.
                              // default is /rest/data/
                              
  FuncHeader2Stream m_FuncHeader2Stream;
  FuncContent2Stream m_FuncContent2Stream;
  

public:  
  
};


typedef std::vector<c_CustomRenderer*> t_CustomRendererVector;

class c_CustomRendererVector
{
public:
  //c_CfgDataLayerVector(void);
  ~c_CustomRendererVector(void);
  void Clear();
  static c_CustomRendererVector* GetInstance();
  
protected:
  t_CustomRendererVector m_CustomRendererVector;
  
public:
  c_CustomRenderer* FindName(const std::wstring& UriTag) ;   
  bool  Add(c_CustomRenderer* Renderer);
  
  int GetCount();
  const c_CustomRenderer* GetRenderer(int Index) ;   
  
};

#endif