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


#ifndef _c_CfgRepresentation_h
#define _c_CfgRepresentation_h

#include <vector>

/*
 c_CfgDataResource is used to describe layer

  if( wcsicmp(Name.c_str(),L"Parcel") == 0)
  {
    DataSource_ResourceId = L"Library://Samples/Sheboygan/Data/Parcels.FeatureSource";
    DataSource_ClassName = L"SHP_Schema:Parcels";
    MapDef = L"Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition";
    return true;
  }
*/

class c_CfgRepTemplateExtraData
{
public:
  std::wstring m_FetchUri;  // Uri to get data from
  std::wstring m_HtmlTemp_Section;  // Uri to get data from
  std::wstring m_HtmlTemp_DataSection;  // Uri to get data from
  std::wstring m_HtmlTemp_Prefix;  // name of prefix used when populating dictionary for templates e.g. {{PREFIX_propertyname}}
};

typedef std::vector<c_CfgRepTemplateExtraData*> t_CfgRepTemplateExtraDataVector;


class c_CfgAccessMethodUser
{
  public:
    std::wstring m_UserName;  // UserName
    std::wstring m_Password;  // Password
    
  public:
    bool IsAnonymous() 
    { 
      if( m_Password.length() == 0 ) return true;
    }    
};
typedef std::vector<c_CfgAccessMethodUser*> t_CfgAccessFormatMethodUserVector;

class c_CfgAccessMethod
{
public:
  c_CfgAccessMethod()
  {
    m_IsMaxBBoxHeight=false;
    m_IsMaxBBoxWidth=false;;
    m_IsMaxCount=false;  
    
    m_IsDefaultCount = false;
    
    m_DefaultCount = 1;
    m_MaxCount = 1;
    m_MaxBBoxHeight = 1;
    m_MaxBBoxWidth = 1;
  }
  ~c_CfgAccessMethod()
  {
    t_CfgAccessFormatMethodUserVector::iterator iter;
    for ( iter = m_Users.begin( ) ; iter != m_Users.end( ) ; iter++ )
    {
      c_CfgAccessMethodUser * adata = *iter;
      delete adata;
    }
  }
  
public:
  std::wstring m_HttpMethod;  // name of http method allowed 
  
  bool m_IsMaxBBoxHeight;
  double m_MaxBBoxHeight;

  bool m_IsMaxBBoxWidth;
  double m_MaxBBoxWidth;

  bool m_IsMaxCount;
  unsigned int m_MaxCount;
  
  bool m_IsDefaultCount;
  unsigned int m_DefaultCount;
  
  
  
  t_CfgAccessFormatMethodUserVector m_Users;
};

typedef std::vector<c_CfgAccessMethod*> t_CfgAccessMethodVector;

/*
class c_CfgAccessRepresentation
{
public:
  ~c_CfgAccessRepresentation()
  {
    t_CfgAccessMethodVector::iterator iter;
    for ( iter = m_MethodVector.begin( ) ; iter != m_MethodVector.end( ) ; iter++ )
    {
      c_CfgAccessMethod * adata = *iter;
      delete adata;
    }
  }
  
public:
  std::wstring m_Format;  // html, xml, json
    
  t_CfgAccessMethodVector m_MethodVector;  // list of allowed methods on layer
    
};

typedef std::vector<c_CfgAccessFormat*> t_CfgAccessFormatVector;
*/




typedef void(*FuncHeader2Stream)(void* , void*);
typedef void(*FuncContent2Stream)(void*, void*);

class REST_CONFIG_API c_CfgRepresentation
{
public:
  enum e_RepresentationType  
  {
    e_Template,  // generate template representation
    e_FeaturesXML, // general FDO XML
    e_FeaturesJSON,     // GeoJson representation of Features
    e_PNG_MapGuide,
    e_FDO_Schema,
    e_Sitemap,
    e_OData,
    e_Custom,
  };
  
  enum e_OrderDirection {
    e_Asc,
    e_Desc
  };
  
public:
  c_CfgRepresentation(e_RepresentationType RepresentationType,const wchar_t* Pattern,const wchar_t* MimeType);
public:
  ~c_CfgRepresentation(void);
 
public: 

  e_RepresentationType GetType() const { return m_RepresentationType; };
  
  void AddMethod(c_CfgAccessMethod* Method);
  e_REST_AccessCodes IsAccess( const wchar_t*Method,const wchar_t*UserName,const wchar_t* Password ) const;

