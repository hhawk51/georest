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


#include "stdafx.h"  
#include "c_OData_Impl.h"
#include "RestConfigInclude.h"



#include <ostream>

#include "Poco/AutoPtr.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/DomParser.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/AttributesImpl.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/DateTimeParser.h"

#include "Poco/DOM/Document.h"
#include "Poco/SAX/ContentHandler.h"
#include "Poco/UTF8String.h"
#include "c_RestRequest.h"
#include "Poco/UnicodeConverter.h"
#include "c_GmlUtil.h"
#include "Poco/String.h"
#include "c_JsonUtil.h"
#include "c_CfgRepresentation.h"

using Poco::XML::Element;

// Atom Qualified and Local names
#define D_XMLTAG_Q_ATOM_TITLE "atom:title"

#define D_XMLTAG_L_ATOM_FEED "feed"
#define D_XMLTAG_L_ATOM_TITLE "title"
#define D_XMLTAG_L_ATOM_ENTRY "entry"
#define D_XMLTAG_L_ATOM_ID "id"
#define D_XMLTAG_L_ATOM_UPDATED "updated"
#define D_XMLTAG_L_ATOM_LINK "link"
#define D_XMLTAG_L_ATOM_AUTHOR "author"
#define D_XMLTAG_L_ATOM_NAME "name"
#define D_XMLTAG_L_ATOM_URI "uri"
#define D_XMLTAG_L_ATOM_EMAIL "email"
#define D_XMLTAG_L_ATOM_CONTENT "content"

#define D_XMLTAG_L_APP_SERVICE "service"
#define D_XMLTAG_L_APP_WORKSPACE "workspace"
#define D_XMLTAG_L_APP_COLLECTION "collection"

// OData
#define D_XMLTAG_L_ODATA_METADATA_PROPERTIES "properties"
#define D_XMLTAG_Q_ODATA_METADATA_PROPERTIES "m:properties"

#define D_XMLTAG_Q_GEODATA_GEOFORMAT D_XMLS_GEO_PREFIX"geoformat"

#define D_GEODATA_GEOFORMAT_GML "Geo.GML"





std::string& c_OData_Impl::GetOdataSchemaName(std::string& name) const
{
  name = "Default";
  
  return name;  
}

std::string& c_OData_Impl::GetOdataEntityName(const c_CfgDataResource* Res,std::string& name) const
{
  name = "ET_";
  name.append(Res->GetUriTag());
  return name;  
}

std::string& c_OData_Impl::GetOdataFullEntityName(const c_CfgDataResource* Res,std::string& fullname) const
{
  GetOdataSchemaName(fullname);
  fullname.append(".");
  std::string en;
  fullname.append(GetOdataEntityName(Res,en));

  return fullname;
}


c_OData_Impl::c_OData_Impl()
{

}

c_OData_Impl::~c_OData_Impl()
{

}



c_OData_Interface::e_OdataFormatType c_OData_Impl::GetResponseFormatType( c_RestRequest* RestRequest )
{
  
  
  // check for format keyword $format
  Ptr<c_RestUriRequestParam> query_params = RestRequest->GetRequestParam();
  STRING param_odata_format = query_params->GetParameterValue(L"$format");
  if( param_odata_format.length() > 0  )
  {
    if( Poco::icompare(param_odata_format,L"json") == 0 )
    {
      return e_OdataJson;
    }
    if( Poco::icompare(param_odata_format,L"atom") == 0 )
    {
      return e_OdataAtom;
    }
    throw c_ExceptionHttpBadRequest("Invalid parameter '$format' value! It can be 'json' or 'atom'.");
  }
  
  // check for accept header
  std::string accept_val;
  if( RestRequest->GetHeaderValue("Accept",accept_val) )
  {
    
    
    std::string mime_json = "application/json";
    
    if( Poco::icompare(mime_json,accept_val) == 0 )
      return e_OdataJson;
  }
  
  return e_OdataAtom;
}


/* 

<?xml version="1.0" encoding="utf-8" standalone="yes" ?> 
<service xml:base="http://services.odata.org/OData/OData.svc/" xmlns:atom="http://www.w3.org/2005/Atom" xmlns:app="http://www.w3.org/2007/app" xmlns="http://www.w3.org/2007/app">
  <workspace>
    <atom:title>Default</atom:title> 
    <collection href="Products">
      <atom:title>Products</atom:title> 
    </collection>
    <collection href="Categories">
      <atom:title>Categories</atom:title> 
    </collection>
    <collection href="Suppliers">
      <atom:title>Suppliers</atom:title> 
    </collection>
  </workspace>
</service>
*/


void c_OData_Impl::CreateServiceDocument( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI,e_OdataFormatType& Format )
{
  switch(Format)
  {
    case e_OdataAtom:
      CreateServiceDocument_Atom( CfgRest,OutStream,BaseURI);
    break;
    case e_OdataJson:
      CreateServiceDocument_Json( CfgRest,OutStream,BaseURI);
    break;
  }
}

void c_OData_Impl::CreateServiceDocument_Atom( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI )
{
  Poco::XML::XMLWriter writer(OutStream, Poco::XML::XMLWriter::WRITE_XML_DECLARATION | Poco::XML::XMLWriter::PRETTY_PRINT);
  writer.setNewLine("\n");
  writer.startDocument();
  writer.startPrefixMapping("",D_XMLS_APP);
  writer.startPrefixMapping("atom",D_XMLS_ATOM);
  writer.startPrefixMapping("app",D_XMLS_APP);
  
  
  Poco::XML::AttributesImpl attrs; 
  attrs.addAttribute("","","xml:base","",BaseURI); // setValue("xml:base",BaseURI);
  writer.startElement("",D_XMLTAG_L_APP_SERVICE,"",attrs);
  
  writer.startElement("",D_XMLTAG_L_APP_WORKSPACE,"");
  writer.startElement("","",D_XMLTAG_Q_ATOM_TITLE);
  writer.characters("GeoREST");
  writer.endElement("","",D_XMLTAG_Q_ATOM_TITLE);
  
  int rescount = CfgRest->GetResourceCount();
  for(int ind=0; ind<rescount;ind++)
  {
    const c_CfgDataResource* res = CfgRest->GetResource(ind);
    if( res->FindRepresentation(c_CfgRepresentation::e_OData)  )
    {    
      Poco::XML::AttributesImpl attrs; 
      attrs.addAttribute("","","href","",res->GetUriTag());
      
      //attrs.addAttribute("","schema","schema","CDATA",res->m_UriTag);
      
      writer.startElement("",D_XMLTAG_L_APP_COLLECTION,"",attrs);
      
      writer.startElement("","",D_XMLTAG_Q_ATOM_TITLE);
      writer.characters(res->GetUriTag());
      writer.endElement("","",D_XMLTAG_Q_ATOM_TITLE);
        
      writer.endElement("",D_XMLTAG_L_APP_COLLECTION,"");
    }
  }
  
  
  writer.endElement("",D_XMLTAG_L_APP_WORKSPACE,"");
  writer.endElement("",D_XMLTAG_L_APP_SERVICE,"");
  writer.endDocument();
}//end of c_OData_Impl::CreateServiceDocument_Atom

