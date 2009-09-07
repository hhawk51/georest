//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef ISAPI_RESPONSE_HANDLER_H
#define ISAPI_RESPONSE_HANDLER_H

#include "HttpExt.h"

class c_RestRequest;
class c_RestResponse;

// This class transforms MgHttpResponse data into an HTTP compliant
// response which is then send back to the web server.
class IsapiResponseHandler
{
public:
    IsapiResponseHandler(c_RestRequest* RestRequest,const string& AgentUri,const string& UriBase,EXTENSION_CONTROL_BLOCK *pECB);
    ~IsapiResponseHandler(void);
    void SendResponse(c_RestResponse* response);
    void SendError(MgException* e);
    void RequestAuth();

private:
    void WriteContext(const char *pszFormat, ...);
    void WriteHeader(const char* szBuffer, const char* szStatusBuffer = NULL);
    
    void CreateJsonpCallbackString(const std::string& CallbackFuncName,const std::string& JsonValue,std::string& JsonP);
    
  EXTENSION_CONTROL_BLOCK *m_pECB;
  string m_UriBase; // part before '/REST/...' e.g. http://localhost/mgrest
  string m_AgentUri; // AgentUri is full URI, like http://localhost/mgrest/rest/data/parcel/.html?filter_1_and_likeright_NAME=u
  
  Ptr<c_RestRequest> m_RestRequest;
};

#endif