  long GetDefaultCount( ) const; // return default number of features to be returned 
  long GetMaxCount() const; // return maximum number of features allowed to be returned

  bool IsBBoxHeightLimitSet() const;
  bool IsBBoxWidthLimitSet() const;
  bool IsMaxCountSet() const;

  bool IsCountInsideLimit(int Count) const;
  bool IsWidthInsideLimit(double Width) const;
  bool IsHeightInsideLimit(double Height) const;

  void SetOrderFields(const wchar_t* Fields) { m_OrderFields = Fields; };
  void SetOrderDirection(e_OrderDirection Direction) { m_OrderDirection = Direction; }
  

protected:
  e_RepresentationType m_RepresentationType;
  
public:
  std::wstring m_Pattern;    
  std::wstring m_MimeType;    
  
  std::wstring m_OrderFields; // comma separated order fields
  e_OrderDirection m_OrderDirection; // order direction asc,desc
  
  //bool m_IsMaxBBoxHeight;
  //double m_MaxBBoxHeight;

  //bool m_IsMaxBBoxWidth;
  //double m_MaxBBoxWidth;

  //bool m_IsMaxCount;
  //unsigned int m_MaxCount;
  
  
  c_CfgAccessMethod* m_Cached_GET; // cached pointer to GET method
  
  FuncHeader2Stream m_FuncHeader2Stream;
  FuncContent2Stream m_FuncContent2Stream;
  
protected:  
  t_CfgAccessMethodVector m_CfgAccessMethodVector;
  
  
};


class c_CfgRepTemplate : public c_CfgRepresentation
{
public:
  c_CfgRepTemplate(const wchar_t* Pattern,const wchar_t* MimeType) : c_CfgRepresentation(c_CfgRepresentation::e_Template,Pattern,MimeType)
  {
    
  }
  ~c_CfgRepTemplate()
  {
    t_CfgRepTemplateExtraDataVector::iterator iter;
    for ( iter = m_RepTemplateExtraDataVector.begin( ) ; iter != m_RepTemplateExtraDataVector.end( ) ; iter++ )
    {
      c_CfgRepTemplateExtraData * adata = *iter;
      delete adata;
    }
  }
public:
  std::wstring m_HtmlTemp_Single; // name of html template to be used when creating html for single feature
  std::wstring m_HtmlTemp_Many;  // name of html template to be used when creating html for many features
  std::wstring m_HtmlTemp_Zero; // name of html template to be used when creating html for single feature
  std::wstring m_HtmlTemp_Error;  // name of html template to be used when creating html for many features
  std::wstring m_HtmlTemp_Section;  // name of prefix used when populating dictionary for templates e.g. {{PREFIX_propertyname}}
  std::wstring m_HtmlTemp_Prefix;  // name of prefix used when populating dictionary for templates e.g. {{PREFIX_propertyname}}


  


  int GetCountHtmlExtraData() const { return m_RepTemplateExtraDataVector.size(); }
  const c_CfgRepTemplateExtraData* GetHtmlExtraData(int ind) const { return m_RepTemplateExtraDataVector.at(ind); }

  t_CfgRepTemplateExtraDataVector m_RepTemplateExtraDataVector; // additional data to be fetched when generating html template 
};




class REST_CONFIG_API c_AtomElementOverride 
{
public:
  c_AtomElementOverride ();
  
public:
  enum e_OverrideType  
  {
    e_Static,
    e_Property,
    e_Template
  };
  
public:  
  const std::string& GetValueUTF8() const;
  const std::wstring& GetValue() const { return m_Value; }  
  void SetValue( const std::wstring& Value,const std::wstring& OverType );

  e_OverrideType GetOverrideType() const { return m_OverridType; }
  void SetOverrideType(e_OverrideType Type) { m_OverridType=Type; }
  void SetOverrideType(const std::wstring& OverType);
  
  bool IsValuePropertyName() const { return m_OverridType==e_Property; }
  bool IsValueEmpty() const { return m_Value.empty(); }
  
protected:  
  e_OverrideType m_OverridType; // if true it means that m_Value holds name of property to be used to set
                              // if false it means that m_Value holds fixed text to be set for element
  std::wstring m_Value; // if non-empty it will be used as values for some atom element     

  mutable bool m_IsValueUTF8; // true==means that m_ValueUTF8 is valid copy of m_FeedTitle
  mutable std::string m_ValueUTF8; // UTF8 copy of m_ValueUTF8    
};

