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

#ifndef _c_RestHandler_Data_h
#define _c_RestHandler_Data_h



class c_CfgDataResource;
//
// Implements REST interface for MAP object. Map object is runtime Map created from MapDefinition
// and kept in session repository.
//
// 1. URI for get data about MAP
// HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]
//
// 2. URI creating new MAP in Session
// HttpMethod: POST URI: REST/SESSION[Id]/MAP ? MapDefName={Map Definition Name}
// 
// 3. Get Image Extent
// HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]/Extent
//
// 4. Get Image ( return png )
// HttpMethod: GET URI: REST/SESSION[Id]/MAP[Id]/Image.Png[...ImageCreationParameters...] {.jpg }
//
// 5. Set Image Extent
// HttpMethod: PUT URI: REST/SESSION[Id]/MAP[Id]/Extent ? {SETCETNTERX=..}&{SETCENTERY=..}&..   
//
//
#include "c_RestHandler.h"
#include "c_RestRequest.h"

class c_RestHandler_Data : public c_RestHandler
{
public:  
  static c_RestHandler* CreateObject( c_RestRequest* Request );
public:
    /// <summary>
    /// Initializes the common parameters of the request.
    /// </summary>
    /// <param name="name">Input
    /// MgHttpRestRequest
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    c_RestHandler_Data(c_RestRequest *Request);
    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    void Execute(c_RestResponse& hResponse);

    
    
    
    void Execute_Get_Class_Feature(c_RestResponse& HttpResponse);
    void Execute_Put_Class_Feature(c_RestResponse& HttpResponse);
    void Execute_Post_Class_Feature(c_RestResponse& HttpResponse);
    void Execute_Delete_Class_Feature(c_RestResponse& HttpResponse);
    
    void CreateFilterString(MgClassDefinition* classDef,MgFeatureQueryOptions*qryOptions,REFSTRING FilterStr);
    //void Execute_Get_Image_Png(c_RestResponse& HttpResponse);
    //void Execute_Get_Image_Png(c_RestResponse& HttpResponse,MgPropertyCollection* MapViewCommands);
    //void Execute_Get_Image_Png(c_RestResponse& HttpResponse,MgEnvelope* ZoomTo);
    //void Execute_Get_Image_Png(c_RestResponse& HttpResponse,MgEnvelope* ZoomTo,MgFeatureReader *FeatureReader);
    void Execute_Get_Image_Png(c_RestResponse& HttpResponse,MgEnvelope* ZoomTo,MgFeatureQueryOptions *QueryOptions);
    //MgEnvelope* GetFeatureExtent(MgFeatureReader *FeatureReader);
    MgEnvelope* GetFeatureExtent(c_RestDataReader *FeatureReader);
    MgEnvelope* GetFeatureExtent(MgFeatureReader *FeatureReader);
    enum e_BoolOper { 
      e_AND,
      e_OR
    };
    enum e_CompOper { 
      e_EQUAL,
      e_LIKE,
      e_LIKERIGHT,
      e_LIKELEFT,
      e_GREATER,
      e_GREATER_EQUAL,
      e_LESSER,   
      e_LESSER_EQUAL
    };
    bool ParseFilterQueryParam(const wstring& Param,int& Level,e_BoolOper& BoolOper,e_CompOper& CompOper,wstring& PropertyName);
    void Execute_Get_Feature_FDO(c_RestResponse& HttpResponse);
    void Execute_Get_Feature_MapGuide(c_RestResponse& HttpResponse);
protected:
  //STRING m_MapDefId;
  //STRING m_FeatureSourceId;
  //STRING m_ClassName;
  STRING m_UriParam_DataFilter;
  
  STRING m_Filter;
  //STRING m_FormatType;
  bool m_IsSitemapXml;
  
  //const c_CfgDataLayer* m_RestConfig;
  

};

#endif  //_MGHTTPCREATESESSION_H_
