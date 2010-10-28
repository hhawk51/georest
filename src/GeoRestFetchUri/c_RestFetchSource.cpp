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
#include "StdAfx.h"
//#include <iostream>
#include "MapGuideCommon.h"
#include "Fdo.h"
#include "System/UserInformation.h"
#include "RestFetchUriInclude.h"

#include "c_RestFetchSource.h"
#include "RestConfigInclude.h"
#include "c_MgServerFeatureUtil.h"


#include <ctemplate/template.h>

#include "Poco/TemporaryFile.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Uri.h"
#include "Poco/StreamCopier.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/XML/XMLString.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/Exception.h"
#include <fstream>
#include "c_RestMgSiteConnection.h"
#include "c_FdoSelectFeatures.h"

using Poco::TemporaryFile;

using Poco::XML::XMLString;
using Poco::XML::XMLWriter;
using Poco::XML::DOMParser;
using Poco::XML::DOMWriter;
using Poco::XML::InputSource;
using Poco::XML::Document;
using Poco::XML::Element;
using Poco::XML::Text;
using Poco::XML::NodeList;
using Poco::XML::NodeIterator;
using Poco::XML::NodeFilter;
//using Poco::XML::Node;
using Poco::XML::AutoPtr;
using Poco::Exception;


using Poco::StreamCopier;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPClientSession;

extern wchar_t g_AppFileName[MAX_PATH+2];



c_RestFetchSource::c_RestFetchSource()
{

}

c_RestFetchSource::~c_RestFetchSource( void )
{

}



/// <summary>
/// Validates the common parameters of the request.
/// </summary>
/// <param name="serviceType">Input
/// This defines the service type to create.
/// </param>
/// <returns>
/// Pointer to MgService based on serviceType.
/// </returns>
MgService* c_RestFetchSource::CreateMapguideService(const std::wstring& UserName,const std::wstring& Password,INT16 serviceType)
{
    MG_TRY()
    
    Ptr<MgUserInformation> userInfo = new MgUserInformation();
    userInfo->SetMgUsernamePassword(UserName, Password);
    
    // And create the site connection
    Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
    siteConn->Open(userInfo);

    Ptr<MgService> service = siteConn->CreateService(serviceType);

    return SAFE_ADDREF((MgService*)service);

    MG_CATCH_AND_THROW(L"c_RestFetchSource::CreateMapguideService")

    return NULL;
}

c_RestDataReader* c_RestFetchSource::FetchSource(const c_CfgDataSource* DataSource,MgFeatureQueryOptions* QueryOptions)
{
  switch(DataSource->GetSourceType())
  {
    case c_CfgDataSource::e_MgFeatureSource:
      return FetchSource_MG((const c_CfgDataSource_MgFeatureSource*)DataSource,QueryOptions);
    break;
    case c_CfgDataSource::e_FDO:
      return FetchSource_FDO((const c_CfgDataSource_FDO*)DataSource,QueryOptions);
      break;
    case c_CfgDataSource::e_Http:
    break;
    
    default:
      throw new MgRuntimeException(L"c_RestFetchSource::FetchSource", __LINE__, __WFILE__, NULL, L"Unsupported type of Source of Data", NULL);
    break;
  }
};


c_RestDataReader* c_RestFetchSource::FetchSource_MG(const c_CfgDataSource_MgFeatureSource* DataSource,MgFeatureQueryOptions* QueryOptions)
{
  MgResourceIdentifier resId(DataSource->m_MgFeatureSource);
  
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(L"Anonymous",L"",DataSource->GetServerIP(),DataSource->GetServerPort());    
  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);
  
  Ptr<MgFeatureReader> featureReader = service->SelectFeatures(&resId, DataSource->m_MgFeatureSourceClassName, QueryOptions); 
  
  MgProxyFeatureReader* proxy_reader = dynamic_cast<MgProxyFeatureReader*>(featureReader.p);
  
  return new c_RestDataReader_MgFeatureReader(proxy_reader);
  
}//end of  c_RestFetchSource::FetchMgFeatureSource

c_RestDataReader* c_RestFetchSource::FetchSource_FDO(const c_CfgDataSource_FDO* DataSource,MgFeatureQueryOptions* QueryOptions)
{
  c_FdoSelectFeatures selfeatures;
  return selfeatures.SelectFeatures(DataSource,  QueryOptions);
  
}//end of  c_RestFetchSource::FetchMgFeatureSource

