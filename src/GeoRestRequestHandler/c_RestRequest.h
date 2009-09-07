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

#ifndef _c_RestRequest_h
#define _c_RestRequest_h


#include "c_RestUriPathParam.h"
#include "c_RestResponse.h"
#include "c_RestUriRequestMetadata.h"
#include "c_RestUri.h"
/// <summary>
/// Purpose of this class is to store request header variables, request parameters
/// and request meta data for use in Common MapAgent API (CMA).
/// </summary>
	
class IsapiPostParser;
class c_CfgDataResource;
class c_CfgRepresentation;
	
class REST_REQUEST_HANDLER_API c_RestRequest : public MgDisposable
{
    EXTERNAL_API:
  c_RestRequest(const std::string& AgentUri,const std::string& BaseUri,const std::string& RestUri,const std::string& HttpMethod,const std::string& XmlPostData);

        /// <summary>
        /// This method executes the request and returns the response
        /// object to the client.
        /// </summary>
        /// <returns>Pointer to c_RestResponse object</returns>
        c_RestResponse* Execute();  
        
        void SetPostData(IsapiPostParser* PostData);
        IsapiPostParser* GetPostData();
        
         /// <summary>
        /// Makes available the pointer to header class.
        /// User will use header class instance to add
        /// header information for a request.
        /// </summary>
        /// <returns>Pointer to MgHttpHeader class</returns>
        c_RestHeader* GetHeader();

        /// <summary>
        /// Makes available the pointer to RequestParam class.
        /// User will use this class instance to add
        /// all parameters specified for a request.
        /// </summary>
        /// <returns>Pointer to MgHttpRequestParam class</returns>
        c_RestUriRequestParam* GetRequestParam();

        /// <summary>
        /// Makes available the pointer to RequestMetadata class.
        /// User will use this class instance to add
        /// all required request metadata.
        /// </summary>
        /// <returns>Pointer to MgHttpRequestMetadata class</returns>
        c_RestUriRequestParam* GetRequestMetadata();

        
public:
 

    INTERNAL_API:
        c_RestRequest();

        /// <summary>
        /// Destructor to clean up internal objects
        /// </summary>
        virtual ~c_RestRequest();
        
        c_RestUriPathParam* GetUriPathParameters();
        
        const std::string& GetHttpMethodString() { return m_RestUri.GetHttpMethodString(); }
        void SetHttpMethod(const std::string& HttpMethod) { m_RestUri.SetHttpMethod(HttpMethod); }
        
        c_RestUri::e_HttpMethod GetHttpMethod();
        
        const std::string& GetBaseUri() const { return m_RestUri.GetBaseUri(); }
        const std::string& GetAgentUri() const { return m_RestUri.GetAgentUri(); }
        //void SetBaseUri(const std::string& BaseUri) { m_BaseUri = BaseUri; }

        
    protected:
        /// <summary>
        /// This method is responsible for releasing all
        /// its member variables and then destruct itself.
        /// </summary>
        /// <returns>Returns nothing</returns>
        virtual void Dispose();

        /// <summary>
        /// Get the class Id
        /// </summary>
        /// <returns>
        /// The integer value
        /// </returns>
        INT32 GetClassId();
        
        void ParseUri(const std::string& Uri, std::vector < std::string > & UriSegments);
        void REST_Request_Hello(c_RestRequest *HttpRequest,c_RestResponse& HttpResponse);
    protected:
           
      Ptr<c_RestHeader> m_RestHttpHeader;
      //Ptr<c_RestUriRequestParam> m_RestRequestParam;
      Ptr<c_RestUriRequestParam> m_RestRequestMetadata;
                 
      c_RestUri m_RestUri;                  
      //std::string m_RestUri;
      //std::string m_BaseUri;
      //std::string m_HttpMethod;
      //Ptr<c_RestUriPathParam> m_UriPathParameters;
      
     
    public: 
      enum e_RestResultStatus
      {
        e_Unknown,
        e_Ok, // result is in result object m_pResultObject
        e_BBox_OutOfRange,
        e_Count_OutOfRange,
      };
      
      e_RestResultStatus m_RestResultObjectStatus;
      // when this request is executed bellow are written parameters used to execute it
      const c_CfgDataResource* m_CfgDataResource;
      const c_CfgRepresentation* m_CfgRepresentation; // this is representation from m_CfgDataResource to be returned by this url
      
      
      Ptr<MgClassDefinition> m_DataClassDef;

CLASS_ID:
        static const INT32 m_cls_id = 0; //HttpHandler_MapAgent_HttpRestRequest;
};

#endif
