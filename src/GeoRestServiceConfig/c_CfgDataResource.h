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
public:
  ~c_CfgDataResource(void);
  
public:
  std::string m_TemplateFolder; // root folder for templates; Most usually it will be folder where this recource config was found
                                // this path is used for generating paths for templates defined in cfg. Template file names can be relative or absolute.
  
  std::wstring m_UriTag; // name to be used in Uri like parcel e.g. "..\rest\data\parcel"
  std::wstring m_RestUriPart; // if defined this uri will be used as base uri when generating links for html,xml. 
                              // This is usefull in case of redirections.
                              // default is /rest/data/
                              
                              
  c_CfgDataSource  * m_DataSource; 
  
  //std::wstring m_FeatureSource;  // feature source used to retrieve data L"Library://Samples/Sheboygan/Data/Parcels.FeatureSource";
  //std::wstring m_ClassName;  // full class name of the data, L"SHP_Schema:Parcels";
  //std::wstring m_ShortClassName;  // short class name of the data, L"Parcels", it is not written in xml but it is extracted from m_ClassName;
  
  //std::wstring m_MapDef;  // map to be used for image representation; L"Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition";
  //std::wstring m_MapSelectionLayer;  // layer to be used to select highlited features images on when ; L"Library://Samples/Sheboygan/Maps/Parcels.LayerDefinition";
  //std::wstring m_MapZoomFactor;  // layer to be used to select images on when ; L"Library://Samples/Sheboygan/Maps/Parcels.LayerDefinition";
  
  //std::wstring m_MapFeatureSource;  // feature source to be used to zoom to data , this is used when original data doesnt have geomtery r simply want to do zoom to other feature; L"Library://Samples/Sheboygan/Data/Buildings.FeatureSource";
  //std::wstring m_MapFeatureSourceFilter;  // filter to be used when selected features on m_MapFeatureSource
  
  
  t_CfgRepresentationVector m_Representations;
  
  //c_CfgRepTemplate m_HtmlTemplateData;
  //c_CfgRepTemplate m_KmlTemplateData;
  
  
  
  
public:
  /*
  bool IsBBoxHeightLimitSet(const wchar_t* Format) const;
  bool IsBBoxWidthLimitSet(const wchar_t* Format) const;
  bool IsCountLimitSet(const wchar_t* Format) const;
  
  bool IsCountInsideLimit(const wchar_t* Format,int Count) const;
  bool IsWidthInsideLimit(const wchar_t* Format,double Width) const;
  bool IsHeightInsideLimit(const wchar_t* Format,double Height) const;
  
  e_REST_AccessCodes IsAccess_HTML_GET_Anonymous();
  e_REST_AccessCodes IsAccess_XML_GET_Anonymous();
  e_REST_AccessCodes IsAccess_JSON_GET_Anonymous();
  
  e_REST_AccessCodes IsAccess(const wchar_t* Format,const wchar_t*Method,const wchar_t*UserName,const wchar_t* Password) const;
  */
  void AddRepresentation(c_CfgRepresentation* Representation)
  {
    m_Representations.push_back(Representation);
  }
  
  const c_CfgRepresentation* FindRepresentation(const wchar_t* Pattern) const;
  
public:  
  
};


typedef std::vector<c_CfgDataResource*> t_CfgDataResourceVector;

class c_CfgDataResourceVector 
{
public:
  //c_CfgDataLayerVector(void);
  ~c_CfgDataResourceVector(void);
  
protected:
  t_CfgDataResourceVector m_CfgDataResourceVector;
  
public:
  const c_CfgDataResource* FindUriTag(const std::wstring& UriTag) ;   
  bool  Add(c_CfgDataResource* DataLayer);
  
  //const c_CfgDataResource* FindClassName(const std::wstring& ClassName) ;   
};

#endif