/*
{
"d" : {
"EntitySets": [
"Products", "Categories", "Suppliers"
]
}
}
*/
void c_OData_Impl::CreateServiceDocument_Json( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI )
{

  OutStream << "{\n" << "\"d\" : {\n";
  OutStream << "\"EntitySets\": [";
  int odata_count=0;
  int rescount = CfgRest->GetResourceCount();
  for(int ind=0; ind<rescount;ind++)
  {
    
    const c_CfgDataResource* res = CfgRest->GetResource(ind);
    if( res->FindRepresentation(c_CfgRepresentation::e_OData)  )
    {    
      if( odata_count > 0 ) OutStream << ","; 
      std::string json_str;
      
      OutStream << "\"" << c_JsonUtil::Utf8ToJsonString(res->GetUriTag().c_str(),json_str) << "\"";            
      odata_count++;
    }
  }

  OutStream << "]\n}\n}";
  
  
}//end of c_OData_Impl::CreateServiceDocument_Json

void XmlWriteEntityType(Poco::XML::XMLWriter* Writer,const c_CfgDataResource* Resource);

void c_OData_Impl::CreateServiceMetadata( c_RestConfig* CfgRest,std::ostream& OutStream,const std::string& BaseURI )
{
  Poco::XML::XMLWriter writer(OutStream, Poco::XML::XMLWriter::WRITE_XML_DECLARATION | Poco::XML::XMLWriter::PRETTY_PRINT);
  writer.setNewLine("\n");
  writer.startDocument();
  writer.startPrefixMapping("edmx",D_XMLS_EDMX);
  writer.startPrefixMapping("geo",D_XMLS_GEO);
  

  
  Poco::XML::AttributesImpl attrs;
   
  attrs.addAttribute("","","Version","","1.0"); // setValue("xml:base",BaseURI);
  writer.startElement("","","edmx:Edmx",attrs);

  
  writer.startPrefixMapping("m",D_XMLS_ODATA_M);
  attrs.clear();
  attrs.addAttribute("","","m:DataServiceVersion","","2.0"); 
  writer.startElement("","","edmx:DataServices",attrs);
  
  
  // element Schema
  writer.startPrefixMapping("m",D_XMLS_ODATA_M);
  writer.startPrefixMapping("d",D_XMLS_ODATA_D);
  writer.startPrefixMapping("",D_XMLS_EDM);
  attrs.clear();
  std::string schema_namespace;
  GetOdataSchemaName(schema_namespace);
  attrs.addAttribute("","","Namespace","",schema_namespace); 
  writer.startElement("","","Schema",attrs);
  
  
  int rescount = CfgRest->GetResourceCount();
  for(int ind=0; ind<rescount;ind++)
  {
    const c_CfgDataResource* res = CfgRest->GetResource(ind);
    if( res->FindRepresentation(c_CfgRepresentation::e_OData) )
      XmlWriteEntityType(&writer,res);
 
  }
  
  // write entity container
  attrs.clear();  
  attrs.addAttribute("","","Name","","GeoREST_ODataService"); 
  attrs.addAttribute("","","m:IsDefaultEntityContainer","","true"); 
  writer.startElement("","","EntityContainer",attrs);
  std::string entitytype_name; 
  entitytype_name.reserve(512);
  for(int ind=0; ind<rescount;ind++)
  {
    const c_CfgDataResource* res = CfgRest->GetResource(ind);
    const c_CfgRepresentation* repr = res->FindRepresentation(c_CfgRepresentation::e_OData);
    if( repr )
    {    
      const c_CfgRepOdata* rep_odata = (c_CfgRepOdata*)repr;
      // Write Entity Set
      attrs.clear();  
      attrs.addAttribute("","","Name","",res->GetUriTag()); 
      
      
      
      //entitytype_name = schema_namespace;
      //entitytype_name.append(".ET_");
      //entitytype_name.append(res->GetUriTag());
      GetOdataFullEntityName(res,entitytype_name);
       
      attrs.addAttribute("","","EntityType","",entitytype_name); 
      writer.startElement("","","EntitySet",attrs);
      writer.endElement("","","EntitySet");
    }
  }
  writer.endElement("","","EntityContainer");

  writer.endElement("","","Schema");
  writer.endElement("","","edmx:DataServices");
  writer.endElement("","","edmx:Edmx");
  writer.endDocument();
}