bool c_RestFetchSource::ParseFilterQueryParam(const wstring& Param,int& Level,e_BoolOper& BoolOper,e_CompOper& CompOper,wstring& PropertyName)
{
  wstring item,rest;

  rest = Param;
  int underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;

  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);
  if( wcsicmp(item.c_str(),L"filter") != 0 ) return false;

  // now check for number of level
  underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;
  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);
  Level = _wtoi(item.c_str());

  // check for Bool oper
  underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;
  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);

  if( wcsicmp(item.c_str(),L"AND") == 0 )
  {
    BoolOper = e_AND;
  }
  else
  {
    if( wcsicmp(item.c_str(),L"OR") == 0 )
    {
      BoolOper = e_OR;
    }
    else
      return false;
  }

  // check for comp. operator
  underind = rest.find(L"_");
  if( underind == wstring::npos ) return false;
  item = rest.substr(0,underind);
  rest = rest.substr(underind+1);

  if( wcsicmp(item.c_str(),L"EQUAL") == 0 )
  {
    CompOper = e_EQUAL;
  }
  else
  {
    if( wcsicmp(item.c_str(),L"LIKE") == 0 )
    {
      CompOper = e_LIKE;
    }
    else
    {
      if( wcsicmp(item.c_str(),L"LIKERIGHT") == 0 )
      {
        CompOper = e_LIKERIGHT;
      }
      else
      {
        if( wcsicmp(item.c_str(),L"LIKELEFT") == 0 )
        {
          CompOper = e_LIKELEFT;
        }
        else
        {
          if( wcsicmp(item.c_str(),L"GTE") == 0 )
          {
            CompOper = e_GREATER_EQUAL;
          }
          else
          {
            if( wcsicmp(item.c_str(),L"GT") == 0 )
            {
              CompOper = e_GREATER;
            }
            else
            {
              if( wcsicmp(item.c_str(),L"LTE") == 0 )
              {
                CompOper = e_LESSER_EQUAL;
              }
              else
              {
                if( wcsicmp(item.c_str(),L"LT") == 0 )
                {
                  CompOper = e_LESSER;
                }
                else
                {
                  return false;
                }
              }
            }
          }

        }
      }

    }

  }

  // now rest is property name
  PropertyName = rest;
  if( PropertyName.length() <= 0 ) return false;

  return true;

}


