#pragma once

#include <vector>

/*
 c_CfgDataLayer is used to describe layer

  if( wcsicmp(Name.c_str(),L"Parcel") == 0)
  {
    DataSource_ResourceId = L"Library://Samples/Sheboygan/Data/Parcels.FeatureSource";
    DataSource_ClassName = L"SHP_Schema:Parcels";
    MapDef = L"Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition";
    return true;
  }
*/

class c_CfgHtmlTemplateExtraData
{
public:
  std::wstring m_FetchUri;  // Uri to get data from
  std::wstring m_HtmlTemp_Section;  // Uri to get data from
  std::wstring m_HtmlTemp_DataSection;  // Uri to get data from
  std::wstring m_HtmlTemp_Prefix;  // name of prefix used when populating dictionary for templates e.g. {{PREFIX_propertyname}}
};

typedef std::vector<c_CfgHtmlTemplateExtraData*> t_CfgHtmlTemplateExtraDataVector;

class c_CfgDataLayer 
{
public:
  c_CfgDataLayer(void);
public:
  ~c_CfgDataLayer(void);
  
public:
  std::wstring m_UriTag; // name to be used in Uri like parcel e.g. "..\rest\data\parcel"
  std::wstring m_FeatureSource;  // feature source used to retrieve data L"Library://Samples/Sheboygan/Data/Parcels.FeatureSource";
  std::wstring m_ClassName;  // full class name of the data, L"SHP_Schema:Parcels";
  std::wstring m_ShortClassName;  // short class name of the data, L"Parcels", it is not written in xml but it is extracted from m_ClassName;
  std::wstring m_MapDef;  // map to be used for image representation; L"Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition";
  
  
  std::wstring m_HtmlTemp_Single; // name of html template to be used when creating html for single feature
  std::wstring m_HtmlTemp_Many;  // name of html template to be used when creating html for many features
  std::wstring m_HtmlTemp_Section;  // name of prefix used when populating dictionary for templates e.g. {{PREFIX_propertyname}}
  std::wstring m_HtmlTemp_Prefix;  // name of prefix used when populating dictionary for templates e.g. {{PREFIX_propertyname}}
  
public:
  int GetCountHtmlExtraData() const { return m_VectorExtraData.size(); }
  const c_CfgHtmlTemplateExtraData* GetHtmlExtraData(int ind) const { return m_VectorExtraData.at(ind); }
  
public:  
  t_CfgHtmlTemplateExtraDataVector m_VectorExtraData; // additional data to be fetched when generating html template 
};


typedef std::vector<c_CfgDataLayer*> t_CfgDataLayerVector;

class c_CfgDataLayerVector 
{
public:
  //c_CfgDataLayerVector(void);
  ~c_CfgDataLayerVector(void);
  
protected:
  t_CfgDataLayerVector m_CfgDataLayerVector;
  
public:
  const c_CfgDataLayer* FindUriTag(const std::wstring& UriTag) ;   
  bool  Add(c_CfgDataLayer* DataLayer);
  
  const c_CfgDataLayer* FindClassName(const std::wstring& ClassName) ;   
};