void XmlWriteEntityType(Poco::XML::XMLWriter* Writer,const c_CfgDataResource* Resource)
{
  MgClassDefinition * classdef=NULL;
  try
  {  
    classdef = c_RestFetchSource::Fetch_MgClassDefinition(Resource->m_DataSource);
  }
  catch (MgConnectionFailedException* e)
  {                                    
    e->Release(); // ignore unable to connect to MapGuide
  } 
  catch (MgRuntimeException* e)
  {                                    
    e->Release(); // ignore unable to connect to MapGuide
  } 
  
  
  // EntityType
  Poco::XML::AttributesImpl attrs;
  attrs.clear(); 
  
  //STRING wclassname = classdef->GetClassName();
  //std::string classname;
  //MgUtil::WideCharToMultiByte(wclassname,classname);
  
  // for entity type name use ET_ + uritag
  // use that rather then classname beacuse when generating entutyset dont need to go and get again class defintion
  std::string et_name;
  et_name = "ET_" + Resource->GetUriTag();
  attrs.addAttribute("","","Name","",et_name);
  Writer->startElement("","","EntityType",attrs);

  
  
  
  if( classdef )
  {
    // Key Element
    Writer->startElement("","","Key");
    // Property Refs for Key
    Ptr<MgPropertyDefinitionCollection> keyprops = classdef->GetIdentityProperties();
    for(int ind=0;ind<keyprops->GetCount();ind++)
    {
      Ptr<MgPropertyDefinition> keyprop = keyprops->GetItem(ind);
      
      // because SDF will return that autogenerated_sdf_id which is key is also nullable
      // edm will not accept that
      // key must not be nullable
      if( keyprop->GetPropertyType() == MgFeaturePropertyType::DataProperty)
      {
        MgDataPropertyDefinition* dataprop = (MgDataPropertyDefinition*)keyprop.p;
        dataprop->SetNullable(false);
      }
      
      attrs.clear(); 
      
      std::string propname;
      MgUtil::WideCharToMultiByte(keyprop->GetName(),propname);
      
      attrs.addAttribute("","","Name","",propname);
      Writer->startElement("","","PropertyRef",attrs);
      Writer->endElement("","","PropertyRef");
    }
    Writer->endElement("","","Key");
  
    // Property
    Ptr<MgPropertyDefinitionCollection> props = classdef->GetProperties();
    for(int ind=0;ind<props->GetCount();ind++)
    {
      Ptr<MgPropertyDefinition> prop = props->GetItem(ind);
     
      if( prop->GetPropertyType() == MgFeaturePropertyType::DataProperty)
      {
        MgDataPropertyDefinition* dataprop = (MgDataPropertyDefinition*)prop.p;
        
        string prop_nullable,prop_type,prop_name;
        
        MgUtil::WideCharToMultiByte(dataprop->GetName(),prop_name);
        
        if( dataprop->GetNullable() && !dataprop->IsAutoGenerated() )
          prop_nullable = "true";
        else
          prop_nullable = "false";
        
        switch(dataprop->GetDataType())
        {
          case MgPropertyType::Int32:
            prop_type = "Edm.Int32";
          break;
          case MgPropertyType::Int16:
            prop_type = "Edm.Int16";
          break;
          case MgPropertyType::Int64:
            prop_type = "Edm.Int64";
          break;
          case MgPropertyType::String:
            prop_type = "Edm.String";
          break;
          case MgPropertyType::DateTime:
            prop_type = "Edm.DateTime";
          break;
          case MgPropertyType::Double:
            prop_type = "Edm.Double";
          break;
          case MgPropertyType::Single:
            prop_type = "Edm.Single";
          break;
          case MgPropertyType::Boolean:
            prop_type = "Edm.Boolean";
          break;
          case MgPropertyType::Byte:
            prop_type = "Edm.Byte";
          break;
        }
        
        if( prop_type.length())
        {
              
          attrs.clear(); 
          attrs.addAttribute("","","Name","",prop_name);
          attrs.addAttribute("","","Type","",prop_type);
          attrs.addAttribute("","","Nullable","",prop_nullable);
          Writer->startElement("","","Property",attrs);
          Writer->endElement("","","Property");
        }
      }
      else
      {
      
        if( prop->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
        {
          MgGeometricPropertyDefinition* geomprop = (MgGeometricPropertyDefinition*)prop.p;
          std::string prop_type = "Edm.String";
          std::string prop_nullable,prop_name;

          MgUtil::WideCharToMultiByte(geomprop->GetName(),prop_name);

          
          prop_nullable = "true";
          
            
          if( prop_type.length())
          {

            attrs.clear(); 
            attrs.addAttribute("",D_XMLTAG_Q_GEODATA_GEOFORMAT,"","",D_GEODATA_GEOFORMAT_GML);
            attrs.addAttribute("","","Name","",prop_name);
            attrs.addAttribute("","","Type","",prop_type);
            attrs.addAttribute("","","Nullable","",prop_nullable);
            Writer->startElement("","","Property",attrs);
            Writer->endElement("","","Property");
          }
        }
        
      }
    }
  }
  
  
  

  Writer->endElement("","","EntityType");
}



void c_OData_Impl::Reader2AtomCollection(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                         ,c_RestDataReader* Reader,std::ostream & OutStream,e_OdataFormatType& Format )
{
  switch(Format)
  {
    case e_OdataAtom:
      Reader2AtomCollection_Atom(RestRequest,ODataRepresentation,Reader,OutStream );
    break;
    case e_OdataJson:
      Reader2AtomCollection_Json(RestRequest,ODataRepresentation,Reader,OutStream );
    break;
  }
}

void c_OData_Impl::Reader2AtomCollection_Atom(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                              ,c_RestDataReader* Reader,std::ostream & OutStream)
{
  Poco::XML::AttributesImpl attrs;
  
  //std::string baseuri = RestRequest->GetBaseUri();
  
  
  Poco::XML::XMLWriter writer(OutStream, Poco::XML::XMLWriter::WRITE_XML_DECLARATION | Poco::XML::XMLWriter::PRETTY_PRINT);
  writer.setNewLine("\n");
  writer.startDocument();
  writer.startPrefixMapping("",D_XMLS_ATOM);
  writer.startPrefixMapping("d",D_XMLS_ODATA_D);
  writer.startPrefixMapping("m",D_XMLS_ODATA_M);
  writer.startPrefixMapping("geo",D_XMLS_GEO);
  
  attrs.clear(); 
  attrs.addAttribute("","","xml:base","",RestRequest->GetServiceURI()); // setValue("xml:base",BaseURI);
  writer.startElement("",D_XMLTAG_L_ATOM_FEED,"",attrs);
  
  // write atom feed elements : title,id,updated,link self
  
  // title
  attrs.clear(); 
  attrs.addAttribute("","type","","","text"); // setValue("xml:base",BaseURI);
  writer.startElement("",D_XMLTAG_L_ATOM_TITLE,"",attrs);
  if( !ODataRepresentation->m_FeedTitle.IsValueEmpty() )
    writer.characters(ODataRepresentation->m_FeedTitle.GetValueUTF8());
  else
    writer.characters(RestRequest->m_CfgDataResource->GetUriTag());
  writer.endElement("",D_XMLTAG_L_ATOM_TITLE,"");
  
  // feed id
  writer.startElement("",D_XMLTAG_L_ATOM_ID,"");
  writer.characters(RestRequest->GetServiceURI());
  writer.characters(RestRequest->m_CfgDataResource->GetUriTag());
  writer.endElement("",D_XMLTAG_L_ATOM_ID,"");
  
  // feed updated
  Poco::Timestamp now;
  Poco::DateTime dt(now);
  std::string atom_updated = Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::ISO8601_FORMAT);
  
  writer.startElement("",D_XMLTAG_L_ATOM_UPDATED,"");
  writer.characters(atom_updated);
  writer.endElement("",D_XMLTAG_L_ATOM_UPDATED,"");
  
  // feed author
  writer.startElement("",D_XMLTAG_L_ATOM_AUTHOR,"");
  writer.startElement("",D_XMLTAG_L_ATOM_NAME,"");
  if( !ODataRepresentation->m_FeedAuthor.m_AuthorName.IsValueEmpty() )
    writer.characters(ODataRepresentation->m_FeedAuthor.m_AuthorName.GetValueUTF8());
  writer.endElement("",D_XMLTAG_L_ATOM_NAME,"");
  if( !ODataRepresentation->m_FeedAuthor.m_AuthorUri.IsValueEmpty() )
  {  
    writer.startElement("",D_XMLTAG_L_ATOM_URI,"");    
    writer.characters(ODataRepresentation->m_FeedAuthor.m_AuthorUri.GetValueUTF8());
    writer.endElement("",D_XMLTAG_L_ATOM_URI,"");
  }
  if( !ODataRepresentation->m_FeedAuthor.m_AuthorEmail.IsValueEmpty() )
  {  
    writer.startElement("",D_XMLTAG_L_ATOM_EMAIL,"");    
    writer.characters(ODataRepresentation->m_FeedAuthor.m_AuthorEmail.GetValueUTF8());
    writer.endElement("",D_XMLTAG_L_ATOM_EMAIL,"");
  }
  writer.endElement("",D_XMLTAG_L_ATOM_AUTHOR,"");
  
  
  // now skip $if $skip defined 
  Ptr<c_RestUriRequestParam> query_params = RestRequest->GetRequestParam();
  STRING param_skip = query_params->GetParameterValue(L"$skip");
  if( param_skip.length() > 0  )
  {
    long skip = MgUtil::StringToInt32(param_skip);
    if( skip > 0 )
    {
      while( (skip>0) && Reader->ReadNext() ) skip--;
    }
  }
  
  long param_top=-1; // not defined
  STRING param_topstr = query_params->GetParameterValue(L"$top");
  if( param_topstr.length() > 0  )
  {
    param_top = MgUtil::StringToInt32(param_topstr);    
  }
  long param_skiptoken=-1;
  STRING param_skiptokenstr = query_params->GetParameterValue(L"$skiptoken");
  if( param_skiptokenstr.length() > 0  )
  {
    param_skiptoken = MgUtil::StringToInt32(param_skiptokenstr);
   
  }
  
  if( param_skiptoken > 0 )
  {
    int ind=0;
    while( (ind<param_skiptoken) && Reader->ReadNext()  ) { ind++;};
  }
  
  long maxcount = 200;
  if( ODataRepresentation->IsMaxCountSet() )
  {
    maxcount = ODataRepresentation->GetMaxCount();    
  }
  
  if( param_top < 0 ) // if not defined in URI
  {
    param_top = ODataRepresentation->GetDefaultCount();
  }
  
  std::wstring one_prop;
  // write entries from reader
  int count=0;
  bool isnext = true; // need to remember last one, because sdf provider has a bug and will "roll-over"
  
  if( param_top > 0 )
  {  
    while((isnext=Reader->ReadNext())==true)
    {
      WriteEntry_Atom(writer,RestRequest,ODataRepresentation,Reader,one_prop,atom_updated);
      count++;
      if( (count >= maxcount) )
      {            
        break;
      }
      if( (param_top >= 0 ) && (count >= param_top) )
      {
        break;
      }
    }
  }
  if(isnext)
  {
    // now I need to generate link for next elements using skiptoken
    long new_skiptoken = param_skiptoken > 0 ? param_skiptoken : 0;
    new_skiptoken+=count;

    // add link next
    std::string href;
    Ptr<MgStringCollection> remove_skiptoken = new MgStringCollection();
    remove_skiptoken->Add(L"$skiptoken");

    Ptr<MgStringPropertyCollection> add_skiptoken = new MgStringPropertyCollection();
    std::wstring st_str;
    MgUtil::Int32ToString(new_skiptoken,st_str);
    add_skiptoken->Add(L"$skiptoken",st_str);

    RestRequest->GetRestUri().GetFullUri(href,remove_skiptoken,add_skiptoken);
    attrs.clear(); 
    attrs.addAttribute("","href","","",href); 
    attrs.addAttribute("","rel","","","next"); 
    writer.startElement("",D_XMLTAG_L_ATOM_LINK,"",attrs);      
    writer.endElement("",D_XMLTAG_L_ATOM_LINK,"");
  }
  
  writer.endElement("",D_XMLTAG_L_ATOM_FEED,"");
}//end of Reader2AtomCollection_Atom