void c_RestFetchSource::CreateFilterString(MgClassDefinition* ClassDef,c_RestUriPathSegment*PathParam,c_RestUriRequestParam* RequestParam
                                          ,const wstring& UriPathFilter
                                          ,MgFeatureQueryOptions*qryOptions,REFSTRING FilterStr)
{
  if( RequestParam ->ContainsParameter(L"filter") )
  {
    STRING paramval = RequestParam->GetParameterValue(L"filter");
    FilterStr=paramval;
    if( qryOptions ) qryOptions->SetFilter(paramval);
  
  }
  
// beside filter string check for filter_ parameters  in URL
// filter arameter is constructed from 5 parts like :
// filter_n_booloper_operation_property
// where
// filter_ is fixed literal
// n.. is number which defines level of nested in filter
// booloper...is either AND or OR
// operation...is EQUAL, LIKE, LIKERIGHT, LIKELEFT, LT (LESS), GT (GREATER). LTE (LESSTHEN), GTE (GRAETERTHEN)
// property...is name of property to be filter by ( property LIKE 'upd%' )
//
// example: http://localhost/mgrest/rest/data/parcel/.html?filter_1_and_likeright_NAME=u
//
  wstring filter_urlparams;
  MgStringPropertyCollection* uri_params = RequestParam->GetParameters();
  int count = uri_params->GetCount();
  for(int ind=0;ind<count;ind++)
  {
    //STRING paramname = uri_params->GetName(ind);
    Ptr<MgStringProperty> strptr = uri_params->GetItem(ind);
    
    wstring name = strptr->GetName();
    wstring val = strptr->GetValue();
    
    int level; e_BoolOper booloper; e_CompOper compoper; wstring propname;
    if( c_RestFetchSource::ParseFilterQueryParam(name,level,booloper,compoper,propname) )
    {
      
    // add that to filter
      switch( compoper )
      {
        case e_LIKE:
          val = L"%" + val + L"%";
        break;  
        case e_LIKELEFT:
          val = L"%" + val;
        break;  
        case e_LIKERIGHT:
          val = val + L"%";
        break;  
      }
    
    // if property is string then I need to add ' ' to value string
      Ptr<MgPropertyDefinitionCollection> propdefcoll = ClassDef->GetProperties();
      Ptr<MgPropertyDefinition> propdef = propdefcoll->FindItem(propname);
      if( propdef.p )
      {
        if( propdef->GetPropertyType() == MgFeaturePropertyType::DataProperty )
        {
          MgDataPropertyDefinition *dataprop = (MgDataPropertyDefinition*)propdef.p;
          if( dataprop->GetDataType() == MgPropertyType::String )
          {
          // need to check if their is ' character
            basic_string <char>::size_type index = val.find('\'');
            while(index != string::npos )
            {
              val = val.replace(index,1,2,'\'');
              index = val.find('\'',index+2);
            }
            
            val = L"'" + val + L"'";            
          }
        }        
      }
    // combine with previous filter
      if( filter_urlparams.length() > 0 )
      {
        switch( booloper )
        {
          case e_AND:
            filter_urlparams = filter_urlparams + L" AND ";
          break;
          case e_OR:
            filter_urlparams = filter_urlparams + L" OR ";
          break;
        }
      }
    // add property name
      filter_urlparams += propname;
      
    // add an operator
      switch( compoper )
      {
        case e_EQUAL:
          filter_urlparams = filter_urlparams + L" = " + val;
        break;
        case e_LIKE:
        case e_LIKERIGHT:
        case e_LIKELEFT:
          filter_urlparams = filter_urlparams + L" LIKE " + val;
        break;
        case e_GREATER:
          filter_urlparams = filter_urlparams + L" > " + val;
        break;
        case e_GREATER_EQUAL:
          filter_urlparams = filter_urlparams + L" >= " + val;
        break;
        case e_LESSER:
          filter_urlparams = filter_urlparams + L" < " + val;
        break;
        case e_LESSER_EQUAL:
          filter_urlparams = filter_urlparams + L" <= " + val;
        break;
      }
    }
  }
  
  // if iflter URL parametrs are defined set them
  // also check if 'filter' was defined in url...if it was then combine both with AND
  if( filter_urlparams.length() > 0 )
  {
    if( FilterStr.length() > 0 )
    {
      FilterStr = FilterStr + L" AND " + filter_urlparams;
    }
    else
      FilterStr = filter_urlparams;
    if( qryOptions ) qryOptions->SetFilter(FilterStr);
  
  }
  
  
  if( RequestParam ->ContainsParameter(L"bbox") )
  {
    bool isbbox=false;
    double x1,y1,x2,y2;
    STRING paramval = RequestParam->GetParameterValue(L"BBOX");
    if( swscanf(paramval.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) == 4 )
    {
      isbbox=true;
     
    }
    else
    {
    // check if it is http url , then fetch that data and take those envelope as bbox
      if( wcsicmp(paramval.substr(0,5).c_str(),L"http:") == 0 )
      {
        c_RestFetchSource::FetchEnvelope(paramval.c_str(),x1,y1,x2,y2);
         isbbox=true;
      }
    }
    if( isbbox )
    {
       // create geometry spatial filter
      STRING geometryname = ClassDef->GetDefaultGeometryPropertyName();
      INT32 selectionVariant = MgFeatureSpatialOperations::EnvelopeIntersects;
      
      if( geometryname.length() >0 )
      {
        //STRING geomstr = m_DataFilter.substr(5,m_DataFilter.length()-6);
        
        // geomstr = POLYGON(x1 y1, x2 y1, x2 y2, x1 y2, x1 y1)
        wchar_t geomstr[1024];
        swprintf(geomstr,L"POLYGON((%.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf ))",x1,y1 ,x2,y1 ,x2,y2 ,x1,y2 ,x1,y1);
        
        MgWktReaderWriter wktReader;
        Ptr<MgGeometry> filterGeometry;
        filterGeometry = wktReader.Read(geomstr);
        
        if( filterGeometry.p != NULL )
        {
          //GEOMETRY ENVELOPEINTERSECTS GeomFromText(' POLYGON(())')
          FilterStr = geometryname + L" ENVELOPEINTERSECTS GeomFromText('" + geomstr + L"')";
          if( qryOptions ) qryOptions->SetSpatialFilter(geometryname,filterGeometry,selectionVariant);
        }
      }
    }
    
  }
  if( RequestParam ->ContainsParameter(L"ibox") )
  {
    double x1,y1,x2,y2;
    STRING paramval = RequestParam->GetParameterValue(L"ibox");
    if( swscanf(paramval.c_str(),L"%lf,%lf,%lf,%lf",&x1,&y1,&x2,&y2) == 4 )
    {
       // create geometry spatial filter
      STRING geometryname = ClassDef->GetDefaultGeometryPropertyName();
      INT32 selectionVariant = MgFeatureSpatialOperations::EnvelopeIntersects;
      
      if( geometryname.length() >0 )
      {
        //STRING geomstr = m_DataFilter.substr(5,m_DataFilter.length()-6);
        
        // geomstr = POLYGON(x1 y1, x2 y1, x2 y2, x1 y2, x1 y1)
        wchar_t geomstr[1024];
        swprintf(geomstr,L"POLYGON((%.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf, %.5lf %.5lf ))",x1,y1 ,x2,y1 ,x2,y2 ,x1,y2 ,x1,y1);
        
        MgWktReaderWriter wktReader;
        Ptr<MgGeometry> filterGeometry;
        filterGeometry = wktReader.Read(geomstr);
        
        if( filterGeometry.p != NULL )
        {
          //GEOMETRY ENVELOPEINTERSECTS GeomFromText(' POLYGON(())')
          FilterStr = geometryname + L" INTERSECTS GeomFromText('" + geomstr + L"')";
          if( qryOptions ) qryOptions->SetSpatialFilter(geometryname,filterGeometry,selectionVariant);
        }
      }
    }
  }
  
  if( UriPathFilter.length() > 0 )
  {
   
    // create filter by identity property
      Ptr<MgPropertyDefinitionCollection> propdefcoll = ClassDef->GetIdentityProperties();
      if( propdefcoll->GetCount() != 1 )
      {
        MgStringCollection arguments;
        arguments.Add(ClassDef->GetClassName());
        throw new MgRuntimeException(L"Execute_Get_Class_Feature",__LINE__, __WFILE__, &arguments, L"Class has more than one identity property. Unable to query by identity!", NULL);
      }
      int ind=0;
      bool isstring=false;
      Ptr<MgPropertyDefinition> propdef = propdefcoll->GetItem(ind);
      if( propdef->GetPropertyType() == MgFeaturePropertyType::DataProperty )
      {
        MgDataPropertyDefinition *dataprop = (MgDataPropertyDefinition*)propdef.p;
        if( dataprop->GetDataType() == MgPropertyType::String )
        {
          isstring = true;
        }
      }
      
      STRING filter;
      filter = propdef->GetName();
      filter += L"=";
      if( isstring )
      {
        filter += L"'" + UriPathFilter + L"'";
      }
      else
        filter += UriPathFilter;
      
      FilterStr = filter;
      if( qryOptions ) qryOptions->SetFilter(filter);
          
  }
}//end of c_RestFetchSource::CreateFilterString