typedef struct t_AtomAuthorElements
{
  c_AtomElementOverride m_AuthorName; // if non-empty it will be used as author/name of atom:feed 
  c_AtomElementOverride m_AuthorUri; // if non-empty it will be used as author/uRI of atom:feed 
  c_AtomElementOverride m_AuthorEmail; // if non-empty it will be used as author/email of atom:feed 
} t_AtomAuthorElements;

class c_CfgRepOdata : public c_CfgRepresentation
{
public:
  c_CfgRepOdata() : c_CfgRepresentation(c_CfgRepresentation::e_OData,L"OData.svc",L"application/xml")
  {
    
  }
  ~c_CfgRepOdata()
  {
    
  }
  
  
  
public:
  
  c_AtomElementOverride m_FeedTitle; // if non-empty it will be used as title of atom:feed   
  t_AtomAuthorElements m_FeedAuthor;
  
  c_AtomElementOverride m_EntryTitle;
  t_AtomAuthorElements m_EntryAuthor;          
  
};

class REST_CONFIG_API c_CfgRepresentation_Png8_MapGuide : public c_CfgRepresentation
{
public:
  c_CfgRepresentation_Png8_MapGuide(void) : c_CfgRepresentation(c_CfgRepresentation::e_PNG_MapGuide,L".png",L"image/png") 
  {
  
  }
public:
  ~c_CfgRepresentation_Png8_MapGuide(void);

public:
  std::wstring m_MapDef;  // map to be used for image representation; L"Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition";
  std::wstring m_MapSelectionLayer;  // layer to be used to select highlited features images on when ; L"Library://Samples/Sheboygan/Maps/Parcels.LayerDefinition";
  std::wstring m_MapZoomFactor;  // layer to be used to select images on when ; L"Library://Samples/Sheboygan/Maps/Parcels.LayerDefinition";

  std::wstring m_MapFeatureSource;  // feature source to be used to zoom to data , this is used when original data doesnt have geomtery r simply want to do zoom to other feature; L"Library://Samples/Sheboygan/Data/Buildings.FeatureSource";
  std::wstring m_MapFeatureSourceFilter;  // filter to be used when selected features on m_MapFeatureSource

};

class REST_CONFIG_API c_CfgRepresentation_FDO_Schema : public c_CfgRepresentation
{
public:
  c_CfgRepresentation_FDO_Schema(void) : c_CfgRepresentation(c_CfgRepresentation::e_FDO_Schema,L".schema",L"text/xml") 
  {

  }
public:
  ~c_CfgRepresentation_FDO_Schema(void);

public:
 

};

class REST_CONFIG_API c_CfgRepresentation_Sitemap : public c_CfgRepresentation
{
public:
  c_CfgRepresentation_Sitemap(void) : c_CfgRepresentation(c_CfgRepresentation::e_Sitemap,L"sitemap.xml",L"text/xml") 
  {

  }
public:
  ~c_CfgRepresentation_Sitemap(void);

public:


};

class REST_CONFIG_API c_CfgRepresentation_FeaturesXML : public c_CfgRepresentation
{
public:
  c_CfgRepresentation_FeaturesXML(void) : c_CfgRepresentation(c_CfgRepresentation::e_FeaturesXML,L".xml",L"text/xml") 
  {

  }
public:
  ~c_CfgRepresentation_FeaturesXML(void);

public:

};

class REST_CONFIG_API c_CfgRepresentation_FeaturesJSON : public c_CfgRepresentation
{
public:
  c_CfgRepresentation_FeaturesJSON(void) : c_CfgRepresentation(c_CfgRepresentation::e_FeaturesJSON,L".json",L"application/json") 
  {

  }
public:
  ~c_CfgRepresentation_FeaturesJSON(void);

public:

};

class c_CustomRenderer;

class REST_CONFIG_API c_CfgRepresentation_Custom : public c_CfgRepresentation
{
public:
  c_CfgRepresentation_Custom(const c_CustomRenderer* Renderer,const wchar_t* Pattern,const wchar_t* Mime) : c_CfgRepresentation(c_CfgRepresentation::e_Custom,Pattern,Mime) 
  {
    m_CustomRenderer = Renderer;
  }
public:
  ~c_CfgRepresentation_Custom(void);

public:
  const c_CustomRenderer* m_CustomRenderer;

};

typedef std::vector<c_CfgRepresentation*> t_CfgRepresentationVector;





#endif