void c_OData_Impl::Reader2AtomCollection_Json(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                              ,c_RestDataReader* Reader,std::ostream & OutStream)
{
 
  OutStream << "{\n" << "\"d\" : [\n";
  
  

  // now skip $if $skip defined 
  Ptr<c_RestUriRequestParam> query_params = RestRequest->GetRequestParam();
  STRING param_skip = query_params->GetParameterValue(L"$skip");
  if( param_skip.length() > 0  )
  {
    long skip = MgUtil::StringToInt32(param_skip);
    if( skip > 0 )
    {
      while( (skip>0) && Reader->ReadNext() ) skip--;
    }
  }

  long param_top=-1; // not defined
  STRING param_topstr = query_params->GetParameterValue(L"$top");
  if( param_topstr.length() > 0  )
  {
    param_top = MgUtil::StringToInt32(param_topstr);    
  }
  long param_skiptoken=-1;
  STRING param_skiptokenstr = query_params->GetParameterValue(L"$skiptoken");
  if( param_skiptokenstr.length() > 0  )
  {
    param_skiptoken = MgUtil::StringToInt32(param_skiptokenstr);

  }

  if( param_skiptoken > 0 )
  {
    int ind=0;
    while( (ind<param_skiptoken) && Reader->ReadNext()  ) { ind++;};
  }

  long maxcount = 200;
  if( ODataRepresentation->IsMaxCountSet() )
  {
    maxcount = ODataRepresentation->GetMaxCount();    
  }

  if( param_top < 0 ) // if not defined in URI
  {
    param_top = ODataRepresentation->GetDefaultCount();
  }
  
  std::wstring one_prop;

  // write entries from reader
  int count=0;
  bool isnext=true; // need to remember last one, because sdf provider has a bug and will "roll-over"
  if( param_top > 0 )
  {
    while((isnext=Reader->ReadNext())==true )
    {
      if( count > 0) OutStream << ",";
      
      WriteEntry_Json(OutStream,RestRequest,ODataRepresentation,Reader,one_prop);
      count++;
      if( (count >= maxcount)  )
      {            
        break;
      }
      if( (param_top >= 0 ) && (count >= param_top) )
      {
        break;
      }
    }
  }
  OutStream << "]"; // end array of objects
  if(isnext)
  {
    // now I need to generate link for next elements using skiptoken
    long new_skiptoken = param_skiptoken > 0 ? param_skiptoken : 0;
    new_skiptoken+=count;

    
    
    std::string href;
    Ptr<MgStringCollection> remove_skiptoken = new MgStringCollection();
    remove_skiptoken->Add(L"$skiptoken");

    Ptr<MgStringPropertyCollection> add_skiptoken = new MgStringPropertyCollection();
    std::wstring st_str;
    MgUtil::Int32ToString(new_skiptoken,st_str);
    add_skiptoken->Add(L"$skiptoken",st_str);

    RestRequest->GetRestUri().GetFullUri(href,remove_skiptoken,add_skiptoken);
    
    // add link next
    OutStream << ", \"__next\": " << "\"" << href << "\""; // end array of objects
  }

  OutStream << "}\n"; // end of "d"
  OutStream << "}"; // end of root object
  
}//end of Reader2AtomCollection_Json

void c_OData_Impl::Reader2AtomEntry(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                    ,c_RestDataReader* Reader,std::ostream & OutStream,e_OdataFormatType& Format
                                    ,std::wstring& OneProperty,bool IsRawValue )
{

  if( OneProperty.length() > 0 && IsRawValue )
  {
    std::string prop_val;
    if( Reader->IsNull(OneProperty)) 
      prop_val = "null";
    else
      GetPropertyValueAsOdataLiteral_URI(Reader,OneProperty,prop_val);
      
    return;
  }
  
  switch(Format)
  {
    case e_OdataAtom:
      Reader2AtomEntry_Atom(RestRequest,ODataRepresentation,Reader,OutStream,OneProperty,IsRawValue );
    break;
    case e_OdataJson:
      Reader2AtomEntry_Json(RestRequest,ODataRepresentation,Reader,OutStream,OneProperty,IsRawValue );
    break;
  }
}//end of c_OData_Impl::Reader2AtomEntry           
void c_OData_Impl::Reader2AtomEntry_Atom(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                        ,c_RestDataReader* Reader,std::ostream & OutStream
                                        ,std::wstring& OneProperty,bool IsRawValue
                                        )
{
  Poco::XML::AttributesImpl attrs;
  attrs.clear(); 
  
  Poco::XML::XMLWriter writer(OutStream, Poco::XML::XMLWriter::WRITE_XML_DECLARATION | Poco::XML::XMLWriter::PRETTY_PRINT);
  writer.setNewLine("\n");
  writer.startDocument();
  if( OneProperty.length() == 0 )  writer.startPrefixMapping("",D_XMLS_ATOM); // if there will be only one property then no need for atom
  writer.startPrefixMapping("d",D_XMLS_ODATA_D);
  writer.startPrefixMapping("m",D_XMLS_ODATA_M);
  writer.startPrefixMapping("geo",D_XMLS_GEO);
  
  if( Reader->ReadNext() )
  {
    Poco::Timestamp now;
    Poco::DateTime dt(now);
    std::string atom_updated = Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::ISO8601_FORMAT,true);
    
    WriteEntry_Atom(writer,RestRequest,ODataRepresentation,Reader,OneProperty,atom_updated);
  }
}//end of c_OData_Impl::Reader2AtomEntry_Atom
void c_OData_Impl::Reader2AtomEntry_Json(c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                         ,c_RestDataReader* Reader,std::ostream & OutStream
                                         ,std::wstring& OneProperty,bool IsRawValue
                                         )
{
  OutStream << "{\n" << "\"d\" : \n";

  if( Reader->ReadNext() )
  {
    WriteEntry_Json(OutStream,RestRequest,ODataRepresentation,Reader,OneProperty);
  }
  OutStream << "\n}";
}//end of c_OData_Impl::Reader2AtomEntry_Json