MgEnvelope* c_RestFetchSource::XmlFeatureToEnvelope(Element * XmlFeature)
{
      Poco::XML::XMLString str_name;
      Poco::XML::XMLString str_val;
  
  //FdoPtr<FdoPropertyDefinitionCollection> coll_propdef =  ClassDef->GetProperties();
  
  MgEnvelope* geomextent = NULL; 
  
  Element* el_prop  = XmlFeature->getChildElement("Property"); //XmlUtil->GetElementNode(XmlFeature,"Property",false);
  while(el_prop)
  {
    Element* el_name  = el_prop->getChildElement("Name");
    Element* el_val  = el_prop->getChildElement("Value");
    
    if( !el_name || !el_val ) return NULL;
    
    Poco::XML::XMLString str_name = el_name->innerText();
    Poco::XML::XMLString str_val = el_val->innerText();
    
    STRING wstr_val;
    MgUtil::MultiByteToWideChar(str_val,wstr_val);
    
    
    MG_TRY()
    MgWktReaderWriter wktReader;
    Ptr<MgGeometry> geomval;
    geomval = wktReader.Read(wstr_val);
    
    if( geomval.p != NULL )
    {
      Ptr<MgEnvelope> e2 = geomval->Envelope();
      if( !geomextent )
        geomextent = new MgEnvelope(e2);
      else
        geomextent->ExpandToInclude(e2);            
    }
    MG_CATCH_AND_RELEASE()
    
    el_prop  = (Element*)el_prop->nextSibling(); //XmlUtil->GetElementNode(XmlFeature,"Property",false);
  }
  
  
  return geomextent;
}//end of XmlFeatureToEnvelope

MgGeometry* c_RestFetchSource::XmlFeature2Geometry(Poco::XML::Element * XmlFeature)
{
  Poco::XML::XMLString str_name;
  Poco::XML::XMLString str_val;

  //FdoPtr<FdoPropertyDefinitionCollection> coll_propdef =  ClassDef->GetProperties();

  MgEnvelope* geomextent = NULL; 

  Element* el_prop  = XmlFeature->getChildElement("Property"); //XmlUtil->GetElementNode(XmlFeature,"Property",false);
  while(el_prop)
  {
    Element* el_name  = el_prop->getChildElement("Name");
    Element* el_val  = el_prop->getChildElement("Value");

    if( !el_name || !el_val ) return NULL;

    Poco::XML::XMLString str_name = el_name->innerText();
    Poco::XML::XMLString str_val = el_val->innerText();

    STRING wstr_val;
    MgUtil::MultiByteToWideChar(str_val,wstr_val);


    MG_TRY()
      MgWktReaderWriter wktReader;
    Ptr<MgGeometry> geomval;
    geomval = wktReader.Read(wstr_val);

    if( geomval.p != NULL )
    {
      return SAFE_ADDREF(geomval.p);
      
    }
    MG_CATCH_AND_RELEASE()

      el_prop  = (Element*)el_prop->nextSibling(); //XmlUtil->GetElementNode(XmlFeature,"Property",false);
  }


  return NULL;
}//end of XmlFeatureGetGeometry

