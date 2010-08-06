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

#ifndef _c_RestHandler_OData_h
#define _c_RestHandler_OData_h



class c_CfgDataResource;

//
#include "c_RestHandler.h"
#include "c_RestRequest.h"

class c_RestHandler_OData : public c_RestHandler
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
    c_RestHandler_OData(c_RestRequest *Request);
    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    void Execute(c_RestResponse& hResponse);

    
    void Execute_ServiceDocument(c_RestResponse& HttpResponse);
    void Execute_ServiceMetadata(c_RestResponse& HttpResponse);
    void Execute_GET_OData(c_RestResponse& HttpResponse,const std::wstring& ResourceUriTag,const std::wstring& KeysString);
    
    
    
protected:
  void CreateQueryOptions(c_RestRequest* RestRequest,const std::wstring& KeysString,MgFeatureQueryOptions* QueryOptions);
  
  //const c_CfgDataLayer* m_RestConfig;
  

};

#endif  //_MGHTTPCREATESESSION_H_