void c_OData_Impl::GetPropertyValueAsOdataLiteral_Atom( c_RestDataReader* Reader,const std::wstring& PropName,std::string& StrVal )
{
  GetPropertyValueAsOdataLiteral_X(Reader,PropName,StrVal,false);
}

void c_OData_Impl::GetPropertyValueAsOdataLiteral_URI(c_RestDataReader* Reader,const std::wstring& PropName,std::string& StrVal)
{
  GetPropertyValueAsOdataLiteral_X(Reader,PropName,StrVal,true);
}
//
void c_OData_Impl::GetPropertyValueAsOdataLiteral_X(c_RestDataReader* Reader,const std::wstring& PropName,std::string& StrVal,bool IsUri)
{
  StrVal.clear();
  if( Reader->IsNull(PropName) )
  { 
    return;
  }
  
  int ptype = Reader->GetPropertyType(PropName);
  
  switch( ptype )
  {
    case MgPropertyType::Geometry:
    {
      try
      {
        Ptr<MgByteReader> bytes = Reader->GetGeometry(PropName);
        MgAgfReaderWriter agfReader;                
        Ptr<MgGeometry> fgfgeom = agfReader.Read(bytes);
        c_GmlUtil::ToGML(fgfgeom,StrVal);
      }
      catch (MgException *ex) // just ignore in case of invalid geometries
      {
        StrVal="";
        ex->Release();
      }
      
    }
    break;
    case MgPropertyType::Int32:
    {
      long val = Reader->GetInt32(PropName);
      Poco::NumberFormatter::append(StrVal,val);
              
    }
    break;
    case MgPropertyType::Int16:
    {
      int val = Reader->GetInt16(PropName);
      Poco::NumberFormatter::append(StrVal,val);
    }
    break;
    case MgPropertyType::Int64:
    {
      INT64 val = Reader->GetInt64(PropName);
      Poco::NumberFormatter::append(StrVal,val);
      if( IsUri ) StrVal.append("L");
    }
    break;
    case MgPropertyType::Double:
    {
      double val = Reader->GetDouble(PropName);
      Poco::NumberFormatter::append(StrVal,val);
      
    }
    break;
    case MgPropertyType::Single:
    {
      float val = Reader->GetSingle(PropName);
      Poco::NumberFormatter::append(StrVal,val);
      if( IsUri ) StrVal.append("f");
    }
    break;
    case MgPropertyType::Boolean:
    {
      bool val = Reader->GetBoolean(PropName);
      StrVal.append(val ? "true" : "false");      
    }
    break;
    case MgPropertyType::Byte:
    {
      BYTE val = Reader->GetByte(PropName);
      Poco::NumberFormatter::appendHex(StrVal,val);
    }
    break;
    case MgPropertyType::DateTime:
    {
      Ptr<MgDateTime> date = Reader->GetDateTime(PropName);
      
      // set date as Atom string 
      Poco::DateTime dt(date->GetYear(), date->GetMonth(), date->GetDay(), date->GetHour(), date->GetMinute(), date->GetSecond());

      std::string str = Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::ISO8601_FORMAT);
      if(IsUri) StrVal.append("datetime'");
      StrVal.append(str);
      if( IsUri ) StrVal.append("'");
    }
    break;
    case MgPropertyType::String:
    {
      string val = MgUtil::WideCharToMultiByte(Reader->GetString(PropName));
      
      if( IsUri ) StrVal.append("'");
      StrVal.append(val);
      if( IsUri ) StrVal.append("'");

    }
    break;
  }
}


//
// creates odata key string, string is constructed of pairs name1=value1,name2=value2
// where name1 name2 are identity properties (keys) of class
//
void c_OData_Impl::GetOdataKeyValues(c_RestDataReader* Reader,MgClassDefinition* DataClassDef,std::string& KeysVal)
{
  Ptr<MgPropertyDefinitionCollection> keyprops = DataClassDef->GetIdentityProperties();
  
  
  int count = keyprops->GetCount();
  if( count == 1)
  {
    int ind=0;
    Ptr<MgPropertyDefinition> prop = keyprops->GetItem(ind);
    GetPropertyValueAsOdataLiteral_URI(Reader,prop->GetName(),KeysVal);
  }
  else
  {
    std::string tempstr;
    tempstr.reserve(128);
    KeysVal.clear();
    for(int ind=0;ind<count;ind++)
    {
      Ptr<MgPropertyDefinition> prop = keyprops->GetItem(ind);
      Poco::UnicodeConverter::toUTF8(prop->GetName(),tempstr);
      KeysVal.append(tempstr);
      GetPropertyValueAsOdataLiteral_URI(Reader,prop->GetName(),tempstr);
      KeysVal.append(tempstr);
      if( ind < count ) KeysVal.append(",");
    }
  }
}



// Write Atom Entry of one feature
// it is just entry part without starting <?xml...
//
// <entry>
// <id>http://services.odata.org/OData/OData.svc/Categories(1)</id>
// <title type="text">Beverages</title>
// <updated>2010-07-23T12:19:24Z</updated>
// <author>
// <name />
// </author>
// <link rel="edit" title="Category" href="Categories(1)" />
// <category term="ODataDemo.Category" scheme="http://schemas.microsoft.com/ado/2007/08/dataservices/scheme" />
// <content type="application/xml">
// <m:properties>
// <d:Name>Beverages</d:Name>
// </m:properties>
// </content>
// </entry>
void c_OData_Impl::WriteProperty_Atom(Poco::XML::XMLWriter& writer,c_RestDataReader* Reader,MgPropertyDefinition* prop)
{
  Poco::XML::AttributesImpl attrs;
  std::string xtag_name,nameutf8,prop_val;
  std::wstring prop_name = prop->GetName();
  GetPropertyValueAsOdataLiteral_Atom(Reader,prop_name,prop_val);
  attrs.clear(); 
  //attrs.addAttribute("","","","m:type","application/xml"); // setValue("xml:base",BaseURI);
  Poco::UnicodeConverter::toUTF8(prop_name,nameutf8);
  xtag_name = "d:";
  xtag_name.append(nameutf8);
  if( prop->GetPropertyType() == MgFeaturePropertyType::GeometricProperty )
  { 
    attrs.addAttribute("",D_XMLTAG_Q_GEODATA_GEOFORMAT,"","",D_GEODATA_GEOFORMAT_GML); 

  }
  writer.startElement("","",xtag_name,attrs);
  writer.characters(prop_val);
  writer.endElement("","",xtag_name);
}