void c_RestFetchSource::FetchEnvelope( const wchar_t* Uri,double& X1,double& Y1,double& X2,double& Y2 )
{
  std::string mbstr;
  MgUtil::WideCharToMultiByte(Uri,mbstr);
  
  Ptr<MgEnvelope> geomextent = NULL;
  
  Poco::URI uri_parser(mbstr);
  
  HTTPClientSession s(uri_parser.getHost(),uri_parser.getPort()); //s("localhost", 80);
	HTTPRequest request(HTTPRequest::HTTP_GET, uri_parser.getPathAndQuery()); //request(HTTPRequest::HTTP_GET, "/mgrest/rest/data/parcel/1.xml");
	s.sendRequest(request);
	HTTPResponse response;
	std::istream& rs = s.receiveResponse(response);
	//assert (response.getContentLength() == HTTPTestServer::SMALL_BODY.length());
	//assert (response.getContentType() == "text/plain");
	std::string rtype = response.getContentType();
	
	
	//std::string str;
	//StreamCopier::copyToString(rs, str);
	int count=0;
	
	Poco::XML::InputSource src(rs);
	try
	{
		Poco::XML::DOMParser parser;
		AutoPtr<Document> pdoc = parser.parse(&src);
		
		
		
		AutoPtr<NodeList> nlist = pdoc->getElementsByTagName("FeatureSet");
		if( nlist->length() == 1 )
		{
		  Element* elem_fset = (Element*)nlist->item(0);
		  
		  Element* elem_features = elem_fset->getChildElement("Features");
		  
		  
           
  
		  AutoPtr<NodeList> flist = elem_features->getElementsByTagName("Feature");
		  int count_feat = flist->length();
		  for(int ind=0;ind<count_feat;ind++)
		  {
		    Element* elem_feature = (Element*)flist->item(ind);
		    
		    //insprop_col->Clear();
		    //XmlToFeature(insprop_col,m_RestFdoClassDef,elem_feature);
		    
		    Ptr<MgEnvelope> e2 = XmlFeatureToEnvelope(elem_feature);
		    if( e2 )
		    {
          if( !geomextent )
            geomextent = new MgEnvelope(e2);
          else
            geomextent->ExpandToInclude(e2);
        }
		    
		    //FdoPtr<FdoIFeatureReader>  insreader = inscomm->Execute();
		    
		    count = count+1;
		  }
		  
		}
		
		
	}
	
	catch (Poco::Exception& exc)
	{
	  std::wstring wstr;
	  MgUtil::MultiByteToWideChar(exc.displayText().c_str(),wstr);
		throw new MgRuntimeException(L"c_RestFetchSource::FetchEnvelope",__LINE__, __WFILE__, NULL, wstr, NULL);
	}
	
	if( geomextent.p )
	{
	  Ptr<MgCoordinate> p1 = geomextent->GetLowerLeftCoordinate();
	  X1 = p1->GetX();Y1 = p1->GetY();
	  Ptr<MgCoordinate> p2 = geomextent->GetUpperRightCoordinate();
	  X2 = p2->GetX();Y2 = p2->GetY();
	}
	else
	{
	  throw new MgRuntimeException(L"c_RestFetchSource::FetchEnvelope",__LINE__, __WFILE__, NULL, L"Unable to calculate Extent. No geometries fetched.", NULL);
	}
	return ;
}
MgGeometry* c_RestFetchSource::FetchBuffer( const wchar_t* Uri,double Distance)
{
  std::string mbstr;
  MgUtil::WideCharToMultiByte(Uri,mbstr);

  Ptr<MgEnvelope> geomextent = NULL;

  Poco::URI uri_parser(mbstr);

  HTTPClientSession s(uri_parser.getHost(),uri_parser.getPort()); //s("localhost", 80);
  HTTPRequest request(HTTPRequest::HTTP_GET, uri_parser.getPathAndQuery()); //request(HTTPRequest::HTTP_GET, "/mgrest/rest/data/parcel/1.xml");
  s.sendRequest(request);
  HTTPResponse response;
  std::istream& rs = s.receiveResponse(response);
  //assert (response.getContentLength() == HTTPTestServer::SMALL_BODY.length());
  //assert (response.getContentType() == "text/plain");
  std::string rtype = response.getContentType();


  //std::string str;
  //StreamCopier::copyToString(rs, str);
  int count=0;

  Poco::XML::InputSource src(rs);
  try
  {
    DOMParser parser;
    AutoPtr<Document> pdoc = parser.parse(&src);



    AutoPtr<NodeList> nlist = pdoc->getElementsByTagName("FeatureSet");
    if( nlist->length() == 1 )
    {
      Element* elem_fset = (Element*)nlist->item(0);

      Element* elem_features = elem_fset->getChildElement("Features");




      AutoPtr<NodeList> flist = elem_features->getElementsByTagName("Feature");
      int count_feat = flist->length();
      for(int ind=0;ind<count_feat;ind++)
      {
        Element* elem_feature = (Element*)flist->item(ind);

        //insprop_col->Clear();
        //XmlToFeature(insprop_col,m_RestFdoClassDef,elem_feature);

        Ptr<MgGeometry> geom = XmlFeature2Geometry(elem_feature);
        if( geom.p )
        {
          return geom->Buffer(Distance,NULL);
          
        }

        //FdoPtr<FdoIFeatureReader>  insreader = inscomm->Execute();

        count = count+1;
      }

    }


  }

  catch (Poco::Exception& exc)
  {
    std::wstring wstr;
    MgUtil::MultiByteToWideChar(exc.displayText().c_str(),wstr);
    throw new MgRuntimeException(L"c_RestFetchSource::FetchEnvelope",__LINE__, __WFILE__, NULL, wstr, NULL);
  }

  
  return NULL;
}


