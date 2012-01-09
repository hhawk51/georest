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

#ifndef _c_RestHandler_EsriGS_h
#define _c_RestHandler_EsriGS_h



class c_CfgDataResource;

//
#include "c_RestHandler.h"
#include "c_RestRequest.h"

class c_RestHandler_EsriGS : public c_RestHandler
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
    c_RestHandler_EsriGS(c_RestRequest *Request);
    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    void Execute(c_RestResponse& hResponse);

    void Execute_CatalogDocument( c_RestResponse& HttpResponse );    
    void Execute_ServiceDocument(c_RestResponse& HttpResponse);
    void Execute_ServiceMetadata(c_RestResponse& HttpResponse);
    void Execute_GET_OData(c_RestResponse& HttpResponse,const std::wstring& ResourceUriTag,const std::wstring& KeysString);
    
    
    
protected:
  void CreateQueryOptions(c_RestRequest* RestRequest,const std::wstring& KeysString,MgFeatureQueryOptions* QueryOptions);
  void Execute_Folder( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder );
  void Execute_Server( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_Server * esrigs_server );
  void Execute_MapServer( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_MapServer * esrigs_server);
  void Execute_MapServer_ExportMap( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_MapServer * esrigs_server);
  void Execute_MapServer_Identify( c_RestResponse& HttpResponse, c_CfgEsriGS_Folder * esrigs_folder, c_CfgEsriGS_MapServer * esrigs_server);
  
  
  enum e_LayersCmd {
    e_Invalid,
    e_Show,
    e_Hide,
    e_Include,
    e_Exclude,
  };
  e_LayersCmd ParseQueryLayers(std::wstring& StrLayers,std::vector<int>& LayerIds);
  
};

#endif  //_MGHTTPCREATESESSION_H_