void c_OData_Impl::WriteEntry_Atom(Poco::XML::XMLWriter& writer
                                    ,const c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                    ,c_RestDataReader* Reader,std::wstring& OneProperty,const std::string& Updated,bool AddBase)
{
  
  
  
  if( OneProperty.length() > 0 )
  {
    Ptr<MgPropertyDefinitionCollection> props = RestRequest->m_DataClassDef->GetProperties();
    Ptr<MgPropertyDefinition> prop = props->GetItem(OneProperty);
    
    WriteProperty_Atom(writer,Reader,prop);
    
    return;
  }
  
  Poco::XML::AttributesImpl attrs;
  
  attrs.clear(); 
  if( AddBase ) attrs.addAttribute("","","xml:base","",RestRequest->GetServiceURI()); // setValue("xml:base",BaseURI);
  writer.startElement("",D_XMLTAG_L_ATOM_ENTRY,"");
  
  Poco::XML::XMLString tempstr;
  tempstr.reserve(512);
  
  // write id
  attrs.clear(); 
  writer.startElement("",D_XMLTAG_L_ATOM_ID,"");
  writer.characters(RestRequest->GetServiceURI());
  writer.characters("/");
  writer.characters(RestRequest->m_CfgDataResource->GetUriTag());
  writer.characters("(");  
  GetOdataKeyValues(Reader,RestRequest->m_DataClassDef,tempstr);
  writer.characters(tempstr);  
  writer.characters(")");
  writer.endElement("",D_XMLTAG_L_ATOM_ID,"");
  
  // title
  attrs.clear(); 
  attrs.addAttribute("","type","","","text"); // setValue("xml:base",BaseURI);
  writer.startElement("",D_XMLTAG_L_ATOM_TITLE,"",attrs);
  
  if( !ODataRepresentation->m_EntryTitle.IsValueEmpty() )
  {  
    if( ODataRepresentation->m_EntryTitle.IsValuePropertyName() )
    {
      GetPropertyValueAsOdataLiteral_Atom(Reader,ODataRepresentation->m_EntryTitle.GetValue(),tempstr);
      writer.characters(tempstr);
    }
    else
    {
      writer.characters(ODataRepresentation->m_EntryTitle.GetValueUTF8());
    }
      
    
  }
  writer.characters("");
  writer.endElement("",D_XMLTAG_L_ATOM_TITLE,"");
  
  // entry author
  writer.startElement("",D_XMLTAG_L_ATOM_AUTHOR,"");
  writer.startElement("",D_XMLTAG_L_ATOM_NAME,"");
  if( !ODataRepresentation->m_EntryAuthor.m_AuthorName.IsValueEmpty() )
  {
    if( ODataRepresentation->m_EntryAuthor.m_AuthorName.IsValuePropertyName() )
    {
      GetPropertyValueAsOdataLiteral_Atom(Reader,ODataRepresentation->m_EntryAuthor.m_AuthorName.GetValue(),tempstr);
      writer.characters(tempstr);
    }
    else
    {
      writer.characters(ODataRepresentation->m_EntryAuthor.m_AuthorName.GetValueUTF8());
    }    
  }
  writer.endElement("",D_XMLTAG_L_ATOM_NAME,"");
  if( !ODataRepresentation->m_EntryAuthor.m_AuthorUri.IsValueEmpty() )
  {  
    writer.startElement("",D_XMLTAG_L_ATOM_URI,"");    
    if( ODataRepresentation->m_EntryAuthor.m_AuthorUri.IsValuePropertyName() )
    {
      GetPropertyValueAsOdataLiteral_Atom(Reader,ODataRepresentation->m_EntryAuthor.m_AuthorUri.GetValue(),tempstr);
      writer.characters(tempstr);
    }
    else
    {
      writer.characters(ODataRepresentation->m_EntryAuthor.m_AuthorUri.GetValueUTF8());
    }   
    writer.endElement("",D_XMLTAG_L_ATOM_URI,"");
  }
  if( !ODataRepresentation->m_EntryAuthor.m_AuthorEmail.IsValueEmpty() )
  {  
    writer.startElement("",D_XMLTAG_L_ATOM_EMAIL,"");    
    if( ODataRepresentation->m_EntryAuthor.m_AuthorEmail.IsValuePropertyName() )
    {
      GetPropertyValueAsOdataLiteral_Atom(Reader,ODataRepresentation->m_EntryAuthor.m_AuthorEmail.GetValue(),tempstr);
      writer.characters(tempstr);
    }
    else
    {
      writer.characters(ODataRepresentation->m_EntryAuthor.m_AuthorEmail.GetValueUTF8());
    }   
    writer.endElement("",D_XMLTAG_L_ATOM_EMAIL,"");
  }
  writer.endElement("",D_XMLTAG_L_ATOM_AUTHOR,"");
  
  writer.startElement("",D_XMLTAG_L_ATOM_UPDATED,"");
  writer.characters(Updated);
  writer.endElement("",D_XMLTAG_L_ATOM_UPDATED,"");
  
  // write category of entry
  
  // Content
  attrs.clear(); 
  attrs.addAttribute("","","type","","application/xml"); // setValue("xml:base",BaseURI);
  writer.startElement("",D_XMLTAG_L_ATOM_CONTENT,"",attrs);
  
  // properties in content
  writer.startElement("","",D_XMLTAG_Q_ODATA_METADATA_PROPERTIES,attrs);
  Ptr<MgPropertyDefinitionCollection> props = RestRequest->m_DataClassDef->GetProperties();
  int count = props->GetCount();
  STRING prop_name;
  std::string prop_val,nameutf8,xtag_name;prop_val.reserve(512);
  for(int ind=0;ind<count;ind++)
  {
    Ptr<MgPropertyDefinition> prop = props->GetItem(ind);
    WriteProperty_Atom(writer,Reader,prop);
  }
  writer.endElement("","",D_XMLTAG_Q_ODATA_METADATA_PROPERTIES);
  
  writer.endElement("",D_XMLTAG_L_ATOM_CONTENT,"");
  
  writer.endElement("",D_XMLTAG_L_ATOM_ENTRY,"");

}//end of c_OData_Impl:WriteEntry