void XmlFeatureToDictionary(Element * XmlFeature,ctemplate::TemplateDictionary*Dict,const std::string& Prefix)
{
      Poco::XML::XMLString str_name;
      Poco::XML::XMLString str_val;
  
  //FdoPtr<FdoPropertyDefinitionCollection> coll_propdef =  ClassDef->GetProperties();
  
  MgEnvelope* geomextent = NULL; 
  
  Element* el_prop  = XmlFeature->getChildElement("Property"); //XmlUtil->GetElementNode(XmlFeature,"Property",false);
  while(el_prop)
  {
    Element* el_name  = el_prop->getChildElement("Name");
    Element* el_val  = el_prop->getChildElement("Value");
    
    if( el_name && el_val ) 
    {
      Poco::XML::XMLString str_name = Prefix;
      str_name += el_name->innerText();
      Poco::XML::XMLString str_val = el_val->innerText();
      
      Dict->SetValue(str_name,str_val);
    }
    
    
    el_prop  = (Element*)el_prop->nextSibling(); //XmlUtil->GetElementNode(XmlFeature,"Property",false);
  }
  
  
  return ;
}//end of XmlFeatureToDictionary


void c_RestFetchSource::FetchFeaturesToDictionary( const std::string& Uri,ctemplate::TemplateDictionary*ParentDict,const std::string& SectionName,const std::string& SectionDataName,const std::string& NamePrefix )
{
// check for URI type
// if it is http: or https: or it is Library:
if( stricmp(Uri.substr(0,5).c_str(),"Library:") == 0 )
{
// now I need to execute this against server
    
}

// if not library the assume it HTTP URL
c_RestFetchSource::HttpFetchFeaturesToDictionary( Uri,ParentDict,SectionName,SectionDataName,NamePrefix );

}

void c_RestFetchSource::HttpFetchFeaturesToDictionary( const std::string& Uri,ctemplate::TemplateDictionary*ParentDict,const std::string& SectionName,const std::string& SectionDataName,const std::string& NamePrefix )
{
  std::string mbstr;
  
  
  
  Poco::URI uri_parser(Uri);
  
  HTTPClientSession s(uri_parser.getHost(),uri_parser.getPort()); //s("localhost", 80);
	HTTPRequest request(HTTPRequest::HTTP_GET, uri_parser.getPathAndQuery()); //request(HTTPRequest::HTTP_GET, "/mgrest/rest/data/parcel/1.xml");
	s.sendRequest(request);
	HTTPResponse response;
	std::istream& rs = s.receiveResponse(response);
	//assert (response.getContentLength() == HTTPTestServer::SMALL_BODY.length());
	//assert (response.getContentType() == "text/plain");
	std::string rtype = response.getContentType();
	
	
	//std::string str;
	//StreamCopier::copyToString(rs, str);
	int count=0;
	
	Poco::XML::InputSource src(rs);
	try
	{
		DOMParser parser;
		AutoPtr<Document> pdoc = parser.parse(&src);
		
		
		
		AutoPtr<NodeList> nlist = pdoc->getElementsByTagName("FeatureSet");
		if( nlist->length() == 1 )
		{
		  Element* elem_fset = (Element*)nlist->item(0);
		  
		  Element* elem_features = elem_fset->getChildElement("Features");
		
		  AutoPtr<NodeList> flist = elem_features->getElementsByTagName("Feature");
		  int count_feat = flist->length();
		  
		  ctemplate::TemplateDictionary* dict_section;
		  ctemplate::TemplateDictionary* dict_datasection;
		  std::string sdataname;
		  if( count_feat > 0 )
		  {
		    // If SectionDataName is defined and is different than SectionName
		    // than create new dictionary for section and insert rows of data into subdictionary 'SectionDataName'
		    // This option is useful to hide whole section together with data and headers if no data
		    if( (SectionDataName.length() > 0 ) && stricmp(SectionDataName.c_str(),SectionName.c_str()) !=0 )
		    {
		      dict_section = ParentDict->AddSectionDictionary(SectionName);
		      
		      sdataname = SectionDataName;
		    }
		    else
		    {
		      dict_section = ParentDict;
		      
		      sdataname = SectionName;
		    }
		  }
		  for(int ind=0;ind<count_feat;ind++)
		  {
		    Element* elem_feature = (Element*)flist->item(ind);
		    
		    //insprop_col->Clear();
		    //XmlToFeature(insprop_col,m_RestFdoClassDef,elem_feature);
		    
		    dict_datasection = dict_section->AddSectionDictionary(sdataname);
		      
		    XmlFeatureToDictionary(elem_feature,dict_datasection,NamePrefix);
		    
		    
		    //FdoPtr<FdoIFeatureReader>  insreader = inscomm->Execute();
		    
		    count = count+1;
		  }
		  
		}
		
		
	}
	
	catch (Poco::Exception& exc)
	{
	  std::wstring wstr;
	  MgUtil::MultiByteToWideChar(exc.displayText().c_str(),wstr);
		//throw new MgInvalidRestUriException(L"c_RestFetchSource::FetchEnvelope",__LINE__, __WFILE__, NULL, wstr, NULL);
	}
	
	return ;
}//end of c_RestFetchSource::FetchFeaturesToDictionary




