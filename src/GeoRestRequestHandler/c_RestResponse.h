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

#ifndef _c_RestResponse_h
#define _c_RestResponse_h

#include "Poco/Net/MessageHeader.h"
#include "c_RestResult.h"




class c_RestResponse_HttpData
{
public:
  c_RestResponse_HttpData()
  {
    //m_HeaderLength = 0;
    m_ContentLength=0;
    m_Status = 200;
    m_StatusReason = "OK";
  }
protected:
  //std::string m_Header;
  //int m_HeaderLength;
  int m_Status;
  std::string m_StatusReason;
  Poco::Net::MessageHeader m_MsgHeaders;

  int m_ContentLength;
  std::string m_Content_String;
  Ptr<MgByteReader> m_Content_OutputReader;  
  
public:
  
  void SetContent(const std::string& Content)
  {
    m_Content_String = Content; m_ContentLength = m_Content_String.length();
    m_Content_OutputReader = NULL;
  };
  void SetContent(const char* Content)
  {
    m_Content_String = Content; m_ContentLength = m_Content_String.length();
    m_Content_OutputReader = NULL;
  };
  void SetContent(MgByteReader* Content)
  {
    m_Content_OutputReader = SAFE_ADDREF(Content);
    m_ContentLength = m_Content_OutputReader.p ? m_Content_OutputReader->GetLength() : 0;
    m_Content_String = "";
  };
  
  void SetStatusAndReason(int Status,const char* Reason)
  {
    m_Status = Status;
    m_StatusReason = Reason;
  };
  int GetStatus()
  {
    return m_Status;
  };
  const char* GetStatusReason()
  {
    return m_StatusReason.c_str();
  };
  void AddHeader(const std::string& Name,const std::string& Value)
  {
    m_MsgHeaders.set(Name,Value);
  };
  
  Poco::Net::MessageHeader& GetMsgHeaders() { return m_MsgHeaders; }
  
  std::string& GetContentString() { return m_Content_String ; }
  
  MgByteReader* GetContentByteReader() { return SAFE_ADDREF(m_Content_OutputReader.p); }
};


/// <summary>
/// Purpose of this class is to package response header variables,
/// response data and status code to return to clients.
/// </summary>
class REST_REQUEST_HANDLER_API c_RestResponse : public MgDisposable
{
    EXTERNAL_API:
        c_RestResponse();


        /// <summary>
        /// Makes available the pointer to c_RestResult.
        /// User will retrieve the actual contents
        /// from this result instance.
        /// </summary>
        /// <returns>Pointer to c_RestResult or NULL</returns>
        c_RestResult* GetResult();

		const std::string& GetJsonpCallbackName() const { return m_JsonpCallbackName; }
        void SetJsonpCallbackName(const std::string& val);
        
        const std::wstring& GetContentType() const 
        { 
          return m_ContentType; 
        }
        void SetContentType(const std::wstring& ContentType) 
        { 
          m_ContentType=ContentType; 
        }
        
        // PrepareHttpResponse will check what MIME type of response is requested
        // and convert (template in some cases-e.g. convert feature reader to html) result to requested type
        
        c_RestResponse_HttpData* GetHttpData(c_RestRequest* RestRequest);

    INTERNAL_API:
        /// <summary>
        /// Destructor to clean up internal objects
        /// </summary>
        virtual ~c_RestResponse();

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
        
        
        void RequestAuth(c_RestRequest* Request);
        void WriteContent(const char *pszFormat, ...);
        void SendError(MgException* e);
        void CreateJsonpCallbackString(const std::string& CallbackFuncName,const std::string& JsonValue,std::string& JsonP);



    private:
      Poco::Net::MessageHeader m_HttpHeader;
        Ptr<c_RestResult> m_result;
		    std::string m_JsonpCallbackName;
        std::wstring m_ContentType; // requested format of response ( XMl, JSON, JSONP )
        
  protected:
    c_RestResponse_HttpData m_HttpData; // here will set data to be returned via Http to client        
    
    c_RestResponse_HttpData* PrepareHttpData(c_RestRequest* Request);

};

#endif