void c_OData_Impl::WriteProperty_Json( std::ostream & OutStream,c_RestDataReader* Reader,MgPropertyDefinition* prop)
{
  std:;string nameutf8,jsontmp;
  switch( prop->GetPropertyType() )
  {
    case MgFeaturePropertyType::GeometricProperty:
    {
      MgGeometricPropertyDefinition* prop_geom = (MgGeometricPropertyDefinition *)prop;
      std::wstring prop_name = prop->GetName();
      Poco::UnicodeConverter::toUTF8(prop_name,nameutf8); 

      // add geojson
      OutStream << ",\"" << c_JsonUtil::Utf8ToJsonString(nameutf8.c_str(),jsontmp) << "\": ";

      if( Reader->IsNull(prop_name) )
      {
        OutStream << "null";          
      }
      else
      {
        try
        {
        
          Ptr<MgByteReader> bytes = Reader->GetGeometry(prop_name);
          MgAgfReaderWriter agfReader;                
          Ptr<MgGeometry> fgfgeom = agfReader.Read(bytes);
          std::string geomstr;
          c_JsonUtil::AppendGeoJson(fgfgeom,geomstr);
          OutStream << geomstr;          
        }
        catch (MgException *ex) // just ignore in case of invalid geometries
        {
          OutStream << NULL;
          ex->Release();
        }
        
      }
    }
    break;
   case MgFeaturePropertyType::DataProperty:
    {
      MgDataPropertyDefinition * prop_data = (MgDataPropertyDefinition *)prop;
      std::wstring prop_name = prop->GetName();
      Poco::UnicodeConverter::toUTF8(prop_name,nameutf8);    
      OutStream << ",\"" << c_JsonUtil::Utf8ToJsonString(nameutf8.c_str(),jsontmp) << "\": ";

      if( Reader->IsNull(prop_name) )
      {
        OutStream << "null";          
      }
      else
      {
        bool as_jsonstring = true;
        if( (prop_data->GetDataType() == MgPropertyType::Int16)
          ||
          (prop_data->GetDataType() == MgPropertyType::Int32)
          )
        {
          as_jsonstring = false;
        }

        std::string prop_val;
        GetPropertyValueAsOdataLiteral_URI(Reader,prop_name,prop_val);


        if( as_jsonstring )
          OutStream << "\"" << c_JsonUtil::Utf8ToJsonString(prop_val.c_str(),jsontmp) << "\"";        
        else
          OutStream << c_JsonUtil::Utf8ToJsonString(prop_val.c_str(),jsontmp);        
      }                
    }
    break;
  }
}//end of c_OData_Impl::WriteProperty_Json

void c_OData_Impl::WriteEntry_Json( std::ostream & OutStream, const c_RestRequest* RestRequest,const c_CfgRepOdata* ODataRepresentation
                                   ,c_RestDataReader* Reader,std::wstring& OneProperty)
{
  
  
  if( OneProperty.length() > 0 )
  {
    Ptr<MgPropertyDefinitionCollection> props = RestRequest->m_DataClassDef->GetProperties();
    Ptr<MgPropertyDefinition> prop = props->GetItem(OneProperty);
    OutStream << "{\n";
    WriteProperty_Json(OutStream,Reader,prop);
    OutStream << "}\n";
    return;
  }
  
  
  std::string uri;
  uri = RestRequest->m_CfgDataResource->GetUriTag();
  uri.append("(");
  std::string tempstr;
  GetOdataKeyValues(Reader,RestRequest->m_DataClassDef,tempstr);
  uri.append(tempstr);
  uri.append(")");
  
  std::string full_ent_name,jsontmp;
  GetOdataFullEntityName(RestRequest->m_CfgDataResource,full_ent_name);
  
  
  
    
  OutStream << "{\n";
  OutStream << "\"__metadata\": {";
  OutStream << "\n\"uri\": " << "\"" << c_JsonUtil::Utf8ToJsonString(uri.c_str(),jsontmp) << "\"";
  OutStream << ",\"type\": " << "\"" << c_JsonUtil::Utf8ToJsonString(full_ent_name.c_str(),jsontmp) << "\"";
  OutStream << "}"; // end of metadata
  

 


  // properties in content
  Ptr<MgPropertyDefinitionCollection> props = RestRequest->m_DataClassDef->GetProperties();
  int count = props->GetCount();
  STRING prop_name;
  std::string prop_val,nameutf8,xtag_name;
  for(int ind=0;ind<count;ind++)
  {
    Ptr<MgPropertyDefinition> prop = props->GetItem(ind);
    
    WriteProperty_Json(OutStream,Reader,prop);
          
  }
  

  OutStream << "}";

}//end of c_OData_Impl:WriteEntry_Json



void FindPartEnd(const wchar_t* startcp,const wchar_t*& endcp,bool& IsDoubleQuote)
{
  IsDoubleQuote = false;
  bool instring=false;
  endcp=startcp;
  while(*endcp )
  {
    if( !instring && (*endcp == ',' || *endcp == '=') )
    {
      break;
    }
    // read until reach = or ,
    if( *endcp == '\'')
    {     
      if( instring ) 
      {
        // check for if next char is ' too
        if( *(endcp+1) == '\'' )
        {
          IsDoubleQuote=true;
          // it is double squote
          endcp++; // just skip it; double ' in string is treated as string with '
        }
        else
          instring = false;
      }
      else instring = true;
    
    }

    endcp++;
  }
  
  if( !*endcp && instring)
  {
    // end of string while i was reading value as string (inside '...')
    throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! End of key-value pair string while reading 'string value'.");
  }

}
bool GetNextKeyValuePair(const wchar_t*& cp,std::wstring& key,std::wstring& val)
{
  if( !cp || !*cp ) return false;
  
  if( *cp == L',') cp++;
  
  if( *cp == L'=') 
  {
    throw c_ExceptionInvalidKeyValueString("Invalid Key Value string.");
  }
  
  
  bool isvalue = false;
  key.clear();val.clear();
  const wchar_t* endcp;
  bool isdoublequote;
  FindPartEnd(cp,endcp,isdoublequote);
  
  
  if( *endcp == '=' )
  {
    if( endcp != cp ) key.append(cp,endcp);
    cp = endcp;
  // read value    
    cp++;
    FindPartEnd(cp,endcp,isdoublequote);  
    
    if( endcp != cp ) val.append(cp,endcp);
    cp = endcp;
  }
  else
  {
    val.append(cp,endcp);
    cp = endcp;
  }  
  if( isdoublequote )
  {  
    std::wstring dq = L"''";
    std::wstring sq = L"'";
    Poco::replaceInPlace(key,dq,sq);
    
  }
  key = Poco::trim(key);
  val = Poco::trim(val);
  
  return true; 
}