//////////////////////////////////////////////////////////////////
STRING GetSerializedXml(FdoFeatureSchemaCollection* fdoSchemaCol)
{
  STRING serializedXml;

  
    CHECKNULL(fdoSchemaCol, L"MgServerDescribeSchema.GetSerializedXml");

  FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
  CHECKNULL((FdoIoMemoryStream*)fmis, L"MgServerDescribeSchema.GetSerializedXml");

  // Write to memory stream
  fdoSchemaCol->WriteXml(fmis);
  fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

  FdoInt64 len = fmis->GetLength();
  FdoByte *bytes = new FdoByte[(size_t)len];
  CHECKNULL(bytes, L"MgServerDescribeSchema.GetSerializedXml");

  fmis->Read(bytes, (FdoSize)len);

  Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
  byteSource->SetMimeType(MgMimeType::Xml);
  Ptr<MgByteReader> byteReader = byteSource->GetReader();

  string out = MgUtil::GetTextFromReader(byteReader);
  serializedXml = MgUtil::MultiByteToWideChar(out);

  delete [] bytes;

  

    return serializedXml;
}

MgByteReader* GetSerializedXml_ByteReader(FdoFeatureSchemaCollection* fdoSchemaCol)
{
  STRING serializedXml;


  CHECKNULL(fdoSchemaCol, L"MgServerDescribeSchema.GetSerializedXml");

  FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
  CHECKNULL((FdoIoMemoryStream*)fmis, L"MgServerDescribeSchema.GetSerializedXml");

  // Write to memory stream
  fdoSchemaCol->WriteXml(fmis);
  fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

  FdoInt64 len = fmis->GetLength();
  FdoByte *bytes = new FdoByte[(size_t)len];
  CHECKNULL(bytes, L"MgServerDescribeSchema.GetSerializedXml");

  fmis->Read(bytes, (FdoSize)len);

  Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
  byteSource->SetMimeType(MgMimeType::Xml);
  
  delete [] bytes;
  
  return byteSource->GetReader();

    
}

MgByteReader* c_RestFetchSource::Fetch_FdoSourceSchema( c_CfgDataSource_FDO* FdoSource )
{
  try
  {
    FdoPtr<IConnectionManager> manager = FdoFeatureAccessManager::GetConnectionManager ();

    FdoPtr<FdoIConnection> conn = manager->CreateConnection (FdoSource->GetProvider().c_str());

    if( !conn.p ) return NULL;

    conn->SetConnectionString(FdoSource->GetConnString().c_str());
    if( conn->Open() != FdoConnectionState_Open )
    {
      std::wstring errmsg;
      errmsg = L"Unable to Open FDO Connection! Provider='";
      errmsg = errmsg + FdoSource->GetProvider() + L"' ConnectionString='" + FdoSource->GetConnString()+ L"'.";
      throw new MgRuntimeException(L"c_RestFetchSource::Fetch_FdoSourceSchema",__LINE__, __WFILE__, NULL, errmsg, NULL);   
    }

    FdoPtr<FdoIDescribeSchema> fdoCommand = (FdoIDescribeSchema*)conn->CreateCommand(FdoCommandType_DescribeSchema);

    FdoPtr<FdoFeatureSchemaCollection> ffsc;
    ffsc = fdoCommand->Execute();

    return GetSerializedXml_ByteReader(ffsc);

  }
  catch ( FdoException* ex ) 
  {  
    MgRuntimeException * mgexc = new MgRuntimeException(L"c_RestFetchSource::Fetch_FdoSourceSchema",__LINE__, __WFILE__, NULL, ex->GetExceptionMessage(), NULL); 
    ex->Release();
    throw mgexc;  
  }

  return NULL;
}

MgClassDefinition* c_RestFetchSource::Fetch_MgClassDefinition( const c_CfgDataSource* DataSource )
{
  switch(DataSource->GetSourceType())
  {
    case c_CfgDataSource::e_MgFeatureSource:
      return Fetch_MgClassDefinition_MG( (c_CfgDataSource_MgFeatureSource*)DataSource );
    break;
    case c_CfgDataSource::e_FDO:
      return Fetch_MgClassDefinition_FDO( (c_CfgDataSource_FDO*)DataSource );
    break;
  }
  
  throw new MgRuntimeException(L"c_RestFetchSource::Fetch_MgClassDefinition",__LINE__, __WFILE__, NULL, L"DataSource is not of correct type. It has to be FDO or MapGuide Feature Source", NULL);         
}

MgClassDefinition* c_RestFetchSource::Fetch_MgClassDefinition_FDO( const c_CfgDataSource_FDO* FdoSource )
{
  try
  {
    FdoPtr<IConnectionManager> manager = FdoFeatureAccessManager::GetConnectionManager ();

    FdoPtr<FdoIConnection> conn = manager->CreateConnection (FdoSource->GetProvider().c_str());

    if( !conn.p ) 
    {
      std::wstring errmsg;
      errmsg = L"Unable to Create FDO Connection! Provider='";
      errmsg = errmsg + FdoSource->GetProvider() + L"'!";
      throw new MgRuntimeException(L"c_RestFetchSource::Fetch_MgClassDefinition",__LINE__, __WFILE__, NULL, errmsg, NULL);         
    }

    conn->SetConnectionString(FdoSource->GetConnString().c_str());
    if( conn->Open() != FdoConnectionState_Open )
    {
      std::wstring errmsg;
      errmsg = L"Unable to Open FDO Connection! Provider='";
      errmsg = errmsg + FdoSource->GetProvider() + L"' ConnectionString='" + FdoSource->GetConnString()+ L"'.";
      throw new MgRuntimeException(L"c_RestFetchSource::Fetch_MgClassDefinition",__LINE__, __WFILE__, NULL, errmsg, NULL);   
    }

    FdoPtr<FdoIDescribeSchema> fdoCommand = (FdoIDescribeSchema*)conn->CreateCommand(FdoCommandType_DescribeSchema);

    FdoPtr<FdoFeatureSchemaCollection> ffsc;
    ffsc = fdoCommand->Execute();

    FdoPtr<FdoIDisposableCollection> coll = ffsc->FindClass(FdoSource->GetFeatureClass().c_str());
    if( coll->GetCount() > 0 )
    {
      FdoPtr<FdoClassDefinition> fdoclass = (FdoClassDefinition*)coll->GetItem(0);
      return c_MgServerFeatureUtil::GetMgClassDefinition(fdoclass,false);
    }
    else
    {
      std::wstring errmsg;
      errmsg = L"Unable to Find FDO class ! FeatureClass='";
      errmsg = errmsg + FdoSource->GetFeatureClass()+ L"'.";
      throw new MgRuntimeException(L"c_RestFetchSource::Fetch_MgClassDefinition",__LINE__, __WFILE__, NULL, errmsg, NULL);   
    }

  }
  catch ( FdoException* ex ) 
  {  
    MgRuntimeException * mgexc = new MgRuntimeException(L"c_RestFetchSource::Fetch_FdoSourceSchema",__LINE__, __WFILE__, NULL, ex->GetExceptionMessage(), NULL); 
    ex->Release();
    throw mgexc;  
  }

  return NULL;
}

MgClassDefinition* c_RestFetchSource::Fetch_MgClassDefinition_MG( const c_CfgDataSource_MgFeatureSource* MgFeatureSource )
{
  STRING schema;
  STRING classname;
  STRING::size_type iColon = MgFeatureSource->m_MgFeatureSourceClassName.find(':');
  if(iColon != STRING::npos) {
    schema = MgFeatureSource->m_MgFeatureSourceClassName.substr(0,iColon);
    classname   = MgFeatureSource->m_MgFeatureSourceClassName.substr(iColon+1);

  }
  else
  {
    schema = L"";
    classname =MgFeatureSource->m_MgFeatureSourceClassName;
  }
  
  MgResourceIdentifier resId(MgFeatureSource->m_MgFeatureSource);
  
  Ptr<c_RestMgSiteConnection> mgsiteconn = c_RestMgSiteConnection::Open(MgFeatureSource->GetUsername(),MgFeatureSource->GetPassword(),MgFeatureSource->GetServerIP(),MgFeatureSource->GetServerPort());    
  // Create Proxy Feature Service instance
  Ptr<MgFeatureService> service = (MgFeatureService*)mgsiteconn->CreateService(MgServiceType::FeatureService);
  
  MgClassDefinition* classdef = service->GetClassDefinition(&resId, schema, classname);
  
  
  return classdef;
}