//
// ParseKeyValueString reads string and converts it to key-value collection
// Example: id=5
// Example: name='5ff'
// Example: id1='5ff',id2='5ff'
//
// If DataClassDef is NULL then property values will be created as strings.
// If DataClassDef is not null then it will check for that key in properties list and convert value to corresponding type.
// If key was not found in properties list or value doesn't match property type exception will be raised.
// Special keys would be when there is only one pair and key is empty.
// Then it is assumed that it is value for identity preoprty of class. ( if class have multiple properties for identity exception is raised)
//
void c_OData_Impl::ParseKeyValueString( const std::wstring& KeyVal,MgClassDefinition* DataClassDef,MgPropertyCollection* Props,std::wstring& SingleValue )
{
  const wchar_t * cp = KeyVal.c_str();
  std::wstring key,val;key.reserve(64);val.reserve(128);
  
try
{
  Ptr<MgPropertyDefinitionCollection> props_def;
  if( DataClassDef ) props_def = DataClassDef->GetProperties();
  
  bool isfirstjustvalue=false;
  int count = 0;
  while( GetNextKeyValuePair(cp,key,val) )
  {
    if( key.empty() && val.empty() )
      throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Both Key and Value are empty.");      
        
    if( key.empty() )
    {
      
      if( count == 0)
        isfirstjustvalue = true;
      else
      {
        throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Key is empty and it is not first key-value pair.");            
      }      
    }
    
    if( isfirstjustvalue && count >0 ) 
    {
      throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! In first pair Key is empty but there are more then one key-value pairs.");            
    }
    
    if( !DataClassDef )
    {
      if( !key.empty() ) 
      {
        Ptr<MgStringProperty> prop = new MgStringProperty(key,val);
        Props->Add(prop);
      }
      else
      {
        SingleValue = val;
      }
    }
    else
    {
      Ptr<MgPropertyDefinition> key_def;
      if( key.empty() )
      {
        Ptr<MgPropertyDefinitionCollection> ident_defs = DataClassDef->GetIdentityProperties();
        if( ident_defs->GetCount() != 1)
        {
          throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Key name can't be empty because class has multiple identity properties.");
        }
        INT32 id=0;
        key_def = ident_defs->GetItem(id);        
      }
      else
      {
        key_def = props_def->FindItem(key);
        if( !key_def.p )
        {
          throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Key name can't be find in class.");        
        }
      }
      
    // now convert value to property value for particular property type
      Ptr<MgProperty> prop = CreatePropertyValue(key_def->GetName(),key_def,val);
      Props->Add(prop);
    }
    
    count++;
  }
}
catch(MgException*ex)
{
  throw c_ExceptionInvalidKeyValueString("Invalid Key Value string!");            
  ex->Release();
}

}

// Parses OData literal value into corresponding value 
// those values are parsed as they appear in URI or HTTP header
// This values are not parsed from Atom or JSON formats
// Values are dscirbed in link bellow
// http://www.odata.org/developers/protocols/overview#AbstractTypeSystem
MgProperty* c_OData_Impl::CreatePropertyValue( const std::wstring& Name,MgPropertyDefinition* PropDef,const std::wstring& Val )
{
  switch(PropDef->GetPropertyType())
  {
    case MgFeaturePropertyType::DataProperty:
    {
      MgDataPropertyDefinition* data_def = (MgDataPropertyDefinition*)PropDef;
      switch(data_def->GetDataType())
      {
        case MgPropertyType::Boolean:
        {
          if( Poco::icompare(Val,L"true") == 0 )
          {
            return new MgBooleanProperty( Name,true );
          }
          else 
          {          
            if( Poco::icompare(Val,L"false") == 0 )
            {
              return new MgBooleanProperty( Name,false );
            }
            else
            {
              throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Invalid Boolean value.");   
            }
          }
        }
        break;
        case MgPropertyType::Byte:
        {
          int val_byte;
          int endind;
          int nump = swscanf_s(Val.c_str(),L"%x%n",&val_byte,&endind);
          if( nump != 1 || endind != Val.length() )
          {
            throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting Byte XX.");   
          }
          else
          {
            if( val_byte < 0 || val_byte > 255 )
              throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Byte value should be between 0..255.");   
          }
          
          return new MgByteProperty( Name,val_byte );
        }
        break;
        case MgPropertyType::Int16:
        {
          int val_byte;
          int endind;
          int nump = swscanf_s(Val.c_str(),L"%hd%n",&val_byte,&endind);
          if( nump != 1 || endind != Val.length() )
          {
            throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting integer int32.");   
          }
          

          return new MgInt16Property( Name,val_byte );
        }
        break;
        case MgPropertyType::Int32:
        {
          int val_byte;
          int endind;
          int nump = swscanf_s(Val.c_str(),L"%d%n",&val_byte,&endind);
          if( nump != 1 || endind != Val.length() )
          {
            throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting integer int32.");   
          }


          return new MgInt32Property( Name,val_byte );
        }
        break;
        case MgPropertyType::Int64:
        {
          INT64 val_byte;
          int endind;
          int nump = swscanf_s(Val.c_str(),L"%I64dL%n",&val_byte,&endind);
          if( nump != 1 || endind != Val.length() )
          {
            throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting integer int64.");   
          }


          return new MgInt64Property( Name,val_byte );
        }
        break;
        case MgPropertyType::Single:
        {
          float val_float;
          int endind;
          int nump = swscanf_s(Val.c_str(),L"%ff%n",&val_float,&endind);
          if( nump != 1 || endind != Val.length() )
          {
            throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting float.");   
          }
          return new MgSingleProperty( Name,val_float );
        }
        break;
        case MgPropertyType::Double:
        {
          double val_float;
          int endind;
          int nump = swscanf_s(Val.c_str(),L"%lf%n",&val_float,&endind);
          if( nump != 1 || endind != Val.length() )
          {
            nump = swscanf_s(Val.c_str(),L"%lE%n",&val_float,&endind);
            if( nump != 1 || endind != Val.length() )
            {
              throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting float.");   
            }
          }
          return new MgDoubleProperty( Name,val_float );
        }
        break;
        case MgPropertyType::String:
        {
        // it is enclosed in '
          int len = Val.length();
          if( len < 2 ) throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting string enclosed with '.");   
          
          if( Val[0]!='\'' || Val[len-1]!='\'') throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting string enclosed with '.");   
          
          std::wstring newval = Val.substr(1,len-2);
          std::wstring dq = L"''";
          std::wstring sq = L"'";
          Poco::replaceInPlace(newval,dq,sq);
          
          return new MgStringProperty( Name,newval);
        }
        break;
        case MgPropertyType::DateTime:
        {
          // datetime has format datetime'2000-12-12T12:00'
          // it is enclosed in '
          std::wstring tmpstr;
          int len = Val.length();
          if( len < 10 ) throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! DateTime cant have less then 10 characters.");   

          if( Val[len-1]!='\'') throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Expecting string enclosed with '.");   
          
          tmpstr = Val.substr(0,9);
          if( Poco::icompare(tmpstr,L"datetime'") != 0 ) throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! DateTime should start with datetime'.");   
      
          Poco::DateTime dt;
          int tz;
          tmpstr = Val.substr(9,len-10);
          std::string u8str;
          Poco::UnicodeConverter::toUTF8(tmpstr,u8str);
          if( !Poco::DateTimeParser::tryParse(Poco::DateTimeFormat::ISO8601_FORMAT,u8str,dt,tz) )
          {
            throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Invalid DateTime string.");   
          }
      
      
          Ptr<MgDateTime> mgdt = new MgDateTime(dt.year(),dt.month(),dt.day(),dt.hour(),dt.minute(),dt.second());
          return new MgDateTimeProperty( Name,mgdt);
        }
        break;
        
        
        default:
          throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Unsupported data property type.");   
        break;  
      }
    }
    break;
    case MgFeaturePropertyType::GeometricProperty:
    {
      MgDataPropertyDefinition* data_def = (MgDataPropertyDefinition*)PropDef;
    }
    break;
    default:
      throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Unsupported property type, only Data and Geometry supported.");   
    break;
  }
  
  throw c_ExceptionInvalidKeyValueString("Invalid Key Value string! Unsupported property type, only Data and Geometry supported.");   
}





